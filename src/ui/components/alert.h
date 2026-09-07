#ifndef MIZAR_COMPONENTS_ALERT_H
#define MIZAR_COMPONENTS_ALERT_H

#include "view/html/tags.h"

typedef enum {
    ALERT_INFO = 0,
    ALERT_SUCCESS,
    ALERT_WARNING,
    ALERT_DANGER
} UiAlertVariant;

typedef struct {
    UiAlertVariant variant;
    const char *title;
    const char *id;
    const char *cls;
    const char *style;
} UiAlertProps;

static inline const char *mz_alert_style(UiAlertVariant v) {
    switch (v) {
        case ALERT_SUCCESS:
            return "background: #f0fdf4; border-left: 4px solid #22c55e; border: 1px solid #bbf7d0; border-left-width: 4px; color: #166534; padding: 14px 18px; border-radius: 6px; margin-bottom: 16px;";
        case ALERT_WARNING:
            return "background: #fefce8; border-left: 4px solid #eab308; border: 1px solid #fef08a; border-left-width: 4px; color: #854d0e; padding: 14px 18px; border-radius: 6px; margin-bottom: 16px;";
        case ALERT_DANGER:
            return "background: #fef2f2; border-left: 4px solid #ef4444; border: 1px solid #fecaca; border-left-width: 4px; color: #991b1b; padding: 14px 18px; border-radius: 6px; margin-bottom: 16px;";
        case ALERT_INFO:
        default:
            return "background: #f0f9ff; border-left: 4px solid #0284c7; border: 1px solid #bae6fd; border-left-width: 4px; color: #075985; padding: 14px 18px; border-radius: 6px; margin-bottom: 16px;";
    }
}

static inline void mz_alert_open(UiAlertProps p) {
    const char *base_style = mz_alert_style(p.variant);
    char full_style[512];
    if (p.style) {
        snprintf(full_style, sizeof(full_style), "%s %s", base_style, p.style);
    } else {
        snprintf(full_style, sizeof(full_style), "%s", base_style);
    }

    mz_tag_open("div", (Attrs){
        .id = p.id,
        .cls = p.cls ? p.cls : "mz-alert",
        .style = full_style,
        .role = "alert"
    });

    if (p.title) {
        mz_tag_open("strong", (Attrs){ .cls = "mz-alert-title", .style = "display: block; margin-bottom: 4px; font-weight: 700;" });
        Text(p.title);
        mz_tag_close("strong");
    }
}

static inline void mz_alert_close(void) {
    mz_tag_close("div");
}

#define UiAlert(...) \
    for (int _mz_ai = (mz_alert_open((UiAlertProps){ __VA_ARGS__ }), 0); \
         !_mz_ai; \
         _mz_ai = 1, mz_alert_close())

#endif
