#include "core/url.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

char *mz_url_decode(const char *src, size_t len, bool plus_as_space) {
    if (!src) return nullptr;
    char *out = (char *)malloc(len + 1);
    if (!out) return nullptr;

    size_t o = 0;
    for (size_t i = 0; i < len; i++) {
        if (src[i] == '%' && i + 2 < len) {
            int h1 = hex_val(src[i + 1]);
            int h2 = hex_val(src[i + 2]);
            if (h1 >= 0 && h2 >= 0) {
                out[o++] = (char)((h1 << 4) | h2);
                i += 2;
                continue;
            }
        }
        if (plus_as_space && src[i] == '+') {
            out[o++] = ' ';
        } else {
            out[o++] = src[i];
        }
    }
    out[o] = '\0';
    return out;
}

char *mz_url_encode(const char *src, size_t len) {
    if (!src) return nullptr;
    // Worst case each byte becomes %XX -> 3x
    char *out = (char *)malloc(len * 3 + 1);
    if (!out) return nullptr;

    const char *hex = "0123456789ABCDEF";
    size_t o = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)src[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out[o++] = c;
        } else {
            out[o++] = '%';
            out[o++] = hex[(c >> 4) & 0xF];
            out[o++] = hex[c & 0xF];
        }
    }
    out[o] = '\0';
    return out;
}
