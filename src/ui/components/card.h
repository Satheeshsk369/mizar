#ifndef MIZAR_COMPONENTS_CARD_H
#define MIZAR_COMPONENTS_CARD_H

#include "view/html/tags.h"

typedef struct {
    const char *title;
    const char *desc;
    const char *id;
    const char *cls;
    const char *style;
} UiCardProps;

static inline void mz_card_open(UiCardProps p) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;

    const char *default_style = "background: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; padding: 20px; box-shadow: 0 1px 3px rgba(0,0,0,0.05); margin-bottom: 16px;";
    char full_style[512];
    if (p.style) {
        snprintf(full_style, sizeof(full_style), "%s %s", default_style, p.style);
    } else {
        snprintf(full_style, sizeof(full_style), "%s", default_style);
    }

    mz_tag_open("div", (Attrs){
        .id = p.id,
        .cls = p.cls ? p.cls : "mz-card",
        .style = full_style
    });

    if (p.title || p.desc) {
        mz_tag_open("div", (Attrs){ .cls = "mz-card-header", .style = "margin-bottom: 12px;" });
        if (p.title) {
            mz_tag_open("h3", (Attrs){ .cls = "mz-card-title", .style = "margin: 0; font-size: 1.15rem; font-weight: 700; color: #0f172a;" });
            Text(p.title);
            mz_tag_close("h3");
        }
        if (p.desc) {
            mz_tag_open("p", (Attrs){ .cls = "mz-card-desc", .style = "margin: 4px 0 0 0; font-size: 0.875rem; color: #64748b;" });
            Text(p.desc);
            mz_tag_close("p");
        }
        mz_tag_close("div");
    }

    mz_tag_open("div", (Attrs){ .cls = "mz-card-body" });
}

static inline void mz_card_close(void) {
    mz_tag_close("div"); // close mz-card-body
    mz_tag_close("div"); // close mz-card
}

#define UiCard(...) \
    for (int _mz_ci = (mz_card_open((UiCardProps){ __VA_ARGS__ }), 0); \
         !_mz_ci; \
         _mz_ci = 1, mz_card_close())

#endif
