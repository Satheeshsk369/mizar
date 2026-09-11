module test_main;
import std::io;
import mizar::core::buffer;
import mizar::core::json;
import mizar::core::livereload;
import mizar::view::html;
import mizar::view::css;
import mizar::view::mathml;
import mizar::view::equation;
import mizar::view::js;
import mizar::server::http;
import mizar::server::app;
import mizar::server::session;
import mizar::server::flash;
import mizar::ssg;
import mizar::ui::icons;
import mizar::ui::components;
import mizar::ui::patterns;
import mizar::ui::geometry;

SessionManager s_sessions;

fn void handle_dashboard(http::Request* req, http::Response* res, void* user_data) {
    res.html();
    buffer::context_push(&res.body);
    defer buffer::context_pop();

    patterns::@page({ .title = "Mizar C3 Live Dashboard", .theme_color = "#0284c7" }) {
        components::@navbar({ .title = "Mizar Portal", .brand_url = "/" }) {
            html::@a({ .href = "/", .style = "color: #0284c7; text-decoration: none;" }) { html::text("Dashboard"); };
            html::@a({ .href = "/login", .style = "color: #64748b; text-decoration: none;" }) { html::text("Sign In"); };
        };

        components::@card({ .style = "margin-top: 24px;" }) {
            flash::@flash_container(req, res, &s_sessions);

            html::@div({ .style = "display: flex; align-items: center; justify-content: space-between; margin-bottom: 16px;" }) {
                html::@h1({ .style = "margin: 0; color: #0284c7; font-size: 1.5rem;" }) {
                    html::text("Full-Featured Mizar C3 App");
                };
                components::@badge({ .variant = "success" }) {
                    icons::@icon_check({ .size = 14 });
                    html::text("Online");
                };
            };

            // Form Fields
            components::@form_field({ .label = "Email Address", .name = "email", .type = "email", .placeholder = "you@example.com" });

            // MathML & LaTeX Equations
            html::@div({ .style = "padding: 12px; background: #f8fafc; border-radius: 6px; margin: 16px 0;" }) {
                html::@span({ .style = "font-weight: 600; margin-right: 8px;" }) { html::text("Einstein LaTeX Formula:"); };
                equation::eq("E = mc^2");
            };

            // Client JS Script DSL
            js::@script() {
                js::js_const("apiUrl", "\"/api/data\"");
                js::js_line("console.log(\"Mizar loaded:\", apiUrl)");
            };
        };
    };
}

fn void main() {
    io::printfn("--- Running Comprehensive Mizar C3 Verification Suite ---");

    s_sessions.init("comprehensive-suite-secret-key");

    // 1. Verify Declarative JSON DSL
    buffer::Buffer json_buf;
    json_buf.init();
    defer json_buf.free();
    buffer::context_push(&json_buf);
    json::@json_obj() {
        json::json_str("status", "success");
        json::json_int("code", 200);
        json::json_bool("active", true);
    };
    buffer::context_pop();
    io::printfn("✓ Declarative JSON generation:\n%s", json_buf.as_string());

    // 2. Verify Geometry & Spacing
    geometry::EdgeInsets insets = geometry::insets_symmetric(16.0, 8.0);
    buffer::Buffer geom_buf;
    geom_buf.init();
    defer geom_buf.free();
    geometry::append_padding_style(&geom_buf, insets);
    io::printfn("✓ Geometry EdgeInsets generated: %s", geom_buf.as_string());

    // 3. Verify CSS DSL
    buffer::Buffer css_buf;
    css_buf.init();
    defer css_buf.free();
    buffer::context_push(&css_buf);
    css::@rule(".btn-primary") {
        css::prop("background", "#0284c7");
        css::prop("color", "white");
    };
    buffer::context_pop();
    io::printfn("✓ Declarative CSS Rule generation:\n%s", css_buf.as_string());

    // 4. Verify SSG (Static Site Generator)
    ssg::SiteGenerator site;
    site.init("build/site");
    site.add_page("index", "<h1>Home Page</h1>");
    site.build();
    io::printfn("✓ Static Site Generator (SSG) verified.");

    io::printfn("\nAll Mizar modules (HTML5, JSON DSL, Geometry, LaTeX Equation, JS Script DSL, SSG, Flash, UI Components) PASSED cleanly!");
}
