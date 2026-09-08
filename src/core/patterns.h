#ifndef MIZAR_CORE_PATTERNS_H
#define MIZAR_CORE_PATTERNS_H

#include "view/html/tags.h"
#include "core/htmx.h"
#include <stdio.h>

// =============================================================================
// 1. Page Shell (Turnkey HTML5 document with HTMX 4, viewport & title)
// =============================================================================

typedef struct {
    const char *title;
    const char *lang;          // default "en"
    const char *theme_color;   // e.g. "#0284c7"
    MzHtmxConfig htmx;         // optional htmx config customization
    const char *stylesheet;    // optional external or local css url
} MzPageProps;

static inline void mz_page_open(MizarBuffer *buf, MzPageProps p) {
    mz_context_push(buf);
    mz_buf_append_str(buf, "<!DOCTYPE html>\n");
    mz_tag_open("html", (Attrs){ .lang = p.lang ? p.lang : "en" });
    mz_tag_open("head", (Attrs){0});
    
    // Essential head tags
    mz_void_tag("meta", (Attrs){ .charset = "UTF-8" });
    mz_void_tag("meta", (Attrs){ .name = "viewport", .content = "width=device-width, initial-scale=1.0" });
    if (p.theme_color) {
        mz_void_tag("meta", (Attrs){ .name = "theme-color", .content = p.theme_color });
    }
    
    // Page title
    mz_tag_open("title", (Attrs){0});
    if (p.title) Text("%s", p.title);
    mz_tag_close("title");

    // Internalized HTMX script & config
    MzHtmxScript(p.htmx);

    // Optional Stylesheet link
    if (p.stylesheet) {
        mz_void_tag("link", (Attrs){ .rel = "stylesheet", .href = p.stylesheet });
    }

    mz_tag_close("head");
    mz_tag_open("body", (Attrs){0});
}

static inline void mz_page_close(void) {
    mz_tag_close("body");
    mz_tag_close("html");
    mz_context_pop();
}

#define MzPage(buf, ...) \
    for (int _mz_pg = (mz_page_open((buf), (MzPageProps){ __VA_ARGS__ }), 0); \
         !_mz_pg; \
         _mz_pg = 1, mz_page_close())


// =============================================================================
// 2. Active Search / Filter Box
// =============================================================================

typedef struct {
    const char *url;           // Endpoint to query (e.g. "/api/search")
    const char *target;        // Target container selector (e.g. "#results")
    const char *placeholder;   // Placeholder text
    const char *name;          // Field name (default "q")
    const char *indicator;     // Indicator selector (default will auto-render spinner)
    int delay_ms;              // Debounce delay in ms (default 300)
    bool query_method;         // If true uses hx-query instead of hx-get
} MzSearchProps;

static inline void mz_render_search(MzSearchProps p) {
    int delay = p.delay_ms > 0 ? p.delay_ms : 300;
    char trigger_buf[128];
    snprintf(trigger_buf, sizeof(trigger_buf), "input changed delay:%dms, search, keyup[key=='Enter']", delay);

    Div(.style = "position: relative; width: 100%; max-width: 480px; display: flex; align-items: center;") {
        Input(
            .type = "search",
            .name = p.name ? p.name : "q",
            .placeholder = p.placeholder ? p.placeholder : "Search...",
            .hx_get = p.query_method ? nullptr : p.url,
            .hx_query = p.query_method ? p.url : nullptr,
            .hx_target = p.target,
            .hx_trigger = trigger_buf,
            .hx_indicator = p.indicator ? p.indicator : "next .mz-spinner",
            .style = "width: 100%; padding: 10px 14px; font-size: 0.95rem; border: 1px solid #cbd5e1; border-radius: 8px; outline: none; transition: border-color 0.2s;"
        );
        if (!p.indicator) {
            Span(
                .cls = "mz-spinner htmx-indicator",
                .style = "position: absolute; right: 12px; font-size: 0.8rem; color: #64748b; pointer-events: none;"
            ) {
                Text("Searching...");
            }
        }
    }
}

#define MzSearch(...) mz_render_search((MzSearchProps){ __VA_ARGS__ })


// =============================================================================
// 3. Click-to-Edit
// =============================================================================

