# Mizar

Mizar is an ISO C23 web library and static site generator. It provides a macro-based DSL for declarative HTML5, SVG, MathML, and CSS, an event-driven HTTP router, middleware pipeline, and session management.

## Installation

Requirements: A C23 compiler (`gcc` >= 14 or `clang` >= 18), GNU Make, and `pkg-config`.

```bash
git clone https://github.com/Satheeshsk369/mizar.git
cd mizar
make
sudo make install
```

To build with native **BearSSL TLS** support:

```bash
make TLS=1
sudo make install TLS=1
```

By default, files are installed to `/usr/local`. Override `PREFIX` to change the destination:

```bash
sudo make install PREFIX=/usr
```

To uninstall:

```bash
sudo make uninstall
```

## Quickstart

### 1. Scaffold a New Project

Use the `mizar` CLI to initialize a project:

```bash
mizar init my-app
cd my-app
make run
```

### 2. Application Entrypoint (`main.c`)

```c
#include <mizar.h>

static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    mz_res_html(res);

    MzPage(&res->body, .title = "Mizar Application", .theme_color = "#0284c7") {
        MzFlashContainer();

        Div(.style = "max-width: 640px; margin: 40px auto; font-family: system-ui, sans-serif;") {
            H1() { Text("Mizar + HTMX 4"); }
            P() { Text("Server-rendered UI in C23."); }

            // Live search
            MzSearch(.url = "/api/search", .target = "#results", .placeholder = "Search items...");
            Div(.id = "results", .style = "margin-top: 12px;") {}

            // Click-to-edit field
            MzClickToEdit(.url = "/api/profile/edit", .label = "Username", .value = "alice");
        }
    }
}

static void handle_search(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    const char *q = mz_req_query(req, "q");
    mz_res_html(res);
    mz_context_push(&res->body);
    Div(.style = "padding: 8px 12px; background: #f1f5f9; border-radius: 6px;") {
        Text("Query: %s", q && *q ? q : "(none)");
    }
    mz_context_pop();
}

int main(void) {
    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 4);

    // Middleware stack
    mz_app_use(&app, mz_middleware_security_headers);
    mz_app_use(&app, mz_middleware_logger);

    // Routes
    mz_app_get(&app, "/", handle_home);
    mz_app_get(&app, "/api/search", handle_search);

    // Start HTTP listener (or mz_app_listen_tls for HTTPS)
    mz_app_listen(&app, 3000);

    mz_app_free(&app);
    return 0;
}
```

### 3. Makefile

```makefile
CC ?= gcc
CFLAGS ?= -std=c23 -Wall -Wextra -O2 $(shell pkg-config --cflags mizar)
LIBS ?= $(shell pkg-config --libs mizar)

build/app: main.c
	@mkdir -p build
	$(CC) $(CFLAGS) main.c $(LIBS) -o build/app

run: build/app
	./build/app

clean:
	rm -rf build

.PHONY: run clean
```

Run the server:

```bash
make run
```

Access at `http://localhost:3000`.

## Architecture & Features

### 1. Declarative View DSL
Macros map to scoped C loops that push active buffers into thread-local storage, allowing HTML, SVG, and MathML generation with compile-time checked attributes:

```c
Div(.id = "container", .cls = "flex") {
    Svg(.width = "24", .height = "24", .viewBox = "0 0 24 24") {
        Svg_Circle_(.cx = "12", .cy = "12", .r = "10", .fill = "#2563eb");
    }
}
```

### 2. HTMX 4 Support
Mizar defines attributes for HTMX 4 specifications, including status matching (`hx-status:200`), explicit inheritance (`:inherited`), and append modifiers:

```c
Button(HxPost("/api/items"), HxTarget("#list"), HxSwap("beforeend")) {
    Text("Add Item");
}
```

### 3. UI Component Helpers
Pre-configured layouts and widgets in `src/ui/components/`:
- **Widgets:** Buttons, Badges, Cards, Alerts, Modals, Forms, Breadcrumbs, Tabs, Dropdowns.
- **SVG Icons:** 28 inline SVG helpers in `src/ui/icons.h` (`mz_icon_search()`, `mz_icon_user()`, `mz_icon_trash()`, etc.).

### 4. Middleware Pipeline
Middlewares follow `bool (*)(const MzRequest *req, MzResponse *res, void *user_data)`. Returning `false` stops execution:
- `mz_middleware_security_headers`: Sets HSTS, X-Content-Type-Options, X-Frame-Options, and Referrer-Policy.
- `mz_middleware_logger`: Writes method, route, response status, and duration to stderr.
- `mz_middleware_cors`: Configures CORS headers and handles preflight OPTIONS.

### 5. Session Management & Flash Messages
- Cryptographic Sessions: Tamper-proof cookie sessions signed with standard RFC 2104 **HMAC-SHA256** and verified in constant time (`mz_crypto_timing_safe_eq`), protecting against forged credentials and side-channel timing attacks.
- Flash messages: One-time messages stored across HTTP redirects (`mz_flash_set(...)` / `MzFlashContainer()`).

### 6. Macro Namespace Isolation (`MIZAR_NO_SHORT_TAGS`)
To prevent collisions with POSIX headers (such as `select()` in `<sys/select.h>` or `time()` in `<time.h>`), all HTML elements provide canonical prefixed macros (`MzDiv`, `MzSelect`, `MzTime`, `MzTable`, etc.).
Define `MIZAR_NO_SHORT_TAGS` before including `<mizar.h>` in projects that require standard POSIX socket and time headers.

### 7. High-Performance Radix Tree Routing & Streaming TCP
- $O(k)$ Radix tree router supporting static paths, parameterized segments (`:id`), and wildcards (`*`).
- Non-blocking socket I/O with automatic multi-chunk buffering and partial `send()` handling resistant to slowloris and TCP fragmentation.

### 8. BearSSL TLS
Compile with `TLS=1` to link BearSSL for HTTPS support:

```c
mz_app_listen_tls(&app, 8443, "cert.pem", "key.pem");
```

### 9. Static Site Generation (SSG)
Pre-render static sites to disk with layout composition and static asset syncing:

```c
#include <mizar.h>

static void render_home(MizarBuffer *buf, void *user_data) {
    (void)user_data;
    Html(buf, .lang = "en") {
        Body() {
            H1() { Text("Static Site"); }
        }
    }
}

int main(void) {
    MizarSite site;
    mz_site_init(&site, "dist");
    mz_site_set_static_dir(&site, "static");

    mz_site_add_page(&site, "/", render_home, nullptr);
    mz_site_build(&site);
    mz_site_free(&site);
    return 0;
}
```

## Documentation

To build and view the full documentation site locally:

```bash
make doc-build
make doc-serve
```

Browse `http://localhost:3000`.
