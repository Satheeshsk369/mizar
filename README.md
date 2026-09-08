# Mizar

Mizar is a web framework written in C23. It provides a declarative DSL for HTML5, SVG, MathML, and CSS, along with a static site generator and an HTTP server.

## Installation

Requirements: A C23-compliant compiler (`gcc` >= 14 or `clang` >= 18).

```bash
git clone https://github.com/Satheeshsk369/mizar.git
cd mizar
make
sudo make install
```

To build with native **BearSSL TLS / HTTPS** support:

```bash
make TLS=1
sudo make install TLS=1
```

By default, files are installed to `/usr/local`. To install to `/usr`:

```bash
sudo make install PREFIX=/usr
```

To uninstall:

```bash
sudo make uninstall
```

## Quick Start

### 1. Scaffold a New Project

Mizar comes with a built-in project generator:

```bash
mizar-new my-app
cd my-app
make run
```

Or assemble manually:

### 2. Write `main.c`

```c
#include <mizar.h>

static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    mz_res_html(res);

    MzPage(&res->body, .title = "My Mizar App", .theme_color = "#0284c7") {
        MzFlashContainer();

        Div(.style = "max-width: 640px; margin: 40px auto; font-family: system-ui, sans-serif;") {
            H1() { Text("Hello from Mizar + HTMX 4"); }
            P() { Text("High-performance server-driven UI in pure C23."); }

            // Turnkey Instant Search Pattern
            MzSearch(.url = "/api/search", .target = "#results", .placeholder = "Type to search...");
            Div(.id = "results", .style = "margin-top: 12px;") {}

            // Turnkey Click-to-Edit Pattern
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
        Text("Search query: %s", q && *q ? q : "(none)");
    }
    mz_context_pop();
}

int main(void) {
    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 4);

    // Standard security & logging middlewares
    mz_app_use(&app, mz_middleware_security_headers);
    mz_app_use(&app, mz_middleware_logger);

    mz_app_get(&app, "/", handle_home);
    mz_app_get(&app, "/api/search", handle_search);

    // Run HTTP (or mz_app_listen_tls(&app, 8443, "cert.pem", "key.pem"))
    mz_app_listen(&app, 3000);

    mz_app_free(&app);
    return 0;
}
```

### 3. Create `Makefile`

```makefile
CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -O2 $(shell pkg-config --cflags mizar)
LIBS = $(shell pkg-config --libs mizar)

app: main.c
	$(CC) $(CFLAGS) main.c $(LIBS) -o app

run: app
	./app

clean:
	rm -f app

.PHONY: run clean
```

### 4. Build and Run

```bash
make run
```

Open `http://localhost:3000`.

## Static Site Generation (SSG)

Mizar can also pre-render static HTML pages to disk:

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
    mz_site_set_static_dir(&site, "static"); // Copies assets to dist/

    mz_site_add_page(&site, "/", render_home, nullptr);

    mz_site_build(&site);
    mz_site_free(&site);
    return 0;
}
```
