#ifndef MIZAR_ALGO_ARENA_H
#define MIZAR_ALGO_ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct MzArenaBlock MzArenaBlock;

struct MzArenaBlock {
    MzArenaBlock *next;
    size_t size;
    size_t used;
    alignas(max_align_t) uint8_t memory[];
};

typedef struct {
    MzArenaBlock *head;
    size_t default_block_size;
    size_t total_allocated;
} MzArena;

// Initialize an arena with a default block size (e.g. 8192 or 65536)
void mz_arena_init(MzArena *arena, size_t default_block_size);

// Allocate aligned memory from arena
void *mz_arena_alloc(MzArena *arena, size_t size);

// Allocate zero-initialized aligned memory
void *mz_arena_calloc(MzArena *arena, size_t count, size_t size);

// Duplicate a null-terminated string into the arena
char *mz_arena_strdup(MzArena *arena, const char *str);

// Duplicate an n-byte buffer + null terminator into the arena
char *mz_arena_strndup(MzArena *arena, const char *str, size_t n);

// Reset the arena for reuse without freeing underlying memory blocks
void mz_arena_reset(MzArena *arena);

// Free all memory blocks back to OS
void mz_arena_free(MzArena *arena);

#endif
