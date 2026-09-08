#ifndef MIZAR_ALGO_URLCODEC_H
#define MIZAR_ALGO_URLCODEC_H

#include "algo/arena.h"
#include <stddef.h>
#include <stdbool.h>

// Decode percent-encoded string.
// If arena is provided, allocates result in arena; otherwise uses malloc.
// Handles '+' as space if plus_as_space is true.
char *mz_urlcodec_decode(const char *src, size_t len, bool plus_as_space, MzArena *arena);

// Encode string with percent-encoding.
// If arena is provided, allocates in arena; otherwise uses malloc.
char *mz_urlcodec_encode(const char *src, size_t len, MzArena *arena);

#endif
