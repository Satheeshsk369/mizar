#include <stdio.h>
#include <string.h>
#include "mizar.h"

// -----------------------------------------------------------------------------
// HTMX Live Dynamic Endpoints
// -----------------------------------------------------------------------------

// Live Search Endpoint
static void handle_api_search(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    mz_res_html(res);
    mz_context_push(&res->body);

    const char *query = mz_req_form(req, "q");
    if (!query || strlen(query) == 0) query = "";

    Div(.style = "margin-top: 16px; border: 1px solid #e2e8f0; border-radius: 8px; background: #f8fafc; padding: 16px;") {
        H4(.style = "font-size: 0.9rem; color: #64748b; margin-bottom: 8px;") {
            Text("Instant Server Search Results (Query: \"%s\")", query);
        }
        Ul(.style = "margin: 0; padding-left: 20px; font-size: 0.9rem; color: #1e293b;") {
            Li() { Text("Result 1: Reactive hypermedia in pure C23"); }
            Li() { Text("Result 2: Zero-allocation JSON and HTML streaming"); }
            Li() { Text("Result 3: Native LaTeX MathML equations ($E=mc^2$)"); }
        }
    }

    mz_context_pop();
}

// Live Metric Increment Endpoint
static void handle_api_click(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    static int click_count = 100;
    click_count++;

    mz_res_html(res);
    mz_context_push(&res->body);

    Div(.id = "metric-box") {
        UiMetric(
            .label = "Live Active Connections",
            .value = (click_count % 2 == 0) ? "1,424" : "1,425",
            .change = "+28.4%",
            .positive = true,
            .subtext = "Updated live from epoll worker thread without full reload"
        );
    }

    mz_context_pop();
}

