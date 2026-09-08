#ifndef MIZAR_VIEW_JS_DSL_H
#define MIZAR_VIEW_JS_DSL_H

#include "core/buffer.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Declarative JavaScript / Client Script DSL for Mizar
// -----------------------------------------------------------------------------
// Formats and emits client-side JavaScript statements and scoped blocks directly
// into the active MizarBuffer without raw unescaped string concatenations.

static inline void mz_js_line(const char *fmt, ...) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    va_list args;
    va_start(args, fmt);
    char tmp[2048];
    int n = vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);
    if (n > 0) {
        mz_buf_append(buf, tmp, (size_t)n);
        mz_buf_append(buf, "\n", 1);
    }
}

// Low-level JS block macros
#define JsBlock() \
    for (int _js_once = (mz_context_get() != nullptr); _js_once; _js_once = 0)

#define Js(fmt, ...) mz_js_line(fmt, ##__VA_ARGS__)

// Variable declarations & assignments
#define JsConst(name, val_expr) \
    mz_js_line("const %s = %s;", (name), (val_expr))

#define JsLet(name, val_expr) \
    mz_js_line("let %s = %s;", (name), (val_expr))

#define JsAssign(target, val_expr) \
    mz_js_line("%s = %s;", (target), (val_expr))

#define JsCall(fn_call) \
    mz_js_line("%s;", (fn_call))

#define JsReturn(expr) \
    mz_js_line("return %s;", (expr))

// Event Listeners
#define JsOnReady() \
    for (int _onready = (mz_js_line("document.addEventListener('DOMContentLoaded', () => {"), 1); \
         _onready; \
         _onready = (mz_js_line("});"), 0))

#define JsOn(target, evt) \
    for (int _on = (mz_js_line("%s?.addEventListener('%s', (e) => {", (target), (evt)), 1); \
         _on; \
         _on = (mz_js_line("});"), 0))

#define JsOnAsync(target, evt) \
    for (int _ona = (mz_js_line("%s?.addEventListener('%s', async (e) => {", (target), (evt)), 1); \
         _ona; \
         _ona = (mz_js_line("});"), 0))

// Control flow & functions
#define JsFn(name, args) \
    for (int _fn = (mz_js_line("function %s(%s) {", (name), (args)), 1); \
         _fn; \
         _fn = (mz_js_line("}"), 0))

#define JsAsyncFn(name, args) \
    for (int _afn = (mz_js_line("async function %s(%s) {", (name), (args)), 1); \
         _afn; \
         _afn = (mz_js_line("}"), 0))

#define JsIf(cond) \
    for (int _jif = (mz_js_line("if (%s) {", (cond)), 1); \
         _jif; \
         _jif = (mz_js_line("}"), 0))

#define JsElse() \
    for (int _jelse = (mz_js_line("else {"), 1); \
         _jelse; \
         _jelse = (mz_js_line("}"), 0))

#define JsFor(init, cond, step) \
    for (int _jfor = (mz_js_line("for (%s; %s; %s) {", (init), (cond), (step)), 1); \
         _jfor; \
         _jfor = (mz_js_line("}"), 0))

#define JsForOf(item_var, iterable) \
    for (int _jforof = (mz_js_line("for (const %s of %s) {", (item_var), (iterable)), 1); \
         _jforof; \
         _jforof = (mz_js_line("}"), 0))

#define JsForEach(iterable, item_var) \
    for (int _jfe = (mz_js_line("%s.forEach((%s) => {", (iterable), (item_var)), 1); \
         _jfe; \
         _jfe = (mz_js_line("});"), 0))

#define JsTry() \
    for (int _jtry = (mz_js_line("try {"), 1); \
         _jtry; \
         _jtry = (0))

#define JsCatch(err_param) \
    for (int _jcatch = (mz_js_line("} catch (%s) {", (err_param)), 1); \
         _jcatch; \
         _jcatch = (mz_js_line("}"), 0))

// DOM helper macros
#define JsGetEl(var_name, id) \
    mz_js_line("const %s = document.getElementById('%s');", (var_name), (id))

#define JsQueryAll(var_name, sel) \
    mz_js_line("const %s = document.querySelectorAll('%s');", (var_name), (sel))

#define JsSetAttr(target, attr, val) \
    mz_js_line("%s.setAttribute('%s', %s);", (target), (attr), (val))

#define JsSetTheme(theme_expr) \
    mz_js_line("document.documentElement.setAttribute('data-theme', %s);", (theme_expr))

#endif // MIZAR_VIEW_JS_DSL_H
