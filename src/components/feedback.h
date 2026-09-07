#ifndef MIZAR_COMPONENTS_FEEDBACK_H
#define MIZAR_COMPONENTS_FEEDBACK_H

#include "view/html/tags.h"

// 1. Spinner: CSS-only animated loading spinner
typedef struct {
    int size_px; // default 24
    const char *color; // default #0284c7
} UiSpinnerProps;

static inline void UiSpinner(UiSpinnerProps p) {
    int sz = p.size_px > 0 ? p.size_px : 24;
    const char *c = p.color ? p.color : "#0284c7";

    char style[256];
    snprintf(style, sizeof(style),
             "display: inline-block; width: %dpx; height: %dpx; border: 3px solid rgba(0,0,0,0.1); border-left-color: %s; border-radius: 50%%; animation: mz-spin 0.8s linear infinite;",
             sz, sz, c);

    Div(.cls = "mz-spinner", .style = style) {}
}

// 2. Skeleton: Placeholder loader block
typedef struct {
    int width_px;
    int height_px; // default 16
    int radius_px; // default 4
    bool circle;
} UiSkeletonProps;

static inline void UiSkeleton(UiSkeletonProps p) {
    int h = p.height_px > 0 ? p.height_px : 16;
    char style[256];
    if (p.circle) {
        int sz = p.width_px > 0 ? p.width_px : h;
        snprintf(style, sizeof(style), "width: %dpx; height: %dpx; border-radius: 50%%; background: #e2e8f0; opacity: 0.7;", sz, sz);
    } else {
        int r = p.radius_px > 0 ? p.radius_px : 4;
        if (p.width_px > 0) {
            snprintf(style, sizeof(style), "width: %dpx; height: %dpx; border-radius: %dpx; background: #e2e8f0; opacity: 0.7;", p.width_px, h, r);
        } else {
            snprintf(style, sizeof(style), "width: 100%%; height: %dpx; border-radius: %dpx; background: #e2e8f0; opacity: 0.7;", h, r);
        }
    }
    Div(.cls = "mz-skeleton", .style = style) {}
}

// 3. Status Indicator Dot (Pulsing operational indicator)
typedef enum {
    STATUS_ONLINE = 0,
    STATUS_AWAY,
    STATUS_BUSY,
    STATUS_OFFLINE
} UiStatusType;

typedef struct {
    UiStatusType status;
    const char *label;
} UiStatusProps;

static inline void UiStatus(UiStatusProps p) {
    const char *col = "#16a34a"; // green
    if (p.status == STATUS_AWAY) col = "#eab308"; // yellow
    else if (p.status == STATUS_BUSY) col = "#dc2626"; // red
    else if (p.status == STATUS_OFFLINE) col = "#94a3b8"; // grey

    char dot_style[128];
    snprintf(dot_style, sizeof(dot_style), "display: inline-block; width: 8px; height: 8px; border-radius: 50%%; background: %s;", col);

    Span(.cls = "mz-status", .style = "display: inline-flex; align-items: center; gap: 6px; font-size: 0.875rem; color: #334155;") {
        Span(.style = dot_style) {}
        if (p.label) Text(p.label);
    }
}

#endif
