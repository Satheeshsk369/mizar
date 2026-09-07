#include "view/html/attrs.h"
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

    // Standard Global Attributes
    mz_render_attr_str(buf, "id", attrs.id);
    mz_render_attr_str(buf, "class", attrs.cls);
    mz_render_attr_str(buf, "style", attrs.style);
    mz_render_attr_str(buf, "title", attrs.title);
    mz_render_attr_str(buf, "lang", attrs.lang);
    mz_render_attr_str(buf, "dir", attrs.dir);
    mz_render_attr_str(buf, "tabindex", attrs.tabindex);
    mz_render_attr_str(buf, "role", attrs.role);
    mz_render_attr_str(buf, "accesskey", attrs.accesskey);
    mz_render_attr_str(buf, "contenteditable", attrs.contenteditable);
    mz_render_attr_str(buf, "inputmode", attrs.inputmode);
    mz_render_attr_str(buf, "enterkeyhint", attrs.enterkeyhint);
    mz_render_attr_str(buf, "spellcheck", attrs.spellcheck);
    mz_render_attr_str(buf, "draggable", attrs.draggable);
    mz_render_attr_str(buf, "hidden", attrs.hidden_str);
    mz_render_attr_str(buf, "popover", attrs.popover);

    // Hyperlinks & Resources
    mz_render_attr_str(buf, "href", attrs.href);
    mz_render_attr_str(buf, "src", attrs.src);
    mz_render_attr_str(buf, "srcset", attrs.srcset);
    mz_render_attr_str(buf, "sizes", attrs.sizes);
    mz_render_attr_str(buf, "alt", attrs.alt);
    mz_render_attr_str(buf, "target", attrs.target);
    mz_render_attr_str(buf, "rel", attrs.rel);
    mz_render_attr_str(buf, "type", attrs.type);
    mz_render_attr_str(buf, "media", attrs.media);
    mz_render_attr_str(buf, "width", attrs.width);
    mz_render_attr_str(buf, "height", attrs.height);
    mz_render_attr_str(buf, "loading", attrs.loading);
    mz_render_attr_str(buf, "download", attrs.download);
    mz_render_attr_str(buf, "crossorigin", attrs.crossorigin);
    mz_render_attr_str(buf, "integrity", attrs.integrity);
    mz_render_attr_str(buf, "referrerpolicy", attrs.referrerpolicy);
    mz_render_attr_str(buf, "fetchpriority", attrs.fetchpriority);

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
    mz_render_attr_str(buf, "minlength", attrs.minlength);
    mz_render_attr_str(buf, "maxlength", attrs.maxlength);
    mz_render_attr_str(buf, "size", attrs.size);
    mz_render_attr_str(buf, "rows", attrs.rows);
    mz_render_attr_str(buf, "cols", attrs.cols);
    mz_render_attr_str(buf, "wrap", attrs.wrap);
    mz_render_attr_str(buf, "enctype", attrs.enctype);
    mz_render_attr_str(buf, "autocomplete", attrs.autocomplete);
    mz_render_attr_str(buf, "accept", attrs.accept);
    mz_render_attr_str(buf, "form", attrs.form);
    mz_render_attr_str(buf, "formaction", attrs.formaction);
    mz_render_attr_str(buf, "formenctype", attrs.formenctype);
    mz_render_attr_str(buf, "formmethod", attrs.formmethod);
    mz_render_attr_str(buf, "formtarget", attrs.formtarget);
    mz_render_attr_str(buf, "list", attrs.list);

    // Buttons & Popovers
    mz_render_attr_str(buf, "popovertarget", attrs.popovertarget);
    mz_render_attr_str(buf, "popovertargetaction", attrs.popovertargetaction);

    // Documents / Metadata / Embeds
    mz_render_attr_str(buf, "charset", attrs.charset);
    mz_render_attr_str(buf, "content", attrs.content);
    mz_render_attr_str(buf, "http-equiv", attrs.http_equiv);
    mz_render_attr_str(buf, "srcdoc", attrs.srcdoc);
    mz_render_attr_str(buf, "sandbox", attrs.sandbox);
    mz_render_attr_str(buf, "allow", attrs.allow);
    mz_render_attr_str(buf, "as", attrs.as_);

    // Media Attributes
    mz_render_attr_str(buf, "preload", attrs.preload);
    mz_render_attr_str(buf, "poster", attrs.poster);
    mz_render_attr_str(buf, "kind", attrs.kind);
    mz_render_attr_str(buf, "srclang", attrs.srclang);

    // Tables
    mz_render_attr_str(buf, "colspan", attrs.colspan);
    mz_render_attr_str(buf, "rowspan", attrs.rowspan);
    mz_render_attr_str(buf, "headers", attrs.headers);
    mz_render_attr_str(buf, "scope", attrs.scope);
    mz_render_attr_str(buf, "span", attrs.span);

    // Time & Metrics
    mz_render_attr_str(buf, "datetime", attrs.datetime);
    mz_render_attr_str(buf, "low", attrs.low);
    mz_render_attr_str(buf, "high", attrs.high);
    mz_render_attr_str(buf, "optimum", attrs.optimum);

    // ARIA Attributes
    mz_render_attr_str(buf, "aria-label", attrs.aria_label);
    mz_render_attr_str(buf, "aria-labelledby", attrs.aria_labelledby);
    mz_render_attr_str(buf, "aria-describedby", attrs.aria_describedby);
    mz_render_attr_str(buf, "aria-hidden", attrs.aria_hidden);
    mz_render_attr_str(buf, "aria-expanded", attrs.aria_expanded);
    mz_render_attr_str(buf, "aria-controls", attrs.aria_controls);
    mz_render_attr_str(buf, "aria-live", attrs.aria_live);
    mz_render_attr_str(buf, "aria-atomic", attrs.aria_atomic);
    mz_render_attr_str(buf, "aria-current", attrs.aria_current);
    mz_render_attr_str(buf, "aria-haspopup", attrs.aria_haspopup);
    mz_render_attr_str(buf, "aria-invalid", attrs.aria_invalid);
    mz_render_attr_str(buf, "aria-required", attrs.aria_required);
    mz_render_attr_str(buf, "aria-disabled", attrs.aria_disabled);
    mz_render_attr_str(buf, "aria-valuenow", attrs.aria_valuenow);
    mz_render_attr_str(buf, "aria-valuemin", attrs.aria_valuemin);
    mz_render_attr_str(buf, "aria-valuemax", attrs.aria_valuemax);
    mz_render_attr_str(buf, "aria-valuetext", attrs.aria_valuetext);

    // HTMX Attributes
    mz_render_attr_str(buf, "hx-get", attrs.hx_get);
    mz_render_attr_str(buf, "hx-post", attrs.hx_post);
    mz_render_attr_str(buf, "hx-put", attrs.hx_put);
    mz_render_attr_str(buf, "hx-delete", attrs.hx_delete);
    mz_render_attr_str(buf, "hx-patch", attrs.hx_patch);
    mz_render_attr_str(buf, "hx-target", attrs.hx_target);
    mz_render_attr_str(buf, "hx-swap", attrs.hx_swap);
    mz_render_attr_str(buf, "hx-trigger", attrs.hx_trigger);
    mz_render_attr_str(buf, "hx-vals", attrs.hx_vals);
    mz_render_attr_str(buf, "hx-headers", attrs.hx_headers);
    mz_render_attr_str(buf, "hx-include", attrs.hx_include);
    mz_render_attr_str(buf, "hx-select", attrs.hx_select);
    mz_render_attr_str(buf, "hx-indicator", attrs.hx_indicator);
    mz_render_attr_str(buf, "hx-confirm", attrs.hx_confirm);
    mz_render_attr_str(buf, "hx-push-url", attrs.hx_push_url);
    mz_render_attr_str(buf, "hx-boost", attrs.hx_boost);
    mz_render_attr_str(buf, "hx-sync", attrs.hx_sync);

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
    mz_render_attr_bool(buf, "novalidate", attrs.novalidate);
    mz_render_attr_bool(buf, "formnovalidate", attrs.formnovalidate);
    mz_render_attr_bool(buf, "open", attrs.open);
    mz_render_attr_bool(buf, "reversed", attrs.reversed);
    mz_render_attr_bool(buf, "ismap", attrs.ismap);
    mz_render_attr_bool(buf, "allowfullscreen", attrs.allowfullscreen);
    mz_render_attr_bool(buf, "playsinline", attrs.playsinline);
    mz_render_attr_bool(buf, "autoplay", attrs.autoplay);
    mz_render_attr_bool(buf, "controls", attrs.controls);
    mz_render_attr_bool(buf, "loop", attrs.loop);
    mz_render_attr_bool(buf, "muted", attrs.muted);
    mz_render_attr_bool(buf, "default", attrs.default_);

    if (attrs.custom && *attrs.custom) {
        mz_buf_append_char(buf, ' ');
        mz_buf_append_str(buf, attrs.custom);
    }
}
