#ifndef ENCODING_7_BIT_TEST_H
#define ENCODING_7_BIT_TEST_H

#include "encoding_7_bit.h"

#include <random>

#define DATA_LENGTH 512

static bool enc7bit_sizesTest() {

    for (uint32_t i = 0; i <= DATA_LENGTH; i ++ )
    {
        if (enc7bit_decodeLength(enc7bit_encodeLength(i)) != i)
        {
            return false;
        }
    }

    return true;
}

static bool enc7bit_streamTest() {

    enc7bit_ctx_t ctx;

    uint8_t data[DATA_LENGTH];
    uint8_t encoded[DATA_LENGTH*2];
    uint8_t decoded[DATA_LENGTH];

    uint32_t data_length = DATA_LENGTH;

    uint32_t encoded_length = 0;
    uint32_t decoded_length = 0;

    for (int i = 0; i < DATA_LENGTH; i ++)
    {
        data[i] = rand()&0xFF;
    }

    enc7bit_ctxInit(&ctx);

    uint32_t c = 0;
    while ( data_length ) {

        uint32_t blockSize = ( data_length >= 17 ) ? 17 : data_length;
        bool flush = ( data_length >= 17 ) ? false : true;
        encoded_length += enc7bit_encodeStream(&ctx, data+c, encoded+encoded_length, blockSize, flush);

        c += blockSize;
        data_length -= blockSize;
    }

    c = 0;
    while ( encoded_length ) {

        uint32_t blockSize = ( data_length >= 13 ) ? 13 : encoded_length;
        decoded_length += enc7bit_decodeStream(&ctx, encoded+c, decoded+decoded_length, blockSize);

        c += blockSize;
        encoded_length -= blockSize;
    }

    if (memcmp(data, decoded, DATA_LENGTH))
    {
        return false;
    }

    return true;
}

static bool enc7bit_blockTest() {

    uint8_t data[DATA_LENGTH];
    uint8_t encoded[DATA_LENGTH*2];
    uint8_t decoded[DATA_LENGTH];

    uint32_t encoded_length = 0;
    uint32_t decoded_length = 0;

    for (int i = 0; i < DATA_LENGTH; i ++)
    {
        data[i] = rand()&0xFF;
    }

    // Codifica
    encoded_length = enc7bit_encode(data, encoded, DATA_LENGTH);

    // Decodifica
    decoded_length = enc7bit_decode(encoded, decoded, encoded_length);

    if (memcmp(data, decoded, DATA_LENGTH))
    {
        return false;
    }

    (void)decoded_length;

    return true;
}




#endif // ENCODING_7_BIT_TEST_H
