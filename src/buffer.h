#ifndef MIZAR_BUFFER_H
#define MIZAR_BUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

// Flush callback for chunked HTTP streaming
// Return true on success, false on write failure
typedef bool (*MizarFlushFn)(const char *chunk, size_t len, void *user_data);

typedef struct {
    char *data;
    size_t len;
    size_t capacity;
    bool has_error;
    
    // Chunked streaming configuration
    size_t chunk_threshold; // If > 0, flushes automatically when len >= threshold
    MizarFlushFn flush_fn;
    void *user_data;
} MizarBuffer;

// Initialize a buffer with an initial capacity and optional chunked streaming callback
void mz_buf_init(MizarBuffer *buf, size_t initial_cap);
void mz_buf_init_stream(MizarBuffer *buf, size_t chunk_threshold, MizarFlushFn flush_fn, void *user_data);
void mz_buf_free(MizarBuffer *buf);

// Appends
void mz_buf_append(MizarBuffer *buf, const char *str, size_t len);
void mz_buf_append_str(MizarBuffer *buf, const char *str);
void mz_buf_append_char(MizarBuffer *buf, char c);
void mz_buf_printf(MizarBuffer *buf, const char *fmt, ...);
void mz_buf_vprintf(MizarBuffer *buf, const char *fmt, va_list args);

// HTML escaping append (&, <, >, ", ')
void mz_buf_append_escaped(MizarBuffer *buf, const char *str, size_t len);

// Explicitly flush buffered data to the stream callback (or socket)
bool mz_buf_flush(MizarBuffer *buf);

// Clear buffer without freeing underlying memory
void mz_buf_clear(MizarBuffer *buf);

// Thread-local active buffer context
void mz_context_push(MizarBuffer *buf);
void mz_context_pop(void);
MizarBuffer *mz_context_get(void);

#endif // MIZAR_BUFFER_H
