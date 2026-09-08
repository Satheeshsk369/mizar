#include "algo/lru.h"
#include "algo/siphash.h"
#include <stdlib.h>
#include <string.h>

typedef struct MzLruEntry MzLruEntry;

struct MzLruEntry {
    char *key;
    void *val;
    MzLruEntry *hash_next;
    MzLruNode list_node; // doubly-linked list node
};

static inline size_t lru_hash_index(const char *key, size_t bucket_count) {
    uint64_t h = mz_siphash13(key, strlen(key), nullptr);
    return (size_t)(h % bucket_count);
}

bool mz_lru_init(MzLruCache *cache, size_t max_capacity, MzLruFreeValFn free_fn) {
    if (!cache || max_capacity == 0) return false;
    size_t buckets = max_capacity * 2;
    if (buckets < 16) buckets = 16;
    cache->buckets = (MzLruNode **)calloc(buckets, sizeof(MzLruNode *));
    if (!cache->buckets) return false;

    cache->bucket_count = buckets;
    cache->head = nullptr;
    cache->tail = nullptr;
    cache->count = 0;
    cache->max_capacity = max_capacity;
    cache->free_fn = free_fn;
    return true;
}

static void lru_detach(MzLruCache *cache, MzLruNode *node) {
    if (node->prev) node->prev->next = node->next;
    else cache->head = node->next;

    if (node->next) node->next->prev = node->prev;
    else cache->tail = node->prev;

    node->prev = nullptr;
    node->next = nullptr;
}

static void lru_attach_head(MzLruCache *cache, MzLruNode *node) {
    node->next = cache->head;
    node->prev = nullptr;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
}

void *mz_lru_get(MzLruCache *cache, const char *key) {
    if (!cache || !key || cache->count == 0) return nullptr;
    size_t idx = lru_hash_index(key, cache->bucket_count);
    MzLruNode *curr = cache->buckets[idx];

    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            // Promote to head (MRU)
            if (cache->head != curr) {
                lru_detach(cache, curr);
                lru_attach_head(cache, curr);
            }
            return curr->val;
        }
        curr = curr->next_hash;
    }
    return nullptr;
}

bool mz_lru_put(MzLruCache *cache, const char *key, void *val) {
    if (!cache || !key) return false;
    size_t idx = lru_hash_index(key, cache->bucket_count);
    MzLruNode *curr = cache->buckets[idx];

    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (cache->free_fn && curr->val != val) {
                cache->free_fn(curr->val);
            }
            curr->val = val;
            if (cache->head != curr) {
                lru_detach(cache, curr);
                lru_attach_head(cache, curr);
            }
            return true;
        }
        curr = curr->next_hash;
    }

    // If at capacity, evict tail (LRU)
    if (cache->count >= cache->max_capacity && cache->tail) {
        MzLruNode *evict = cache->tail;
        size_t evict_idx = lru_hash_index(evict->key, cache->bucket_count);

        // Remove from hash chain
        MzLruNode **b = &cache->buckets[evict_idx];
        while (*b && *b != evict) b = &(*b)->next_hash;
        if (*b) *b = evict->next_hash;

        lru_detach(cache, evict);

        if (cache->free_fn) cache->free_fn(evict->val);
        free(evict->key);
        free(evict);
        cache->count--;
    }

    // Insert new node
    MzLruNode *node = (MzLruNode *)malloc(sizeof(MzLruNode));
    if (!node) return false;
    node->key = strdup(key);
    node->val = val;
    node->prev = nullptr;
    node->next = nullptr;
    node->next_hash = cache->buckets[idx];
    cache->buckets[idx] = node;

    lru_attach_head(cache, node);
    cache->count++;
    return true;
}

void mz_lru_free(MzLruCache *cache) {
    if (!cache) return;
    MzLruNode *curr = cache->head;
    while (curr) {
        MzLruNode *next = curr->next;
        if (cache->free_fn) cache->free_fn(curr->val);
        free(curr->key);
        free(curr);
        curr = next;
    }
    free(cache->buckets);
    cache->buckets = nullptr;
    cache->head = nullptr;
    cache->tail = nullptr;
    cache->count = 0;
    cache->max_capacity = 0;
}
