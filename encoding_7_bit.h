/*
 * encodign_7_bit.h
 *
 *  Created on: 9 apr 2026
 *      Author: kzu0
 *
 *  Descrizione:
 *  Codec a 7 bit basato su frame di 8 byte:
 *
 *		INPUT (7 byte)
 *      +--------+--------+--------+--------+--------+--------+--------+
 *		| 0xA1   | 0x66   | 0x97   | 0x0E   | 0xF1   | 0x55   | 0x81   |
 *		|10100001|01100110|10010111|00001110|11110001|01010101|10000001|
 *		+--------+--------+--------+--------+--------+--------+--------+
 *         d0       d1       d2       d3       d4       d5       d6
 *
 *		OUTPUT (8 byte)
 *		+--------+--------+--------+--------+--------+--------+--------+--------+
 *		| 0x55   | 0x21   | 0x66   | 0x17   | 0x0E   | 0x71   | 0x55   | 0x01   |
 *		|01010101|01000001|01100110|00010111|00001110|01110001|01010101|00000001|
 *		+--------+--------+--------+--------+--------+--------+--------+--------+
 *         MSB      d0       d1       d2       d3       d4       d5       d6
 *
 *  Dove:
 *  - MSB contiene i bit più significativi dei 7 byte originali
 *  - ogni byte dati contiene solo i 7 bit meno significativi
 *
 *  NOTE:
 *  - Overhead: +1 byte ogni 7 byte (~14.3%)
 *  - Codec lossless e simmetrico
 */

#ifndef INC_ENCODING_7_BIT_H_
#define INC_ENCODING_7_BIT_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Dimensione di un frame codificato (1 header + 7 dati) */
#define ENCODED_DATA_FRAME_SIZE 8

/**
 * @brief Contesto per codifica/decodifica streaming
 *
 * Mantiene lo stato tra chiamate successive per supportare stream
 * spezzati in chunk arbitrari.
 *
 * encoded_data[0]:
 *   - contiene il byte MSB (bitmask)
 *
 * encoded_data[1..7]:
 *   - buffer temporaneo usato in codifica
 */
typedef struct {
    uint8_t encoded_data_pos;
    uint8_t encoded_data[ENCODED_DATA_FRAME_SIZE];
} enc7bit_ctx_t;

/**
 * @brief Inizializza il contesto
 *
 * Deve essere chiamata prima dell’uso del contesto.
 */
void enc7bit_ctxInit( enc7bit_ctx_t* ctx );

/**
 * @brief Codifica stream a 7 bit
 *
 * @param ctx   contesto di codifica
 * @param in    buffer input (byte originali)
 * @param out   buffer output (byte codificati)
 * @param size  numero di byte da codificare
 * @param flush se true, forza la chiusura di un frame incompleto
 *
 * @return numero di byte scritti in output
 *
 * @note
 * - Può essere chiamata con chunk arbitrari
 * - Se flush = false, un frame incompleto NON viene emesso
 * - Se flush = true, un frame parziale viene scritto (header + dati presenti)
 *
 * @warning
 * Il chiamante deve garantire che il buffer di output sia sufficientemente grande.
 */
uint32_t enc7bit_encodeStream(enc7bit_ctx_t* ctx, const uint8_t *in, uint8_t *out, uint32_t size, bool flush);

/**
 * @brief Decodifica stream a 7 bit
 *
 * @param ctx   contesto di decodifica
 * @param in    buffer input (byte codificati)
 * @param out   buffer output (byte decodificati)
 * @param size  numero di byte da decodificare
 *
 * @return numero di byte scritti in output
 *
 * @note
 * - Supporta input spezzato in chunk arbitrari
 * - Mantiene automaticamente lo stato tra chiamate
 * - Un byte MSB senza dati successivi NON produce output
 *
 * @warning
 * Il chiamante deve garantire che il buffer di output sia sufficientemente grande.
 */
uint32_t enc7bit_decodeStream(enc7bit_ctx_t* ctx, const uint8_t *in, uint8_t *out, uint32_t size );

/**
 * @brief Codifica buffer completo (non streaming)
 *
 * Wrapper di enc7bit_encodeStream con flush automatico.
 */
uint32_t enc7bit_encode( const uint8_t *in, uint8_t *out, uint32_t size );

/**
 * @brief Decodifica buffer completo (non streaming)
 *
 * Wrapper di enc7bit_decodeStream.
 */
uint32_t enc7bit_decode( const uint8_t *in, uint8_t *out, uint32_t size );

/**
 * @brief Calcola overhead di codifica
 *
 * @param size byte input
 * @return byte aggiuntivi necessari
 */
static inline uint32_t enc7bit_overhead( uint32_t size )
{
    if ( size == 0 )
        return 0;

    return ( size / 7 ) + ( ( size % 7 ) ? 1 : 0 );
}

/** @brief Lunghezza di una sequenza codificata
 * @param  size numero di byte di informazione da codificare
 * @retval numero totale di byte codificati
 *
 * Risponde alla domanda: di quanti byte ho bisogno per codificare
 * un certo numero di byte?
 */
static inline uint32_t enc7bit_encodeLength( uint32_t size )
{
    if ( size == 0 )
        return 0;

    return size + enc7bit_overhead( size );
}

/** @brief Lunghezza di una sequenza decodificata
 * @param  size numero di byte da decodificare
 * @retval numero di byte d'informazione decodificati
 *
 * Risponde alla domanda: quanti byte decodificati posso tirar fuori
 * da encoded_size byte codificati?
 *
 * Ogni blocco completo da 8 byte codificati produce 7 byte decodificati
 * Se ho un blocco parziale e quel blocco ha solo un byte, allora non
 * contiene informazione, perché il byte rimasto è il byte di codifica
 * senza ulteriori byte di dati. Se invece ha più di un byte, allora
 * il blocco parziale contiene un byte di codifica più successivi byte
 * di dato
 */
static inline uint32_t enc7bit_decodeLength( uint32_t size )
{
    if ( size == 0 )
        return 0;

    return ( ( size / 8 ) * 7 ) + ( (size % 8 > 1) ? (size % 8 - 1) : 0 );
}

#ifdef __cplusplus
}
#endif

#endif /* INC_ENCODING_7_BIT_H_ */
