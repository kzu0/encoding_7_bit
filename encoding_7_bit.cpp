/*
 * encodign_7_bit.c
 *
 *  Created on: 9 apr 2026
 *      Author: kzu0
 */

#include "encoding_7_bit.h"

uint32_t enc7bit_encodeStream( enc7bit_ctx_t *ctx, const uint8_t *in, uint8_t *out, uint32_t size, bool flush ) {

    if ( !ctx || !in || !out || ( size == 0 ) )
    {
        return 0;
    }

    uint32_t written = 0; // accumulatore dei byte codificati scritti in out

    while ( size-- )
    {
        //assert( ctx->encoded_data_pos < ( ENCODED_DATA_FRAME_SIZE - 1 ) );

        uint8_t raw_byte = *in++;

        ctx->encoded_data[ 0 ] |= ( ( raw_byte & 0x80 ) >> 7 ) << ctx->encoded_data_pos;
        ctx->encoded_data[ ctx->encoded_data_pos + 1 ] = raw_byte & 0x7F;
        ctx->encoded_data_pos++;

        // Ho codificato 7 byte, scrivo nel buffer d'uscita
        if ( ctx->encoded_data_pos >= ( ENCODED_DATA_FRAME_SIZE - 1 ) )
        {
            for ( uint8_t i = 0; i < ENCODED_DATA_FRAME_SIZE; i ++ )
            {
                *out = ctx->encoded_data[i];
                written++;
                out++;
            }

            ctx->encoded_data[0] = 0;
            ctx->encoded_data_pos = 0;
        }
    }

    // Se flushing, chiudo eventuale blocco incompleto
    if ( flush && ctx->encoded_data_pos > 0 )
    {
        for ( uint8_t i = 0; ( i < ctx->encoded_data_pos + 1 ) && ( i < ENCODED_DATA_FRAME_SIZE ) ; i ++ )
        {
            *out = ctx->encoded_data[i];
            written++;
            out++;
        }

        ctx->encoded_data[0] = 0;
        ctx->encoded_data_pos = 0;
    }


    return written;
}

uint32_t enc7bit_decodeStream(enc7bit_ctx_t *ctx, const uint8_t *in, uint8_t *out, uint32_t size)
{
    if ( !ctx || !in || !out || ( size == 0 ) )
    {
        return 0;
    }

    uint32_t written = 0; // accumulatore dei byte codificati scritti in out

    while ( size-- )
    {
        uint8_t encoded_byte = *in++;

        if ( ctx->encoded_data_pos == 0 )
        {
            ctx->encoded_data[ 0 ] = encoded_byte;
            ctx->encoded_data_pos = 1;
            continue;
        }

        *out = encoded_byte | ( ( ( ctx->encoded_data[0] >> ( ctx->encoded_data_pos - 1 ) ) & 1 ) << 7 );
        written++;
        out++;

        ctx->encoded_data_pos++;

        if ( ctx->encoded_data_pos >= ENCODED_DATA_FRAME_SIZE )
        {
            ctx->encoded_data_pos = 0;
        }
    }

    return written;
}

void enc7bit_ctxInit( enc7bit_ctx_t* ctx ) {

    if ( !ctx )
    {
        return;
    }

    ctx->encoded_data_pos = 0;

    for ( uint8_t i = 0; i < 8; i ++ )
    {
        ctx->encoded_data[i] = 0;
    }
}

uint32_t enc7bit_encode( const uint8_t *in, uint8_t *out, uint32_t size ) {

    if ( !in || !out || ( size == 0 ) )
    {
        return 0;
    }

    enc7bit_ctx_t ctx;
    enc7bit_ctxInit( &ctx );

    return enc7bit_encodeStream( &ctx, in, out, size, true );
}

uint32_t enc7bit_decode( const uint8_t *in, uint8_t *out, uint32_t size ) {

    if ( !in || !out || ( size == 0 ) )
    {
        return 0;
    }

    enc7bit_ctx_t ctx;
    enc7bit_ctxInit( &ctx );

    return enc7bit_decodeStream( &ctx, in, out, size );
}


/*
    uint32_t count = 0;

    while ( size > 0 )
    {
        // Processo blocchi di 7 bit alla volta
        uint32_t blockSize = ( size >= 7 ) ? 7 : size;

        // Esco in caso di calcolo errato
        if ( blockSize == 0 )
        {
            return count;
        }

        out[0] = 0;

        for ( uint32_t i = 0; i < blockSize; i++ )
        {
            out[0] |= ( (in[i] & 0x80 ) >> 7 ) << i;
            out[i+1] = in[i]&0x7F;
        }

        in += blockSize;
        out += blockSize + 1;
        size -= blockSize;
        count += blockSize + 1;
    }

    return count;
    */


/*
    uint32_t count = 0;

    while ( size > 0 )
    {
        // Processo blocchi di 8 bit alla volta
        uint32_t blockSize = ( size >= 8 ) ? 8 : size;

        // Ignoro eventuale ultimo byte anomalo
        // (Ho solo il byte di codifica, ovvero il byte
        // contenente gli MSB, ma nessun  byte
        // di informazione)
        if ( blockSize <= 1 )
        {
            return count;
        }

        for ( uint32_t i = 0; i < blockSize - 1; i++ )
        {
            out[i] = in[i+1] | (uint32_t)( ( ( in[0] >> i ) & 1 ) << 7 );
        }

        in += blockSize;
        out += blockSize - 1;
        size -= blockSize;
        count += blockSize - 1;
    }

    return count;

    */
