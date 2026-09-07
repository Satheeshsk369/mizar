#ifndef MIZAR_VIEW_MATHML_TAGS_H
#define MIZAR_VIEW_MATHML_TAGS_H

#include "core/buffer.h"
#include "view/mathml/attrs.h"

static inline void mz_math_tag_open(const char *tag, MathAttrs attrs) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '<');
    mz_buf_append_str(buf, tag);
    mz_render_math_attrs(buf, attrs);
    mz_buf_append_char(buf, '>');
}

static inline void mz_math_tag_close(const char *tag) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_str(buf, "</");
    mz_buf_append_str(buf, tag);
    mz_buf_append_char(buf, '>');
}

static inline void mz_math_void_tag(const char *tag, MathAttrs attrs) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '<');
    mz_buf_append_str(buf, tag);
    mz_render_math_attrs(buf, attrs);
    mz_buf_append_str(buf, " />");
}

#define _MZ_MATH_TAG(tag_name, ...) \
    for (int _mz_m_i = (mz_math_tag_open(tag_name, (MathAttrs){ __VA_ARGS__ }), 0); \
         !_mz_m_i; \
         _mz_m_i = 1, mz_math_tag_close(tag_name))

// Standalone MathML document support with buffer context push/pop
#define MathDoc(buf, ...) \
    for (int _mz_mdoc = (mz_context_push(buf), \
                         mz_math_tag_open("math", (MathAttrs){ .xmlns = "http://www.w3.org/1998/Math/MathML", __VA_ARGS__ }), 0); \
         !_mz_mdoc; \
         _mz_mdoc = 1, mz_math_tag_close("math"), mz_context_pop())

// Top-level MathML container (embedded in Html)
#define Math(...)             _MZ_MATH_TAG("math",             __VA_ARGS__)

// Token elements
#define Math_Mi(...)          _MZ_MATH_TAG("mi",               __VA_ARGS__)
#define Math_Mn(...)          _MZ_MATH_TAG("mn",               __VA_ARGS__)
#define Math_Mo(...)          _MZ_MATH_TAG("mo",               __VA_ARGS__)
#define Math_Ms(...)          _MZ_MATH_TAG("ms",               __VA_ARGS__)
#define Math_Mtext(...)       _MZ_MATH_TAG("mtext",            __VA_ARGS__)
#define Math_Mspace(...)      _MZ_MATH_TAG("mspace",           __VA_ARGS__)
#define Math_Mspace_(...)     mz_math_void_tag("mspace",       (MathAttrs){ __VA_ARGS__ })

// General layout
#define Math_Mrow(...)        _MZ_MATH_TAG("mrow",             __VA_ARGS__)
#define Math_Mfrac(...)       _MZ_MATH_TAG("mfrac",            __VA_ARGS__)
#define Math_Msqrt(...)       _MZ_MATH_TAG("msqrt",            __VA_ARGS__)
#define Math_Mroot(...)       _MZ_MATH_TAG("mroot",            __VA_ARGS__)
#define Math_Mstyle(...)      _MZ_MATH_TAG("mstyle",           __VA_ARGS__)
#define Math_Merror(...)      _MZ_MATH_TAG("merror",           __VA_ARGS__)
#define Math_Mpadded(...)     _MZ_MATH_TAG("mpadded",          __VA_ARGS__)
#define Math_Mphantom(...)    _MZ_MATH_TAG("mphantom",         __VA_ARGS__)
#define Math_Menclose(...)    _MZ_MATH_TAG("menclose",         __VA_ARGS__)
#define Math_Mfenced(...)     _MZ_MATH_TAG("mfenced",          __VA_ARGS__)

// Script and limit elements
#define Math_Msub(...)        _MZ_MATH_TAG("msub",             __VA_ARGS__)
#define Math_Msup(...)        _MZ_MATH_TAG("msup",             __VA_ARGS__)
#define Math_Msubsup(...)     _MZ_MATH_TAG("msubsup",          __VA_ARGS__)
#define Math_Munder(...)      _MZ_MATH_TAG("munder",           __VA_ARGS__)
#define Math_Mover(...)       _MZ_MATH_TAG("mover",            __VA_ARGS__)
#define Math_Munderover(...)  _MZ_MATH_TAG("munderover",       __VA_ARGS__)
#define Math_Mmultiscripts(...) _MZ_MATH_TAG("mmultiscripts",  __VA_ARGS__)
#define Math_Mprescripts(...) _MZ_MATH_TAG("mprescripts",      __VA_ARGS__)
#define Math_Mprescripts_(...) mz_math_void_tag("mprescripts", (MathAttrs){ __VA_ARGS__ })
#define Math_None(...)        _MZ_MATH_TAG("none",             __VA_ARGS__)
#define Math_None_(...)       mz_math_void_tag("none",         (MathAttrs){ __VA_ARGS__ })

// Tabular math
#define Math_Mtable(...)      _MZ_MATH_TAG("mtable",           __VA_ARGS__)
#define Math_Mtr(...)         _MZ_MATH_TAG("mtr",              __VA_ARGS__)
#define Math_Mtd(...)         _MZ_MATH_TAG("mtd",              __VA_ARGS__)

// Semantic Annotations
#define Math_Semantics(...)   _MZ_MATH_TAG("semantics",        __VA_ARGS__)
#define Math_Annotation(...)  _MZ_MATH_TAG("annotation",       __VA_ARGS__)
#define Math_AnnotationXml(...) _MZ_MATH_TAG("annotation-xml", __VA_ARGS__)

// Uncategorized / Interactive
#define Math_Maction(...)     _MZ_MATH_TAG("maction",          __VA_ARGS__)

#endif
