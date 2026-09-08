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

// Canonical prefixed HTML void elements
#define MzArea(...)   mz_void_tag("area",   (Attrs){ __VA_ARGS__ })
#define MzBase(...)   mz_void_tag("base",   (Attrs){ __VA_ARGS__ })
#define MzBr(...)     mz_void_tag("br",     (Attrs){ __VA_ARGS__ })
#define MzCol(...)    mz_void_tag("col",    (Attrs){ __VA_ARGS__ })
#define MzEmbed(...)  mz_void_tag("embed",  (Attrs){ __VA_ARGS__ })
#define MzHr(...)     mz_void_tag("hr",     (Attrs){ __VA_ARGS__ })
#define MzImg(...)    mz_void_tag("img",    (Attrs){ __VA_ARGS__ })
#define MzInput(...)  mz_void_tag("input",  (Attrs){ __VA_ARGS__ })
#define MzLink(...)   mz_void_tag("link",   (Attrs){ __VA_ARGS__ })
#define MzMeta(...)   mz_void_tag("meta",   (Attrs){ __VA_ARGS__ })
#define MzSource(...) mz_void_tag("source", (Attrs){ __VA_ARGS__ })
#define MzTrack(...)  mz_void_tag("track",  (Attrs){ __VA_ARGS__ })
#define MzWbr(...)    mz_void_tag("wbr",    (Attrs){ __VA_ARGS__ })

