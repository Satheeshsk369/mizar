#include "algo/ringbuf.h"
#include <stdlib.h>
#include <string.h>

static inline size_t next_pow2(size_t n) {
    if (n < 16) return 16;
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

bool mz_ringbuf_init(MzRingBuf *rb, size_t capacity) {
    if (!rb) return false;
    capacity = next_pow2(capacity);
    rb->buffer = (uint8_t *)malloc(capacity);
    if (!rb->buffer) return false;
    rb->capacity = capacity;
    rb->mask = capacity - 1;
    rb->head = 0;
    rb->tail = 0;
    return true;
}

size_t mz_ringbuf_readable(const MzRingBuf *rb) {
    if (!rb) return 0;
    return rb->head - rb->tail;
}

size_t mz_ringbuf_writable(const MzRingBuf *rb) {
    if (!rb) return 0;
    return rb->capacity - (rb->head - rb->tail);
}

size_t mz_ringbuf_write(MzRingBuf *rb, const void *data, size_t len) {
    if (!rb || !data || len == 0) return 0;
    size_t space = mz_ringbuf_writable(rb);
    if (len > space) len = space;
    if (len == 0) return 0;

    const uint8_t *src = (const uint8_t *)data;
    size_t offset = rb->head & rb->mask;
    size_t chunk1 = rb->capacity - offset;
    if (chunk1 > len) chunk1 = len;

    memcpy(rb->buffer + offset, src, chunk1);
    if (len > chunk1) {
        memcpy(rb->buffer, src + chunk1, len - chunk1);
    }
    rb->head += len;
    return len;
}

size_t mz_ringbuf_read(MzRingBuf *rb, void *dest, size_t len) {
    if (!rb || !dest || len == 0) return 0;
    size_t avail = mz_ringbuf_readable(rb);
    if (len > avail) len = avail;
    if (len == 0) return 0;

    uint8_t *dst = (uint8_t *)dest;
    size_t offset = rb->tail & rb->mask;
    size_t chunk1 = rb->capacity - offset;
    if (chunk1 > len) chunk1 = len;

    memcpy(dst, rb->buffer + offset, chunk1);
    if (len > chunk1) {
        memcpy(dst + chunk1, rb->buffer, len - chunk1);
    }
    rb->tail += len;
    return len;
}

size_t mz_ringbuf_peek(const MzRingBuf *rb, void *dest, size_t len) {
    if (!rb || !dest || len == 0) return 0;
    size_t avail = mz_ringbuf_readable(rb);
    if (len > avail) len = avail;
    if (len == 0) return 0;

    uint8_t *dst = (uint8_t *)dest;
    size_t offset = rb->tail & rb->mask;
    size_t chunk1 = rb->capacity - offset;
    if (chunk1 > len) chunk1 = len;

    memcpy(dst, rb->buffer + offset, chunk1);
    if (len > chunk1) {
        memcpy(dst + chunk1, rb->buffer, len - chunk1);
    }
    return len;
}

void mz_ringbuf_free(MzRingBuf *rb) {
    if (!rb) return;
    free(rb->buffer);
    rb->buffer = nullptr;
    rb->capacity = 0;
    rb->mask = 0;
    rb->head = 0;
    rb->tail = 0;
}