// -----------------------------------------------------------------------------
// Showcase Page Content (Using all UI Components)
// -----------------------------------------------------------------------------
static void showcase_page(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    mz_res_html(res);

    Html(&res->body, .lang = "en") {
        Head() {
            Meta(.charset = "UTF-8");
            Meta(.name = "viewport", .content = "width=device-width, initial-scale=1.0");
            Title() { Text("Mizar — Modern C23 UI Showcase"); }
            Script(.src = "https://unpkg.com/htmx.org@2.0.4") {}
            Link(.rel = "stylesheet", .href = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/contrib/auto-render.min.js");
            Script() {
                Raw("document.addEventListener('DOMContentLoaded', function() {\n"
                    "  renderMathInElement(document.body);\n"
                    "});\n");
            }
            Style() {
                Rule("*") { Prop("box-sizing", "border-box"); }
                Rule("body") {
                    Prop("margin", "0");
                    Prop("font-family", "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif");
                    Prop("background", "#f8fafc");
                    Prop("color", "#0f172a");
                    Prop("line-height", "1.6");
                }
            }
        }
        Body() {
            // 1. Navigation
            UiNavbar(.brand = "Mizar Framework", .brand_href = "/") {
                UiBadge("C23 Native", .variant = BADGE_SUCCESS);
                UiBadge("Zero JS Bundle", .variant = BADGE_PRIMARY);
                UiBtn("GitHub", .variant = BTN_OUTLINE);
            }

            UiContainer(.max_width_px = 1100) {
                // 2. Hero Section
                UiHero(
                    .badge = "Pure C23 + HTMX",
                    .title = "A Web Framework Written in C",
                    .tagline = "Declarative HTML, CSS, SVG, MathML, and modern React-like UI components in a single ultra-lightweight binary."
                ) {
                    UiBtn("Live Click Counter", .variant = BTN_PRIMARY, HxPost("/api/click"), HxTarget("#metric-box"));
                    UiBtn("Open Modal", .variant = BTN_SECONDARY, .hx_get = "#", .style = "cursor: pointer;", .cls = "open-modal-btn");
                }

                // 3. Grid of Metric Cards
                UiGrid(.cols = 3, .gap_px = 20) {
                    Div(.id = "metric-box") {
                        UiMetric(.label = "Total Server Requests", .value = "482,900", .change = "+18.2%", .positive = true, .subtext = "Handled across 4 epoll workers");
                    }
                    UiMetric(.label = "Memory Usage", .value = "2.4 MB", .change = "-4.1%", .positive = true, .subtext = "Single compiled machine binary");
                    UiMetric(.label = "Cold Startup Latency", .value = "< 1 ms", .change = "Instant", .positive = true, .subtext = "Zero V8 engine or runtime startup");
                }

                UiDivider(.label = "Interactive Hypermedia & Components");

                // 4. Two-Column Content: Form & Live Search vs. Math & Feedback
                UiGrid(.cols = 2, .gap_px = 24) {
                    // Left Column: Interactive Form & Live Search
                    UiCard(.title = "Interactive HTMX Live Search", .desc = "Type below to trigger real-time server queries:") {
                        UiLiveSearch(.placeholder = "Search backend items...", .hx_post_url = "/api/search", .hx_target = "#search-results", .delay_ms = 250);
                        Div(.id = "search-results") {}

                        UiDivider();

                        H4(.style = "margin-bottom: 12px;") { Text("Form Controls in C23"); }
                        UiField(.label = "Email Address", .name = "email", .type = "email", .placeholder = "you@example.com", .required = true);
                        UiStack(.horizontal = true, .gap_px = 16) {
                            UiCheckbox(.label = "Enable WebSockets", .name = "ws", .checked = true);
                            UiSwitch(.label = "Live Reload", .name = "reload", .checked = true);
                        }
                    }

                    // Right Column: Scientific Math, Status & Testimonial
                    UiCard(.title = "Native Scientific Typesetting", .desc = "Zero-dependency MathML with KaTeX auto-repair:") {
                        Div(.style = "background: #f1f5f9; padding: 16px; border-radius: 8px; margin-bottom: 16px; text-align: center;") {
                            EqText("$$\\int_0^\\infty e^{-x^2} dx = \\frac{\\sqrt{\\pi}}{2}$$");
                            P(.style = "margin-top: 8px; font-size: 0.85rem; color: #64748b;") {
                                EqText("Mass-energy equivalence: $E = mc^2$");
                            }
                        }

                        UiAlert(.variant = ALERT_INFO, .title = "Kernel Multiplexing") {
                            Text("Running with non-blocking edge-triggered epoll and kqueue sockets.");
                        }

                        UiStack(.horizontal = true, .gap_px = 16, .align = "center") {
                            UiStatus(.status = STATUS_ONLINE, .label = "Production Engine");
                            UiKbd("Ctrl + Shift + R");
                        }
                    }
                }

                UiDivider(.label = "Testimonial & Pricing");

                // 5. Testimonial & Pricing Section
                UiGrid(.cols = 2, .gap_px = 24) {
                    UiTestimonial(
                        .quote = "Mizar completely eliminates the Node.js maintenance treadmill. We run our entire web dashboard in under 3MB of memory.",
                        .author = "Alexander Wright",
                        .role = "Systems Engineer",
                        .avatar_initials = "AW"
                    );

                    UiPricing(.plan = "Pro Edition", .price = "$0", .period = "/forever (Open Source)", .popular = true) {
                        Ul(.style = "padding-left: 20px; font-size: 0.9rem; color: #334155; margin: 0;") {
                            Li() { Text("100% C23 type-safe templates & CSS"); }
                            Li() { Text("Single standalone self-contained binary"); }
                            Li() { Text("Linux epoll & macOS kqueue worker pool"); }
                            Li() { Text("Zero node_modules, zero vulnerability alerts"); }
                        }
                    }
                }

                // 6. Modal Dialog (HTML5 native <dialog>)
                UiModal(.id = "demo-modal", .title = "Native Accessible Modal") {
                    P() { Text("This modal is rendered with HTML5 <dialog> and closed natively by the browser without any JavaScript framework."); }
                    UiBtn("Got it", .variant = BTN_PRIMARY, .style = "margin-top: 12px;");
                }
            }

            Footer(.style = "margin-top: 64px; border-top: 1px solid #e2e8f0; padding: 32px 0; text-align: center; color: #64748b; font-size: 0.875rem;") {
                Text("Built with Mizar (ISO C23 Web Framework) • Single Binary Executable");
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Main Entrypoint
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    int port = 4000;
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0) port = 4000;
    }

    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 4);

    // Routes
    mz_app_get(&app, "/", showcase_page);
    mz_app_post(&app, "/api/search", handle_api_search);
    mz_app_post(&app, "/api/click", handle_api_click);

    printf("===================================================================\n");
    printf("  🚀 Mizar Single-Binary Web Showcase Running!                      \n");
    printf("  👉 Open in your browser: http://localhost:%d/                     \n", port);
    printf("  ⚡ Features active: epoll workers, HTMX live search, UI components\n");
    printf("===================================================================\n");

    mz_app_listen(&app, port);

    mz_app_free(&app);
    return 0;
}
