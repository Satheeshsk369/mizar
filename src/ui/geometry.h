#ifndef MIZAR_UI_GEOMETRY_H
#define MIZAR_UI_GEOMETRY_H

#include "core/buffer.h"
#include "view/html/tags.h"
#include <stdbool.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// 1. EdgeInsets: Geometry spacing calculations (Flutter-inspired)
// -----------------------------------------------------------------------------
typedef struct {
    double top;
    double right;
    double bottom;
    double left;
    bool active;
} EdgeInsets;

static inline EdgeInsets EdgeInsets_All(double val) {
    return (EdgeInsets){ .top = val, .right = val, .bottom = val, .left = val, .active = true };
}

typedef struct { double h; double v; } EdgeInsetsSymmetricArgs;
#define EdgeInsets_Symmetric(...) mz_calc_edge_insets_symmetric((EdgeInsetsSymmetricArgs){ __VA_ARGS__ })

static inline EdgeInsets mz_calc_edge_insets_symmetric(EdgeInsetsSymmetricArgs args) {
    return (EdgeInsets){ .top = args.v, .right = args.h, .bottom = args.v, .left = args.h, .active = true };
}

typedef struct { double top; double right; double bottom; double left; } EdgeInsetsOnlyArgs;
#define EdgeInsets_Only(...) mz_calc_edge_insets_only((EdgeInsetsOnlyArgs){ __VA_ARGS__ })

static inline EdgeInsets mz_calc_edge_insets_only(EdgeInsetsOnlyArgs args) {
    return (EdgeInsets){ .top = args.top, .right = args.right, .bottom = args.bottom, .left = args.left, .active = true };
}

static inline void mz_buf_append_double(MizarBuffer *buf, double val) {
    if (val == (long)val) {
        char num[32];
        snprintf(num, sizeof(num), "%ld", (long)val);
        mz_buf_append_str(buf, num);
    } else {
        char num[32];
        snprintf(num, sizeof(num), "%.4g", val);
        mz_buf_append_str(buf, num);
    }
}

static inline void mz_stream_edge_insets(MizarBuffer *buf, const char *prop_name, EdgeInsets insets) {
    if (!insets.active || !buf) return;
    mz_buf_append_str(buf, prop_name);
    mz_buf_append_str(buf, ": ");

    // Optimize shorthand syntax
    if (insets.top == insets.bottom && insets.right == insets.left) {
        if (insets.top == insets.right) {
            mz_buf_append_double(buf, insets.top);
            mz_buf_append_str(buf, "px; ");
        } else {
            mz_buf_append_double(buf, insets.top);
            mz_buf_append_str(buf, "px ");
            mz_buf_append_double(buf, insets.right);
            mz_buf_append_str(buf, "px; ");
        }
    } else {
        mz_buf_append_double(buf, insets.top);
        mz_buf_append_str(buf, "px ");
        mz_buf_append_double(buf, insets.right);
        mz_buf_append_str(buf, "px ");
        mz_buf_append_double(buf, insets.bottom);
        mz_buf_append_str(buf, "px ");
        mz_buf_append_double(buf, insets.left);
        mz_buf_append_str(buf, "px; ");
    }
}

// -----------------------------------------------------------------------------
// 2. Alignment & Flex Alignment Geometry
// -----------------------------------------------------------------------------
typedef enum {
    ALIGN_START = 0,
    ALIGN_CENTER,
    ALIGN_END,
    ALIGN_STRETCH,
    ALIGN_BASELINE,
    ALIGN_SPACE_BETWEEN,
    ALIGN_SPACE_AROUND,
    ALIGN_SPACE_EVENLY
} Alignment;

static inline const char *mz_align_to_css(Alignment a) {
    switch (a) {
        case ALIGN_CENTER: return "center";
        case ALIGN_END: return "flex-end";
        case ALIGN_STRETCH: return "stretch";
        case ALIGN_BASELINE: return "baseline";
        case ALIGN_SPACE_BETWEEN: return "space-between";
        case ALIGN_SPACE_AROUND: return "space-around";
        case ALIGN_SPACE_EVENLY: return "space-evenly";
        case ALIGN_START:
        default: return "flex-start";
    }
}

