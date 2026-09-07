#ifndef DOCS_LAYOUT_H
#define DOCS_LAYOUT_H

#include "mizar.h"

static inline void render_sidebar(const char *current_slug) {
    Aside(.cls = "sidebar") {
        A(.cls = "brand", .href = "/") {
            Text("Mizar");
            Span() { Text("C23"); }
        }

        Div(.cls = "nav-section") {
            Div(.cls = "nav-title") { Text("Start"); }
            A(.cls = strcmp(current_slug, "index") == 0 ? "nav-link active" : "nav-link", .href = "/") {
                Text("Overview");
            }
            A(.cls = strcmp(current_slug, "quickstart") == 0 ? "nav-link active" : "nav-link", .href = "/quickstart/") {
                Text("Quickstart");
            }
        }

        Div(.cls = "nav-section") {
            Div(.cls = "nav-title") { Text("Views"); }
            A(.cls = strcmp(current_slug, "html-svg") == 0 ? "nav-link active" : "nav-link", .href = "/html-svg/") {
                Text("HTML & SVG");
            }
            A(.cls = strcmp(current_slug, "equations") == 0 ? "nav-link active" : "nav-link", .href = "/equations/") {
                Text("Equations");
            }
        }

        Div(.cls = "nav-section") {
            Div(.cls = "nav-title") { Text("Backend"); }
            A(.cls = strcmp(current_slug, "ssg") == 0 ? "nav-link active" : "nav-link", .href = "/ssg/") {
                Text("SSG");
            }
            A(.cls = strcmp(current_slug, "server") == 0 ? "nav-link active" : "nav-link", .href = "/server/") {
                Text("Server");
            }
        }
    }
}

static inline void docs_layout(MizarBuffer *buf, const char *title, const char *slug, void (*content_fn)(void)) {
    Html(buf, .lang = "en") {
        Head() {
            Meta(.charset = "UTF-8");
            Meta(.name = "viewport", .content = "width=device-width, initial-scale=1.0");
            Title() { Text("%s — Mizar Framework Documentation", title); }
            Link(.rel = "stylesheet", .href = "/style.css");
            // KaTeX for math pages
            Link(.rel = "stylesheet", .href = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/contrib/auto-render.min.js") {}
            Script() {
                Raw("document.addEventListener('DOMContentLoaded', function() {\n"
                    "  renderMathInElement(document.body);\n"
                    "});\n");
            }
        }
        Body() {
            render_sidebar(slug);
            Main(.cls = "main-wrapper") {
                content_fn();
                Footer() {
                    Span() { Text("Built with Mizar static site generator in C23."); }
                    Span() { Text("Zero external runtime dependencies."); }
                }
            }
        }
    }
}

#endif
