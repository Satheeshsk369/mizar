#ifndef MIZAR_VIEW_CSS_RULES_H
#define MIZAR_VIEW_CSS_RULES_H

#include "core/buffer.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Type-Safe CSS Keywords & Units
// -----------------------------------------------------------------------------
typedef enum {
    CSS_NONE = 0,

    // Display
    CSS_BLOCK,
    CSS_INLINE,
    CSS_INLINE_BLOCK,
    CSS_FLEX,
    CSS_INLINE_FLEX,
    CSS_GRID,
    CSS_INLINE_GRID,
    CSS_CONTENTS,

    // Position
    CSS_STATIC,
    CSS_RELATIVE,
    CSS_ABSOLUTE,
    CSS_FIXED,
    CSS_STICKY,

    // Flex direction
    CSS_ROW,
    CSS_ROW_REVERSE,
    CSS_COLUMN,
    CSS_COLUMN_REVERSE,

    // Flex wrap
    CSS_NOWRAP,
    CSS_WRAP,
    CSS_WRAP_REVERSE,

    // Alignment & Justification
    CSS_CENTER,
    CSS_START,
    CSS_END,
    CSS_FLEX_START,
    CSS_FLEX_END,
    CSS_SPACE_BETWEEN,
    CSS_SPACE_AROUND,
    CSS_SPACE_EVENLY,
    CSS_STRETCH,
    CSS_BASELINE,

    // Text & Visibility
    CSS_LEFT,
    CSS_RIGHT,
    CSS_JUSTIFY,
    CSS_VISIBLE,
    CSS_HIDDEN,
    CSS_COLLAPSE,

    // Overflow
    CSS_AUTO,
    CSS_SCROLL,
    CSS_CLIP,

    // Box sizing
    CSS_BORDER_BOX,
    CSS_CONTENT_BOX,

    // Pointer events & user select
    CSS_POINTER,
    CSS_NOT_ALLOWED,
    CSS_ALL,

    // Cursor
    CSS_DEFAULT,
    CSS_MOVE,
    CSS_GRAB
} CssKeyword;

static inline const char *mz_css_keyword_str(CssKeyword kw) {
    switch (kw) {
        case CSS_BLOCK: return "block";
        case CSS_INLINE: return "inline";
        case CSS_INLINE_BLOCK: return "inline-block";
        case CSS_FLEX: return "flex";
        case CSS_INLINE_FLEX: return "inline-flex";
        case CSS_GRID: return "grid";
        case CSS_INLINE_GRID: return "inline-grid";
        case CSS_CONTENTS: return "contents";

        case CSS_STATIC: return "static";
        case CSS_RELATIVE: return "relative";
        case CSS_ABSOLUTE: return "absolute";
        case CSS_FIXED: return "fixed";
        case CSS_STICKY: return "sticky";

        case CSS_ROW: return "row";
        case CSS_ROW_REVERSE: return "row-reverse";
        case CSS_COLUMN: return "column";
        case CSS_COLUMN_REVERSE: return "column-reverse";

        case CSS_NOWRAP: return "nowrap";
        case CSS_WRAP: return "wrap";
        case CSS_WRAP_REVERSE: return "wrap-reverse";

        case CSS_CENTER: return "center";
        case CSS_START: return "start";
        case CSS_END: return "end";
        case CSS_FLEX_START: return "flex-start";
        case CSS_FLEX_END: return "flex-end";
        case CSS_SPACE_BETWEEN: return "space-between";
        case CSS_SPACE_AROUND: return "space-around";
        case CSS_SPACE_EVENLY: return "space-evenly";
        case CSS_STRETCH: return "stretch";
        case CSS_BASELINE: return "baseline";

        case CSS_LEFT: return "left";
        case CSS_RIGHT: return "right";
        case CSS_JUSTIFY: return "justify";
        case CSS_VISIBLE: return "visible";
        case CSS_HIDDEN: return "hidden";
        case CSS_COLLAPSE: return "collapse";

        case CSS_AUTO: return "auto";
        case CSS_SCROLL: return "scroll";
        case CSS_CLIP: return "clip";

        case CSS_BORDER_BOX: return "border-box";
        case CSS_CONTENT_BOX: return "content-box";

        case CSS_POINTER: return "pointer";
        case CSS_NOT_ALLOWED: return "not-allowed";
        case CSS_ALL: return "all";

        case CSS_DEFAULT: return "default";
        case CSS_MOVE: return "move";
        case CSS_GRAB: return "grab";

        case CSS_NONE:
        default: return "none";
    }
}

