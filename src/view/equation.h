#ifndef MIZAR_VIEW_EQUATION_H
#define MIZAR_VIEW_EQUATION_H

#include "core/buffer.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Writes an inline equation directly: \( <expr> \)
static inline void mz_render_eq_inline(const char *expr) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !expr) return;
    mz_buf_append_str(buf, "\\(");
    mz_buf_append_str(buf, expr);
    mz_buf_append_str(buf, "\\)");
}

// Writes a block/display equation directly: \[ <expr> \]
static inline void mz_render_eq_block(const char *expr) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !expr) return;
    mz_buf_append_str(buf, "\\[");
    mz_buf_append_str(buf, expr);
    mz_buf_append_str(buf, "\\]");
}

#define eq(expr) mz_render_eq_inline(expr)
#define Eq(expr) mz_render_eq_block(expr)

// Appends math content while repairing common unintended C escape sequences:
// \f (0x0C) -> \f (e.g. \frac)
// \a (0x07) -> \a (e.g. \alpha, \approx)
// \b (0x08) -> \b (e.g. \beta, \begin, \bar)
// \v (0x0B) -> \v (e.g. \vec)
static inline void mz_append_math_repaired(MizarBuffer *buf, const char *start, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = start[i];
        if (c == 0x0C) {        // \f in C
            mz_buf_append_str(buf, "\\f");
        } else if (c == 0x07) { // \a in C
            mz_buf_append_str(buf, "\\a");
        } else if (c == 0x08) { // \b in C
            mz_buf_append_str(buf, "\\b");
        } else if (c == 0x0B) { // \v in C
            mz_buf_append_str(buf, "\\v");
        } else {
            mz_buf_append_char(buf, c);
        }
    }
}

// Internal parser for mixed text and math expressions with $inline$ and $$block$$
static inline void mz_render_eq_text(const char *text) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !text) return;

    const char *p = text;
    while (*p) {
        // Escaped dollar: "\$" -> print literal '$'
        if (*p == '\\' && *(p + 1) == '$') {
            mz_buf_append_char(buf, '$');
            p += 2;
            continue;
        }

        // Block math: $$ ... $$
        if (*p == '$' && *(p + 1) == '$') {
            p += 2;
            const char *math_start = p;
            while (*p && !(*p == '$' && *(p + 1) == '$')) {
                p++;
            }
            size_t math_len = (size_t)(p - math_start);
            mz_buf_append_str(buf, "\\[");
            mz_append_math_repaired(buf, math_start, math_len);
            mz_buf_append_str(buf, "\\]");
            if (*p == '$' && *(p + 1) == '$') {
                p += 2;
            }
            continue;
        }

        // Inline math: $ ... $
        if (*p == '$') {
            p++;
            const char *math_start = p;
            while (*p && *p != '$') {
                p++;
            }
            size_t math_len = (size_t)(p - math_start);
            mz_buf_append_str(buf, "\\(");
            mz_append_math_repaired(buf, math_start, math_len);
            mz_buf_append_str(buf, "\\)");
            if (*p == '$') {
                p++;
            }
            continue;
        }

        // Regular prose: accumulate and escape HTML chars (&, <, >)
        const char *text_start = p;
        while (*p && *p != '$' && !(*p == '\\' && *(p + 1) == '$')) {
            p++;
        }
        mz_buf_append_escaped(buf, text_start, (size_t)(p - text_start));
    }
}

// EqText(fmt, ...) - Supports formatted or plain text containing $inline$ and $$block$$ math.
static inline void EqText(const char *fmt, ...) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !fmt) return;

    va_list args;
    va_start(args, fmt);

    char stack_buf[1024];
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(stack_buf, sizeof(stack_buf), fmt, copy);
    va_end(copy);

    if (needed >= 0) {
        if ((size_t)needed < sizeof(stack_buf)) {
            mz_render_eq_text(stack_buf);
        } else {
            char *dyn = (char *)malloc((size_t)needed + 1);
            if (dyn) {
                vsnprintf(dyn, (size_t)needed + 1, fmt, args);
                mz_render_eq_text(dyn);
                free(dyn);
            }
        }
    }
    va_end(args);
}

#endif