// -----------------------------------------------------------------------------
// 3. Physics-based Elevation Shadow Calculation
// -----------------------------------------------------------------------------
static inline void mz_stream_elevation_shadow(MizarBuffer *buf, int elevation) {
    if (!buf || elevation <= 0) return;
    if (elevation > 24) elevation = 24;

    int y1 = (elevation <= 1) ? 1 : (elevation * 2 - 1);
    int blur1 = elevation * 3;
    double op1 = 0.12 + (elevation * 0.005);

    int y2 = elevation;
    int blur2 = elevation * 2;
    double op2 = 0.08 + (elevation * 0.003);

    char shadow[128];
    snprintf(shadow, sizeof(shadow),
             "box-shadow: 0 %dpx %dpx rgba(0,0,0,%.3f), 0 %dpx %dpx rgba(0,0,0,%.3f); ",
             y1, blur1, op1, y2, blur2, op2);
    mz_buf_append_str(buf, shadow);
}

// -----------------------------------------------------------------------------
// 4. Box Constraints & Calculated Headless Box
// -----------------------------------------------------------------------------
typedef struct {
    double width;
    double height;
    double min_width;
    double max_width;
    double min_height;
    double max_height;
    EdgeInsets padding;
    EdgeInsets margin;
    int elevation; // 0..24
    const char *id;
    const char *cls;
    const char *style;
} BoxConstraints;

static inline void mz_box_open(BoxConstraints c) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;

    mz_buf_append_str(buf, "<div");
    if (c.id) {
        mz_buf_append_str(buf, " id=\"");
        mz_buf_append_str(buf, c.id);
        mz_buf_append_char(buf, '"');
    }
    if (c.cls) {
        mz_buf_append_str(buf, " class=\"");
        mz_buf_append_str(buf, c.cls);
        mz_buf_append_char(buf, '"');
    }

    mz_buf_append_str(buf, " style=\"");
    if (c.width > 0) { mz_buf_append_str(buf, "width: "); mz_buf_append_double(buf, c.width); mz_buf_append_str(buf, "px; "); }
    if (c.height > 0) { mz_buf_append_str(buf, "height: "); mz_buf_append_double(buf, c.height); mz_buf_append_str(buf, "px; "); }
    if (c.min_width > 0) { mz_buf_append_str(buf, "min-width: "); mz_buf_append_double(buf, c.min_width); mz_buf_append_str(buf, "px; "); }
    if (c.max_width > 0) { mz_buf_append_str(buf, "max-width: "); mz_buf_append_double(buf, c.max_width); mz_buf_append_str(buf, "px; "); }
    if (c.min_height > 0) { mz_buf_append_str(buf, "min-height: "); mz_buf_append_double(buf, c.min_height); mz_buf_append_str(buf, "px; "); }
    if (c.max_height > 0) { mz_buf_append_str(buf, "max-height: "); mz_buf_append_double(buf, c.max_height); mz_buf_append_str(buf, "px; "); }

    mz_stream_edge_insets(buf, "padding", c.padding);
    mz_stream_edge_insets(buf, "margin", c.margin);
    mz_stream_elevation_shadow(buf, c.elevation);

    if (c.style) mz_buf_append_str(buf, c.style);
    mz_buf_append_str(buf, "\">");
}

#define Box(...) \
    for (int _mz_bx = (mz_box_open((BoxConstraints){ __VA_ARGS__ }), 0); !_mz_bx; _mz_bx = 1, mz_tag_close("div"))

// -----------------------------------------------------------------------------
// 5. Flex Layout Geometry (Row & Column)
// -----------------------------------------------------------------------------
typedef struct {
    double gap;
    Alignment main_align;
    Alignment cross_align;
    bool wrap;
    const char *id;
    const char *cls;
    const char *style;
} FlexGeometry;

static inline void mz_flex_geom_open(const char *direction, FlexGeometry g) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;

    mz_buf_append_str(buf, "<div");
    if (g.id) {
        mz_buf_append_str(buf, " id=\"");
        mz_buf_append_str(buf, g.id);
        mz_buf_append_char(buf, '"');
    }
    if (g.cls) {
        mz_buf_append_str(buf, " class=\"");
        mz_buf_append_str(buf, g.cls);
        mz_buf_append_char(buf, '"');
    }

    mz_buf_append_str(buf, " style=\"display: flex; flex-direction: ");
    mz_buf_append_str(buf, direction);
    mz_buf_append_str(buf, "; justify-content: ");
    mz_buf_append_str(buf, mz_align_to_css(g.main_align));
    mz_buf_append_str(buf, "; align-items: ");
    mz_buf_append_str(buf, mz_align_to_css(g.cross_align));
    mz_buf_append_str(buf, "; ");

    if (g.wrap) mz_buf_append_str(buf, "flex-wrap: wrap; ");
    mz_buf_append_str(buf, "gap: ");
    mz_buf_append_double(buf, g.gap);
    mz_buf_append_str(buf, "px; ");

    if (g.style) mz_buf_append_str(buf, g.style);
    mz_buf_append_str(buf, "\">");
}

