#include "algo/siphash.h"
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define ROTL64(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define SIPROUND do { \
    v0 += v1; v1 = ROTL64(v1, 13); v1 ^= v0; v0 = ROTL64(v0, 32); \
    v2 += v3; v3 = ROTL64(v3, 16); v3 ^= v2; \
    v0 += v3; v3 = ROTL64(v3, 21); v3 ^= v0; \
    v2 += v1; v1 = ROTL64(v1, 17); v1 ^= v2; v2 = ROTL64(v2, 32); \
} while (0)

static MzSipKey s_global_sip_key;
static bool s_sip_key_initialized = false;

const MzSipKey *mz_siphash_get_global_key(void) {
    if (!s_sip_key_initialized) {
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd >= 0) {
            ssize_t rd = read(fd, &s_global_sip_key, sizeof(s_global_sip_key));
            close(fd);
            if (rd == sizeof(s_global_sip_key)) {
                s_sip_key_initialized = true;
                return &s_global_sip_key;
            }
        }
        // Fallback pseudorandom initialization
        uint64_t t = (uint64_t)time(nullptr);
        s_global_sip_key.k0 = 0x0706050403020100ULL ^ t;
        s_global_sip_key.k1 = 0x0f0e0d0c0b0a0908ULL ^ (t << 32);
        s_sip_key_initialized = true;
    }
    return &s_global_sip_key;
}

static inline uint64_t read_u64_le(const uint8_t *p) {
    return ((uint64_t)p[0]) |
           (((uint64_t)p[1]) << 8) |
           (((uint64_t)p[2]) << 16) |
           (((uint64_t)p[3]) << 24) |
           (((uint64_t)p[4]) << 32) |
           (((uint64_t)p[5]) << 40) |
           (((uint64_t)p[6]) << 48) |
           (((uint64_t)p[7]) << 56);
}

static inline uint64_t read_u64_le_ci(const uint8_t *p) {
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t c = p[i];
        if (c >= 'A' && c <= 'Z') c += 32;
        v |= (((uint64_t)c) << (i * 8));
    }
    return v;
}

uint64_t mz_siphash13(const void *data, size_t len, const MzSipKey *key) {
    if (!key) key = mz_siphash_get_global_key();

    uint64_t v0 = 0x736f6d6570736575ULL ^ key->k0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ key->k1;
    uint64_t v2 = 0x6c65747371756174ULL ^ key->k0;
    uint64_t v3 = 0x7465646279746573ULL ^ key->k1;

    const uint8_t *p = (const uint8_t *)data;
    size_t left = len;

    while (left >= 8) {
        uint64_t m = read_u64_le(p);
        v3 ^= m;
        SIPROUND;
        v0 ^= m;
        p += 8;
        left -= 8;
    }

    uint64_t b = ((uint64_t)len) << 56;
    switch (left) {
        case 7: b |= ((uint64_t)p[6]) << 48; [[fallthrough]];
        case 6: b |= ((uint64_t)p[5]) << 40; [[fallthrough]];
        case 5: b |= ((uint64_t)p[4]) << 32; [[fallthrough]];
        case 4: b |= ((uint64_t)p[3]) << 24; [[fallthrough]];
        case 3: b |= ((uint64_t)p[2]) << 16; [[fallthrough]];
        case 2: b |= ((uint64_t)p[1]) << 8;  [[fallthrough]];
        case 1: b |= ((uint64_t)p[0]); break;
        case 0: break;
    }

    v3 ^= b;
    SIPROUND;
    v0 ^= b;

    v2 ^= 0xff;
    SIPROUND;
    SIPROUND;
    SIPROUND;

    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t mz_siphash13_ci(const void *data, size_t len, const MzSipKey *key) {
    if (!key) key = mz_siphash_get_global_key();

    uint64_t v0 = 0x736f6d6570736575ULL ^ key->k0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ key->k1;
    uint64_t v2 = 0x6c65747371756174ULL ^ key->k0;
    uint64_t v3 = 0x7465646279746573ULL ^ key->k1;

    const uint8_t *p = (const uint8_t *)data;
    size_t left = len;

    while (left >= 8) {
        uint64_t m = read_u64_le_ci(p);
        v3 ^= m;
        SIPROUND;
        v0 ^= m;
        p += 8;
        left -= 8;
    }

    uint64_t b = ((uint64_t)len) << 56;
    for (size_t i = 0; i < left; i++) {
        uint8_t c = p[i];
        if (c >= 'A' && c <= 'Z') c += 32;
        b |= ((uint64_t)c) << (i * 8);
    }

    v3 ^= b;
    SIPROUND;
    v0 ^= b;

    v2 ^= 0xff;
    SIPROUND;
    SIPROUND;
    SIPROUND;

    return v0 ^ v1 ^ v2 ^ v3;
}
