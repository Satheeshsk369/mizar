#include "attrs.h"
#include <string.h>

static inline void mz_render_attr_str(MizarBuffer *buf, const char *name, const char *val) {
    if (!val) return;
    mz_buf_append_char(buf, ' ');
    mz_buf_append_str(buf, name);
    mz_buf_append_str(buf, "=\"");
    mz_buf_append_escaped(buf, val, strlen(val));
    mz_buf_append_char(buf, '"');
}

static inline void mz_render_attr_bool(MizarBuffer *buf, const char *name, bool val) {
    if (!val) return;
    mz_buf_append_char(buf, ' ');
    mz_buf_append_str(buf, name);
}

void mz_render_attrs(MizarBuffer *buf, Attrs attrs) {
    if (!buf) return;
    
    // Globals
    mz_render_attr_str(buf, "id", attrs.id);
    mz_render_attr_str(buf, "class", attrs.cls);
    mz_render_attr_str(buf, "style", attrs.style);
    mz_render_attr_str(buf, "title", attrs.title);
    mz_render_attr_str(buf, "lang", attrs.lang);
    mz_render_attr_str(buf, "dir", attrs.dir);
    mz_render_attr_str(buf, "tabindex", attrs.tabindex);
    mz_render_attr_str(buf, "role", attrs.role);
    
    // Links & Media
    mz_render_attr_str(buf, "href", attrs.href);
    mz_render_attr_str(buf, "src", attrs.src);
    mz_render_attr_str(buf, "alt", attrs.alt);
    mz_render_attr_str(buf, "target", attrs.target);
    mz_render_attr_str(buf, "rel", attrs.rel);
    mz_render_attr_str(buf, "type", attrs.type);
    mz_render_attr_str(buf, "media", attrs.media);
    mz_render_attr_str(buf, "width", attrs.width);
    mz_render_attr_str(buf, "height", attrs.height);
    mz_render_attr_str(buf, "loading", attrs.loading);
    
    // Forms & Inputs
    mz_render_attr_str(buf, "action", attrs.action);
    mz_render_attr_str(buf, "method", attrs.method);
    mz_render_attr_str(buf, "name", attrs.name);
    mz_render_attr_str(buf, "value", attrs.value);
    mz_render_attr_str(buf, "placeholder", attrs.placeholder);
    mz_render_attr_str(buf, "for", attrs.for_);
    mz_render_attr_str(buf, "pattern", attrs.pattern);
    mz_render_attr_str(buf, "min", attrs.min);
    mz_render_attr_str(buf, "max", attrs.max);
    mz_render_attr_str(buf, "step", attrs.step);
    mz_render_attr_str(buf, "enctype", attrs.enctype);
    
    // Metadata
    mz_render_attr_str(buf, "charset", attrs.charset);
    mz_render_attr_str(buf, "content", attrs.content);
    
    // HTMX
    mz_render_attr_str(buf, "hx-get", attrs.hx_get);
    mz_render_attr_str(buf, "hx-post", attrs.hx_post);
    mz_render_attr_str(buf, "hx-put", attrs.hx_put);
    mz_render_attr_str(buf, "hx-delete", attrs.hx_delete);
    mz_render_attr_str(buf, "hx-patch", attrs.hx_patch);
    mz_render_attr_str(buf, "hx-target", attrs.hx_target);
    mz_render_attr_str(buf, "hx-swap", attrs.hx_swap);
    mz_render_attr_str(buf, "hx-trigger", attrs.hx_trigger);
    
    // Booleans
    mz_render_attr_bool(buf, "disabled", attrs.disabled);
    mz_render_attr_bool(buf, "checked", attrs.checked);
    mz_render_attr_bool(buf, "required", attrs.required);
    mz_render_attr_bool(buf, "readonly", attrs.readonly);
    mz_render_attr_bool(buf, "autofocus", attrs.autofocus);
    mz_render_attr_bool(buf, "hidden", attrs.hidden);
    mz_render_attr_bool(buf, "multiple", attrs.multiple);
    mz_render_attr_bool(buf, "selected", attrs.selected);
    mz_render_attr_bool(buf, "defer", attrs.defer);
    mz_render_attr_bool(buf, "async", attrs.async);
    
    // Custom / Arbitrary (e.g. data-* or raw attributes)
    if (attrs.custom && *attrs.custom) {
        mz_buf_append_char(buf, ' ');
        mz_buf_append_str(buf, attrs.custom);
    }
}
