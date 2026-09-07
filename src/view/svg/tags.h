#ifndef MIZAR_VIEW_SVG_TAGS_H
#define MIZAR_VIEW_SVG_TAGS_H

#include "core/buffer.h"
#include "view/svg/attrs.h"

static inline void mz_svg_tag_open(const char *tag, SvgAttrs attrs) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '<');
    mz_buf_append_str(buf, tag);
    mz_render_svg_attrs(buf, attrs);
    mz_buf_append_char(buf, '>');
}

static inline void mz_svg_tag_close(const char *tag) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_str(buf, "</");
    mz_buf_append_str(buf, tag);
    mz_buf_append_char(buf, '>');
}

static inline void mz_svg_void_tag(const char *tag, SvgAttrs attrs) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_char(buf, '<');
    mz_buf_append_str(buf, tag);
    mz_render_svg_attrs(buf, attrs);
    mz_buf_append_str(buf, " />");
}

#define _MZ_SVG_TAG(tag_name, ...) \
    for (int _mz_s_i = (mz_svg_tag_open(tag_name, (SvgAttrs){ __VA_ARGS__ }), 0); \
         !_mz_s_i; \
         _mz_s_i = 1, mz_svg_tag_close(tag_name))

// Root & Container elements
#define Svg(...)                 _MZ_SVG_TAG("svg",                 __VA_ARGS__)
#define Svg_G(...)               _MZ_SVG_TAG("g",                   __VA_ARGS__)
#define Svg_Defs(...)            _MZ_SVG_TAG("defs",                __VA_ARGS__)
#define Svg_Symbol(...)          _MZ_SVG_TAG("symbol",              __VA_ARGS__)
#define Svg_Marker(...)          _MZ_SVG_TAG("marker",              __VA_ARGS__)
#define Svg_Mask(...)            _MZ_SVG_TAG("mask",                __VA_ARGS__)
#define Svg_ClipPath(...)        _MZ_SVG_TAG("clipPath",            __VA_ARGS__)
#define Svg_Pattern(...)         _MZ_SVG_TAG("pattern",             __VA_ARGS__)
#define Svg_Switch(...)          _MZ_SVG_TAG("switch",              __VA_ARGS__)
#define Svg_A(...)               _MZ_SVG_TAG("a",                   __VA_ARGS__)
#define Svg_ForeignObject(...)   _MZ_SVG_TAG("foreignObject",       __VA_ARGS__)

// Descriptive elements
#define Svg_Desc(...)            _MZ_SVG_TAG("desc",                __VA_ARGS__)
#define Svg_Title(...)           _MZ_SVG_TAG("title",               __VA_ARGS__)
#define Svg_Metadata(...)        _MZ_SVG_TAG("metadata",            __VA_ARGS__)

// Shape elements (both container & self-closing supported)
#define Svg_Circle(...)          _MZ_SVG_TAG("circle",              __VA_ARGS__)
#define Svg_Circle_(...)         mz_svg_void_tag("circle",          (SvgAttrs){ __VA_ARGS__ })
#define Svg_Ellipse(...)         _MZ_SVG_TAG("ellipse",             __VA_ARGS__)
#define Svg_Ellipse_(...)        mz_svg_void_tag("ellipse",         (SvgAttrs){ __VA_ARGS__ })
#define Svg_Line(...)            _MZ_SVG_TAG("line",                __VA_ARGS__)
#define Svg_Line_(...)           mz_svg_void_tag("line",            (SvgAttrs){ __VA_ARGS__ })
#define Svg_Path(...)            _MZ_SVG_TAG("path",                __VA_ARGS__)
#define Svg_Path_(...)           mz_svg_void_tag("path",            (SvgAttrs){ __VA_ARGS__ })
#define Svg_Polygon(...)         _MZ_SVG_TAG("polygon",             __VA_ARGS__)
#define Svg_Polygon_(...)        mz_svg_void_tag("polygon",         (SvgAttrs){ __VA_ARGS__ })
#define Svg_Polyline(...)        _MZ_SVG_TAG("polyline",            __VA_ARGS__)
#define Svg_Polyline_(...)       mz_svg_void_tag("polyline",        (SvgAttrs){ __VA_ARGS__ })
#define Svg_Rect(...)            _MZ_SVG_TAG("rect",                __VA_ARGS__)
#define Svg_Rect_(...)           mz_svg_void_tag("rect",            (SvgAttrs){ __VA_ARGS__ })