// Canonical prefixed HTML container elements
#define MzA(...)               _MZ_TAG("a",               __VA_ARGS__)
#define MzAbbr(...)            _MZ_TAG("abbr",            __VA_ARGS__)
#define MzAddress(...)         _MZ_TAG("address",         __VA_ARGS__)
#define MzArticle(...)         _MZ_TAG("article",         __VA_ARGS__)
#define MzAside(...)           _MZ_TAG("aside",           __VA_ARGS__)
#define MzAudio(...)           _MZ_TAG("audio",           __VA_ARGS__)
#define MzB(...)               _MZ_TAG("b",               __VA_ARGS__)
#define MzBdi(...)             _MZ_TAG("bdi",             __VA_ARGS__)
#define MzBdo(...)             _MZ_TAG("bdo",             __VA_ARGS__)
#define MzBlockquote(...)      _MZ_TAG("blockquote",      __VA_ARGS__)
#define MzBody(...)            _MZ_TAG("body",            __VA_ARGS__)
#define MzButton(...)          _MZ_TAG("button",          __VA_ARGS__)
#define MzCanvas(...)          _MZ_TAG("canvas",          __VA_ARGS__)
#define MzCaption(...)         _MZ_TAG("caption",         __VA_ARGS__)
#define MzCite(...)            _MZ_TAG("cite",            __VA_ARGS__)
#define MzCode(...)            _MZ_TAG("code",            __VA_ARGS__)
#define MzColgroup(...)        _MZ_TAG("colgroup",        __VA_ARGS__)
#define MzData(...)            _MZ_TAG("data",            __VA_ARGS__)
#define MzDatalist(...)        _MZ_TAG("datalist",        __VA_ARGS__)
#define MzDd(...)              _MZ_TAG("dd",              __VA_ARGS__)
#define MzDel(...)             _MZ_TAG("del",             __VA_ARGS__)
#define MzDetails(...)         _MZ_TAG("details",         __VA_ARGS__)
#define MzDfn(...)             _MZ_TAG("dfn",             __VA_ARGS__)
#define MzDialog(...)          _MZ_TAG("dialog",          __VA_ARGS__)
#define MzDiv(...)             _MZ_TAG("div",             __VA_ARGS__)
#define MzDl(...)              _MZ_TAG("dl",              __VA_ARGS__)
#define MzDt(...)              _MZ_TAG("dt",              __VA_ARGS__)
#define MzEm(...)              _MZ_TAG("em",              __VA_ARGS__)
#define MzFencedframe(...)     _MZ_TAG("fencedframe",     __VA_ARGS__)
#define MzFieldset(...)        _MZ_TAG("fieldset",        __VA_ARGS__)
#define MzFigcaption(...)      _MZ_TAG("figcaption",      __VA_ARGS__)
#define MzFigure(...)          _MZ_TAG("figure",          __VA_ARGS__)
#define MzFooter(...)          _MZ_TAG("footer",          __VA_ARGS__)
#define MzForm(...)            _MZ_TAG("form",            __VA_ARGS__)
#define MzGeolocation(...)     _MZ_TAG("geolocation",     __VA_ARGS__)
#define MzH1(...)              _MZ_TAG("h1",              __VA_ARGS__)
#define MzH2(...)              _MZ_TAG("h2",              __VA_ARGS__)
#define MzH3(...)              _MZ_TAG("h3",              __VA_ARGS__)
#define MzH4(...)              _MZ_TAG("h4",              __VA_ARGS__)
#define MzH5(...)              _MZ_TAG("h5",              __VA_ARGS__)
#define MzH6(...)              _MZ_TAG("h6",              __VA_ARGS__)
#define MzHead(...)            _MZ_TAG("head",            __VA_ARGS__)
#define MzHeader(...)          _MZ_TAG("header",          __VA_ARGS__)
#define MzHgroup(...)          _MZ_TAG("hgroup",          __VA_ARGS__)
#define MzI(...)               _MZ_TAG("i",               __VA_ARGS__)
#define MzIframe(...)          _MZ_TAG("iframe",          __VA_ARGS__)
#define MzIns(...)             _MZ_TAG("ins",             __VA_ARGS__)
#define MzKbd(...)             _MZ_TAG("kbd",             __VA_ARGS__)
#define MzLabel(...)           _MZ_TAG("label",           __VA_ARGS__)
#define MzLegend(...)          _MZ_TAG("legend",          __VA_ARGS__)
#define MzLi(...)              _MZ_TAG("li",              __VA_ARGS__)
#define MzMain(...)            _MZ_TAG("main",            __VA_ARGS__)
#define MzMap(...)             _MZ_TAG("map",             __VA_ARGS__)
#define MzMark(...)            _MZ_TAG("mark",            __VA_ARGS__)
#define MzMenu(...)            _MZ_TAG("menu",            __VA_ARGS__)
#define MzMeter(...)           _MZ_TAG("meter",           __VA_ARGS__)
#define MzNav(...)             _MZ_TAG("nav",             __VA_ARGS__)
#define MzNoscript(...)        _MZ_TAG("noscript",        __VA_ARGS__)
#define MzObject(...)          _MZ_TAG("object",          __VA_ARGS__)
#define MzOl(...)              _MZ_TAG("ol",              __VA_ARGS__)
#define MzOptgroup(...)        _MZ_TAG("optgroup",        __VA_ARGS__)
#define MzOption(...)          _MZ_TAG("option",          __VA_ARGS__)
#define MzOutput(...)          _MZ_TAG("output",          __VA_ARGS__)
#define MzP(...)               _MZ_TAG("p",               __VA_ARGS__)
#define MzPicture(...)         _MZ_TAG("picture",         __VA_ARGS__)
#define MzPortal(...)          _MZ_TAG("portal",          __VA_ARGS__)
#define MzPre(...)             _MZ_TAG("pre",             __VA_ARGS__)
#define MzProgress(...)        _MZ_TAG("progress",        __VA_ARGS__)
#define MzQ(...)               _MZ_TAG("q",               __VA_ARGS__)
#define MzRb(...)              _MZ_TAG("rb",              __VA_ARGS__)
#define MzRp(...)              _MZ_TAG("rp",              __VA_ARGS__)
#define MzRt(...)              _MZ_TAG("rt",              __VA_ARGS__)
#define MzRtc(...)             _MZ_TAG("rtc",             __VA_ARGS__)
#define MzRuby(...)            _MZ_TAG("ruby",            __VA_ARGS__)
#define MzS(...)               _MZ_TAG("s",               __VA_ARGS__)
#define MzSamp(...)            _MZ_TAG("samp",            __VA_ARGS__)
#define MzScript(...)          _MZ_TAG("script",          __VA_ARGS__)
#define MzSearchTag(...)       _MZ_TAG("search",          __VA_ARGS__)
#define MzSection(...)         _MZ_TAG("section",         __VA_ARGS__)
#define MzSelect(...)          _MZ_TAG("select",          __VA_ARGS__)
#define MzSelectedcontent(...) _MZ_TAG("selectedcontent", __VA_ARGS__)
#define MzSlot(...)            _MZ_TAG("slot",            __VA_ARGS__)
#define MzSmall(...)           _MZ_TAG("small",           __VA_ARGS__)
#define MzSpan(...)            _MZ_TAG("span",            __VA_ARGS__)
#define MzStrong(...)          _MZ_TAG("strong",          __VA_ARGS__)
#define MzStyle(...)           _MZ_TAG("style",           __VA_ARGS__)
#define MzSub(...)             _MZ_TAG("sub",             __VA_ARGS__)
#define MzSummary(...)         _MZ_TAG("summary",         __VA_ARGS__)
#define MzSup(...)             _MZ_TAG("sup",             __VA_ARGS__)
#define MzTable(...)           _MZ_TAG("table",           __VA_ARGS__)
#define MzTbody(...)           _MZ_TAG("tbody",           __VA_ARGS__)
#define MzTd(...)              _MZ_TAG("td",              __VA_ARGS__)
#define MzTemplate(...)        _MZ_TAG("template",        __VA_ARGS__)
#define MzTextarea(...)        _MZ_TAG("textarea",        __VA_ARGS__)
#define MzTfoot(...)           _MZ_TAG("tfoot",           __VA_ARGS__)
#define MzTh(...)              _MZ_TAG("th",              __VA_ARGS__)
#define MzThead(...)           _MZ_TAG("thead",           __VA_ARGS__)
#define MzTime(...)            _MZ_TAG("time",            __VA_ARGS__)
#define MzTitle(...)           _MZ_TAG("title",           __VA_ARGS__)
#define MzTr(...)              _MZ_TAG("tr",              __VA_ARGS__)
#define MzU(...)               _MZ_TAG("u",               __VA_ARGS__)
#define MzUl(...)              _MZ_TAG("ul",              __VA_ARGS__)
#define MzVar(...)             _MZ_TAG("var",             __VA_ARGS__)
#define MzVideo(...)           _MZ_TAG("video",           __VA_ARGS__)

