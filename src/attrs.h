#ifndef MIZAR_ATTRS_H
#define MIZAR_ATTRS_H

#include "buffer.h"
#include <stdbool.h>

typedef struct {
    // Core Globals
    const char *id;
    const char *cls;
    const char *style;
    const char *title;
    const char *lang;
    const char *dir;
    const char *tabindex;
    const char *role;
    
    // Links & Media
    const char *href;
    const char *src;
    const char *alt;
    const char *target;
    const char *rel;
    const char *type;
    const char *media;
    const char *width;
    const char *height;
    const char *loading;
    
    // Forms & Inputs
    const char *action;
    const char *method;
    const char *name;
    const char *value;
    const char *placeholder;
    const char *for_;        // HTML 'for' attribute (named for_ to avoid C keyword)
    const char *pattern;
    const char *min;
    const char *max;
    const char *step;
    const char *enctype;
    
    // Metadata
    const char *charset;
    const char *content;
    
    // HTMX Attributes
    const char *hx_get;
    const char *hx_post;
    const char *hx_put;
    const char *hx_delete;
    const char *hx_patch;
    const char *hx_target;
    const char *hx_swap;
    const char *hx_trigger;
    
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
    
    // Arbitrary / data-* / custom attributes
    const char *custom;
} Attrs;

// Render attributes to buffer
void mz_render_attrs(MizarBuffer *buf, Attrs attrs);

#endif // MIZAR_ATTRS_H