// Graphics & Referencing
#define Svg_Image(...)           _MZ_SVG_TAG("image",               __VA_ARGS__)
#define Svg_Image_(...)          mz_svg_void_tag("image",           (SvgAttrs){ __VA_ARGS__ })
#define Svg_Use(...)             _MZ_SVG_TAG("use",                 __VA_ARGS__)
#define Svg_Use_(...)            mz_svg_void_tag("use",             (SvgAttrs){ __VA_ARGS__ })
#define Svg_View(...)            _MZ_SVG_TAG("view",                __VA_ARGS__)
#define Svg_View_(...)           mz_svg_void_tag("view",            (SvgAttrs){ __VA_ARGS__ })

// Text elements
#define Svg_Text(...)            _MZ_SVG_TAG("text",                __VA_ARGS__)
#define Svg_Tspan(...)           _MZ_SVG_TAG("tspan",               __VA_ARGS__)
#define Svg_TextPath(...)        _MZ_SVG_TAG("textPath",            __VA_ARGS__)

// Gradients & Stops
#define Svg_LinearGradient(...)  _MZ_SVG_TAG("linearGradient",      __VA_ARGS__)
#define Svg_RadialGradient(...)  _MZ_SVG_TAG("radialGradient",      __VA_ARGS__)
#define Svg_Stop(...)            _MZ_SVG_TAG("stop",                __VA_ARGS__)
#define Svg_Stop_(...)           mz_svg_void_tag("stop",            (SvgAttrs){ __VA_ARGS__ })

