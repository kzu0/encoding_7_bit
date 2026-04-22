# 7-bit Stream Encoding Library

A lightweight, lossless 7-bit encoding/decoding library designed for embedded systems and stream-based data processing.

It provides both streaming and buffer-based APIs to encode binary data into a compact 7-bit format and decode it back without information loss.

The library compresses binary data by splitting each byte into:

- 1 MSB bitstream byte (bitmask)

- 7-bit payload bytes

Each frame encodes:

```
OUTPUT FRAME (8 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
|  MSB   |  d0    |  d1    |  d2    |  d3    |  d4    |  d5    |  d6    |
+--------+--------+--------+--------+--------+--------+--------+--------+
```

Where:

- MSB = bitmask containing MSB bits of each data byte

- d0..d6 = original byte values with MSB cleared (7-bit payload)

## Streaming encode

```
uint32_t enc7bit_encodeStream(
    enc7bit_ctx_t* ctx,
    const uint8_t *in,
    uint8_t *out,
    uint32_t size,
    bool flush);
```

Encodes input data in chunks.

Supports partial frames across multiple calls.

flush = true → forces final incomplete frame to be written

## Streaming decode

```
uint32_t enc7bit_decodeStream(
    enc7bit_ctx_t* ctx,
    const uint8_t *in,
    uint8_t *out,
    uint32_t size);
```

Decodes streamed 7-bit encoded data.

Maintains internal state to handle chunked input safely.

## Streaming decode

```
uint32_t enc7bit_encode(const uint8_t *in, uint8_t *out, uint32_t size);
uint32_t enc7bit_decode(const uint8_t *in, uint8_t *out, uint32_t size);
```

## Example

```
uint8_t input[] = {0xA1, 0x66, 0x97, 0x0E, 0xF1, 0x55, 0x81};
uint8_t encoded[16];
uint8_t decoded[16];

enc7bit_ctx_t ctx;
enc7bit_ctxInit(&ctx);

uint32_t enc_size = enc7bit_encodeStream(&ctx, input, encoded, 7, true);

enc7bit_ctxInit(&ctx);

uint32_t dec_size = enc7bit_decodeStream(&ctx, encoded, decoded, enc_size);
```

