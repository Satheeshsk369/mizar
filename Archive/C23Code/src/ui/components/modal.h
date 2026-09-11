#ifndef MIZAR_COMPONENTS_MODAL_H
#define MIZAR_COMPONENTS_MODAL_H

#include "view/html/tags.h"

typedef struct {
    const char *id;
    const char *title;
    const char *cls;
    const char *style;
} UiModalProps;

// Uses standard HTML5 <dialog> element. Native browser accessibility, zero JS bundle!
static inline void mz_modal_open(UiModalProps p) {
    const char *default_style = "border: none; border-radius: 8px; padding: 0; box-shadow: 0 20px 25px -5px rgba(0,0,0,0.1); max-width: 500px; width: 90%; background: #ffffff;";
    char full_style[512];
    if (p.style) {
        snprintf(full_style, sizeof(full_style), "%s %s", default_style, p.style);
    } else {
        snprintf(full_style, sizeof(full_style), "%s", default_style);
    }

    mz_tag_open("dialog", (Attrs){
        .id = p.id,
        .cls = p.cls ? p.cls : "mz-dialog",
        .style = full_style
    });

    mz_tag_open("div", (Attrs){ .cls = "mz-dialog-content", .style = "padding: 24px;" });

    if (p.title) {
        mz_tag_open("div", (Attrs){
            .cls = "mz-dialog-header",
            .style = "display: flex; justify-content: space-between; align-items: center; margin-bottom: 16px; border-bottom: 1px solid #e2e8f0; padding-bottom: 12px;"
        });
        mz_tag_open("h3", (Attrs){ .style = "margin: 0; font-size: 1.25rem; font-weight: 700; color: #0f172a;" });
        Text(p.title);
        mz_tag_close("h3");

        // Native close button using form method="dialog"
        mz_tag_open("form", (Attrs){ .method = "dialog" });
        mz_tag_open("button", (Attrs){
            .style = "background: transparent; border: none; font-size: 1.2rem; cursor: pointer; color: #64748b; padding: 0 4px;",
            .aria_label = "Close"
        });
        Text("✕");
        mz_tag_close("button");
        mz_tag_close("form");

        mz_tag_close("div");
    }

    mz_tag_open("div", (Attrs){ .cls = "mz-dialog-body" });
}

static inline void mz_modal_close(void) {
    mz_tag_close("div"); // close body
    mz_tag_close("div"); // close content
    mz_tag_close("dialog");
}

#define UiModal(...) \
    for (int _mz_mi = (mz_modal_open((UiModalProps){ __VA_ARGS__ }), 0); \
         !_mz_mi; \
         _mz_mi = 1, mz_modal_close())

#endif
