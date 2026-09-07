#ifndef MIZAR_VIEW_CSS_RULES_H
#define MIZAR_VIEW_CSS_RULES_H

#include "core/buffer.h"
#include <stdio.h>
#include <stdarg.h>

static inline void mz_css_rule_open(const char *selector) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !selector) return;
    mz_buf_append_str(buf, selector);
    mz_buf_append_str(buf, " {\n");
}

static inline void mz_css_rule_close(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_str(buf, "}\n");
}

#define Stylesheet(buf) \
    for (int _mz_ss = (mz_context_push(buf), 0); \
         !_mz_ss; \
         _mz_ss = 1, mz_context_pop())

#define Rule(selector) \
    for (int _mz_r = (mz_css_rule_open(selector), 0); \
         !_mz_r; \
         _mz_r = 1, mz_css_rule_close())

#define Media(query) \
    for (int _mz_m = (mz_css_rule_open(query), 0); \
         !_mz_m; \
         _mz_m = 1, mz_css_rule_close())

#define Keyframes(name) \
    for (int _mz_k = (mz_css_rule_open(name), 0); \
         !_mz_k; \
         _mz_k = 1, mz_css_rule_close())

static inline void Prop(const char *name, const char *fmt, ...) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !name || !fmt) return;
    
    mz_buf_append_str(buf, "  ");
    mz_buf_append_str(buf, name);
    mz_buf_append_str(buf, ": ");
    
    va_list args;
    va_start(args, fmt);
    mz_buf_vprintf(buf, fmt, args);
    va_end(args);
    
    mz_buf_append_str(buf, ";\n");
}

static inline void Css(const char *raw_css) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !raw_css) return;
    mz_buf_append_str(buf, raw_css);
}

static inline void Css_Comment(const char *fmt, ...) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !fmt) return;
    mz_buf_append_str(buf, "/* ");
    va_list args;
    va_start(args, fmt);
    mz_buf_vprintf(buf, fmt, args);
    va_end(args);
    mz_buf_append_str(buf, " */\n");
}

#define CSS_PROP(k, v) k ": " v ";"

#endif
