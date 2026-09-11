#ifndef MIZAR_ALGO_UTF8_H
#define MIZAR_ALGO_UTF8_H

#include "algo/strview.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Validate UTF-8 byte stream. Returns true if completely valid UTF-8.
bool mz_utf8_validate(const char *s, size_t len);

// Decode next UTF-8 code point.
// Returns number of bytes consumed (1-4), or 0 on invalid sequence.
size_t mz_utf8_decode(const char *s, size_t len, uint32_t *out_codepoint);

// Encode a Unicode code point to UTF-8.
// Returns number of bytes written (1-4), or 0 on invalid code point.
size_t mz_utf8_encode(uint32_t codepoint, char out[4]);

// Count number of Unicode code points (characters) in a UTF-8 string
size_t mz_utf8_strlen(const char *s, size_t len);

#endif
