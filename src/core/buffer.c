#include "core/buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MZ_MAX_CONTEXT_DEPTH 64
static thread_local MizarBuffer *s_context_stack[MZ_MAX_CONTEXT_DEPTH];
static thread_local int s_context_depth = 0;

void mz_context_push(MizarBuffer *buf) {
    if (s_context_depth < MZ_MAX_CONTEXT_DEPTH) {
        s_context_stack[s_context_depth++] = buf;
    }
}

void mz_context_pop(void) {
    if (s_context_depth > 0) {
        s_context_depth--;
    }
}

MizarBuffer *mz_context_get(void) {
    if (s_context_depth > 0) {
        return s_context_stack[s_context_depth - 1];
    }
    return nullptr;
}

int mz_context_get_depth(void) {
    return s_context_depth;
}

void mz_context_restore_depth(int depth) {
    if (depth >= 0 && depth <= MZ_MAX_CONTEXT_DEPTH) {
        s_context_depth = depth;
    }
}

void mz_buf_init(MizarBuffer *buf, size_t initial_cap) {
    if (!buf) return;
    if (initial_cap < 256) initial_cap = 256;
    buf->data = (char *)malloc(initial_cap);
    buf->len = 0;
    buf->capacity = buf->data ? initial_cap : 0;
    buf->has_error = (buf->data == nullptr);
    buf->chunk_threshold = 0;
    buf->flush_fn = nullptr;
    buf->user_data = nullptr;
    if (buf->data) {
        buf->data[0] = '\0';
    }
}

void mz_buf_init_stream(MizarBuffer *buf, size_t chunk_threshold, MizarFlushFn flush_fn, void *user_data) {
    mz_buf_init(buf, chunk_threshold > 0 ? chunk_threshold * 2 : 4096);
    buf->chunk_threshold = chunk_threshold;
    buf->flush_fn = flush_fn;
    buf->user_data = user_data;
}

void mz_buf_free(MizarBuffer *buf) {
    if (!buf) return;
    free(buf->data);
    buf->data = nullptr;
    buf->len = 0;
    buf->capacity = 0;
    buf->has_error = false;
}

bool mz_buf_flush(MizarBuffer *buf) {
    if (!buf || buf->has_error) return false;
    if (buf->len == 0) return true;
    if (buf->flush_fn) {
        bool ok = buf->flush_fn(buf->data, buf->len, buf->user_data);
        if (!ok) {
            buf->has_error = true;
            return false;
        }
        buf->len = 0;
        if (buf->data) buf->data[0] = '\0';
        return true;
    }
    return true;
}

static bool mz_buf_ensure_space(MizarBuffer *buf, size_t extra) {
    if (!buf || buf->has_error) return false;
    
    if (buf->chunk_threshold > 0 && buf->len >= buf->chunk_threshold && buf->flush_fn) {
        if (!mz_buf_flush(buf)) return false;
    }
    
    size_t needed = buf->len + extra + 1;
    if (needed <= buf->capacity) return true;
    
    size_t new_cap = buf->capacity ? buf->capacity * 2 : 256;
    while (new_cap < needed) {
        new_cap *= 2;
    }
    
    char *new_data = (char *)realloc(buf->data, new_cap);
    if (!new_data) {
        buf->has_error = true;
        return false;
    }
    buf->data = new_data;
    buf->capacity = new_cap;
    return true;
}

void mz_buf_append(MizarBuffer *buf, const char *str, size_t len) {
    if (!buf || !str || len == 0) return;
    if (!mz_buf_ensure_space(buf, len)) return;
    
    memcpy(buf->data + buf->len, str, len);
    buf->len += len;
    buf->data[buf->len] = '\0';
}

void mz_buf_append_str(MizarBuffer *buf, const char *str) {
    if (!str) return;
    mz_buf_append(buf, str, strlen(str));
}

void mz_buf_append_char(MizarBuffer *buf, char c) {
    if (!buf) return;
    if (!mz_buf_ensure_space(buf, 1)) return;
    buf->data[buf->len++] = c;
    buf->data[buf->len] = '\0';
}

void mz_buf_printf(MizarBuffer *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    mz_buf_vprintf(buf, fmt, args);
    va_end(args);
}

void mz_buf_vprintf(MizarBuffer *buf, const char *fmt, va_list args) {
    if (!buf || !fmt || buf->has_error) return;
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);
    
    if (needed < 0) {
        buf->has_error = true;
        return;
    }
    
    if (!mz_buf_ensure_space(buf, (size_t)needed)) return;
    vsnprintf(buf->data + buf->len, needed + 1, fmt, args);
    buf->len += (size_t)needed;
}

void mz_buf_append_escaped(MizarBuffer *buf, const char *str, size_t len) {
    if (!buf || !str || len == 0 || buf->has_error) return;
    
    size_t start = 0;
    for (size_t i = 0; i < len; i++) {
        const char *replacement = nullptr;
        size_t rep_len = 0;
        switch (str[i]) {
            case '&':  replacement = "&amp;";  rep_len = 5; break;
            case '<':  replacement = "&lt;";   rep_len = 4; break;
            case '>':  replacement = "&gt;";   rep_len = 4; break;
            case '"':  replacement = "&quot;"; rep_len = 6; break;
            case '\'': replacement = "&#39;";  rep_len = 5; break;
            default:   continue;
        }
        
        if (i > start) {
            mz_buf_append(buf, str + start, i - start);
        }
        mz_buf_append(buf, replacement, rep_len);
        start = i + 1;
    }
    
    if (len > start) {
        mz_buf_append(buf, str + start, len - start);
    }
}

void mz_buf_clear(MizarBuffer *buf) {
    if (!buf) return;
    buf->len = 0;
    buf->has_error = false;
    if (buf->data) buf->data[0] = '\0';
}
