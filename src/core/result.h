#ifndef MIZAR_CORE_RESULT_H
#define MIZAR_CORE_RESULT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Type-Safe String Slices (Non-allocating, bounds-safe string view)
// -----------------------------------------------------------------------------
typedef struct {
    const char *data;
    size_t len;
} MzSlice;

#define MZ_SLICE_LIT(s) ((MzSlice){ .data = (s), .len = sizeof(s) - 1 })
#define MZ_SLICE_NULL   ((MzSlice){ .data = nullptr, .len = 0 })

static inline MzSlice mz_slice_from_cstr(const char *str) {
    if (!str) return MZ_SLICE_NULL;
    return (MzSlice){ .data = str, .len = strlen(str) };
}

static inline bool mz_slice_is_empty(MzSlice s) {
    return s.data == nullptr || s.len == 0;
}

static inline bool mz_slice_eq(MzSlice a, const char *literal) {
    if (!a.data || !literal) return false;
    size_t lit_len = strlen(literal);
    return (a.len == lit_len) && (memcmp(a.data, literal, lit_len) == 0);
}

static inline bool mz_slice_eq_slice(MzSlice a, MzSlice b) {
    if (a.len != b.len) return false;
    if (a.len == 0) return true;
    if (!a.data || !b.data) return false;
    return memcmp(a.data, b.data, a.len) == 0;
}

// -----------------------------------------------------------------------------
// Tagged Result Types (Eliminating null-pointer exceptions & silent errors)
// -----------------------------------------------------------------------------

typedef struct {
    bool ok;
    int64_t val;
} MzIntResult;

typedef struct {
    bool ok;
    double val;
} MzFloatResult;

typedef struct {
    bool ok;
    MzSlice val;
} MzSliceResult;

typedef struct {
    bool ok;
    const char *val;
} MzStrResult;

#endif // MIZAR_CORE_RESULT_H
