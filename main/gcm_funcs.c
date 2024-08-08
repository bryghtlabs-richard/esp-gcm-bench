/*
 * Author: Richard Allen @ Bryght Labs
 *
 * Created in 2024
 */

#ifdef __GNUC__
#pragma GCC optimize "O2"
#endif

#include "gcm_funcs.h"
#include "esp_attr.h"

static inline void mbedtls_xor_no_simd(unsigned char *r,
                                          const unsigned char *a,
                                          const unsigned char *b,
                                          size_t n)
   {
       size_t i = 0;
       for (; i < n; i++) {
           r[i] = a[i] ^ b[i];
       }
  }


#ifndef MBEDTLS_PUT_UINT32_BE
#define MBEDTLS_PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

DRAM_ATTR const uint16_t last4[16] =
{
    0x0000, 0x1c20, 0x3840, 0x2460,
    0x7080, 0x6ca0, 0x48c0, 0x54e0,
    0xe100, 0xfd20, 0xd940, 0xc560,
    0x9180, 0x8da0, 0xa9c0, 0xb5e0
};

DRAM_ATTR const uint32_t last4_preshift[16] = {
    0x00000000, 0x1c200000, 0x38400000, 0x24600000,
    0x70800000, 0x6ca00000, 0x48c00000, 0x54e00000,
    0xe1000000, 0xfd200000, 0xd9400000, 0xc5600000,
    0x91800000, 0x8da00000, 0xa9c00000, 0xb5e00000
};

DRAM_ATTR const uint16_t last8[256] = {
    0x0000, 0xc201, 0x8403, 0x4602, 0x0807, 0xca06, 0x8c04, 0x4e05,
    0x100e, 0xd20f, 0x940d, 0x560c, 0x1809, 0xda08, 0x9c0a, 0x5e0b,
    0x201c, 0xe21d, 0xa41f, 0x661e, 0x281b, 0xea1a, 0xac18, 0x6e19,
    0x3012, 0xf213, 0xb411, 0x7610, 0x3815, 0xfa14, 0xbc16, 0x7e17,
    0x4038, 0x8239, 0xc43b, 0x063a, 0x483f, 0x8a3e, 0xcc3c, 0x0e3d,
    0x5036, 0x9237, 0xd435, 0x1634, 0x5831, 0x9a30, 0xdc32, 0x1e33,
    0x6024, 0xa225, 0xe427, 0x2626, 0x6823, 0xaa22, 0xec20, 0x2e21,
    0x702a, 0xb22b, 0xf429, 0x3628, 0x782d, 0xba2c, 0xfc2e, 0x3e2f,
    0x8070, 0x4271, 0x0473, 0xc672, 0x8877, 0x4a76, 0x0c74, 0xce75,
    0x907e, 0x527f, 0x147d, 0xd67c, 0x9879, 0x5a78, 0x1c7a, 0xde7b,
    0xa06c, 0x626d, 0x246f, 0xe66e, 0xa86b, 0x6a6a, 0x2c68, 0xee69,
    0xb062, 0x7263, 0x3461, 0xf660, 0xb865, 0x7a64, 0x3c66, 0xfe67,
    0xc048, 0x0249, 0x444b, 0x864a, 0xc84f, 0x0a4e, 0x4c4c, 0x8e4d,
    0xd046, 0x1247, 0x5445, 0x9644, 0xd841, 0x1a40, 0x5c42, 0x9e43,
    0xe054, 0x2255, 0x6457, 0xa656, 0xe853, 0x2a52, 0x6c50, 0xae51,
    0xf05a, 0x325b, 0x7459, 0xb658, 0xf85d, 0x3a5c, 0x7c5e, 0xbe5f,
    0x00e1, 0xc2e0, 0x84e2, 0x46e3, 0x08e6, 0xcae7, 0x8ce5, 0x4ee4,
    0x10ef, 0xd2ee, 0x94ec, 0x56ed, 0x18e8, 0xdae9, 0x9ceb, 0x5eea,
    0x20fd, 0xe2fc, 0xa4fe, 0x66ff, 0x28fa, 0xeafb, 0xacf9, 0x6ef8,
    0x30f3, 0xf2f2, 0xb4f0, 0x76f1, 0x38f4, 0xfaf5, 0xbcf7, 0x7ef6,
    0x40d9, 0x82d8, 0xc4da, 0x06db, 0x48de, 0x8adf, 0xccdd, 0x0edc,
    0x50d7, 0x92d6, 0xd4d4, 0x16d5, 0x58d0, 0x9ad1, 0xdcd3, 0x1ed2,
    0x60c5, 0xa2c4, 0xe4c6, 0x26c7, 0x68c2, 0xaac3, 0xecc1, 0x2ec0,
    0x70cb, 0xb2ca, 0xf4c8, 0x36c9, 0x78cc, 0xbacd, 0xfccf, 0x3ece,
    0x8091, 0x4290, 0x0492, 0xc693, 0x8896, 0x4a97, 0x0c95, 0xce94,
    0x909f, 0x529e, 0x149c, 0xd69d, 0x9898, 0x5a99, 0x1c9b, 0xde9a,
    0xa08d, 0x628c, 0x248e, 0xe68f, 0xa88a, 0x6a8b, 0x2c89, 0xee88,
    0xb083, 0x7282, 0x3480, 0xf681, 0xb884, 0x7a85, 0x3c87, 0xfe86,
    0xc0a9, 0x02a8, 0x44aa, 0x86ab, 0xc8ae, 0x0aaf, 0x4cad, 0x8eac,
    0xd0a7, 0x12a6, 0x54a4, 0x96a5, 0xd8a0, 0x1aa1, 0x5ca3, 0x9ea2,
    0xe0b5, 0x22b4, 0x64b6, 0xa6b7, 0xe8b2, 0x2ab3, 0x6cb1, 0xaeb0,
    0xf0bb, 0x32ba, 0x74b8, 0xb6b9, 0xf8bc, 0x3abd, 0x7cbf, 0xbebe
};


