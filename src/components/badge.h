#ifndef MIZAR_COMPONENTS_BADGE_H
#define MIZAR_COMPONENTS_BADGE_H

#include "view/html/tags.h"

typedef enum {
    BADGE_DEFAULT = 0,
    BADGE_PRIMARY,
    BADGE_SUCCESS,
    BADGE_WARNING,
    BADGE_DANGER,
    BADGE_OUTLINE
} UiBadgeVariant;

typedef struct {
    UiBadgeVariant variant;
    const char *cls;
    const char *style;
} UiBadgeProps;

static inline const char *mz_badge_variant_style(UiBadgeVariant v) {
    switch (v) {
        case BADGE_PRIMARY:
            return "display: inline-flex; align-items: center; padding: 2px 8px; font-size: 0.75rem; font-weight: 600; border-radius: 9999px; background: #e0f2fe; color: #0369a1; border: 1px solid #bae6fd;";
        case BADGE_SUCCESS:
            return "display: inline-flex; align-items: center; padding: 2px 8px; font-size: 0.75rem; font-weight: 600; border-radius: 9999px; background: #dcfce7; color: #15803d; border: 1px solid #bbf7d0;";
        case BADGE_WARNING:
            return "display: inline-flex; align-items: center; padding: 2px 8px; font-size: 0.75rem; font-weight: 600; border-radius: 9999px; background: #fef9c3; color: #a16207; border: 1px solid #fef08a;";
        case BADGE_DANGER:
            return "display: inline-flex; align-items: center; padding: 2px 8px; font-size: 0.75rem; font-weight: 600; border-radius: 9999px; background: #fee2e2; color: #b91c1c; border: 1px solid #fecaca;";
        case BADGE_OUTLINE:
            return "display: inline-flex; align-items: center; padding: 2px 8px; font-size: 0.75rem; font-weight: 600; border-radius: 9999px; background: transparent; color: #475569; border: 1px solid #cbd5e1;";
        case BADGE_DEFAULT:
        default:
            return "display: inline-flex; align-items: center; padding: 2px 8px; font-size: 0.75rem; font-weight: 600; border-radius: 9999px; background: #f1f5f9; color: #334155; border: 1px solid #e2e8f0;";
    }
}

static inline void mz_render_ui_badge(const char *label, UiBadgeProps p) {
    const char *base_style = mz_badge_variant_style(p.variant);
    char full_style[512];
    if (p.style) {
        snprintf(full_style, sizeof(full_style), "%s %s", base_style, p.style);
    } else {
        snprintf(full_style, sizeof(full_style), "%s", base_style);
    }

    Span(.cls = p.cls ? p.cls : "mz-badge", .style = full_style) {
        Text(label ? label : "");
    }
}

#define UiBadge(label, ...) mz_render_ui_badge((label), (UiBadgeProps){ __VA_ARGS__ })

#endif
