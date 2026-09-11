#include "algo/urlcodec.h"
#include <stdlib.h>
#include <string.h>

static const int8_t s_hex_digits[256] = {
    ['0'] = 0, ['1'] = 1, ['2'] = 2, ['3'] = 3, ['4'] = 4,
    ['5'] = 5, ['6'] = 6, ['7'] = 7, ['8'] = 8, ['9'] = 9,
    ['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15,
    ['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15,
    [0 ... '0' - 1] = -1,
    ['9' + 1 ... 'A' - 1] = -1,
    ['F' + 1 ... 'a' - 1] = -1,
    ['f' + 1 ... 255] = -1
};

static const char s_hex_chars[] = "0123456789ABCDEF";

static inline bool mz_is_unreserved(unsigned char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           c == '-' || c == '_' || c == '.' || c == '~';
}

char *mz_urlcodec_decode(const char *src, size_t len, bool plus_as_space, MzArena *arena) {
    if (!src) return nullptr;
    char *out = arena
        ? (char *)mz_arena_alloc(arena, len + 1)
        : (char *)malloc(len + 1);
    if (!out) return nullptr;

    size_t r = 0, w = 0;
    while (r < len) {
        if (src[r] == '%' && r + 2 < len) {
            int8_t h1 = s_hex_digits[(unsigned char)src[r + 1]];
            int8_t h2 = s_hex_digits[(unsigned char)src[r + 2]];
            if (h1 >= 0 && h2 >= 0) {
                out[w++] = (char)((h1 << 4) | h2);
                r += 3;
                continue;
            }
        }
        if (plus_as_space && src[r] == '+') {
            out[w++] = ' ';
        } else {
            out[w++] = src[r];
        }
        r++;
    }
    out[w] = '\0';
    return out;
}

char *mz_urlcodec_encode(const char *src, size_t len, MzArena *arena) {
    if (!src) return nullptr;
    size_t max_len = len * 3 + 1;
    char *out = arena
        ? (char *)mz_arena_alloc(arena, max_len)
        : (char *)malloc(max_len);
    if (!out) return nullptr;

    size_t w = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)src[i];
        if (mz_is_unreserved(c)) {
            out[w++] = (char)c;
        } else {
            out[w++] = '%';
            out[w++] = s_hex_chars[(c >> 4) & 0xF];
            out[w++] = s_hex_chars[c & 0xF];
        }
    }
    out[w] = '\0';
    return out;
}
