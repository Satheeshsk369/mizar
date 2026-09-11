# Mizar (Pure C3 Hypermedia Web Framework)

Mizar is a declarative hypermedia web framework for the **C3 Programming Language**. It provides a compile-time macro DSL for HTML5, SVG icons, and HTMX 4, an event-driven HTTP router with radix route matching, scoped route groups, middleware, and cryptographic session management.

## Requirements

- **C3 Compiler (`c3c`)** >= 0.8.0

## Quickstart

### 1. Build Mizar CLI & Library

```bash
make
sudo make install
```

### 2. Scaffold a New Project

```bash
mizar init my-app
cd my-app
```

### 3. Application Entrypoint (`src/main.c3`)

```c3
module app_main;
import std::io;
import mizar::core::buffer;
import mizar::view::html;
import mizar::server::http;
import mizar::server::app;
import mizar::ui::icons;
import mizar::ui::components;
import mizar::ui::patterns;

fn void handle_home(http::Request* req, http::Response* res, void* user_data) {
    res.html();
    buffer::context_push(&res.body);
    defer buffer::context_pop();

    patterns::@page({ .title = "Mizar + C3 App", .theme_color = "#0284c7" }) {
        components::@card() {
            html::@div({ .style = "display: flex; align-items: center; justify-content: space-between;" }) {
                html::@h1({ .style = "color: #0284c7;" }) {
                    html::text("Mizar Hypermedia in Pure C3");
                };
                components::@badge({ .variant = "success" }) {
                    icons::@icon_check({ .size = 14 });
                    html::text("Active");
                };
            };

            components::@alert({ .title = "HTMX 4 Ready", .variant = "info" }) {
                html::text("Declarative server-rendered components with zero client JavaScript!");
            };

            // Active debounced search
            patterns::@search({ .url = "/api/search", .target = "#results", .placeholder = "Search items..." });
            html::@div({ .id = "results", .style = "margin-top: 12px;" }) {};

            // Hypermedia Click-to-Edit
            patterns::@click_to_edit({ .url = "/api/profile/edit", .label = "Username", .value = "alice_c3" });
        };
    };
}

fn void handle_search(http::Request* req, http::Response* res, void* user_data) {
    res.html();
    buffer::context_push(&res.body);
    defer buffer::context_pop();

    String q = req.query("q");
    html::@div({ .style = "padding: 8px 12px; background: #f1f5f9; border-radius: 6px;" }) {
        html::text("Query: ");
        html::text(q.len > 0 ? q : "(none)");
    };
}

fn void main() {
    app::App server;
    server.init();
    defer server.free();

    // Middleware
    server.use(&app::security_headers_middleware);

    // Routes
    server.get("/", &handle_home);

    // Scoped Route Groups
    app::RouteGroup api = server.group("/api");
    api.get("/search", &handle_search);

    // Cross-platform TCP listener
    server.listen(8080);
}
```

### 4. Build and Run

```bash
c3c run
# or
make run
```

Then visit `http://localhost:8080` in your browser.

## Features

- **Trailing Body Macros**: Intuitive nested HTML5 / SVG DSL (`@div`, `@card`, `@search`) without preprocessor hacks.
- **Cross-Platform Networking**: Built on C3's `std::net::tcp`.
- **Radix Router**: Prefix tree routing supporting static paths, parameterized `:param` routes, and `*` catch-alls.
- **Route Groups**: Nest routes and middlewares under common prefixes (`server.group("/api")`).
- **Cryptographic Sessions**: Tamper-proof cookie signing using C3 standard library `std::hash::sha256::HmacSha256`.
- **Standard SVG Icons**: 28 inline vector icons rendered with zero external asset loading.
- **Zero-Dependency**: No external C/C++ runtimes required. Compiles with `c3c` in seconds.
