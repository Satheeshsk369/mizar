#ifndef MIZAR_VIEW_HTML_TAGS_H
#define MIZAR_VIEW_HTML_TAGS_H

#include "core/buffer.h"
#include "view/html/attrs.h"
#include "core/result.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static inline void mz_tag_close(const char *tag);

static inline void mz_auto_close_tag(const char **tag_ptr) {
    if (tag_ptr && *tag_ptr) {
        mz_tag_close(*tag_ptr);
    }
}

static inline void mz_tag_open(const char *tag, Attrs attrs) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '<');
    mz_buf_append_str(buf, tag);
    mz_render_attrs(buf, attrs);
    mz_buf_append_char(buf, '>');
}

static inline void mz_tag_close(const char *tag) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_str(buf, "</");
    mz_buf_append_str(buf, tag);
    mz_buf_append_char(buf, '>');
}

static inline void mz_void_tag(const char *tag, Attrs attrs) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '<');
    mz_buf_append_str(buf, tag);
    mz_render_attrs(buf, attrs);
    mz_buf_append_str(buf, " />");
}

#define _MZ_TAG(tag_name, ...) \
    for (int _mz_i = (mz_tag_open(tag_name, (Attrs){ __VA_ARGS__ }), 0); \
         !_mz_i; \
         _mz_i = 1, mz_tag_close(tag_name))

#define Html(buf, ...) \
    for (int _mz_doc = (mz_context_push(buf), \
                        mz_buf_append_str(buf, "<!DOCTYPE html>\n"), \
                        mz_tag_open("html", (Attrs){ __VA_ARGS__ }), 0); \
         !_mz_doc; \
         _mz_doc = 1, mz_tag_close("html"), mz_context_pop())

#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 1, 2)))
#endif
static inline void Text(const char *fmt, ...) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !fmt) return;
    va_list args;
    va_start(args, fmt);
    
    char stack_buf[512];
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(stack_buf, sizeof(stack_buf), fmt, copy);
    va_end(copy);
    
    if (needed >= 0) {
        if ((size_t)needed < sizeof(stack_buf)) {
            mz_buf_append_escaped(buf, stack_buf, (size_t)needed);
        } else {
            char *dyn = (char *)malloc((size_t)needed + 1);
            if (dyn) {
                vsnprintf(dyn, (size_t)needed + 1, fmt, args);
                mz_buf_append_escaped(buf, dyn, (size_t)needed);
                free(dyn);
            }
        }
    }
    va_end(args);
}

static inline void Raw(const char *raw_html) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !raw_html) return;
    mz_buf_append_str(buf, raw_html);
}

static inline void Comment(const char *fmt, ...) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !fmt) return;
    mz_buf_append_str(buf, "<!-- ");
    va_list args;
    va_start(args, fmt);
    mz_buf_vprintf(buf, fmt, args);
    va_end(args);
    mz_buf_append_str(buf, " -->\n");
}

static inline bool Flush(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return false;
    return mz_buf_flush(buf);
}

// Void Elements (self-closing)
#define Area(...)   mz_void_tag("area",   (Attrs){ __VA_ARGS__ })
#define Base(...)   mz_void_tag("base",   (Attrs){ __VA_ARGS__ })
#define Br(...)     mz_void_tag("br",     (Attrs){ __VA_ARGS__ })
#define Col(...)    mz_void_tag("col",    (Attrs){ __VA_ARGS__ })
#define Embed(...)  mz_void_tag("embed",  (Attrs){ __VA_ARGS__ })
#define Hr(...)     mz_void_tag("hr",     (Attrs){ __VA_ARGS__ })
#define Img(...)    mz_void_tag("img",    (Attrs){ __VA_ARGS__ })
#define Input(...)  mz_void_tag("input",  (Attrs){ __VA_ARGS__ })
#define Link(...)   mz_void_tag("link",   (Attrs){ __VA_ARGS__ })
#define Meta(...)   mz_void_tag("meta",   (Attrs){ __VA_ARGS__ })
#define Source(...) mz_void_tag("source", (Attrs){ __VA_ARGS__ })
#define Track(...)  mz_void_tag("track",  (Attrs){ __VA_ARGS__ })
#define Wbr(...)    mz_void_tag("wbr",    (Attrs){ __VA_ARGS__ })

