#ifndef MIZAR_ALGO_SIPHASH_H
#define MIZAR_ALGO_SIPHASH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// 128-bit key for SipHash
typedef struct {
    uint64_t k0;
    uint64_t k1;
} MzSipKey;

// Initialize or get the global per-process random key (prevents Hash DoS)
const MzSipKey *mz_siphash_get_global_key(void);

// Compute 64-bit SipHash-1-3 (fast, collision-resistant hash)
uint64_t mz_siphash13(const void *data, size_t len, const MzSipKey *key);

// Compute 64-bit SipHash-1-3 case-insensitively for ASCII strings (HTTP headers)
uint64_t mz_siphash13_ci(const void *data, size_t len, const MzSipKey *key);

#endif
