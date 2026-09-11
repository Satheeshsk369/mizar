module mizar::ui::patterns;
import mizar::view::html;
import mizar::core::buffer;
import mizar::ui::icons;

// 1. Full Turnkey HTML5 Page with HTMX 4 included
struct PageProps {
    String title;
    String lang;        // default "en"
    String theme_color; // e.g. "#0284c7"
    String stylesheet;  // optional CSS URL
}

macro void @page(PageProps p = {}; @body()) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf != null) {
        buf.append_str("<!DOCTYPE html>\n");
    }

    String l = p.lang.len > 0 ? p.lang : "en";
    html::@tag("html", { .name = "lang", .value = l }) {
        html::@tag("head", {}) {
            html::void_tag("meta", { .name = "charset", .value = "UTF-8" });
            html::void_tag("meta", { .name = "viewport", .value = "width=device-width, initial-scale=1.0" });
            if (p.theme_color.len > 0) {
                html::void_tag("meta", { .name = "theme-color", .value = p.theme_color });
            }
            if (p.title.len > 0) {
                html::@tag("title", {}) {
                    html::text(p.title);
                };
            }
            // Include HTMX 4 script
            html::@tag("script", { .src = "https://unpkg.com/htmx.org@2.0.2" }) {};
        };
        html::@tag("body", { .style = "font-family: system-ui, sans-serif; margin: 0; padding: 24px; background: #f8fafc; color: #0f172a;" }) {
            @body();
        };
    };
}

// 2. Active Search / Live Query Box with debounce
struct SearchProps {
    String url;         // Endpoint to query (e.g. "/api/search")
    String target;      // Target container selector (e.g. "#results")
    String placeholder; // Placeholder text
}

macro void @search(SearchProps p) {
    html::@div({ .style = "position: relative; width: 100%; max-width: 480px; display: flex; align-items: center;" }) {
        html::@input({
            .type = "search",
            .name = "q",
            .placeholder = p.placeholder.len > 0 ? p.placeholder : "Search...",
            .hx_get = p.url,
            .hx_target = p.target,
            .hx_trigger = "input changed delay:300ms, search, keyup[key=='Enter']",
            .style = "width: 100%; padding: 10px 14px; font-size: 0.95rem; border: 1px solid #cbd5e1; border-radius: 8px; outline: none;"
        });
        html::@span({ .style = "position: absolute; right: 12px; pointer-events: none; color: #94a3b8;" }) {
            icons::@icon_search({ .size = 18 });
        };
    };
}

// 3. Click-to-Edit Pattern
struct ClickToEditProps {
    String url;         // Form edit endpoint
    String label;       // Field label
    String value;       // Current display value
}

macro void @click_to_edit(ClickToEditProps p) {
    html::@div({ .style = "display: flex; align-items: center; justify-content: space-between; padding: 12px; background: white; border: 1px solid #e2e8f0; border-radius: 6px; margin: 8px 0;" }) {
        html::@div() {
            html::@span({ .style = "font-weight: 600; color: #64748b; margin-right: 12px;" }) {
                html::text(p.label);
                html::text(":");
            };
            html::@span({ .style = "color: #0f172a;" }) {
                html::text(p.value);
            };
        };
        html::@button({
            .class = "btn-edit",
            .hx_get = p.url,
            .hx_target = "closest div",
            .hx_swap = "outerHTML",
            .style = "padding: 6px 12px; font-size: 0.85rem; background: #f1f5f9; border: 1px solid #cbd5e1; border-radius: 4px; cursor: pointer; display: flex; align-items: center; gap: 4px;"
        }) {
            icons::@icon_edit({ .size = 14 });
            html::text("Edit");
        };
    };
}
