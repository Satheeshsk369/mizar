#ifndef MIZAR_ALGO_VEC_H
#define MIZAR_ALGO_VEC_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    void **items;
    size_t count;
    size_t capacity;
} MzVec;

static inline void mz_vec_init(MzVec *v, size_t initial_cap) {
    if (!v) return;
    if (initial_cap < 8) initial_cap = 8;
    v->items = (void **)malloc(initial_cap * sizeof(void *));
    v->count = 0;
    v->capacity = v->items ? initial_cap : 0;
}

static inline bool mz_vec_push(MzVec *v, void *item) {
    if (!v) return false;
    if (v->count >= v->capacity) {
        size_t new_cap = v->capacity ? v->capacity * 2 : 8;
        void **new_items = (void **)realloc(v->items, new_cap * sizeof(void *));
        if (!new_items) return false;
        v->items = new_items;
        v->capacity = new_cap;
    }
    v->items[v->count++] = item;
    return true;
}

static inline void *mz_vec_get(const MzVec *v, size_t index) {
    if (!v || index >= v->count) return nullptr;
    return v->items[index];
}

static inline void *mz_vec_pop(MzVec *v) {
    if (!v || v->count == 0) return nullptr;
    return v->items[--v->count];
}

static inline void mz_vec_clear(MzVec *v) {
    if (!v) return;
    v->count = 0;
}

static inline void mz_vec_free(MzVec *v) {
    if (!v) return;
    free(v->items);
    v->items = nullptr;
    v->count = 0;
    v->capacity = 0;
}

#endif
