#ifndef MIZAR_COMPONENTS_BUTTON_H
#define MIZAR_COMPONENTS_BUTTON_H

#include "view/html/tags.h"

typedef enum {
    BTN_PRIMARY = 0,
    BTN_SECONDARY,
    BTN_SUCCESS,
    BTN_DANGER,
    BTN_OUTLINE,
    BTN_GHOST
} UiBtnVariant;

typedef struct {
    UiBtnVariant variant;
    const char *id;
    const char *cls;
    const char *style;
    const char *type; // "button", "submit", "reset"
    bool disabled;

    // First-Class HTMX Integration
    const char *hx_get;
    const char *hx_post;
    const char *hx_put;
    const char *hx_delete;
    const char *hx_target;
    const char *hx_swap;
    const char *hx_indicator;
    const char *hx_confirm;
} UiBtnProps;

static inline const char *mz_btn_variant_style(UiBtnVariant v) {
    switch (v) {
        case BTN_SECONDARY:
            return "display: inline-flex; align-items: center; justify-content: center; padding: 8px 16px; font-size: 0.875rem; font-weight: 600; border-radius: 6px; background: #f1f5f9; color: #1e293b; border: 1px solid #cbd5e1; cursor: pointer;";
        case BTN_SUCCESS:
            return "display: inline-flex; align-items: center; justify-content: center; padding: 8px 16px; font-size: 0.875rem; font-weight: 600; border-radius: 6px; background: #16a34a; color: #ffffff; border: 1px solid #15803d; cursor: pointer;";
        case BTN_DANGER:
            return "display: inline-flex; align-items: center; justify-content: center; padding: 8px 16px; font-size: 0.875rem; font-weight: 600; border-radius: 6px; background: #dc2626; color: #ffffff; border: 1px solid #b91c1c; cursor: pointer;";
        case BTN_OUTLINE:
            return "display: inline-flex; align-items: center; justify-content: center; padding: 8px 16px; font-size: 0.875rem; font-weight: 600; border-radius: 6px; background: transparent; color: #0284c7; border: 1px solid #0284c7; cursor: pointer;";
        case BTN_GHOST:
            return "display: inline-flex; align-items: center; justify-content: center; padding: 8px 16px; font-size: 0.875rem; font-weight: 600; border-radius: 6px; background: transparent; color: #334155; border: none; cursor: pointer;";
        case BTN_PRIMARY:
        default:
            return "display: inline-flex; align-items: center; justify-content: center; padding: 8px 16px; font-size: 0.875rem; font-weight: 600; border-radius: 6px; background: #0284c7; color: #ffffff; border: 1px solid #0369a1; cursor: pointer;";
    }
}

static inline void mz_render_ui_btn(const char *label, UiBtnProps p) {
    const char *base_style = mz_btn_variant_style(p.variant);
    char full_style[512];
    if (p.style) {
        snprintf(full_style, sizeof(full_style), "%s %s", base_style, p.style);
    } else {
        snprintf(full_style, sizeof(full_style), "%s", base_style);
    }

    Button(
        .id = p.id,
        .cls = p.cls ? p.cls : "mz-btn",
        .style = full_style,
        .type = p.type ? p.type : "button",
        .disabled = p.disabled,
        .hx_get = p.hx_get,
        .hx_post = p.hx_post,
        .hx_put = p.hx_put,
        .hx_delete = p.hx_delete,
        .hx_target = p.hx_target,
        .hx_swap = p.hx_swap,
        .hx_indicator = p.hx_indicator,
        .hx_confirm = p.hx_confirm
    ) {
        Text(label ? label : "");
    }
}

#define UiBtn(label, ...) mz_render_ui_btn((label), (UiBtnProps){ __VA_ARGS__ })

#endif
