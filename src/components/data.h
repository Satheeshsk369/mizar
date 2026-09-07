#ifndef MIZAR_COMPONENTS_DATA_H
#define MIZAR_COMPONENTS_DATA_H

#include "view/html/tags.h"

// 1. Avatar: User picture or initial fallback
typedef struct {
    const char *src;
    const char *alt;
    const char *initials;
    int size_px; // default 40
    bool rounded; // true = circle, false = rounded rect
} UiAvatarProps;

static inline void mz_render_ui_avatar(UiAvatarProps p) {
    int sz = p.size_px > 0 ? p.size_px : 40;
    const char *radius = (p.rounded || (!p.rounded && !p.initials)) ? "50%" : "6px";
    char style[256];

    if (p.src) {
        snprintf(style, sizeof(style), "width: %dpx; height: %dpx; border-radius: %s; object-fit: cover; border: 1px solid #cbd5e1;", sz, sz, radius);
        Img(.src = p.src, .alt = p.alt ? p.alt : "Avatar", .style = style);
    } else {
        snprintf(style, sizeof(style), "display: inline-flex; align-items: center; justify-content: center; width: %dpx; height: %dpx; border-radius: %s; background: #0284c7; color: #ffffff; font-weight: 700; font-size: %dpx; user-select: none;",
                 sz, sz, radius, sz / 2);
        Div(.cls = "mz-avatar-fallback", .style = style) {
            Text(p.initials ? p.initials : "?");
        }
    }
}

#define UiAvatar(...) mz_render_ui_avatar((UiAvatarProps){ __VA_ARGS__ })

// 2. Metric / Statistic: Big KPI number display
typedef struct {
    const char *label;
    const char *value;
    const char *change; // e.g. "+12%" or "-3%"
    bool positive;
    const char *subtext;
} UiMetricProps;

static inline void mz_render_ui_metric(UiMetricProps p) {
    Div(.cls = "mz-metric", .style = "background: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; padding: 20px; box-shadow: 0 1px 2px rgba(0,0,0,0.05);") {
        if (p.label) {
            Div(.style = "font-size: 0.875rem; color: #64748b; font-weight: 600; margin-bottom: 6px;") {
                Text(p.label);
            }
        }
        Div(.style = "display: flex; align-items: baseline; gap: 8px;") {
            Span(.style = "font-size: 1.875rem; font-weight: 800; color: #0f172a; letter-spacing: -0.02em;") {
                Text(p.value ? p.value : "0");
            }
            if (p.change) {
                const char *col = p.positive ? "#16a34a" : "#dc2626";
                Span(.style = col ? col : "color: #64748b; font-size: 0.875rem; font-weight: 700;") {
                    Text(p.change);
                }
            }
        }
        if (p.subtext) {
            Div(.style = "font-size: 0.75rem; color: #94a3b8; margin-top: 4px;") {
                Text(p.subtext);
            }
        }
    }
}

#define UiMetric(...) mz_render_ui_metric((UiMetricProps){ __VA_ARGS__ })

// 3. Progress Bar
typedef struct {
    int percent; // 0 to 100
    const char *color; // hex or var (default #0284c7)
    int height_px; // default 8
} UiProgressProps;

static inline void mz_render_ui_progress(UiProgressProps p) {
    int val = p.percent < 0 ? 0 : (p.percent > 100 ? 100 : p.percent);
    int h = p.height_px > 0 ? p.height_px : 8;
    const char *c = p.color ? p.color : "#0284c7";

    char container_style[128];
    snprintf(container_style, sizeof(container_style), "width: 100%%; background: #e2e8f0; border-radius: 9999px; overflow: hidden; height: %dpx;", h);

    char bar_style[128];
    snprintf(bar_style, sizeof(bar_style), "width: %d%%; background: %s; height: 100%%; transition: width 0.3s ease;", val, c);

    Div(.cls = "mz-progress", .style = container_style) {
        Div(.style = bar_style) {}
    }
}

#define UiProgress(...) mz_render_ui_progress((UiProgressProps){ __VA_ARGS__ })

// 4. Empty State: Empty list / search graphic placeholder
typedef struct {
    const char *title;
    const char *desc;
} UiEmptyProps;

static inline void mz_ui_empty_open(UiEmptyProps p) {
    mz_tag_open("div", (Attrs){
        .cls = "mz-empty-state",
        .style = "text-align: center; padding: 48px 24px; border: 2px dashed #e2e8f0; border-radius: 8px; background: #f8fafc;"
    });
    if (p.title) {
        mz_tag_open("h3", (Attrs){ .style = "font-size: 1.15rem; font-weight: 700; color: #0f172a; margin-bottom: 6px;" });
        Text(p.title);
        mz_tag_close("h3");
    }
    if (p.desc) {
        mz_tag_open("p", (Attrs){ .style = "font-size: 0.875rem; color: #64748b; margin-bottom: 16px;" });
        Text(p.desc);
        mz_tag_close("p");
    }
}

#define UiEmpty(...) \
    for (int _mz_ui_em = (mz_ui_empty_open((UiEmptyProps){ __VA_ARGS__ }), 0); !_mz_ui_em; _mz_ui_em = 1, mz_tag_close("div"))

// 5. Table Layout Wrapper
typedef struct {
    bool striped;
    bool bordered;
    bool hover;
    const char *cls;
} UiTableProps;

static inline void mz_ui_table_open(UiTableProps p) {
    (void)p;
    mz_tag_open("div", (Attrs){ .style = "overflow-x: auto; width: 100%; border: 1px solid #e2e8f0; border-radius: 8px; margin: 16px 0;" });
    mz_tag_open("table", (Attrs){ .cls = "mz-table", .style = "width: 100%; border-collapse: collapse; text-align: left; font-size: 0.875rem;" });
}

static inline void mz_ui_table_close(void) {
    mz_tag_close("table");
    mz_tag_close("div");
}

#define UiTable(...) \
    for (int _mz_ui_t = (mz_ui_table_open((UiTableProps){ __VA_ARGS__ }), 0); !_mz_ui_t; _mz_ui_t = 1, mz_ui_table_close())

#endif
