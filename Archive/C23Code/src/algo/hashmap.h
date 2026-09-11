#ifndef MIZAR_ALGO_HASHMAP_H
#define MIZAR_ALGO_HASHMAP_H

#include "algo/strview.h"
#include "algo/arena.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    MzStrView key;
    void *val;
    uint64_t hash;
    bool occupied;
    bool deleted;
} MzHashMapEntry;

typedef struct {
    MzHashMapEntry *entries;
    size_t capacity;
    size_t count;
    bool case_insensitive;
    MzArena *arena; // If non-null, bucket arrays and keys are allocated from arena
} MzHashMap;

// Initialize standard malloc-backed hash map with initial capacity
void mz_map_init(MzHashMap *map, size_t initial_cap, bool case_insensitive);

// Initialize arena-backed hash map (table and keys allocated from arena, 0 extra heap frees needed)
void mz_map_init_arena(MzHashMap *map, size_t initial_cap, bool case_insensitive, MzArena *arena);

// Put key-value pair. Returns true on success, false on OOM
bool mz_map_set(MzHashMap *map, MzStrView key, void *val);

// Put with C-string key convenience
bool mz_map_set_cstr(MzHashMap *map, const char *key, void *val);

// Get value for key (returns nullptr if not found)
void *mz_map_get(const MzHashMap *map, MzStrView key);

// Get with C-string key convenience
void *mz_map_get_cstr(const MzHashMap *map, const char *key);

// Delete key from map. Returns true if key was present
bool mz_map_remove(MzHashMap *map, MzStrView key);

// Clear all entries
void mz_map_clear(MzHashMap *map);

// Free underlying table (no-op if arena-backed)
void mz_map_free(MzHashMap *map);

#endif
