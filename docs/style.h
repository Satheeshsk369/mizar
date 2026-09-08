#ifndef DOCS_STYLE_H
#define DOCS_STYLE_H

#include "mizar.h"

// Generates the entire documentation stylesheet using 100% type-safe C23 CSS DSL
static inline void render_docs_stylesheet(MizarBuffer *buf) {
    Stylesheet(buf) {
        // CSS Custom Properties (Variables)
        Rule(":root") {
            Prop("--bg", "#ffffff");
            Prop("--bg-surface", "#f8fafc");
            Prop("--bg-subtle", "#f1f5f9");
            Prop("--border", "#e2e8f0");
            Prop("--border-focus", "#94a3b8");
            Prop("--text", "#0f172a");
            Prop("--text-body", "#334155");
            Prop("--text-muted", "#64748b");
            Prop("--accent", "#0284c7");
            Prop("--accent-hover", "#0369a1");
            Prop("--code-bg", "#f8fafc");
            Prop("--code-color", "#0f172a");
            Prop("--inline-code-color", "#0369a1");
            Prop("--active-link-bg", "#e0f2fe");
            Prop("--badge-bg", "#e0f2fe");
            Prop("--badge-text", "#0369a1");
            Prop("--badge-border", "#bae6fd");
            Prop("--header-bg", "rgba(255, 255, 255, 0.82)");
            Prop("--dropdown-shadow", "0 10px 25px -5px rgba(0,0,0,0.1), 0 8px 10px -6px rgba(0,0,0,0.05)");
            Prop("--font-sans", "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif");
            Prop("--font-mono", "'JetBrains Mono', 'Fira Code', ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace");
        }

        Rule("[data-theme='dark']") {
            Prop("--bg", "#0b0f17");
            Prop("--bg-surface", "#111827");
            Prop("--bg-subtle", "#1f2937");
            Prop("--border", "#1f2937");
            Prop("--border-focus", "#4b5563");
            Prop("--text", "#f8fafc");
            Prop("--text-body", "#cbd5e1");
            Prop("--text-muted", "#94a3b8");
            Prop("--accent", "#38bdf8");
            Prop("--accent-hover", "#7dd3fc");
            Prop("--code-bg", "#0f172a");
            Prop("--code-color", "#e2e8f0");
            Prop("--inline-code-color", "#38bdf8");
            Prop("--active-link-bg", "#1e293b");
            Prop("--badge-bg", "#1e293b");
            Prop("--badge-text", "#38bdf8");
            Prop("--badge-border", "#334155");
            Prop("--header-bg", "rgba(11, 15, 23, 0.82)");
            Prop("--dropdown-shadow", "0 10px 25px -5px rgba(0,0,0,0.5), 0 0 0 1px #374151");
        }

        // Top Navigation Bar (Persistent Header with Frosted Glass)
        Rule(".top-bar") {
            Prop("backdrop-filter", "blur(12px)");
            Prop("-webkit-backdrop-filter", "blur(12px)");
        }

        CssRule(".top-bar", (CssProps){
            .position = CSS_FIXED,
            .top = Zero,
            .left = Px(250),
            .right = Zero,
            .height = Px(60),
            .display = CSS_FLEX,
            .justify_content = CSS_FLEX_END,
            .align_items = CSS_CENTER,
            .gap = Rem(0.75),
            .padding_left = Rem(2.0),
            .padding_right = Rem(2.5),
            .background_color = "var(--header-bg)",
            .border_bottom = "1px solid var(--border)",
            .z_index = 50,
            .has_z_index = true
        });

        CssRule(".search-box", (CssProps){
            .position = CSS_RELATIVE,
            .display = CSS_FLEX,
            .align_items = CSS_CENTER
        });

        CssRule(".search-input", (CssProps){
            .font_family = "var(--font-sans)",
            .font_size = Rem(0.88),
            .padding_top = Rem(0.45),
            .padding_bottom = Rem(0.45),
            .padding_left = Rem(0.85),
            .padding_right = Rem(0.85),
            .border_radius = Px(6),
            .border = "1px solid var(--border)",
            .background_color = "var(--bg-surface)",
            .color = "var(--text)",
            .outline = "none",
            .width = Px(260)
        });

        CssRule(".search-dropdown", (CssProps){
            .position = CSS_ABSOLUTE,
            .top = Px(42),
            .right = Zero,
            .width = Px(380),
            .max_height = Px(360),
            .overflow_y = CSS_AUTO,
            .background_color = "var(--bg-surface)",
            .border = "1px solid var(--border)",
            .border_radius = Px(8),
            .box_shadow = "var(--dropdown-shadow)",
            .display = CSS_NONE,
            .flex_direction = CSS_COLUMN,
            .z_index = 100,
            .has_z_index = true
        });

        CssRule(".search-result-item", (CssProps){
            .display = CSS_FLEX,
            .flex_direction = CSS_COLUMN,
            .gap = Px(4),
            .padding = Rem(0.75),
            .border_bottom = "1px solid var(--border)",
            .text_decoration = "none",
            .color = "var(--text)"
        });

        CssRule(".search-result-item:hover, .search-result-item.selected", (CssProps){
            .background_color = "var(--bg-subtle)"
        });

        CssRule(".search-result-title", (CssProps){
            .font_size = Rem(0.88),
            .font_weight = "600",
            .color = "var(--accent)"
        });

        CssRule(".search-result-snippet", (CssProps){
            .font_size = Rem(0.78),
            .color = "var(--text-muted)",
            .line_height = "1.4"
        });

        CssRule(".theme-toggle", (CssProps){
            .display = CSS_INLINE_FLEX,
            .align_items = CSS_CENTER,
            .justify_content = CSS_CENTER,
            .width = Px(36),
            .height = Px(36),
            .padding = Zero,
            .border_radius = Px(6),
            .border = "1px solid var(--border)",
            .background_color = "var(--bg-surface)",
            .color = "var(--text)",
            .cursor = CSS_POINTER
        });

        // Global Reset
        CssRule("*", (CssProps){
            .box_sizing = CSS_BORDER_BOX,
            .margin = Zero,
            .padding = Zero
        });

        // Body
        CssRule("body", (CssProps){
            .display = CSS_FLEX,
            .min_height = Vh(100),
            .background_color = "var(--bg)",
            .color = "var(--text)",
            .font_family = "var(--font-sans)",
            .font_size = Px(16),
            .line_height = "1.65"
        });

        // Sidebar Navigation
        CssRule(".sidebar", (CssProps){
            .position = CSS_FIXED,
            .top = Zero,
            .bottom = Zero,
            .left = Zero,
            .width = Px(250),
            .display = CSS_FLEX,
            .flex_direction = CSS_COLUMN,
            .gap = Rem(1.5),
            .padding = Rem(1.25),
            .background_color = "var(--bg-surface)",
            .border_right = "1px solid var(--border)",
            .overflow_y = CSS_AUTO
        });

        CssRule(".brand", (CssProps){
            .display = CSS_FLEX,
            .align_items = CSS_CENTER,
            .gap = Rem(0.5),
            .font_size = Rem(1.25),
            .font_weight = "700",
            .letter_spacing = "-0.02em",
            .color = "var(--text)",
            .text_decoration = "none"
        });

        CssRule(".brand span", (CssProps){
            .font_size = Rem(0.75),
            .font_weight = "600",
            .font_family = "var(--font-mono)",
            .color = "var(--badge-text)",
            .background_color = "var(--badge-bg)",
            .border = "1px solid var(--badge-border)",
            .border_radius = Px(999),
            .padding = Px(2),
            .padding_left = Px(8),
            .padding_right = Px(8)
        });

        CssRule(".nav-section", (CssProps){
            .display = CSS_FLEX,
            .flex_direction = CSS_COLUMN,
            .gap = Rem(0.25)
        });

        CssRule(".nav-title", (CssProps){
            .font_size = Rem(0.72),
            .font_weight = "700",
            .letter_spacing = "0.08em",
            .text_transform = "uppercase",
            .color = "var(--text-muted)",
            .margin_bottom = Rem(0.35)
        });

        CssRule(".nav-link", (CssProps){
            .font_size = Rem(0.92),
            .color = "var(--text-muted)",
            .text_decoration = "none",
            .padding_top = Rem(0.4),
            .padding_bottom = Rem(0.4),
            .padding_left = Rem(0.65),
            .padding_right = Rem(0.65),
            .border_radius = Px(6),
            .transition = "all 0.15s ease"
        });

        CssRule(".nav-link:hover", (CssProps){
            .color = "var(--text)",
            .background = "var(--bg-subtle)"
        });

        CssRule(".nav-link.active", (CssProps){
            .color = "var(--accent)",
            .background = "var(--active-link-bg)",
            .font_weight = "600"
        });

        // Main Wrapper
        CssRule(".main-wrapper", (CssProps){
            .margin_left = Px(250),
            .margin_top = Px(60),
            .padding_top = Rem(2.5),
            .padding_bottom = Rem(4.0),
            .padding_left = Rem(2.75),
            .padding_right = Rem(2.75),
            .max_width = Px(940),
            .flex_grow = 1,
            .has_flex_grow = true
        });

        // Typography & Headings
        CssRule("header.page-header", (CssProps){
            .margin_bottom = Rem(2.5),
            .padding_bottom = Rem(1.75),
            .border_bottom = "1px solid var(--border)"
        });

        CssRule("h1", (CssProps){
            .font_size = Rem(2.1),
            .font_weight = "800",
            .letter_spacing = "-0.03em",
            .margin_bottom = Rem(0.5),
            .color = "var(--text)"
        });

        CssRule(".tagline", (CssProps){
            .font_size = Rem(1.1),
            .color = "var(--text-muted)",
            .line_height = "1.5"
        });

        CssRule("h2", (CssProps){
            .font_size = Rem(1.4),
            .font_weight = "700",
            .letter_spacing = "-0.02em",
            .margin_top = Rem(3.25),
            .margin_bottom = Rem(0.85),
            .padding_bottom = Rem(0.35),
            .border_bottom = "1px solid var(--border)",
            .color = "var(--text)"
        });

        CssRule("h3", (CssProps){
            .font_size = Rem(1.15),
            .font_weight = "600",
            .margin_top = Rem(2.25),
            .margin_bottom = Rem(0.5),
            .color = "var(--text)"
        });

        CssRule("p", (CssProps){
            .margin_bottom = Rem(1.25),
            .color = "var(--text-body)"
        });

        CssRule("ul, ol", (CssProps){
            .margin_bottom = Rem(1.5),
            .padding_left = Rem(1.5),
            .color = "var(--text-body)"
        });

        CssRule("li", (CssProps){
            .margin_bottom = Rem(0.4)
        });

        // Code Blocks
        Rule("pre[class*=\"language-\"], code[class*=\"language-\"]") {
            Prop("background", "var(--code-bg) !important");
            Prop("border-color", "var(--border) !important");
            Prop("font-size", "0.88rem !important");
            Prop("line-height", "1.55 !important");
        }

        CssRule("pre", (CssProps){
            .background_color = "var(--code-bg)",
            .border = "1px solid var(--border)",
            .border_radius = Px(8),
            .padding_top = Rem(1.1),
            .padding_bottom = Rem(1.1),
            .padding_left = Rem(1.35),
            .padding_right = Rem(1.35),
            .margin_bottom = Rem(1.75),
            .font_size = Rem(0.88),
            .line_height = "1.55",
            .color = "var(--code-color)",
            .overflow_x = CSS_AUTO
        });

        CssRule("code", (CssProps){
            .font_size = CssEm(0.88),
            .background_color = "var(--bg-subtle)",
            .border = "1px solid var(--border)",
            .border_radius = Px(4),
            .padding_top = CssEm(0.15),
            .padding_bottom = CssEm(0.15),
            .padding_left = CssEm(0.4),
            .padding_right = CssEm(0.4),
            .color = "var(--inline-code-color)"
        });

        CssRule("pre code", (CssProps){
            .background = "none",
            .border = "none",
            .padding = Zero,
            .color = "inherit"
        });

        // Callout Box
        CssRule(".callout", (CssProps){
            .background_color = "var(--bg-surface)",
            .border = "1px solid var(--border)",
            .border_left = "4px solid var(--accent)",
            .border_radius = Px(8),
            .padding_top = Rem(1.1),
            .padding_bottom = Rem(1.1),
            .padding_left = Rem(1.35),
            .padding_right = Rem(1.35),
            .margin_bottom = Rem(2.0)
        });

        CssRule(".callout h4", (CssProps){
            .font_size = Rem(0.95),
            .font_weight = "600",
            .color = "var(--accent)",
            .margin_bottom = Rem(0.4)
        });

        CssRule(".callout p:last-child", (CssProps){
            .margin_bottom = Zero
        });

        // Math Display
        CssRule(".math-display", (CssProps){
            .display = CSS_FLEX,
            .justify_content = CSS_CENTER,
            .align_items = CSS_CENTER,
            .background = "var(--bg-surface)",
            .border = "1px solid var(--border)",
            .border_radius = Px(8),
            .padding = Rem(1.25),
            .margin_top = Rem(1.5),
            .margin_bottom = Rem(1.5),
            .overflow_x = CSS_AUTO
        });

        // Footer
        CssRule("footer", (CssProps){
            .margin_top = Rem(4.5),
            .padding_top = Rem(1.75),
            .border_top = "1px solid var(--border)",
            .font_size = Rem(0.88),
            .color = "var(--text-muted)",
            .display = CSS_FLEX,
            .justify_content = CSS_SPACE_BETWEEN,
            .align_items = CSS_CENTER
        });

        // Responsive Media Query
        Media("@media (max-width: 860px)") {
            CssRule("body", (CssProps){
                .flex_direction = CSS_COLUMN
            });
            CssRule(".sidebar", (CssProps){
                .position = CSS_STATIC,
                .width = Pct(100),
                .border_right = "none",
                .border_bottom = "1px solid var(--border)"
            });
            CssRule(".top-bar", (CssProps){
                .position = CSS_STATIC,
                .left = Zero,
                .width = Pct(100),
                .padding_left = Rem(1.25),
                .padding_right = Rem(1.25)
            });
            CssRule(".main-wrapper", (CssProps){
                .margin_left = Zero,
                .margin_top = Zero,
                .padding_top = Rem(2.0),
                .padding_bottom = Rem(2.0),
                .padding_left = Rem(1.25),
                .padding_right = Rem(1.25)
            });
        }
    }
}

#endif
