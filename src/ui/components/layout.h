#ifndef MIZAR_COMPONENTS_LAYOUT_H
#define MIZAR_COMPONENTS_LAYOUT_H

#include "view/html/tags.h"

// 1. Container: Max-width centered wrapper
typedef struct {
    int max_width_px; // default 1200
    const char *cls;
    const char *style;
} UiContainerProps;

static inline void mz_ui_container_open(UiContainerProps p) {
    int max_w = p.max_width_px > 0 ? p.max_width_px : 1200;
    char style[256];
    snprintf(style, sizeof(style), "max-width: %dpx; margin-left: auto; margin-right: auto; padding-left: 16px; padding-right: 16px; %s",
             max_w, p.style ? p.style : "");
    mz_tag_open("div", (Attrs){ .cls = p.cls ? p.cls : "mz-container", .style = style });
}

#define UiContainer(...) \
    for (int _mz_ui_c = (mz_ui_container_open((UiContainerProps){ __VA_ARGS__ }), 0); !_mz_ui_c; _mz_ui_c = 1, mz_tag_close("div"))

// 2. Stack: Flex layout with uniform gap (vertical or horizontal)
typedef struct {
    bool horizontal;
    int gap_px; // default 16
    const char *align; // "center", "flex-start", "stretch"
    const char *justify; // "space-between", "center", etc.
    const char *cls;
    const char *style;
} UiStackProps;

static inline void mz_ui_stack_open(UiStackProps p) {
    int gap = p.gap_px > 0 ? p.gap_px : 16;
    char style[256];
    snprintf(style, sizeof(style), "display: flex; flex-direction: %s; gap: %dpx; align-items: %s; justify-content: %s; %s",
             p.horizontal ? "row" : "column", gap,
             p.align ? p.align : (p.horizontal ? "center" : "stretch"),
             p.justify ? p.justify : "flex-start",
             p.style ? p.style : "");
    mz_tag_open("div", (Attrs){ .cls = p.cls ? p.cls : "mz-stack", .style = style });
}

#define UiStack(...) \
    for (int _mz_ui_s = (mz_ui_stack_open((UiStackProps){ __VA_ARGS__ }), 0); !_mz_ui_s; _mz_ui_s = 1, mz_tag_close("div"))

// 3. Grid: Multi-column responsive CSS grid
typedef struct {
    int cols; // default 3
    int gap_px; // default 16
    int min_col_width_px; // default 250 (responsive auto-fit)
    const char *cls;
    const char *style;
} UiGridProps;

static inline void mz_ui_grid_open(UiGridProps p) {
    int gap = p.gap_px > 0 ? p.gap_px : 16;
    char style[256];
    if (p.min_col_width_px > 0) {
        snprintf(style, sizeof(style), "display: grid; grid-template-columns: repeat(auto-fit, minmax(%dpx, 1fr)); gap: %dpx; %s",
                 p.min_col_width_px, gap, p.style ? p.style : "");
    } else {
        int cols = p.cols > 0 ? p.cols : 3;
        snprintf(style, sizeof(style), "display: grid; grid-template-columns: repeat(%d, minmax(0, 1fr)); gap: %dpx; %s",
                 cols, gap, p.style ? p.style : "");
    }
    mz_tag_open("div", (Attrs){ .cls = p.cls ? p.cls : "mz-grid", .style = style });
}

#define UiGrid(...) \
    for (int _mz_ui_g = (mz_ui_grid_open((UiGridProps){ __VA_ARGS__ }), 0); !_mz_ui_g; _mz_ui_g = 1, mz_tag_close("div"))

// 4. Divider / Separator
typedef struct {
    const char *label;
    bool vertical;
    const char *cls;
} UiDividerProps;