// Container Elements
#define A(...)               _MZ_TAG("a",               __VA_ARGS__)
#define Abbr(...)            _MZ_TAG("abbr",            __VA_ARGS__)
#define Address(...)         _MZ_TAG("address",         __VA_ARGS__)
#define Article(...)         _MZ_TAG("article",         __VA_ARGS__)
#define Aside(...)           _MZ_TAG("aside",           __VA_ARGS__)
#define Audio(...)           _MZ_TAG("audio",           __VA_ARGS__)
#define B(...)               _MZ_TAG("b",               __VA_ARGS__)
#define Bdi(...)             _MZ_TAG("bdi",             __VA_ARGS__)
#define Bdo(...)             _MZ_TAG("bdo",             __VA_ARGS__)
#define Blockquote(...)      _MZ_TAG("blockquote",      __VA_ARGS__)
#define Body(...)            _MZ_TAG("body",            __VA_ARGS__)
#define Button(...)          _MZ_TAG("button",          __VA_ARGS__)
#define Canvas(...)          _MZ_TAG("canvas",          __VA_ARGS__)
#define Caption(...)         _MZ_TAG("caption",         __VA_ARGS__)
#define Cite(...)            _MZ_TAG("cite",            __VA_ARGS__)
#define Code(...)            _MZ_TAG("code",            __VA_ARGS__)
#define Colgroup(...)        _MZ_TAG("colgroup",        __VA_ARGS__)
#define Data(...)            _MZ_TAG("data",            __VA_ARGS__)
#define Datalist(...)        _MZ_TAG("datalist",        __VA_ARGS__)
#define Dd(...)              _MZ_TAG("dd",              __VA_ARGS__)
#define Del(...)             _MZ_TAG("del",             __VA_ARGS__)
#define Details(...)         _MZ_TAG("details",         __VA_ARGS__)
#define Dfn(...)             _MZ_TAG("dfn",             __VA_ARGS__)
#define Dialog(...)          _MZ_TAG("dialog",          __VA_ARGS__)
#define Div(...)             _MZ_TAG("div",             __VA_ARGS__)
#define Dl(...)              _MZ_TAG("dl",              __VA_ARGS__)
#define Dt(...)              _MZ_TAG("dt",              __VA_ARGS__)
#define Em(...)              _MZ_TAG("em",              __VA_ARGS__)
#define Fencedframe(...)     _MZ_TAG("fencedframe",     __VA_ARGS__)
#define Fieldset(...)        _MZ_TAG("fieldset",        __VA_ARGS__)
#define Figcaption(...)      _MZ_TAG("figcaption",      __VA_ARGS__)
#define Figure(...)          _MZ_TAG("figure",          __VA_ARGS__)
#define Footer(...)          _MZ_TAG("footer",          __VA_ARGS__)
#define Form(...)            _MZ_TAG("form",            __VA_ARGS__)
#define Geolocation(...)     _MZ_TAG("geolocation",     __VA_ARGS__)
#define H1(...)              _MZ_TAG("h1",              __VA_ARGS__)
#define H2(...)              _MZ_TAG("h2",              __VA_ARGS__)
#define H3(...)              _MZ_TAG("h3",              __VA_ARGS__)
#define H4(...)              _MZ_TAG("h4",              __VA_ARGS__)
#define H5(...)              _MZ_TAG("h5",              __VA_ARGS__)
#define H6(...)              _MZ_TAG("h6",              __VA_ARGS__)
#define Head(...)            _MZ_TAG("head",            __VA_ARGS__)
#define Header(...)          _MZ_TAG("header",          __VA_ARGS__)
#define Hgroup(...)          _MZ_TAG("hgroup",          __VA_ARGS__)
#define I(...)               _MZ_TAG("i",               __VA_ARGS__)
#define Iframe(...)          _MZ_TAG("iframe",          __VA_ARGS__)
#define Ins(...)             _MZ_TAG("ins",             __VA_ARGS__)
#define Kbd(...)             _MZ_TAG("kbd",             __VA_ARGS__)
#define Label(...)           _MZ_TAG("label",           __VA_ARGS__)
#define Legend(...)          _MZ_TAG("legend",          __VA_ARGS__)
#define Li(...)              _MZ_TAG("li",              __VA_ARGS__)
#define Main(...)            _MZ_TAG("main",            __VA_ARGS__)
#define Map(...)             _MZ_TAG("map",             __VA_ARGS__)
#define Mark(...)            _MZ_TAG("mark",            __VA_ARGS__)
#define Menu(...)            _MZ_TAG("menu",            __VA_ARGS__)
#define Meter(...)           _MZ_TAG("meter",           __VA_ARGS__)
#define Nav(...)             _MZ_TAG("nav",             __VA_ARGS__)
#define Noscript(...)        _MZ_TAG("noscript",        __VA_ARGS__)
#define Object(...)          _MZ_TAG("object",          __VA_ARGS__)
#define Ol(...)              _MZ_TAG("ol",              __VA_ARGS__)
#define Optgroup(...)        _MZ_TAG("optgroup",        __VA_ARGS__)
#define Option(...)          _MZ_TAG("option",          __VA_ARGS__)
#define Output(...)          _MZ_TAG("output",          __VA_ARGS__)
#define P(...)               _MZ_TAG("p",               __VA_ARGS__)
#define Picture(...)         _MZ_TAG("picture",         __VA_ARGS__)
#define Portal(...)          _MZ_TAG("portal",          __VA_ARGS__)
#define Pre(...)             _MZ_TAG("pre",             __VA_ARGS__)
#define Progress(...)        _MZ_TAG("progress",        __VA_ARGS__)
#define Q(...)               _MZ_TAG("q",               __VA_ARGS__)
#define Rb(...)              _MZ_TAG("rb",              __VA_ARGS__)
#define Rp(...)              _MZ_TAG("rp",              __VA_ARGS__)
#define Rt(...)              _MZ_TAG("rt",              __VA_ARGS__)
#define Rtc(...)             _MZ_TAG("rtc",             __VA_ARGS__)
#define Ruby(...)            _MZ_TAG("ruby",            __VA_ARGS__)
#define S(...)               _MZ_TAG("s",               __VA_ARGS__)
#define Samp(...)            _MZ_TAG("samp",            __VA_ARGS__)
#define Script(...)          _MZ_TAG("script",          __VA_ARGS__)
#define Search(...)          _MZ_TAG("search",          __VA_ARGS__)
#define Section(...)         _MZ_TAG("section",         __VA_ARGS__)
#define Select(...)          _MZ_TAG("select",          __VA_ARGS__)
#define Selectedcontent(...) _MZ_TAG("selectedcontent", __VA_ARGS__)
#define Slot(...)            _MZ_TAG("slot",            __VA_ARGS__)
#define Small(...)           _MZ_TAG("small",           __VA_ARGS__)
#define Span(...)            _MZ_TAG("span",            __VA_ARGS__)
#define Strong(...)          _MZ_TAG("strong",          __VA_ARGS__)
#define Style(...)           _MZ_TAG("style",           __VA_ARGS__)
#define Sub(...)             _MZ_TAG("sub",             __VA_ARGS__)
#define Summary(...)         _MZ_TAG("summary",         __VA_ARGS__)
#define Sup(...)             _MZ_TAG("sup",             __VA_ARGS__)
#define Table(...)           _MZ_TAG("table",           __VA_ARGS__)
#define Tbody(...)           _MZ_TAG("tbody",           __VA_ARGS__)
#define Td(...)              _MZ_TAG("td",              __VA_ARGS__)
#define Template(...)        _MZ_TAG("template",        __VA_ARGS__)
#define Textarea(...)        _MZ_TAG("textarea",        __VA_ARGS__)
#define Tfoot(...)           _MZ_TAG("tfoot",           __VA_ARGS__)
#define Th(...)              _MZ_TAG("th",              __VA_ARGS__)
#define Thead(...)           _MZ_TAG("thead",           __VA_ARGS__)
#define Time(...)            _MZ_TAG("time",            __VA_ARGS__)
#define Title(...)           _MZ_TAG("title",           __VA_ARGS__)
#define Tr(...)              _MZ_TAG("tr",              __VA_ARGS__)
#define U(...)               _MZ_TAG("u",               __VA_ARGS__)
#define Ul(...)              _MZ_TAG("ul",              __VA_ARGS__)
#define Var(...)             _MZ_TAG("var",             __VA_ARGS__)
#define Video(...)           _MZ_TAG("video",           __VA_ARGS__)

// HTMX 4 Elements
#define HxPartial(...)       _MZ_TAG("hx-partial",      __VA_ARGS__)

#endif