#define Row(...) \
    for (int _mz_rw = (mz_flex_geom_open("row", (FlexGeometry){ __VA_ARGS__ }), 0); !_mz_rw; _mz_rw = 1, mz_tag_close("div"))

#define Column(...) \
    for (int _mz_col = (mz_flex_geom_open("column", (FlexGeometry){ __VA_ARGS__ }), 0); !_mz_col; _mz_col = 1, mz_tag_close("div"))

// -----------------------------------------------------------------------------
// 6. Expanded / Flexible (Flex weight proportion)
// -----------------------------------------------------------------------------
typedef struct {
    int flex; // default 1
    const char *cls;
    const char *style;
} ExpandedProps;

static inline void mz_expanded_open(ExpandedProps p) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    int factor = p.flex > 0 ? p.flex : 1;

    mz_buf_append_str(buf, "<div");
    if (p.cls) {
        mz_buf_append_str(buf, " class=\"");
        mz_buf_append_str(buf, p.cls);
        mz_buf_append_char(buf, '"');
    }
    mz_buf_append_str(buf, " style=\"flex: ");
    char num[16];
    snprintf(num, sizeof(num), "%d", factor);
    mz_buf_append_str(buf, num);
    mz_buf_append_str(buf, " 1 0%; min-width: 0; min-height: 0; ");
    if (p.style) mz_buf_append_str(buf, p.style);
    mz_buf_append_str(buf, "\">");
}

#define Expanded(...) \
    for (int _mz_exp = (mz_expanded_open((ExpandedProps){ __VA_ARGS__ }), 0); !_mz_exp; _mz_exp = 1, mz_tag_close("div"))

// -----------------------------------------------------------------------------
// 7. AspectRatio Geometry Calculator
// -----------------------------------------------------------------------------
typedef struct {
    double ratio;
    const char *cls;
    const char *style;
} AspectRatioProps;

static inline void mz_aspect_ratio_open(AspectRatioProps p) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    double r = p.ratio > 0 ? p.ratio : 1.0;

    mz_buf_append_str(buf, "<div");
    if (p.cls) {
        mz_buf_append_str(buf, " class=\"");
        mz_buf_append_str(buf, p.cls);
        mz_buf_append_char(buf, '"');
    }
    mz_buf_append_str(buf, " style=\"aspect-ratio: ");
    mz_buf_append_double(buf, r);
    mz_buf_append_str(buf, "; width: 100%; ");
    if (p.style) mz_buf_append_str(buf, p.style);
    mz_buf_append_str(buf, "\">");
}

#define AspectRatio(ratio_val) \
    for (int _mz_ar = (mz_aspect_ratio_open((AspectRatioProps){ .ratio = (double)(ratio_val) }), 0); !_mz_ar; _mz_ar = 1, mz_tag_close("div"))

// -----------------------------------------------------------------------------
// 8. Responsive Grid Calculation
// -----------------------------------------------------------------------------
typedef struct {
    int columns;
    double min_col_width; // responsive auto-fit threshold
    double gap;
    const char *cls;
    const char *style;
} GridGeometry;

static inline void mz_grid_geom_open(GridGeometry g) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;

    mz_buf_append_str(buf, "<div");
    if (g.cls) {
        mz_buf_append_str(buf, " class=\"");
        mz_buf_append_str(buf, g.cls);
        mz_buf_append_char(buf, '"');
    }
    mz_buf_append_str(buf, " style=\"display: grid; ");
    if (g.min_col_width > 0) {
        mz_buf_append_str(buf, "grid-template-columns: repeat(auto-fit, minmax(");
        mz_buf_append_double(buf, g.min_col_width);
        mz_buf_append_str(buf, "px, 1fr)); ");
    } else {
        int cols = g.columns > 0 ? g.columns : 1;
        mz_buf_append_str(buf, "grid-template-columns: repeat(");
        char c_str[16];
        snprintf(c_str, sizeof(c_str), "%d", cols);
        mz_buf_append_str(buf, c_str);
        mz_buf_append_str(buf, ", minmax(0, 1fr)); ");
    }
    mz_buf_append_str(buf, "gap: ");
    mz_buf_append_double(buf, g.gap);
    mz_buf_append_str(buf, "px; ");

    if (g.style) mz_buf_append_str(buf, g.style);
    mz_buf_append_str(buf, "\">");
}

#define Grid(...) \
    for (int _mz_grd = (mz_grid_geom_open((GridGeometry){ __VA_ARGS__ }), 0); !_mz_grd; _mz_grd = 1, mz_tag_close("div"))

#endif