// Type-safe dimension structure
typedef struct {
    double val;
    const char *unit; // "px", "rem", "em", "%", "vh", "vw", "ch", "auto"
    bool active;
} CssDim;

#define Px(v)     (CssDim){ .val = (double)(v), .unit = "px",   .active = true }
#define Rem(v)    (CssDim){ .val = (double)(v), .unit = "rem",  .active = true }
#define CssEm(v)  (CssDim){ .val = (double)(v), .unit = "em",   .active = true }
#define Pct(v)    (CssDim){ .val = (double)(v), .unit = "%",    .active = true }
#define Vh(v)     (CssDim){ .val = (double)(v), .unit = "vh",   .active = true }
#define Vw(v)     (CssDim){ .val = (double)(v), .unit = "vw",   .active = true }
#define Ch(v)     (CssDim){ .val = (double)(v), .unit = "ch",   .active = true }
#define Auto      (CssDim){ .val = 0.0,         .unit = "auto", .active = true }
#define Zero      (CssDim){ .val = 0.0,         .unit = "px",   .active = true }

// -----------------------------------------------------------------------------
// Type-Safe CSS Properties Struct
// -----------------------------------------------------------------------------
typedef struct {
    // Layout & Flow
    CssKeyword display;
    CssKeyword position;
    CssKeyword box_sizing;
    CssKeyword overflow;
    CssKeyword overflow_x;
    CssKeyword overflow_y;
    int z_index;
    bool has_z_index;

    // Flexbox
    CssKeyword flex_direction;
    CssKeyword flex_wrap;
    CssKeyword justify_content;
    CssKeyword align_items;
    CssKeyword align_self;
    CssKeyword align_content;
    double flex_grow;
    bool has_flex_grow;
    double flex_shrink;
    bool has_flex_shrink;

    // Dimensions & Spacing
    CssDim width;
    CssDim height;
    CssDim min_width;
    CssDim min_height;
    CssDim max_width;
    CssDim max_height;

    CssDim padding;
    CssDim padding_top;
    CssDim padding_bottom;
    CssDim padding_left;
    CssDim padding_right;

    CssDim margin;
    CssDim margin_top;
    CssDim margin_bottom;
    CssDim margin_left;
    CssDim margin_right;

    CssDim top;
    CssDim bottom;
    CssDim left;
    CssDim right;

    CssDim gap;
    CssDim row_gap;
    CssDim column_gap;

    // Borders & Outlines
    CssDim border_radius;
    const char *border;
    const char *border_top;
    const char *border_bottom;
    const char *border_left;
    const char *border_right;
    const char *outline;

    // Colors & Backgrounds
    const char *color;
    const char *background;
    const char *background_color;
    double opacity;
    bool has_opacity;

    // Typography
    const char *font_family;
    CssDim font_size;
    const char *font_weight;
    const char *line_height;
    CssKeyword text_align;
    const char *text_decoration;
    const char *text_transform;
    const char *letter_spacing;

    // Visual Effects & UI
    const char *box_shadow;
    const char *transition;
    const char *transform;
    CssKeyword cursor;
    CssKeyword user_select;
    CssKeyword pointer_events;

    // Escape hatch for custom/CSS variables (e.g. "--primary: #fff;")
    const char *custom;
} CssProps;