/*
 * Sets output to x times H using the precomputed tables.
 * x and output are seen as elements of GF(2^128) as in [MGV].
 */
IRAM_ATTR void gcm_mult_old( esp_gcm_context *ctx, const unsigned char x[16],
                      unsigned char output[16] )
{
    int i = 0;
    unsigned char lo, hi, rem;
    uint64_t zh, zl;

    lo = x[15] & 0xf;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    for ( i = 15; i >= 0; i-- ) {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        if ( i != 15 ) {
            rem = (unsigned char) zl & 0xf;
            zl = ( zh << 60 ) | ( zl >> 4 );
            zh = ( zh >> 4 );
            zh ^= (uint64_t) last4[rem] << 48;
            zh ^= ctx->HH[lo];
            zl ^= ctx->HL[lo];

        }

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4[rem] << 48;
        zh ^= ctx->HH[hi];
        zl ^= ctx->HL[hi];
    }
    MBEDTLS_PUT_UINT32_BE( zh >> 32, output, 0 );
    MBEDTLS_PUT_UINT32_BE( zh, output, 4 );
    MBEDTLS_PUT_UINT32_BE( zl >> 32, output, 8 );
    MBEDTLS_PUT_UINT32_BE( zl, output, 12 );
}

IRAM_ATTR void gcm_mult_largetable(uint8_t *output, const uint8_t *x, uint64_t H[256][2])
{
    int i;
    uint64_t u64z[2];
    uint16_t *u16z = (uint16_t *) u64z;
    uint8_t *u8z = (uint8_t *) u64z;
    uint8_t rem;

    u64z[0] = 0;
    u64z[1] = 0;

    for (i = 15; i > 0; i--) {
        mbedtls_xor_no_simd(u8z, u8z, (uint8_t *) H[x[i]], 16);

        rem = u8z[15];

        u64z[1] <<= 8;
        u8z[8] = u8z[7];
        u64z[0] <<= 8;

        u8z[0] = 0;
        u16z[0] ^= last8[rem];
    }

    mbedtls_xor_no_simd(output, u8z, (uint8_t *) H[x[0]], 16);
}