static inline void mz_render_ui_divider(UiDividerProps p) {
    if (p.vertical) {
        Div(.cls = p.cls ? p.cls : "mz-divider-v", .style = "display: inline-block; width: 1px; background: #e2e8f0; height: 1.25em; vertical-align: middle; margin: 0 8px;") {}
    } else if (p.label) {
        Div(.cls = p.cls ? p.cls : "mz-divider-labeled", .style = "display: flex; align-items: center; text-align: center; margin: 16px 0; color: #64748b; font-size: 0.8rem; text-transform: uppercase;") {
            Span(.style = "flex: 1; border-bottom: 1px solid #e2e8f0;") {}
            Span(.style = "padding: 0 12px; font-weight: 600;") { Text(p.label); }
            Span(.style = "flex: 1; border-bottom: 1px solid #e2e8f0;") {}
        }
    } else {
        Hr(.cls = p.cls ? p.cls : "mz-divider", .style = "border: 0; border-top: 1px solid #e2e8f0; margin: 16px 0;");
    }
}

#define UiDivider(...) mz_render_ui_divider((UiDividerProps){ __VA_ARGS__ })

// 5. Paper / Surface: Elevated container with shadow
typedef struct {
    int elevation; // 1, 2, 3
    int radius_px;
    const char *cls;
    const char *style;
} UiPaperProps;

static inline void mz_ui_paper_open(UiPaperProps p) {
    const char *shadow = "0 1px 3px rgba(0,0,0,0.08)";
    if (p.elevation == 2) shadow = "0 4px 6px -1px rgba(0,0,0,0.1), 0 2px 4px -2px rgba(0,0,0,0.1)";
    else if (p.elevation >= 3) shadow = "0 10px 15px -3px rgba(0,0,0,0.1), 0 4px 6px -4px rgba(0,0,0,0.1)";
    int rad = p.radius_px > 0 ? p.radius_px : 8;

    char style[256];
    snprintf(style, sizeof(style), "background: #ffffff; border-radius: %dpx; box-shadow: %s; border: 1px solid #e2e8f0; padding: 20px; %s",
             rad, shadow, p.style ? p.style : "");
    mz_tag_open("div", (Attrs){ .cls = p.cls ? p.cls : "mz-paper", .style = style });
}

#define UiPaper(...) \
    for (int _mz_ui_p = (mz_ui_paper_open((UiPaperProps){ __VA_ARGS__ }), 0); !_mz_ui_p; _mz_ui_p = 1, mz_tag_close("div"))

// 6. Center: Flex centering wrapper
static inline void mz_ui_center_open(const char *cls, const char *style) {
    char full_style[256];
    snprintf(full_style, sizeof(full_style), "display: flex; align-items: center; justify-content: center; %s", style ? style : "");
    mz_tag_open("div", (Attrs){ .cls = cls ? cls : "mz-center", .style = full_style });
}

#define UiCenter(...) \
    for (int _mz_ui_ce = (mz_ui_center_open(nullptr, nullptr), 0); !_mz_ui_ce; _mz_ui_ce = 1, mz_tag_close("div"))

// 7. Collapsible: Expandable section using HTML5 details & summary
typedef struct {
    const char *title;
    bool open;
    const char *cls;
    const char *style;
} UiCollapsibleProps;

static inline void mz_ui_collapsible_open(UiCollapsibleProps p) {
    mz_tag_open("details", (Attrs){ .cls = p.cls ? p.cls : "mz-collapsible", .style = p.style, .open = p.open });
    mz_tag_open("summary", (Attrs){ .style = "cursor: pointer; font-weight: 600; padding: 8px 0; user-select: none; color: #0f172a;" });
    Text(p.title ? p.title : "Details");
    mz_tag_close("summary");
    mz_tag_open("div", (Attrs){ .style = "padding-top: 8px;" });
}

static inline void mz_ui_collapsible_close(void) {
    mz_tag_close("div");
    mz_tag_close("details");
}

#define UiCollapsible(...) \
    for (int _mz_ui_col = (mz_ui_collapsible_open((UiCollapsibleProps){ __VA_ARGS__ }), 0); !_mz_ui_col; _mz_ui_col = 1, mz_ui_collapsible_close())

#endif
