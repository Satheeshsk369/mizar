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

    // HTMX 4 & Core Attributes
    const char *hx_get;
    const char *hx_post;
    const char *hx_put;
    const char *hx_delete;
    const char *hx_patch;
    const char *hx_query;
    const char *hx_action;
    const char *hx_method;
    const char *hx_target;
    const char *hx_swap;
    const char *hx_trigger;
    const char *hx_vals;
    const char *hx_headers;
    const char *hx_include;
    const char *hx_select;
    const char *hx_select_oob;
    const char *hx_indicator;
    const char *hx_confirm;
    const char *hx_push_url;
    const char *hx_replace_url;
    const char *hx_boost;
    const char *hx_sync;
    const char *hx_config;
    const char *hx_disable;
    const char *hx_disabled_elt; // htmx 2 alias for hx-disable
    const char *hx_preload;
    const char *hx_pending;
    const char *hx_encoding;
    const char *hx_validate;
    const char *hx_history_elt;
    const char *hx_on;

    // HTMX 4 Inheritance Modifiers (:inherited)
    const char *hx_target_inherited;
    const char *hx_include_inherited;
    const char *hx_swap_inherited;
    const char *hx_headers_inherited;
    const char *hx_vals_inherited;
    const char *hx_confirm_inherited;
    const char *hx_indicator_inherited;
    const char *hx_sync_inherited;
    const char *hx_config_inherited;
    const char *hx_boost_inherited;
    const char *hx_disable_inherited;
    const char *hx_encoding_inherited;
    const char *hx_validate_inherited;

    // HTMX 4 Inheritance Modifiers (:append and :inherited:append)
    const char *hx_include_append;
    const char *hx_include_inherited_append;
    const char *hx_headers_append;
    const char *hx_headers_inherited_append;
    const char *hx_vals_append;
    const char *hx_vals_inherited_append;

    // HTMX 4 Status Response Handlers
    const char *hx_status_200;
    const char *hx_status_204;
    const char *hx_status_304;
    const char *hx_status_400;
    const char *hx_status_401;
    const char *hx_status_403;
    const char *hx_status_404;
    const char *hx_status_422;
    const char *hx_status_4xx;
    const char *hx_status_500;
    const char *hx_status_502;
    const char *hx_status_503;
    const char *hx_status_5xx;

    // HTMX Boolean / Flag Attributes
    bool hx_preserve;
    bool hx_ignore;
    bool hx_morph_skip;
    bool hx_morph_skip_children;

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
