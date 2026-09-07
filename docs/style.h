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
            Prop("--border-focus", "#cbd5e1");
            Prop("--text", "#0f172a");
            Prop("--text-muted", "#64748b");
            Prop("--accent", "#0284c7");
            Prop("--accent-hover", "#0369a1");
            Prop("--code-bg", "#f8fafc");
            Prop("--font-sans", "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif");
            Prop("--font-mono", "'JetBrains Mono', 'Fira Code', ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace");
        }

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
            .color = "#0369a1",
            .background_color = "#e0f2fe",
            .border = "1px solid #bae6fd",
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
            .background = "#e0f2fe",
            .font_weight = "600"
        });

        // Main Wrapper
        CssRule(".main-wrapper", (CssProps){
            .margin_left = Px(250),
            .padding_top = Rem(3.5),
            .padding_bottom = Rem(3.5),
            .padding_left = Rem(4.5),
            .padding_right = Rem(4.5),
            .max_width = Px(900),
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
            .font_size = Rem(2.25),
            .font_weight = "800",
            .letter_spacing = "-0.03em",
            .margin_bottom = Rem(0.5),
            .color = "var(--text)"
        });

        CssRule(".tagline", (CssProps){
            .font_size = Rem(1.15),
            .color = "var(--text-muted)",
            .line_height = "1.5"
        });

        CssRule("h2", (CssProps){
            .font_size = Rem(1.45),
            .font_weight = "700",
            .letter_spacing = "-0.02em",
            .margin_top = Rem(2.75),
            .margin_bottom = Rem(0.9),
            .padding_bottom = Rem(0.35),
            .border_bottom = "1px solid var(--border)",
            .color = "var(--text)"
        });

        CssRule("h3", (CssProps){
            .font_size = Rem(1.15),
            .font_weight = "600",
            .margin_top = Rem(1.75),
            .margin_bottom = Rem(0.6),
            .color = "var(--text)"
        });

        CssRule("p", (CssProps){
            .margin_bottom = Rem(1.25),
            .color = "#334155"
        });

        CssRule("ul, ol", (CssProps){
            .margin_bottom = Rem(1.5),
            .padding_left = Rem(1.5),
            .color = "#334155"
        });

        CssRule("li", (CssProps){
            .margin_bottom = Rem(0.4)
        });

        // Code Blocks
        CssRule("pre", (CssProps){
            .background_color = "var(--code-bg)",
            .border = "1px solid var(--border)",
            .border_radius = Px(8),
            .padding_top = Rem(1.1),
            .padding_bottom = Rem(1.1),
            .padding_left = Rem(1.35),
            .padding_right = Rem(1.35),
            .margin_bottom = Rem(1.75),
            .font_family = "var(--font-mono)",
            .font_size = Rem(0.88),
            .line_height = "1.55",
            .color = "#1e293b",
            .overflow_x = CSS_AUTO
        });

        CssRule("code", (CssProps){
            .font_family = "var(--font-mono)",
            .font_size = CssEm(0.88),
            .background_color = "var(--bg-subtle)",
            .border = "1px solid var(--border)",
            .border_radius = Px(4),
            .padding_top = CssEm(0.15),
            .padding_bottom = CssEm(0.15),
            .padding_left = CssEm(0.4),
            .padding_right = CssEm(0.4),
            .color = "#0369a1"
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
            CssRule(".main-wrapper", (CssProps){
                .margin_left = Zero,
                .padding_top = Rem(2.0),
                .padding_bottom = Rem(2.0),
                .padding_left = Rem(1.25),
                .padding_right = Rem(1.25)
            });
        }
    }
}

#endif