typedef struct {
    const char *url;          // Edit endpoint URL returning edit form (e.g. "/items/1/edit")
    const char *label;        // Field label (e.g. "Full Name")
    const char *value;        // Current value display (e.g. "Jane Doe")
    const char *button_text;  // Text on button (default "Edit")
    const char *id;           // Optional wrapper ID
} MzClickToEditProps;

static inline void mz_render_click_to_edit(MzClickToEditProps p) {
    Div(
        .id = p.id,
        .cls = "mz-click-to-edit",
        .style = "display: flex; align-items: center; justify-content: space-between; padding: 12px 16px; background: #f8fafc; border: 1px solid #e2e8f0; border-radius: 8px; max-width: 480px;"
    ) {
        Div() {
            if (p.label) {
                Div(.style = "font-size: 0.75rem; font-weight: 600; text-transform: uppercase; color: #64748b; margin-bottom: 2px;") {
                    Text("%s", p.label);
                }
            }
            Div(.style = "font-size: 1rem; color: #0f172a; font-weight: 500;") {
                Text("%s", p.value ? p.value : "-");
            }
        }
        Button(
            .hx_get = p.url,
            .hx_swap = "outerHTML",
            .style = "padding: 6px 14px; background: #0284c7; color: #ffffff; border: none; border-radius: 6px; font-size: 0.875rem; font-weight: 600; cursor: pointer;"
        ) {
            Text("%s", p.button_text ? p.button_text : "Edit");
        }
    }
}

#define MzClickToEdit(...) mz_render_click_to_edit((MzClickToEditProps){ __VA_ARGS__ })


// =============================================================================
// 4. Infinite Scroll Trigger / Load More Button
// =============================================================================

typedef enum {
    MZ_LOAD_CLICK = 0,    // User clicks button to load next page
    MZ_LOAD_REVEALED = 1, // Automatically triggers when element scrolls into view
} MzLoadTrigger;

typedef struct {
    const char *url;           // Next page URL (e.g. "/items?page=2")
    const char *target;        // Append target (e.g. "#items-list")
    const char *label;         // Button text for click mode (default "Load More")
    MzLoadTrigger trigger;     // Click or Revealed
} MzLoadMoreProps;

static inline void mz_render_load_more(MzLoadMoreProps p) {
    if (p.trigger == MZ_LOAD_REVEALED) {
        Div(
            .hx_get = p.url,
            .hx_trigger = "revealed",
            .hx_swap = "outerHTML",
            .hx_target = p.target,
            .cls = "mz-infinite-scroll-trigger",
            .style = "padding: 16px; text-align: center; color: #64748b; font-size: 0.875rem;"
        ) {
            Text("Loading more...");
        }
    } else {
        Div(.style = "text-align: center; margin: 20px 0;") {
            Button(
                .hx_get = p.url,
                .hx_target = p.target,
                .hx_swap = "beforeend",
                .style = "padding: 10px 20px; background: #f1f5f9; color: #334155; border: 1px solid #cbd5e1; border-radius: 6px; font-weight: 600; cursor: pointer; transition: background 0.2s;"
            ) {
                Text("%s", p.label ? p.label : "Load More");
            }
        }
    }
}

#define MzLoadMore(...) mz_render_load_more((MzLoadMoreProps){ __VA_ARGS__ })


// =============================================================================
// 5. Polling / Live Refresh Container
// =============================================================================

typedef struct {
    const char *url;           // Polling endpoint URL
    int interval_sec;          // Polling interval in seconds (default 2)
    const char *target;        // Optional target (defaults to this element itself)
    const char *swap;          // Swap mode (default "innerHTML")
    const char *id;            // Element ID
} MzPollProps;

static inline void mz_render_poll_open(MzPollProps p) {
    int sec = p.interval_sec > 0 ? p.interval_sec : 2;
    char trig[32];
    snprintf(trig, sizeof(trig), "every %ds", sec);

    mz_tag_open("div", (Attrs){
        .id = p.id,
        .hx_get = p.url,
        .hx_trigger = trig,
        .hx_target = p.target,
        .hx_swap = p.swap ? p.swap : "innerHTML"
    });
}

static inline void mz_render_poll_close(void) {
    mz_tag_close("div");
}

#define MzPoll(...) \
    for (int _mz_poll = (mz_render_poll_open((MzPollProps){ __VA_ARGS__ }), 0); \
         !_mz_poll; \
         _mz_poll = 1, mz_render_poll_close())

#endif
