#include "algo/arena.h"
#include <stdlib.h>
#include <string.h>

#define MZ_ARENA_ALIGN sizeof(max_align_t)
#define MZ_ALIGN_UP(n, align) (((n) + (align) - 1) & ~((align) - 1))

void mz_arena_init(MzArena *arena, size_t default_block_size) {
    if (!arena) return;
    if (default_block_size < 1024) default_block_size = 4096;
    arena->head = nullptr;
    arena->default_block_size = default_block_size;
    arena->total_allocated = 0;
}

static MzArenaBlock *mz_arena_new_block(size_t size) {
    MzArenaBlock *block = (MzArenaBlock *)malloc(sizeof(MzArenaBlock) + size);
    if (!block) return nullptr;
    block->next = nullptr;
    block->size = size;
    block->used = 0;
    return block;
}

void *mz_arena_alloc(MzArena *arena, size_t size) {
    if (!arena || size == 0) return nullptr;
    size = MZ_ALIGN_UP(size, MZ_ARENA_ALIGN);

    if (!arena->head || (arena->head->used + size > arena->head->size)) {
        size_t block_size = arena->default_block_size;
        if (size > block_size) {
            block_size = size;
        }
        MzArenaBlock *block = mz_arena_new_block(block_size);
        if (!block) return nullptr;
        block->next = arena->head;
        arena->head = block;
        arena->total_allocated += block_size;
    }

    void *ptr = arena->head->memory + arena->head->used;
    arena->head->used += size;
    return ptr;
}

void *mz_arena_calloc(MzArena *arena, size_t count, size_t size) {
    size_t total = count * size;
    void *ptr = mz_arena_alloc(arena, total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

char *mz_arena_strdup(MzArena *arena, const char *str) {
    if (!str) return nullptr;
    size_t len = strlen(str);
    return mz_arena_strndup(arena, str, len);
}

char *mz_arena_strndup(MzArena *arena, const char *str, size_t n) {
    if (!str) return nullptr;
    char *dup = (char *)mz_arena_alloc(arena, n + 1);
    if (!dup) return nullptr;
    memcpy(dup, str, n);
    dup[n] = '\0';
    return dup;
}

void mz_arena_reset(MzArena *arena) {
    if (!arena) return;
    MzArenaBlock *curr = arena->head;
    while (curr) {
        curr->used = 0;
        curr = curr->next;
    }
}

void mz_arena_free(MzArena *arena) {
    if (!arena) return;
    MzArenaBlock *curr = arena->head;
    while (curr) {
        MzArenaBlock *next = curr->next;
        free(curr);
        curr = next;
    }
    arena->head = nullptr;
    arena->total_allocated = 0;
}
