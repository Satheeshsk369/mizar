#ifndef MIZAR_ALGO_LRU_H
#define MIZAR_ALGO_LRU_H

#include "algo/strview.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct MzLruNode MzLruNode;

struct MzLruNode {
    char *key;
    void *val;
    MzLruNode *prev;
    MzLruNode *next;
    MzLruNode *next_hash;
};

typedef void (*MzLruFreeValFn)(void *val);

typedef struct {
    MzLruNode **buckets;
    size_t bucket_count;
    MzLruNode *head; // Most recently used
    MzLruNode *tail; // Least recently used
    size_t count;
    size_t max_capacity;
    MzLruFreeValFn free_fn;
} MzLruCache;

// Initialize LRU cache with maximum entry capacity
bool mz_lru_init(MzLruCache *cache, size_t max_capacity, MzLruFreeValFn free_fn);

// Retrieve item from cache and promote to MRU
void *mz_lru_get(MzLruCache *cache, const char *key);

// Insert or update item in cache, evicting LRU item if capacity is reached
bool mz_lru_put(MzLruCache *cache, const char *key, void *val);

// Free all cache nodes and buckets
void mz_lru_free(MzLruCache *cache);

#endif
