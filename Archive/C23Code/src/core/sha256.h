#ifndef MIZAR_CORE_SHA256_H
#define MIZAR_CORE_SHA256_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t buffer[64];
} MzSha256Ctx;

static inline uint32_t mz_rotr32(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

#define MZ_CH(x, y, z)  (((x) & (y)) ^ (~(x) & (z)))
#define MZ_MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define MZ_EP0(x)       (mz_rotr32(x, 2) ^ mz_rotr32(x, 13) ^ mz_rotr32(x, 22))
#define MZ_EP1(x)       (mz_rotr32(x, 6) ^ mz_rotr32(x, 11) ^ mz_rotr32(x, 25))
#define MZ_SIG0(x)      (mz_rotr32(x, 7) ^ mz_rotr32(x, 18) ^ ((x) >> 3))
#define MZ_SIG1(x)      (mz_rotr32(x, 17) ^ mz_rotr32(x, 19) ^ ((x) >> 10))

static const uint32_t mz_sha256_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static inline void mz_sha256_transform(uint32_t state[8], const uint8_t data[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
    uint32_t m[64];

    for (int i = 0; i < 16; i++) {
        m[i] = ((uint32_t)data[i * 4] << 24) |
               ((uint32_t)data[i * 4 + 1] << 16) |
               ((uint32_t)data[i * 4 + 2] << 8) |
               ((uint32_t)data[i * 4 + 3]);
    }
    for (int i = 16; i < 64; i++) {
        m[i] = MZ_SIG1(m[i - 2]) + m[i - 7] + MZ_SIG0(m[i - 15]) + m[i - 16];
    }

    for (int i = 0; i < 64; i++) {
        uint32_t t1 = h + MZ_EP1(e) + MZ_CH(e, f, g) + mz_sha256_k[i] + m[i];
        uint32_t t2 = MZ_EP0(a) + MZ_MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

static inline void mz_sha256_init(MzSha256Ctx *ctx) {
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->count = 0;
}

static inline void mz_sha256_update(MzSha256Ctx *ctx, const void *data, size_t len) {
    const uint8_t *p = (const uint8_t *)data;
    size_t left = (size_t)(ctx->count & 0x3f);
    ctx->count += len;

    if (left > 0) {
        size_t to_copy = 64 - left;
        if (len < to_copy) {
            memcpy(ctx->buffer + left, p, len);
            return;
        }
        memcpy(ctx->buffer + left, p, to_copy);
        mz_sha256_transform(ctx->state, ctx->buffer);
        p += to_copy;
        len -= to_copy;
    }

    while (len >= 64) {
        mz_sha256_transform(ctx->state, p);
        p += 64;
        len -= 64;
    }

    if (len > 0) {
        memcpy(ctx->buffer, p, len);
    }
}

static inline void mz_sha256_final(MzSha256Ctx *ctx, uint8_t digest[32]) {
    uint8_t pad[64] = { 0x80 };
    size_t left = (size_t)(ctx->count & 0x3f);
    size_t pad_len = (left < 56) ? (56 - left) : (120 - left);

    uint64_t bits = ctx->count * 8;
    uint8_t len_bytes[8];
    for (int i = 0; i < 8; i++) {
        len_bytes[i] = (uint8_t)((bits >> ((7 - i) * 8)) & 0xff);
    }

    mz_sha256_update(ctx, pad, pad_len);
    mz_sha256_update(ctx, len_bytes, 8);

    for (int i = 0; i < 8; i++) {
        digest[i * 4]     = (uint8_t)((ctx->state[i] >> 24) & 0xff);
        digest[i * 4 + 1] = (uint8_t)((ctx->state[i] >> 16) & 0xff);
        digest[i * 4 + 2] = (uint8_t)((ctx->state[i] >> 8) & 0xff);
        digest[i * 4 + 3] = (uint8_t)(ctx->state[i] & 0xff);
    }
}

// RFC 2104 HMAC-SHA256
static inline void mz_hmac_sha256(const void *key, size_t key_len,
                                  const void *data, size_t data_len,
                                  uint8_t out[32]) {
    uint8_t k_ipad[64];
    uint8_t k_opad[64];
    uint8_t tk[32];

    if (key_len > 64) {
        MzSha256Ctx tctx;
        mz_sha256_init(&tctx);
        mz_sha256_update(&tctx, key, key_len);
        mz_sha256_final(&tctx, tk);
        key = tk;
        key_len = 32;
    }

    memset(k_ipad, 0x36, sizeof(k_ipad));
    memset(k_opad, 0x5c, sizeof(k_opad));

    const uint8_t *k = (const uint8_t *)key;
    for (size_t i = 0; i < key_len; i++) {
        k_ipad[i] ^= k[i];
        k_opad[i] ^= k[i];
    }

    MzSha256Ctx ctx;
    // Inner hash: H(K ^ ipad || data)
    mz_sha256_init(&ctx);
    mz_sha256_update(&ctx, k_ipad, 64);
    mz_sha256_update(&ctx, data, data_len);
    uint8_t inner[32];
    mz_sha256_final(&ctx, inner);

    // Outer hash: H(K ^ opad || inner)
    mz_sha256_init(&ctx);
    mz_sha256_update(&ctx, k_opad, 64);
    mz_sha256_update(&ctx, inner, 32);
    mz_sha256_final(&ctx, out);
}

// Constant-time equality comparison
static inline bool mz_crypto_timing_safe_eq(const void *a, const void *b, size_t len) {
    const uint8_t *x = (const uint8_t *)a;
    const uint8_t *y = (const uint8_t *)b;
    uint8_t diff = 0;
    for (size_t i = 0; i < len; i++) {
        diff |= (x[i] ^ y[i]);
    }
    return diff == 0;
}

#endif
