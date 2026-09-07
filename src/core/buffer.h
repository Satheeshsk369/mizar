#ifndef MIZAR_CORE_BUFFER_H
#define MIZAR_CORE_BUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

typedef bool (*MizarFlushFn)(const char *chunk, size_t len, void *user_data);

typedef struct {
    char *data;
    size_t len;
    size_t capacity;
    bool has_error;
    size_t chunk_threshold;
    MizarFlushFn flush_fn;
    void *user_data;
} MizarBuffer;

void mz_buf_init(MizarBuffer *buf, size_t initial_cap);
void mz_buf_init_stream(MizarBuffer *buf, size_t chunk_threshold, MizarFlushFn flush_fn, void *user_data);
void mz_buf_free(MizarBuffer *buf);

void mz_buf_append(MizarBuffer *buf, const char *str, size_t len);
void mz_buf_append_str(MizarBuffer *buf, const char *str);
void mz_buf_append_char(MizarBuffer *buf, char c);
void mz_buf_printf(MizarBuffer *buf, const char *fmt, ...);
void mz_buf_vprintf(MizarBuffer *buf, const char *fmt, va_list args);

void mz_buf_append_escaped(MizarBuffer *buf, const char *str, size_t len);
bool mz_buf_flush(MizarBuffer *buf);
void mz_buf_clear(MizarBuffer *buf);

void mz_context_push(MizarBuffer *buf);
void mz_context_pop(void);
MizarBuffer *mz_context_get(void);

#endif