// -----------------------------------------------------------------------------
// CSS Serializer Helpers
// -----------------------------------------------------------------------------
static inline void mz_css_print_dim(MizarBuffer *buf, const char *prop, CssDim dim) {
    if (!dim.active) return;
    mz_buf_append_str(buf, "  ");
    mz_buf_append_str(buf, prop);
    mz_buf_append_str(buf, ": ");
    if (strcmp(dim.unit, "auto") == 0) {
        mz_buf_append_str(buf, "auto;\n");
    } else if (dim.val == (long)dim.val) {
        char val_str[32];
        snprintf(val_str, sizeof(val_str), "%ld", (long)dim.val);
        mz_buf_append_str(buf, val_str);
        mz_buf_append_str(buf, dim.unit);
        mz_buf_append_str(buf, ";\n");
    } else {
        char val_str[32];
        snprintf(val_str, sizeof(val_str), "%.2f", dim.val);
        mz_buf_append_str(buf, val_str);
        mz_buf_append_str(buf, dim.unit);
        mz_buf_append_str(buf, ";\n");
    }
}

static inline void mz_css_print_kw(MizarBuffer *buf, const char *prop, CssKeyword kw) {
    if (kw == CSS_NONE) return;
    mz_buf_append_str(buf, "  ");
    mz_buf_append_str(buf, prop);
    mz_buf_append_str(buf, ": ");
    mz_buf_append_str(buf, mz_css_keyword_str(kw));
    mz_buf_append_str(buf, ";\n");
}

static inline void mz_css_print_str(MizarBuffer *buf, const char *prop, const char *val) {
    if (!val) return;
    mz_buf_append_str(buf, "  ");
    mz_buf_append_str(buf, prop);
    mz_buf_append_str(buf, ": ");
    mz_buf_append_str(buf, val);
    mz_buf_append_str(buf, ";\n");
}

