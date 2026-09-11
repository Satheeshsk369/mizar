#ifndef MIZAR_CORE_URL_H
#define MIZAR_CORE_URL_H

#include <stddef.h>
#include <stdbool.h>

// Decodes percent-encoded string in-place or into a newly allocated buffer
// Converts '+' to space if plus_as_space is true
char *mz_url_decode(const char *src, size_t len, bool plus_as_space);

// Encodes a string with percent-encoding, returns newly allocated string
char *mz_url_encode(const char *src, size_t len);

#endif
