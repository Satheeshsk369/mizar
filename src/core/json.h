#ifndef MIZAR_CORE_JSON_H
#define MIZAR_CORE_JSON_H

#include "core/buffer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Declarative Streaming JSON DSL
// -----------------------------------------------------------------------------

static inline void mz_json_open_obj(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '{');
}

static inline void mz_json_close_obj(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    // Strip trailing comma if present before closing
    if (buf->len > 0 && buf->data[buf->len - 1] == ',') {
        buf->len--;
        buf->data[buf->len] = '\0';
    }
    mz_buf_append_str(buf, "},");
}

static inline void mz_json_open_arr(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '[');
}

static inline void mz_json_close_arr(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    if (buf->len > 0 && buf->data[buf->len - 1] == ',') {
        buf->len--;
        buf->data[buf->len] = '\0';
    }
    mz_buf_append_str(buf, "],");
}

// Scope macros for JSON Objects and Arrays
#define JsonObj() \
    for (int _mz_jo = (mz_json_open_obj(), 0); !_mz_jo; _mz_jo = 1, mz_json_close_obj())

#define JsonArr() \
    for (int _mz_ja = (mz_json_open_arr(), 0); !_mz_ja; _mz_ja = 1, mz_json_close_arr())

#define JsonKeyObj(key) \
    mz_json_key_raw(key); \
    for (int _mz_jko = (mz_json_open_obj(), 0); !_mz_jko; _mz_jko = 1, mz_json_close_obj())

#define JsonKeyArr(key) \
    mz_json_key_raw(key); \
    for (int _mz_jka = (mz_json_open_arr(), 0); !_mz_jka; _mz_jka = 1, mz_json_close_arr())

// Top-level document macro
#define JsonDoc(buf) \
    for (int _mz_jd = (mz_context_push(buf), 0); !_mz_jd; _mz_jd = 1, mz_json_finalize_doc(buf), mz_context_pop())

static inline void mz_json_finalize_doc(MizarBuffer *buf) {
    if (!buf) return;
    if (buf->len > 0 && buf->data[buf->len - 1] == ',') {
        buf->len--;
        buf->data[buf->len] = '\0';
    }
}

static inline void mz_json_key_raw(const char *key) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !key) return;
    mz_buf_append_char(buf, '"');
    mz_buf_append_str(buf, key);
    mz_buf_append_str(buf, "\":");
}

static inline void JsonKeyStr(const char *key, const char *val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !key) return;
    mz_json_key_raw(key);
    if (!val) {
        mz_buf_append_str(buf, "null,");
        return;
    }
    mz_buf_append_char(buf, '"');
    // Escape JSON string characters
    for (const char *p = val; *p; p++) {
        if (*p == '"') mz_buf_append_str(buf, "\\\"");
        else if (*p == '\\') mz_buf_append_str(buf, "\\\\");
        else if (*p == '\n') mz_buf_append_str(buf, "\\n");
        else if (*p == '\r') mz_buf_append_str(buf, "\\r");
        else if (*p == '\t') mz_buf_append_str(buf, "\\t");
        else mz_buf_append_char(buf, *p);
    }
    mz_buf_append_str(buf, "\",");
}

static inline void JsonKeyInt(const char *key, long long val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !key) return;
    mz_json_key_raw(key);
    char num_str[32];
    snprintf(num_str, sizeof(num_str), "%lld,", val);
    mz_buf_append_str(buf, num_str);
}

static inline void JsonKeyDouble(const char *key, double val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !key) return;
    mz_json_key_raw(key);
    char num_str[32];
    snprintf(num_str, sizeof(num_str), "%.4g,", val);
    mz_buf_append_str(buf, num_str);
}

static inline void JsonKeyBool(const char *key, bool val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !key) return;
    mz_json_key_raw(key);
    mz_buf_append_str(buf, val ? "true," : "false,");
}

static inline void JsonKeyNull(const char *key) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !key) return;
    mz_json_key_raw(key);
    mz_buf_append_str(buf, "null,");
}

static inline void JsonValStr(const char *val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    if (!val) {
        mz_buf_append_str(buf, "null,");
        return;
    }
    mz_buf_append_char(buf, '"');
    for (const char *p = val; *p; p++) {
        if (*p == '"') mz_buf_append_str(buf, "\\\"");
        else if (*p == '\\') mz_buf_append_str(buf, "\\\\");
        else mz_buf_append_char(buf, *p);
    }
    mz_buf_append_str(buf, "\",");
}

static inline void JsonValInt(long long val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    char num_str[32];
    snprintf(num_str, sizeof(num_str), "%lld,", val);
    mz_buf_append_str(buf, num_str);
}

static inline void JsonValBool(bool val) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_str(buf, val ? "true," : "false,");
}

#endif
