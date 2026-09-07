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

By default, files are installed to `/usr/local`. To install to `/usr`:

```bash
sudo make install PREFIX=/usr
```

To uninstall:

```bash
sudo make uninstall
```

## Quick Start

### 1. Create a Project

```bash
mkdir my-app && cd my-app
```

### 2. Write `main.c`

```c
#include <mizar.h>

static void page_content(void) {
    H1() { Text("Hello from Mizar"); }
    P() { Text("Rendered using C23 and HTMX."); }
    Button(HxGet("/api/data"), HxTarget("#result"), HxSwap("innerHTML")) {
        Text("Fetch Content");
    }
    Div(.id = "result") {}
}

static void handle_index(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    mz_res_html(res);
    Html(&res->body, .lang = "en") {
        Head() {
            Title() { Text("My App"); }
            Script(.src = "https://unpkg.com/htmx.org@2.0.4") {}
        }
        Body() {
            page_content();
        }
    }
}

static void handle_data(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    mz_res_html(res);
    mz_context_push(&res->body);
    Span(.style = "color: green;") {
        Text("Dynamic response from server");
    }
    mz_context_pop();
}

int main(void) {
    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 4);

    mz_app_get(&app, "/", handle_index);
    mz_app_get(&app, "/api/data", handle_data);

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
