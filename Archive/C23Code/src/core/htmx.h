#ifndef MIZAR_CORE_HTMX_H
#define MIZAR_CORE_HTMX_H

#include "core/buffer.h"

#define MIZAR_HTMX_VERSION "4.0.0"
#define MIZAR_HTMX_CDN_URL "https://unpkg.com/htmx.org@" MIZAR_HTMX_VERSION

typedef struct {
    const char *version;       // e.g. "4.0.0" (default if null)
    const char *src;           // custom script URL or path (overrides CDN if set)
    bool include_compat;       // include htmx-2-compat.js
    bool implicit_inheritance; // restore htmx 2 implicit attribute inheritance
    const char *default_swap;  // e.g. "innerHTML", "outerHTML"
    const char *no_swap;       // status codes skipping swap, e.g. "[204, 304, '4xx', '5xx']"
    const char *extensions;    // allowed extensions whitelist
    bool transitions;          // enable View Transitions globally
    bool log_all;              // debug log all events to console
    const char *prefix;        // attribute prefix (default: "data-hx-")
    const char *custom_config; // additional raw HCON / JSON config
} MzHtmxConfig;

// Internalizes the HTMX script tag and configuration meta tags.
// If config is omitted or zero-initialized, defaults to HTMX 4 CDN release.
static inline void MzHtmxScript(MzHtmxConfig config) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;

    // Render <meta name="htmx-config"> if any config option is requested
    bool has_meta = config.implicit_inheritance || config.default_swap || config.no_swap ||
                    config.extensions || config.transitions || config.log_all ||
                    config.prefix || config.custom_config;
    if (has_meta) {
        mz_buf_append_str(buf, "<meta name=\"htmx-config\" content='{");
        bool first = true;
        if (config.implicit_inheritance) {
            mz_buf_append_str(buf, "\"implicitInheritance\": true");
            first = false;
        }
        if (config.default_swap) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_printf(buf, "\"defaultSwap\": \"%s\"", config.default_swap);
            first = false;
        }
        if (config.no_swap) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_printf(buf, "\"noSwap\": %s", config.no_swap);
            first = false;
        }
        if (config.extensions) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_printf(buf, "\"extensions\": \"%s\"", config.extensions);
            first = false;
        }
        if (config.transitions) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_append_str(buf, "\"transitions\": true");
            first = false;
        }
        if (config.log_all) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_append_str(buf, "\"logAll\": true");
            first = false;
        }
        if (config.prefix) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_printf(buf, "\"prefix\": \"%s\"", config.prefix);
            first = false;
        }
        if (config.custom_config) {
            if (!first) mz_buf_append_str(buf, ", ");
            mz_buf_append_str(buf, config.custom_config);
        }
        mz_buf_append_str(buf, "}' />\n");
    }

    // Render primary HTMX script tag
    const char *ver = config.version ? config.version : MIZAR_HTMX_VERSION;
    if (config.src) {
        mz_buf_printf(buf, "<script src=\"%s\"></script>\n", config.src);
    } else {
        mz_buf_printf(buf, "<script src=\"https://unpkg.com/htmx.org@%s\"></script>\n", ver);
    }

    // Optionally include compatibility extension
    if (config.include_compat) {
        mz_buf_printf(buf, "<script src=\"https://unpkg.com/htmx.org@%s/dist/ext/compat.js\"></script>\n", ver);
    }
}

// Simple macro overload: MzHtmx() defaults to standard HTMX 4, or MzHtmx(...) with options
#define MzHtmx(...) MzHtmxScript((MzHtmxConfig){ __VA_ARGS__ })

#endif
