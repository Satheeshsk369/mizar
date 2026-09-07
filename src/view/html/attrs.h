#ifndef MIZAR_VIEW_HTML_ATTRS_H
#define MIZAR_VIEW_HTML_ATTRS_H

#include "core/buffer.h"
#include <stdbool.h>

typedef struct {
    // Standard Global Attributes
    const char *id;
    const char *cls;
    const char *style;
    const char *title;
    const char *lang;
    const char *dir;
    const char *tabindex;
    const char *role;
    const char *accesskey;
    const char *contenteditable;
    const char *inputmode;
    const char *enterkeyhint;
    const char *spellcheck;
    const char *draggable;
    const char *hidden_str;
    const char *popover;

    // Hyperlinks & Resources
    const char *href;
    const char *src;
    const char *srcset;
    const char *sizes;
    const char *alt;
    const char *target;
    const char *rel;
    const char *type;
    const char *media;
    const char *width;
    const char *height;
    const char *loading;
    const char *download;
    const char *crossorigin;
    const char *integrity;
    const char *referrerpolicy;
    const char *fetchpriority;

    // Forms & Inputs
    const char *action;
    const char *method;
    const char *name;
    const char *value;
    const char *placeholder;
    const char *for_;
    const char *pattern;
    const char *min;
    const char *max;
    const char *step;
    const char *minlength;
    const char *maxlength;
    const char *size;
    const char *rows;
    const char *cols;
    const char *wrap;
    const char *enctype;
    const char *autocomplete;
    const char *accept;
    const char *form;
    const char *formaction;
    const char *formenctype;
    const char *formmethod;
    const char *formtarget;
    const char *list;

    // Buttons & Popovers
    const char *popovertarget;
    const char *popovertargetaction;

    // Documents / Metadata / Embeds
    const char *charset;
    const char *content;
    const char *http_equiv;
    const char *srcdoc;
    const char *sandbox;
    const char *allow;
    const char *as_;

    // Media Attributes (Audio, Video, Track)
    const char *preload;
    const char *poster;
    const char *kind;
    const char *srclang;

    // Tables
    const char *colspan;
    const char *rowspan;
    const char *headers;
    const char *scope;
    const char *span;

    // Time & Metrics
    const char *datetime;
    const char *low;
    const char *high;
    const char *optimum;

    // ARIA Attributes
    const char *aria_label;
    const char *aria_labelledby;
    const char *aria_describedby;
    const char *aria_hidden;
    const char *aria_expanded;
    const char *aria_controls;
    const char *aria_live;
    const char *aria_atomic;
    const char *aria_current;
    const char *aria_haspopup;
    const char *aria_invalid;
    const char *aria_required;
    const char *aria_disabled;
    const char *aria_valuenow;
    const char *aria_valuemin;
    const char *aria_valuemax;
    const char *aria_valuetext;

    // HTMX Attributes
    const char *hx_get;
    const char *hx_post;
    const char *hx_put;
    const char *hx_delete;
    const char *hx_patch;
    const char *hx_target;
    const char *hx_swap;
    const char *hx_trigger;
    const char *hx_vals;
    const char *hx_headers;
    const char *hx_include;
    const char *hx_select;
    const char *hx_indicator;
    const char *hx_confirm;
    const char *hx_push_url;
    const char *hx_boost;
    const char *hx_sync;

    // Boolean Attributes
    bool disabled;
    bool checked;
    bool required;
    bool readonly;
    bool autofocus;
    bool hidden;
    bool multiple;
    bool selected;
    bool defer;
    bool async;
    bool novalidate;
    bool formnovalidate;
    bool open;
    bool reversed;
    bool ismap;
    bool allowfullscreen;
    bool playsinline;
    bool autoplay;
    bool controls;
    bool loop;
    bool muted;
    bool default_;

    // Raw/arbitrary attribute string injection (e.g. data-* attributes)
    const char *custom;
} Attrs;

void mz_render_attrs(MizarBuffer *buf, Attrs attrs);

#endif
