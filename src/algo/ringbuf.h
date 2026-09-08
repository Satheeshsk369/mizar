#ifndef MIZAR_ALGO_RINGBUF_H
#define MIZAR_ALGO_RINGBUF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity; // Power of 2
    size_t mask;
    size_t head;     // Write index
    size_t tail;     // Read index
} MzRingBuf;

// Initialize circular buffer with power-of-2 capacity
bool mz_ringbuf_init(MzRingBuf *rb, size_t capacity);

// Returns available bytes to read
size_t mz_ringbuf_readable(const MzRingBuf *rb);

// Returns remaining space to write
size_t mz_ringbuf_writable(const MzRingBuf *rb);

// Write data into ring buffer (returns bytes written)
size_t mz_ringbuf_write(MzRingBuf *rb, const void *data, size_t len);

// Read data from ring buffer (returns bytes read)
size_t mz_ringbuf_read(MzRingBuf *rb, void *dest, size_t len);

// Peek data without consuming from ring buffer
size_t mz_ringbuf_peek(const MzRingBuf *rb, void *dest, size_t len);

// Free ring buffer memory
void mz_ringbuf_free(MzRingBuf *rb);

#endif
