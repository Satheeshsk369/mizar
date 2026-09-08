#ifndef MIZAR_ALGO_STRVIEW_H
#define MIZAR_ALGO_STRVIEW_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *data;
    size_t len;
} MzStrView;

#define MZ_SV_NULL ((MzStrView){ .data = nullptr, .len = 0 })
#define MZ_SV_LIT(lit) ((MzStrView){ .data = "" lit "", .len = sizeof(lit) - 1 })

static inline MzStrView mz_sv_from_cstr(const char *str) {
    if (!str) return MZ_SV_NULL;
    size_t len = 0;
    while (str[len]) len++;
    return (MzStrView){ .data = str, .len = len };
}

static inline MzStrView mz_sv_from_parts(const char *data, size_t len) {
    return (MzStrView){ .data = data, .len = len };
}

static inline bool mz_sv_is_empty(MzStrView sv) {
    return sv.data == nullptr || sv.len == 0;
}

static inline bool mz_sv_eq(MzStrView a, MzStrView b) {
    if (a.len != b.len) return false;
    if (a.data == b.data) return true;
    if (!a.data || !b.data) return false;
    for (size_t i = 0; i < a.len; i++) {
        if (a.data[i] != b.data[i]) return false;
    }
    return true;
}

static inline bool mz_sv_eq_ci(MzStrView a, MzStrView b) {
    if (a.len != b.len) return false;
    if (a.data == b.data) return true;
    if (!a.data || !b.data) return false;
    for (size_t i = 0; i < a.len; i++) {
        char ca = a.data[i];
        char cb = b.data[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}

static inline bool mz_sv_starts_with(MzStrView sv, MzStrView prefix) {
    if (prefix.len > sv.len) return false;
    return mz_sv_eq(mz_sv_from_parts(sv.data, prefix.len), prefix);
}

static inline bool mz_sv_ends_with(MzStrView sv, MzStrView suffix) {
    if (suffix.len > sv.len) return false;
    return mz_sv_eq(mz_sv_from_parts(sv.data + (sv.len - suffix.len), suffix.len), suffix);
}

static inline MzStrView mz_sv_trim(MzStrView sv) {
    while (sv.len > 0 && ((unsigned char)sv.data[0] <= ' ')) {
        sv.data++;
        sv.len--;
    }
    while (sv.len > 0 && ((unsigned char)sv.data[sv.len - 1] <= ' ')) {
        sv.len--;
    }
    return sv;
}

static inline bool mz_sv_split_char(MzStrView sv, char delim, MzStrView *left, MzStrView *right) {
    for (size_t i = 0; i < sv.len; i++) {
        if (sv.data[i] == delim) {
            if (left) *left = mz_sv_from_parts(sv.data, i);
            if (right) *right = mz_sv_from_parts(sv.data + i + 1, sv.len - (i + 1));
            return true;
        }
    }
    if (left) *left = sv;
    if (right) *right = MZ_SV_NULL;
    return false;
}

// 64-bit SipHash-1-3 or FNV-1a for string views
static inline uint64_t mz_sv_hash(MzStrView sv) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < sv.len; i++) {
        hash ^= (uint8_t)sv.data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

static inline uint64_t mz_sv_hash_ci(MzStrView sv) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < sv.len; i++) {
        char c = sv.data[i];
        if (c >= 'A' && c <= 'Z') c += 32;
        hash ^= (uint8_t)c;
        hash *= 1099511628211ULL;
    }
    return hash;
}

#endif