// Filter Elements & Filter Primitives
#define Svg_Filter(...)                 _MZ_SVG_TAG("filter",                 __VA_ARGS__)
#define Svg_FeBlend(...)                _MZ_SVG_TAG("feBlend",                __VA_ARGS__)
#define Svg_FeBlend_(...)               mz_svg_void_tag("feBlend",            (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeColorMatrix(...)          _MZ_SVG_TAG("feColorMatrix",          __VA_ARGS__)
#define Svg_FeColorMatrix_(...)         mz_svg_void_tag("feColorMatrix",      (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeComponentTransfer(...)   _MZ_SVG_TAG("feComponentTransfer",    __VA_ARGS__)
#define Svg_FeComposite(...)            _MZ_SVG_TAG("feComposite",            __VA_ARGS__)
#define Svg_FeComposite_(...)           mz_svg_void_tag("feComposite",        (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeConvolveMatrix(...)       _MZ_SVG_TAG("feConvolveMatrix",        __VA_ARGS__)
#define Svg_FeConvolveMatrix_(...)      mz_svg_void_tag("feConvolveMatrix",   (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeDiffuseLighting(...)      _MZ_SVG_TAG("feDiffuseLighting",       __VA_ARGS__)
#define Svg_FeDisplacementMap(...)      _MZ_SVG_TAG("feDisplacementMap",       __VA_ARGS__)
#define Svg_FeDisplacementMap_(...)     mz_svg_void_tag("feDisplacementMap",  (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeDistantLight(...)         _MZ_SVG_TAG("feDistantLight",          __VA_ARGS__)
#define Svg_FeDistantLight_(...)        mz_svg_void_tag("feDistantLight",     (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeDropShadow(...)           _MZ_SVG_TAG("feDropShadow",            __VA_ARGS__)
#define Svg_FeDropShadow_(...)          mz_svg_void_tag("feDropShadow",       (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeFlood(...)                _MZ_SVG_TAG("feFlood",                 __VA_ARGS__)
#define Svg_FeFlood_(...)               mz_svg_void_tag("feFlood",            (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeFuncA(...)                _MZ_SVG_TAG("feFuncA",                 __VA_ARGS__)
#define Svg_FeFuncA_(...)               mz_svg_void_tag("feFuncA",             (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeFuncB(...)                _MZ_SVG_TAG("feFuncB",                 __VA_ARGS__)
#define Svg_FeFuncB_(...)               mz_svg_void_tag("feFuncB",             (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeFuncG(...)                _MZ_SVG_TAG("feFuncG",                 __VA_ARGS__)
#define Svg_FeFuncG_(...)               mz_svg_void_tag("feFuncG",             (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeFuncR(...)                _MZ_SVG_TAG("feFuncR",                 __VA_ARGS__)
#define Svg_FeFuncR_(...)               mz_svg_void_tag("feFuncR",             (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeGaussianBlur(...)         _MZ_SVG_TAG("feGaussianBlur",          __VA_ARGS__)
#define Svg_FeGaussianBlur_(...)        mz_svg_void_tag("feGaussianBlur",     (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeImage(...)                _MZ_SVG_TAG("feImage",                 __VA_ARGS__)
#define Svg_FeImage_(...)               mz_svg_void_tag("feImage",             (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeMerge(...)                _MZ_SVG_TAG("feMerge",                 __VA_ARGS__)
#define Svg_FeMergeNode(...)            _MZ_SVG_TAG("feMergeNode",             __VA_ARGS__)
#define Svg_FeMergeNode_(...)           mz_svg_void_tag("feMergeNode",         (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeMorphology(...)           _MZ_SVG_TAG("feMorphology",            __VA_ARGS__)
#define Svg_FeMorphology_(...)          mz_svg_void_tag("feMorphology",        (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeOffset(...)               _MZ_SVG_TAG("feOffset",                __VA_ARGS__)
#define Svg_FeOffset_(...)              mz_svg_void_tag("feOffset",            (SvgAttrs){ __VA_ARGS__ })
#define Svg_FePointLight(...)           _MZ_SVG_TAG("fePointLight",            __VA_ARGS__)
#define Svg_FePointLight_(...)          mz_svg_void_tag("fePointLight",        (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeSpecularLighting(...)     _MZ_SVG_TAG("feSpecularLighting",      __VA_ARGS__)
#define Svg_FeSpotLight(...)            _MZ_SVG_TAG("feSpotLight",             __VA_ARGS__)
#define Svg_FeSpotLight_(...)           mz_svg_void_tag("feSpotLight",         (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeTile(...)                 _MZ_SVG_TAG("feTile",                  __VA_ARGS__)
#define Svg_FeTile_(...)                mz_svg_void_tag("feTile",              (SvgAttrs){ __VA_ARGS__ })
#define Svg_FeTurbulence(...)           _MZ_SVG_TAG("feTurbulence",            __VA_ARGS__)
#define Svg_FeTurbulence_(...)          mz_svg_void_tag("feTurbulence",        (SvgAttrs){ __VA_ARGS__ })

// Animation elements
#define Svg_Animate(...)                _MZ_SVG_TAG("animate",                __VA_ARGS__)
#define Svg_Animate_(...)               mz_svg_void_tag("animate",            (SvgAttrs){ __VA_ARGS__ })
#define Svg_AnimateMotion(...)          _MZ_SVG_TAG("animateMotion",          __VA_ARGS__)
#define Svg_AnimateMotion_(...)         mz_svg_void_tag("animateMotion",      (SvgAttrs){ __VA_ARGS__ })
#define Svg_AnimateTransform(...)       _MZ_SVG_TAG("animateTransform",       __VA_ARGS__)
#define Svg_AnimateTransform_(...)      mz_svg_void_tag("animateTransform",   (SvgAttrs){ __VA_ARGS__ })
#define Svg_Mpath(...)                  _MZ_SVG_TAG("mpath",                  __VA_ARGS__)
#define Svg_Mpath_(...)                 mz_svg_void_tag("mpath",              (SvgAttrs){ __VA_ARGS__ })
#define Svg_Set(...)                    _MZ_SVG_TAG("set",                    __VA_ARGS__)
#define Svg_Set_(...)                   mz_svg_void_tag("set",                (SvgAttrs){ __VA_ARGS__ })

// Embedded Script/Style within SVG
#define Svg_Script(...)                 _MZ_SVG_TAG("script",                 __VA_ARGS__)
#define Svg_Style(...)                  _MZ_SVG_TAG("style",                  __VA_ARGS__)

#endif