IRAM_ATTR void gcm_mult_smalltable(uint8_t *output, const uint8_t *x, uint64_t H[16][2])
{
    int i = 0;
    unsigned char lo, hi, rem;
    uint64_t u64z[2];
    const uint64_t *pu64z = 0;
    uint8_t *u8z = (uint8_t *) u64z;

    lo = x[15] & 0xf;
    hi = (x[15] >> 4) & 0xf;

    pu64z = H[lo];

    rem = (unsigned char) pu64z[1] & 0xf;
    u64z[1] = (pu64z[0] << 60) | (pu64z[1] >> 4);
    u64z[0] = (pu64z[0] >> 4);
    u64z[0] ^= (uint64_t) last4[rem] << 48;
    mbedtls_xor_no_simd(u8z, u8z, (uint8_t *) H[hi], 16);

    for (i = 14; i >= 0; i--) {
        lo = x[i] & 0xf;
        hi = (x[i] >> 4) & 0xf;

        rem = (unsigned char) u64z[1] & 0xf;
        u64z[1] = (u64z[0] << 60) | (u64z[1] >> 4);
        u64z[0] = (u64z[0] >> 4);
        u64z[0] ^= (uint64_t) last4[rem] << 48;
        mbedtls_xor_no_simd(u8z, u8z, (uint8_t *) H[lo], 16);

        rem = (unsigned char) u64z[1] & 0xf;
        u64z[1] = (u64z[0] << 60) | (u64z[1] >> 4);
        u64z[0] = (u64z[0] >> 4);
        u64z[0] ^= (uint64_t) last4[rem] << 48;
        mbedtls_xor_no_simd(u8z, u8z, (uint8_t *) H[hi], 16);
    }

    MBEDTLS_PUT_UINT32_BE(u64z[0] >> 32, output, 0);
    MBEDTLS_PUT_UINT32_BE(u64z[0], output, 4);
    MBEDTLS_PUT_UINT32_BE(u64z[1] >> 32, output, 8);
    MBEDTLS_PUT_UINT32_BE(u64z[1], output, 12);
}


/* Based on MbedTLS's implementation
 *
 * Sets output to x times H using the precomputed tables.
 * x and output are seen as elements of GF(2^128) as in [MGV].
 */
IRAM_ATTR void gcm_mult_richard( esp_gcm_context *ctx, const unsigned char x[16],
                      unsigned char output[16] )
{
    int i = 0;
    unsigned char lo, hi, rem;
    uint64_t zh, zl;

    lo = x[15] & 0xf;
    hi = x[15] >> 4;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    rem = (unsigned char) zl & 0xf;
    zl = ( zh << 60 ) | ( zl >> 4 );
    zh = ( zh >> 4 );
    zh ^= (uint64_t) last4_preshift[rem] << 32;
    zh ^= ctx->HH[hi];
    zl ^= ctx->HL[hi];

    for ( i = 14; i >= 0; i-- ) {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4_preshift[rem] << 32;
        zh ^= ctx->HH[lo];
        zl ^= ctx->HL[lo];

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4_preshift[rem] << 32;
        zh ^= ctx->HH[hi];
        zl ^= ctx->HL[hi];
    }

    MBEDTLS_PUT_UINT32_BE( zh >> 32, output, 0 );
    MBEDTLS_PUT_UINT32_BE( zh, output, 4 );
    MBEDTLS_PUT_UINT32_BE( zl >> 32, output, 8 );
    MBEDTLS_PUT_UINT32_BE( zl, output, 12 );
}