static inline void mz_css_render_props(MizarBuffer *buf, CssProps p) {
    // Layout
    mz_css_print_kw(buf, "display", p.display);
    mz_css_print_kw(buf, "position", p.position);
    mz_css_print_kw(buf, "box-sizing", p.box_sizing);
    mz_css_print_kw(buf, "overflow", p.overflow);
    mz_css_print_kw(buf, "overflow-x", p.overflow_x);
    mz_css_print_kw(buf, "overflow-y", p.overflow_y);
    if (p.has_z_index) {
        char z_str[32];
        snprintf(z_str, sizeof(z_str), "%d", p.z_index);
        mz_css_print_str(buf, "z-index", z_str);
    }

    // Flexbox
    mz_css_print_kw(buf, "flex-direction", p.flex_direction);
    mz_css_print_kw(buf, "flex-wrap", p.flex_wrap);
    mz_css_print_kw(buf, "justify-content", p.justify_content);
    mz_css_print_kw(buf, "align-items", p.align_items);
    mz_css_print_kw(buf, "align-self", p.align_self);
    mz_css_print_kw(buf, "align-content", p.align_content);
    if (p.has_flex_grow) {
        char g_str[32];
        snprintf(g_str, sizeof(g_str), "%.2g", p.flex_grow);
        mz_css_print_str(buf, "flex-grow", g_str);
    }
    if (p.has_flex_shrink) {
        char s_str[32];
        snprintf(s_str, sizeof(s_str), "%.2g", p.flex_shrink);
        mz_css_print_str(buf, "flex-shrink", s_str);
    }

    // Dimensions
    mz_css_print_dim(buf, "width", p.width);
    mz_css_print_dim(buf, "height", p.height);
    mz_css_print_dim(buf, "min-width", p.min_width);
    mz_css_print_dim(buf, "min-height", p.min_height);
    mz_css_print_dim(buf, "max-width", p.max_width);
    mz_css_print_dim(buf, "max-height", p.max_height);

    // Padding & Margins
    mz_css_print_dim(buf, "padding", p.padding);
    mz_css_print_dim(buf, "padding-top", p.padding_top);
    mz_css_print_dim(buf, "padding-bottom", p.padding_bottom);
    mz_css_print_dim(buf, "padding-left", p.padding_left);
    mz_css_print_dim(buf, "padding-right", p.padding_right);

    mz_css_print_dim(buf, "margin", p.margin);
    mz_css_print_dim(buf, "margin-top", p.margin_top);
    mz_css_print_dim(buf, "margin-bottom", p.margin_bottom);
    mz_css_print_dim(buf, "margin-left", p.margin_left);
    mz_css_print_dim(buf, "margin-right", p.margin_right);

    // Offsets
    mz_css_print_dim(buf, "top", p.top);
    mz_css_print_dim(buf, "bottom", p.bottom);
    mz_css_print_dim(buf, "left", p.left);
    mz_css_print_dim(buf, "right", p.right);

    // Gap
    mz_css_print_dim(buf, "gap", p.gap);
    mz_css_print_dim(buf, "row-gap", p.row_gap);
    mz_css_print_dim(buf, "column-gap", p.column_gap);

    // Borders
    mz_css_print_dim(buf, "border-radius", p.border_radius);
    mz_css_print_str(buf, "border", p.border);
    mz_css_print_str(buf, "border-top", p.border_top);
    mz_css_print_str(buf, "border-bottom", p.border_bottom);
    mz_css_print_str(buf, "border-left", p.border_left);
    mz_css_print_str(buf, "border-right", p.border_right);
    mz_css_print_str(buf, "outline", p.outline);

    // Colors & Backgrounds
    mz_css_print_str(buf, "color", p.color);
    mz_css_print_str(buf, "background", p.background);
    mz_css_print_str(buf, "background-color", p.background_color);
    if (p.has_opacity) {
        char op_str[32];
        snprintf(op_str, sizeof(op_str), "%.2g", p.opacity);
        mz_css_print_str(buf, "opacity", op_str);
    }

    // Typography
    mz_css_print_str(buf, "font-family", p.font_family);
    mz_css_print_dim(buf, "font-size", p.font_size);
    mz_css_print_str(buf, "font-weight", p.font_weight);
    mz_css_print_str(buf, "line-height", p.line_height);
    mz_css_print_kw(buf, "text-align", p.text_align);
    mz_css_print_str(buf, "text-decoration", p.text_decoration);
    mz_css_print_str(buf, "text-transform", p.text_transform);
    mz_css_print_str(buf, "letter-spacing", p.letter_spacing);

    // Visual & UI
    mz_css_print_str(buf, "box-shadow", p.box_shadow);
    mz_css_print_str(buf, "transition", p.transition);
    mz_css_print_str(buf, "transform", p.transform);
    mz_css_print_kw(buf, "cursor", p.cursor);
    mz_css_print_kw(buf, "user-select", p.user_select);
    mz_css_print_kw(buf, "pointer-events", p.pointer_events);

    // Custom
    if (p.custom) {
        mz_buf_append_str(buf, "  ");
        mz_buf_append_str(buf, p.custom);
        mz_buf_append_str(buf, "\n");
    }
}

// Writes a complete type-safe CSS rule in one statement:
// CssRule(".card", (CssProps){ .display = CSS_FLEX, .padding = Px(16) });
static inline void CssRule(const char *selector, CssProps props) {
    MizarBuffer *buf = mz_context_get();
    if (!buf || !selector) return;
    mz_buf_append_str(buf, selector);
    mz_buf_append_str(buf, " {\n");
    mz_css_render_props(buf, props);
    mz_buf_append_str(buf, "}\n");
}

// -----------------------------------------------------------------------------
// Scoped Block CSS DSL (Retained for custom / block composition)
// -----------------------------------------------------------------------------
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

#endif
