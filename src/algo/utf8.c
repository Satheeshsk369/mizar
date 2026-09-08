#include "algo/utf8.h"

// State table for Bjoern Hoehrmann's DFA UTF-8 decoder
#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const uint8_t utf8d[] = {
    // The first part maps bytes to character classes
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

    // The second part transitions between states
     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12,
};

static inline uint32_t decode_step(uint32_t *state, uint32_t *codep, uint8_t byte) {
    uint32_t type = utf8d[byte];
    *codep = (*state != UTF8_ACCEPT) ?
             (byte & 0x3fu) | (*codep << 6) :
             (0xff >> type) & (byte);
    *state = utf8d[256 + *state + type];
    return *state;
}

bool mz_utf8_validate(const char *s, size_t len) {
    if (!s) return false;
    uint32_t state = UTF8_ACCEPT;
    uint32_t codep = 0;
    const uint8_t *p = (const uint8_t *)s;

    for (size_t i = 0; i < len; i++) {
        if (decode_step(&state, &codep, p[i]) == UTF8_REJECT) {
            return false;
        }
    }
    return state == UTF8_ACCEPT;
}

size_t mz_utf8_decode(const char *s, size_t len, uint32_t *out_codepoint) {
    if (!s || len == 0) return 0;
    uint32_t state = UTF8_ACCEPT;
    uint32_t codep = 0;
    const uint8_t *p = (const uint8_t *)s;

    for (size_t i = 0; i < len && i < 4; i++) {
        decode_step(&state, &codep, p[i]);
        if (state == UTF8_ACCEPT) {
            if (out_codepoint) *out_codepoint = codep;
            return i + 1;
        }
        if (state == UTF8_REJECT) {
            return 0;
        }
    }
    return 0;
}

size_t mz_utf8_encode(uint32_t cp, char out[4]) {
    if (!out) return 0;
    if (cp <= 0x7F) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp <= 0x7FF) {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp >= 0xD800 && cp <= 0xDFFF) {
        return 0; // Surrogate pairs are invalid in UTF-8
    }
    if (cp <= 0xFFFF) {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    if (cp <= 0x10FFFF) {
        out[0] = (char)(0xF0 | (cp >> 18));
        out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[3] = (char)(0x80 | (cp & 0x3F));
        return 4;
    }
    return 0;
}

size_t mz_utf8_strlen(const char *s, size_t len) {
    if (!s) return 0;
    size_t count = 0;
    size_t offset = 0;
    while (offset < len) {
        uint32_t cp = 0;
        size_t bytes = mz_utf8_decode(s + offset, len - offset, &cp);
        if (bytes == 0) {
            offset++; // skip malformed byte
        } else {
            offset += bytes;
        }
        count++;
    }
    return count;
}
