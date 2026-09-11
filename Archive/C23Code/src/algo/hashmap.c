#include "algo/hashmap.h"
#include "algo/siphash.h"
#include <stdlib.h>
#include <string.h>

#define MZ_MAP_MAX_LOAD_NUM 7
#define MZ_MAP_MAX_LOAD_DEN 10

static inline size_t mz_next_pow2(size_t n) {
    if (n < 16) return 16;
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

void mz_map_init(MzHashMap *map, size_t initial_cap, bool case_insensitive) {
    if (!map) return;
    map->arena = nullptr;
    map->entries = nullptr;
    map->capacity = 0;
    map->count = 0;
    map->case_insensitive = case_insensitive;
    if (initial_cap > 0) {
        size_t cap = mz_next_pow2(initial_cap);
        map->entries = (MzHashMapEntry *)calloc(cap, sizeof(MzHashMapEntry));
        map->capacity = map->entries ? cap : 0;
    }
}

void mz_map_init_arena(MzHashMap *map, size_t initial_cap, bool case_insensitive, MzArena *arena) {
    if (!map) return;
    map->arena = arena;
    map->entries = nullptr;
    map->capacity = 0;
    map->count = 0;
    map->case_insensitive = case_insensitive;
    if (initial_cap > 0 && arena) {
        size_t cap = mz_next_pow2(initial_cap);
        map->entries = (MzHashMapEntry *)mz_arena_calloc(arena, cap, sizeof(MzHashMapEntry));
        map->capacity = map->entries ? cap : 0;
    }
}

static inline uint64_t mz_map_calc_hash(const MzHashMap *map, MzStrView key) {
    return map->case_insensitive
        ? mz_siphash13_ci(key.data, key.len, nullptr)
        : mz_siphash13(key.data, key.len, nullptr);
}

static inline bool mz_map_keys_match(const MzHashMap *map, MzStrView a, MzStrView b) {
    return map->case_insensitive ? mz_sv_eq_ci(a, b) : mz_sv_eq(a, b);
}

static bool mz_map_resize(MzHashMap *map, size_t new_cap) {
    MzHashMapEntry *new_entries = map->arena
        ? (MzHashMapEntry *)mz_arena_calloc(map->arena, new_cap, sizeof(MzHashMapEntry))
        : (MzHashMapEntry *)calloc(new_cap, sizeof(MzHashMapEntry));
    if (!new_entries) return false;

    if (map->capacity > 0 && map->entries) {
        size_t mask = new_cap - 1;
        for (size_t i = 0; i < map->capacity; i++) {
            MzHashMapEntry *old = &map->entries[i];
            if (old->occupied && !old->deleted) {
                size_t idx = (size_t)(old->hash & mask);
                while (new_entries[idx].occupied) {
                    idx = (idx + 1) & mask;
                }
                new_entries[idx] = *old;
            }
        }
    }

    if (!map->arena) {
        free(map->entries);
    }
    map->entries = new_entries;
    map->capacity = new_cap;
    return true;
}

bool mz_map_set(MzHashMap *map, MzStrView key, void *val) {
    if (!map || mz_sv_is_empty(key)) return false;

    if (map->capacity == 0 || (map->count + 1) * MZ_MAP_MAX_LOAD_DEN >= map->capacity * MZ_MAP_MAX_LOAD_NUM) {
        size_t new_cap = map->capacity ? map->capacity * 2 : 16;
        if (!mz_map_resize(map, new_cap)) return false;
    }

    uint64_t hash = mz_map_calc_hash(map, key);
    size_t mask = map->capacity - 1;
    size_t idx = (size_t)(hash & mask);
    size_t first_deleted = (size_t)-1;

    while (map->entries[idx].occupied) {
        if (map->entries[idx].deleted) {
            if (first_deleted == (size_t)-1) {
                first_deleted = idx;
            }
        } else if (map->entries[idx].hash == hash && mz_map_keys_match(map, map->entries[idx].key, key)) {
            map->entries[idx].val = val;
            return true;
        }
        idx = (idx + 1) & mask;
    }

    size_t target_idx = (first_deleted != (size_t)-1) ? first_deleted : idx;
    if (map->arena && !mz_sv_is_empty(key)) {
        char *kbuf = mz_arena_strndup(map->arena, key.data, key.len);
        map->entries[target_idx].key = mz_sv_from_parts(kbuf, key.len);
    } else {
        map->entries[target_idx].key = key;
    }
    map->entries[target_idx].val = val;
    map->entries[target_idx].hash = hash;
    map->entries[target_idx].occupied = true;
    map->entries[target_idx].deleted = false;
    map->count++;
    return true;
}

bool mz_map_set_cstr(MzHashMap *map, const char *key, void *val) {
    return mz_map_set(map, mz_sv_from_cstr(key), val);
}

void *mz_map_get(const MzHashMap *map, MzStrView key) {
    if (!map || map->capacity == 0 || mz_sv_is_empty(key)) return nullptr;

    uint64_t hash = mz_map_calc_hash(map, key);
    size_t mask = map->capacity - 1;
    size_t idx = (size_t)(hash & mask);

    while (map->entries[idx].occupied) {
        if (!map->entries[idx].deleted &&
            map->entries[idx].hash == hash &&
            mz_map_keys_match(map, map->entries[idx].key, key)) {
            return map->entries[idx].val;
        }
        idx = (idx + 1) & mask;
    }
    return nullptr;
}

void *mz_map_get_cstr(const MzHashMap *map, const char *key) {
    return mz_map_get(map, mz_sv_from_cstr(key));
}

bool mz_map_remove(MzHashMap *map, MzStrView key) {
    if (!map || map->capacity == 0 || mz_sv_is_empty(key)) return false;

    uint64_t hash = mz_map_calc_hash(map, key);
    size_t mask = map->capacity - 1;
    size_t idx = (size_t)(hash & mask);

    while (map->entries[idx].occupied) {
        if (!map->entries[idx].deleted &&
            map->entries[idx].hash == hash &&
            mz_map_keys_match(map, map->entries[idx].key, key)) {
            map->entries[idx].deleted = true;
            map->count--;
            return true;
        }
        idx = (idx + 1) & mask;
    }
    return false;
}

void mz_map_clear(MzHashMap *map) {
    if (!map || !map->entries) return;
    memset(map->entries, 0, map->capacity * sizeof(MzHashMapEntry));
    map->count = 0;
}

void mz_map_free(MzHashMap *map) {
    if (!map) return;
    if (!map->arena) {
        free(map->entries);
    }
    map->entries = nullptr;
    map->capacity = 0;
    map->count = 0;
    map->arena = nullptr;
}
