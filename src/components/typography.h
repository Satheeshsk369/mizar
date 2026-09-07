#ifndef MIZAR_COMPONENTS_TYPOGRAPHY_H
#define MIZAR_COMPONENTS_TYPOGRAPHY_H

#include "view/html/tags.h"

// 1. Kbd: Keyboard shortcut badge (<kbd>Ctrl + K</kbd>)
static inline void UiKbd(const char *key_combo) {
    Kbd(.style = "display: inline-block; padding: 2px 6px; font-size: 0.75rem; font-family: monospace; line-height: 1; color: #1e293b; background: #f8fafc; border: 1px solid #cbd5e1; border-radius: 4px; box-shadow: 0 1px 1px rgba(0,0,0,0.1);") {
        Text(key_combo ? key_combo : "");
    }
}

// 2. Blockquote: Editorial quote with left border
typedef struct {
    const char *cite;
    const char *author;
    const char *cls;
} UiQuoteProps;

static inline void mz_ui_quote_open(UiQuoteProps p) {
    mz_tag_open("blockquote", (Attrs){
        .cls = p.cls ? p.cls : "mz-quote",
        .style = "border-left: 4px solid #0284c7; padding: 12px 16px; margin: 16px 0; background: #f8fafc; color: #334155; font-style: italic; border-radius: 0 6px 6px 0;"
    });
}

#define UiQuote(...) \
    for (int _mz_ui_q = (mz_ui_quote_open((UiQuoteProps){ __VA_ARGS__ }), 0); !_mz_ui_q; _mz_ui_q = 1, mz_tag_close("blockquote"))

// 3. Highlight / Mark: Inline text highlight
static inline void UiMark(const char *text) {
    Mark(.style = "background: #fef08a; color: #854d0e; padding: 1px 4px; border-radius: 3px;") {
        Text(text ? text : "");
    }
}

// 4. CodeBlock with syntax styling container
typedef struct {
    const char *lang;
    const char *filename;
} UiCodeBlockProps;

static inline void mz_ui_code_open(UiCodeBlockProps p) {
    mz_tag_open("div", (Attrs){ .style = "background: #0f172a; border-radius: 8px; overflow: hidden; margin: 16px 0;" });
    if (p.filename || p.lang) {
        mz_tag_open("div", (Attrs){ .style = "display: flex; justify-content: space-between; padding: 8px 16px; background: #1e293b; color: #94a3b8; font-size: 0.75rem; font-family: monospace;" });
        Text(p.filename ? p.filename : "");
        Text(p.lang ? p.lang : "");
        mz_tag_close("div");
    }
    mz_tag_open("pre", (Attrs){ .style = "margin: 0; padding: 16px; overflow-x: auto; color: #f8fafc; font-family: monospace; font-size: 0.875rem; line-height: 1.5;" });
    mz_tag_open("code", (Attrs){ 0 });
}

static inline void mz_ui_code_close(void) {
    mz_tag_close("code");
    mz_tag_close("pre");
    mz_tag_close("div");
}

#define UiCodeBlock(...) \
    for (int _mz_ui_cb = (mz_ui_code_open((UiCodeBlockProps){ __VA_ARGS__ }), 0); !_mz_ui_cb; _mz_ui_cb = 1, mz_ui_code_close())

#endif