#ifndef MIZAR_NO_SHORT_TAGS
// Void Elements (self-closing shorthand)
#define Area(...)   MzArea(__VA_ARGS__)
#define Base(...)   MzBase(__VA_ARGS__)
#define Br(...)     MzBr(__VA_ARGS__)
#define Col(...)    MzCol(__VA_ARGS__)
#define Embed(...)  MzEmbed(__VA_ARGS__)
#define Hr(...)     MzHr(__VA_ARGS__)
#define Img(...)    MzImg(__VA_ARGS__)
#define Input(...)  MzInput(__VA_ARGS__)
#define Link(...)   MzLink(__VA_ARGS__)
#define Meta(...)   MzMeta(__VA_ARGS__)
#define Source(...) MzSource(__VA_ARGS__)
#define Track(...)  MzTrack(__VA_ARGS__)
#define Wbr(...)    MzWbr(__VA_ARGS__)

// Container Elements shorthand
#define A(...)               MzA(__VA_ARGS__)
#define Abbr(...)            MzAbbr(__VA_ARGS__)
#define Address(...)         MzAddress(__VA_ARGS__)
#define Article(...)         MzArticle(__VA_ARGS__)
#define Aside(...)           MzAside(__VA_ARGS__)
#define Audio(...)           MzAudio(__VA_ARGS__)
#define B(...)               MzB(__VA_ARGS__)
#define Bdi(...)             MzBdi(__VA_ARGS__)
#define Bdo(...)             MzBdo(__VA_ARGS__)
#define Blockquote(...)      MzBlockquote(__VA_ARGS__)
#define Body(...)            MzBody(__VA_ARGS__)
#define Button(...)          MzButton(__VA_ARGS__)
#define Canvas(...)          MzCanvas(__VA_ARGS__)
#define Caption(...)         MzCaption(__VA_ARGS__)
#define Cite(...)            MzCite(__VA_ARGS__)
#define Code(...)            MzCode(__VA_ARGS__)
#define Colgroup(...)        MzColgroup(__VA_ARGS__)
#define Data(...)            MzData(__VA_ARGS__)
#define Datalist(...)        MzDatalist(__VA_ARGS__)
#define Dd(...)              MzDd(__VA_ARGS__)
#define Del(...)             MzDel(__VA_ARGS__)
#define Details(...)         MzDetails(__VA_ARGS__)
#define Dfn(...)             MzDfn(__VA_ARGS__)
#define Dialog(...)          MzDialog(__VA_ARGS__)
#define Div(...)             MzDiv(__VA_ARGS__)
#define Dl(...)              MzDl(__VA_ARGS__)
#define Dt(...)              MzDt(__VA_ARGS__)
#define Em(...)              MzEm(__VA_ARGS__)
#define Fencedframe(...)     MzFencedframe(__VA_ARGS__)
#define Fieldset(...)        MzFieldset(__VA_ARGS__)
#define Figcaption(...)      MzFigcaption(__VA_ARGS__)
#define Figure(...)          MzFigure(__VA_ARGS__)
#define Footer(...)          MzFooter(__VA_ARGS__)
#define Form(...)            MzForm(__VA_ARGS__)
#define Geolocation(...)     MzGeolocation(__VA_ARGS__)
#define H1(...)              MzH1(__VA_ARGS__)
#define H2(...)              MzH2(__VA_ARGS__)
#define H3(...)              MzH3(__VA_ARGS__)
#define H4(...)              MzH4(__VA_ARGS__)
#define H5(...)              MzH5(__VA_ARGS__)
#define H6(...)              MzH6(__VA_ARGS__)
#define Head(...)            MzHead(__VA_ARGS__)
#define Header(...)          MzHeader(__VA_ARGS__)
#define Hgroup(...)          MzHgroup(__VA_ARGS__)
#define I(...)               MzI(__VA_ARGS__)
#define Iframe(...)          MzIframe(__VA_ARGS__)
#define Ins(...)             MzIns(__VA_ARGS__)
#define Kbd(...)             MzKbd(__VA_ARGS__)
#define Label(...)           MzLabel(__VA_ARGS__)
#define Legend(...)          MzLegend(__VA_ARGS__)
#define Li(...)              MzLi(__VA_ARGS__)
#define Main(...)            MzMain(__VA_ARGS__)
#define Map(...)             MzMap(__VA_ARGS__)
#define Mark(...)            MzMark(__VA_ARGS__)
#define Menu(...)            MzMenu(__VA_ARGS__)
#define Meter(...)           MzMeter(__VA_ARGS__)
#define Nav(...)             MzNav(__VA_ARGS__)
#define Noscript(...)        MzNoscript(__VA_ARGS__)
#define Object(...)          MzObject(__VA_ARGS__)
#define Ol(...)              MzOl(__VA_ARGS__)
#define Optgroup(...)        MzOptgroup(__VA_ARGS__)
#define Option(...)          MzOption(__VA_ARGS__)
#define Output(...)          MzOutput(__VA_ARGS__)
#define P(...)               MzP(__VA_ARGS__)
#define Picture(...)         MzPicture(__VA_ARGS__)
#define Portal(...)          MzPortal(__VA_ARGS__)
#define Pre(...)             MzPre(__VA_ARGS__)
#define Progress(...)        MzProgress(__VA_ARGS__)
#define Q(...)               MzQ(__VA_ARGS__)
#define Rb(...)              MzRb(__VA_ARGS__)
#define Rp(...)              MzRp(__VA_ARGS__)
#define Rt(...)              MzRt(__VA_ARGS__)
#define Rtc(...)             MzRtc(__VA_ARGS__)
#define Ruby(...)            MzRuby(__VA_ARGS__)
#define S(...)               MzS(__VA_ARGS__)
#define Samp(...)            MzSamp(__VA_ARGS__)
#define Script(...)          MzScript(__VA_ARGS__)
#define SearchTag(...)       MzSearchTag(__VA_ARGS__)
#define Section(...)         MzSection(__VA_ARGS__)
#define Select(...)          MzSelect(__VA_ARGS__)
#define Selectedcontent(...) MzSelectedcontent(__VA_ARGS__)
#define Slot(...)            MzSlot(__VA_ARGS__)
#define Small(...)           MzSmall(__VA_ARGS__)
#define Span(...)            MzSpan(__VA_ARGS__)
#define Strong(...)          MzStrong(__VA_ARGS__)
#define Style(...)           MzStyle(__VA_ARGS__)
#define Sub(...)             MzSub(__VA_ARGS__)
#define Summary(...)         MzSummary(__VA_ARGS__)
#define Sup(...)             MzSup(__VA_ARGS__)
#define Table(...)           MzTable(__VA_ARGS__)
#define Tbody(...)           MzTbody(__VA_ARGS__)
#define Td(...)              MzTd(__VA_ARGS__)
#define Template(...)        MzTemplate(__VA_ARGS__)
#define Textarea(...)        MzTextarea(__VA_ARGS__)
#define Tfoot(...)           MzTfoot(__VA_ARGS__)
#define Th(...)              MzTh(__VA_ARGS__)
#define Thead(...)           MzThead(__VA_ARGS__)
#define Time(...)            MzTime(__VA_ARGS__)
#define Title(...)           MzTitle(__VA_ARGS__)
#define Tr(...)              MzTr(__VA_ARGS__)
#define U(...)               MzU(__VA_ARGS__)
#define Ul(...)              MzUl(__VA_ARGS__)
#define Var(...)             MzVar(__VA_ARGS__)
#define Video(...)           MzVideo(__VA_ARGS__)
#endif

// HTMX 4 Elements
#define HxPartial(...)       _MZ_TAG("hx-partial",      __VA_ARGS__)

#endif
