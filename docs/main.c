#include <stdio.h>
#include <string.h>
#include "mizar.h"
#include "layout.h"
#include "style.h"

// -----------------------------------------------------------------------------
// 1. INDEX / ARCHITECTURE
// -----------------------------------------------------------------------------
static void content_index(void) {
    Header(.cls = "page-header") {
        H1() { Text("Mizar Reference Manual"); }
        P(.cls = "tagline") {
            Text("C23 Web Framework and Static Site Generator.");
        }
    }

    H2() { Text("Design Principles"); }
    P() {
        Text("Mizar is an ISO C23 web library and static generator designed for predictable memory consumption, "
             "non-blocking socket I/O, and type-checked template composition. It requires no code generation, transpilation, "
             "or runtime garbage collection.");
    }

    Div(.cls = "callout") {
        H4() { Text("ISO C23 Primitives"); }
        P() {
            Text("Mizar uses standardized nullptr, designated compound literals, typeof_unqual, and thread-local storage. "
                 "Attributes are validated at compile time by the C compiler via compound literal designated initializers: "
                 "(Attrs){ .id = \"foo\", .required = true }.");
        }
    }

    H2() { Text("Macro-Driven Block Scoping"); }
    P() {
        Text("Every view macro expands into a scoped C for-loop. The loop writes the opening tag to a thread-local MizarBuffer, "
             "executes the inner statement block, and emits the closing tag upon exit:");
    }

    Pre() {
        Code() {
            Text("// Source code:\n"
                 "Div(.id = \"card\", .cls = \"p-4\") {\n"
                 "    H2() { Text(\"Dashboard\"); }\n"
                 "    P() { Text(\"Buffered directly into res->body without dynamic heap strings.\"); }\n"
                 "}\n");
        }
    }

    H2() { Text("Core Architecture & Memory Model"); }
    P() {
        Text("Understanding memory lifecycle in Mizar is straightforward because ownership contracts are explicit:");
    }
    Ul() {
        Li() {
            Code() { Text("MzRequest"); }
            Text(" — Borrowed state. URL parameters, query strings, headers, and body pointers point directly into the incoming thread socket buffer. They require zero heap allocations and are valid only for the duration of your handler.");
        }
        Li() {
            Code() { Text("MzResponse"); }
            Text(" — Owned state. The handler writes into res->body (a dynamic MizarBuffer). Headers added with mz_res_header() are stored in a fixed array inside MzResponse. When the response is dispatched over the wire, MzApp automatically frees the buffer memory.");
        }
        Li() {
            Code() { Text("MizarBuffer"); }
            Text(" — Linear dynamic byte buffer with 2x exponential growth strategy and zero-copy string slicing.");
        }
        Li() {
            Code() { Text("Thread-Local Context"); }
            Text(" — Nested view macros (Div, P, Form) record the active target buffer in a thread-local pointer. Calling Text(...) or inner tags automatically writes into the current active context without needing to pass buffer pointers around.");
        }
    }
}

static void page_index(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Architecture & Design", "index", content_index);
}

// -----------------------------------------------------------------------------
// 2. QUICKSTART
// -----------------------------------------------------------------------------
static void content_quickstart(void) {
    Header(.cls = "page-header") {
        H1() { Text("Quickstart"); }
        P(.cls = "tagline") {
            Text("Building and running your first Mizar application.");
        }
    }

    H2() { Text("Toolchain Requirements"); }
    Ul() {
        Li() { Text("C compiler supporting ISO C23 (GCC 14+ or Clang 18+)."); }
        Li() { Text("POSIX system (Linux, macOS, *BSD) with pthreads."); }
        Li() { Text("pkg-config and GNU Make."); }
    }

    H2() { Text("Project Initialization"); }
    P() {
        Text("Use the mizar CLI to scaffold a new project directory:");
    }

    Pre() {
        Code() {
            Text("mizar init myapp\n"
                 "cd myapp\n"
                 "make run\n");
        }
    }

    H2() { Text("Minimal Server (main.c)"); }
    Pre() {
        Code() {
            Text("#include <mizar.h>\n\n"
                 "static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    (void)req; (void)user_data;\n"
                 "    mz_res_html(res);\n"
                 "    Html(&res->body, .lang = \"en\") {\n"
                 "        Head() {\n"
                 "            Title() { Text(\"Mizar App\"); }\n"
                 "            MzHtmx();\n"
                 "        }\n"
                 "        Body() {\n"
                 "            H1() { Text(\"Application Root\"); }\n"
                 "            Button(HxGet(\"/api/status\"), HxTarget(\"#status\")) {\n"
                 "                Text(\"Check Status\");\n"
                 "            }\n"
                 "            Div(.id = \"status\") {}\n"
                 "        }\n"
                 "    }\n"
                 "}\n\n"
                 "int main(void) {\n"
                 "    MzApp app;\n"
                 "    mz_app_init(&app);\n"
                 "    mz_app_use(&app, mz_middleware_security_headers);\n"
                 "    mz_app_get(&app, \"/\", handle_home);\n"
                 "    mz_app_listen(&app, 3000);\n"
                 "    mz_app_free(&app);\n"
                 "    return 0;\n"
                 "}\n");
        }
    }

    H2() { Text("Compiling Manually"); }
    Pre() {
        Code() {
            Text("gcc -std=c23 $(pkg-config --cflags mizar) main.c $(pkg-config --libs mizar) -o build/app\n"
                 "./build/app\n");
        }
    }
}

static void page_quickstart(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Quickstart", "quickstart", content_quickstart);
}

// -----------------------------------------------------------------------------
// 3. HTML & SVG DSL
// -----------------------------------------------------------------------------
static void content_html_svg(void) {
    Header(.cls = "page-header") {
        H1() { Text("HTML5 & SVG DSL"); }
        P(.cls = "tagline") {
            Text("Compile-time typed elements and vector generation.");
        }
    }

    H2() { Text("HTML Elements and Attributes"); }
    P() {
        Text("Paired tags (such as Div, P, Span, Form) accept child statements in curly braces. "
             "Self-closing void elements (such as Input_, Img_, Meta_, Link_) end with an underscore and accept no block.");
    }

    Pre() {
        Code() {
            Text("Form(.method = \"POST\", .action = \"/login\") {\n"
                 "    Label(.for_id = \"user\") { Text(\"Username:\"); }\n"
                 "    Input_(.type = \"text\", .id = \"user\", .name = \"user\", .required = true);\n"
                 "    Button(.type = \"submit\") { Text(\"Sign In\"); }\n"
                 "}\n");
        }
    }

    H2() { Text("Text Formatting & Automatic HTML Escaping"); }
    P() {
        Text("To prevent Cross-Site Scripting (XSS), Text() automatically escapes HTML entities (<, >, &, \", ') "
             "unless you explicitly use Raw():");
    }

    Pre() {
        Code() {
            Text("%s",
                 "// Safely escaped: converts '<script>' to '&lt;script&gt;'\n"
                 "Text(\"User input: %s\", user_provided_string);\n\n"
                 "// Raw unescaped HTML (use with caution):\n"
                 "Raw(\"<span class=\\\"custom\\\">trusted markup</span>\");\n");
        }
    }

    H2() { Text("Writing Custom Reusable Components"); }
    P() {
        Text("Because Mizar uses thread-local buffers, creating custom components requires only a regular C function. "
             "You do not need to pass buffer pointers through every helper:");
    }

    Pre() {
        Code() {
            Text("%s",
                 "void render_user_card(const char *name, const char *role) {\n"
                 "    Div(.cls = \"user-card\") {\n"
                 "        H3() { Text(\"%s\", name); }\n"
                 "        P(.cls = \"text-muted\") { Text(\"%s\", role); }\n"
                 "    }\n"
                 "}\n\n"
                 "// In your route handler:\n"
                 "render_user_card(\"Alice\", \"Site Reliability Engineer\");\n");
        }
    }

    H2() { Text("SVG Primitives"); }
    P() {
        Text("SVG elements output directly to the active MizarBuffer without third-party graphics dependencies:");
    }

    Pre() {
        Code() {
            Text("Svg(.width = \"120\", .height = \"120\", .viewBox = \"0 0 100 100\") {\n"
                 "    Svg_Circle_(.cx = \"50\", .cy = \"50\", .r = \"40\", .fill = \"#3b82f6\", .stroke = \"#1d4ed8\", .stroke_width = \"3\");\n"
                 "}\n");
        }
    }

    P() { Text("Rendered SVG output:"); }
    Div(.cls = "callout") {
        Svg(.width = "100", .height = "100", .viewBox = "0 0 100 100") {
            Svg_Circle_(.cx = "50", .cy = "50", .r = "40", .fill = "#58a6ff", .stroke = "#1f6feb", .stroke_width = "4");
        }
    }
}

static void page_html_svg(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "HTML5 & SVG DSL", "html-svg", content_html_svg);
}

// -----------------------------------------------------------------------------
// 4. UI COMPONENTS
// -----------------------------------------------------------------------------
static void content_components(void) {
    Header(.cls = "page-header") {
        H1() { Text("UI Components"); }
        P(.cls = "tagline") {
            Text("Pre-styled UI elements configured via C23 designated initializers.");
        }
    }

    H2() { Text("Buttons and Badges"); }
    Pre() {
        Code() {
            Text("MizarButton(.variant = MZ_BTN_PRIMARY, .size = MZ_SIZE_MD) {\n"
                 "    Text(\"Submit\");\n"
                 "}\n"
                 "MizarBadge(.variant = MZ_BADGE_SUCCESS) {\n"
                 "    Text(\"Connected\");\n"
                 "}\n");
        }
    }

    H2() { Text("Alerts and Cards"); }
    Pre() {
        Code() {
            Text("MizarAlert(.variant = MZ_ALERT_INFO, .title = \"Notice\") {\n"
                 "    Text(\"Session configuration reloaded.\");\n"
                 "}\n"
                 "\n"
                 "MizarCard(.title = \"Node 01\") {\n"
                 "    P() { Text(\"CPU: 12%%, Memory: 1.2 GB / 16 GB\"); }\n"
                 "}\n");
        }
    }

    H2() { Text("SVG Icons"); }
    P() {
        Text("Standard icons are defined in src/ui/icons.h as inline SVG emitters: "
             "mz_icon_search(), mz_icon_user(), mz_icon_check(), mz_icon_trash(), mz_icon_settings(), mz_icon_refresh(), etc.");
    }
}

static void page_components(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "UI Components", "components", content_components);
}

// -----------------------------------------------------------------------------
// 5. HTMX 4 & HYPERMEDIA
// -----------------------------------------------------------------------------
static void content_htmx(void) {
    Header(.cls = "page-header") {
        H1() { Text("Pages, Fragments & HTMX 4"); }
        P(.cls = "tagline") {
            Text("The hypermedia mental model: returning full documents vs isolated HTML fragments.");
        }
    }

    H2() { Text("The Mental Model: Full Page vs HTML Fragment"); }
    P() {
        Text("In traditional web development, APIs return JSON and a client framework renders it. In hypermedia (HTMX), the server returns HTML. "
             "The key concept to master is distinguishing between a Full Page Request and an HTMX Partial Swap:");
    }

    Ul() {
        Li() {
            Code() { Text("Full Page (Initial Visit)"); }
            Text(" — Browser performs a standard GET. You render an entire HTML document (<!DOCTYPE html>, <html>, <head>, <body>) using MzPage() or Html().");
        }
        Li() {
            Code() { Text("Fragment (Partial Swap)"); }
            Text(" — User clicks an HTMX button. The browser sends an hx-get request. Your handler only returns the target snippet (e.g. just a <div> or <tr>), which HTMX swaps directly into the existing DOM.");
        }
    }

    H2() { Text("Full Document Example vs Fragment Example"); }
    Pre() {
        Code() {
            Text("%s",
                 "// 1. Full Page Handler (Initial visit to /)\n"
                 "static void handle_page(const MzRequest *req, MzResponse *res, void *udata) {\n"
                 "    mz_res_html(res);\n"
                 "    MzPage(&res->body, .title = \"Hypermedia App\") {\n"
                 "        Div(.id = \"counter-box\") {\n"
                 "            P() { Text(\"Count: 0\"); }\n"
                 "            Button(HxPost(\"/api/increment\"), HxTarget(\"#counter-box\"), HxSwap(\"outerHTML\")) {\n"
                 "                Text(\"Increment\");\n"
                 "            }\n"
                 "        }\n"
                 "    }\n"
                 "}\n\n"
                 "// 2. Fragment Handler (HTMX swap target)\n"
                 "static void handle_increment(const MzRequest *req, MzResponse *res, void *udata) {\n"
                 "    static int count = 0;\n"
                 "    count++;\n"
                 "    mz_res_html(res);\n"
                 "    mz_context_push(&res->body);\n"
                 "    // Notice: No <html>, <head>, or <body>! Only the updated snippet:\n"
                 "    Div(.id = \"counter-box\") {\n"
                 "        P() { Text(\"Count: %d\", count); }\n"
                 "        Button(HxPost(\"/api/increment\"), HxTarget(\"#counter-box\"), HxSwap(\"outerHTML\")) {\n"
                 "            Text(\"Increment\");\n"
                 "        }\n"
                 "    }\n"
                 "    mz_context_pop();\n"
                 "}\n");
        }
    }

    H2() { Text("HTMX 4 Attribute DSL"); }
    P() {
        Text("Mizar maps all modern HTMX attributes directly into type-checked C macros:");
    }

    Ul() {
        Li() { Code() { Text("HxGet(url), HxPost(url), HxPut(url), HxDelete(url)"); } }
        Li() { Code() { Text("HxTarget(sel), HxSwap(method), HxTrigger(evt)"); } }
        Li() { Code() { Text("HxPending(sel), HxDisable(sel), HxConfirm(msg)"); } }
        Li() { Code() { Text("HxTargetInherited(sel), HxSwapInherited(m)"); } Text(" — HTMX 4 explicit inheritance modifier."); }
        Li() { Code() { Text("HxStatus200(spec), HxStatus404(spec), HxStatus5xx(spec)"); } Text(" — Status-driven response handlers."); }
    }

    H2() { Text("Standard Interaction Patterns"); }
    P() {
        Text("Header src/core/patterns.h provides helper macros for common UI workflows:");
    }

    Pre() {
        Code() {
            Text("// Search input with 300ms keyup debounce\n"
                 "MzSearch(.url = \"/api/search\", .target = \"#results\", .placeholder = \"Search...\");\n"
                 "\n"
                 "// Inline click-to-edit field\n"
                 "MzClickToEdit(.url = \"/users/1/status\", .label = \"Status\", .value = \"Active\");\n"
                 "\n"
                 "// Infinite scroll sentinel\n"
                 "MzInfiniteScroll(.url = \"/items?cursor=100\", .trigger = \"revealed\");\n"
                 "\n"
                 "// Table row editing with PUT/DELETE handlers\n"
                 "MzInlineEditRow(.edit_url = \"/items/42/edit\", .delete_url = \"/items/42\");\n");
        }
    }
}

static void page_htmx(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "HTMX 4 Integration", "htmx", content_htmx);
}

// -----------------------------------------------------------------------------
// 6. EQUATIONS & MATHML
// -----------------------------------------------------------------------------
static void content_equations(void) {
    Header(.cls = "page-header") {
        H1() { Text("Equations & MathML"); }
        P(.cls = "tagline") {
            Text("LaTeX text processing and W3C MathML generation.");
        }
    }

    H2() { Text("LaTeX Delimiter Parsing with EqText()"); }
    P() {
        Text("EqText() parses $inline$ and $$block$$ math delimiters and normalizes C literal escape sequences:");
    }

    Pre() {
        Code() {
            Text("EqText(\"Total energy $E = mc^2$.\");\n"
                 "EqText(\"$$\\int_0^\\infty e^{-x^2} dx = \\frac{\\sqrt{\\pi}}{2}$$\");\n");
        }
    }

    H2() { Text("Rendered Formulas"); }
    Div(.cls = "math-display") {
        EqText("$$x = \\frac{-b \\pm \\sqrt{b^2 - 4ac}}{2a}$$");
    }

    Div(.cls = "math-display") {
        EqText("$$\\int_0^\\infty e^{-x^2} dx = \\frac{\\sqrt{\\pi}}{2}$$");
    }

    H2() { Text("W3C MathML"); }
    P() {
        Text("Generate pure MathML XML tags without client JavaScript:");
    }

    Pre() {
        Code() {
            Text("Math(.display = \"block\") {\n"
                 "    Math_Mrow() {\n"
                 "        Math_Mi() { Text(\"x\"); }\n"
                 "        Math_Mo() { Text(\"=\"); }\n"
                 "        Math_Mn() { Text(\"1\"); }\n"
                 "    }\n"
                 "}\n");
        }
    }
}

static void page_equations(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Equations & MathML", "equations", content_equations);
}

// -----------------------------------------------------------------------------
// 7. HTTP SERVER & ROUTING
// -----------------------------------------------------------------------------
static void content_server(void) {
    Header(.cls = "page-header") {
        H1() { Text("HTTP Server Engine"); }
        P(.cls = "tagline") {
            Text("Non-blocking event loop with SO_REUSEPORT worker threads.");
        }
    }

    H2() { Text("I/O Multiplexing"); }
    P() {
        Text("MzApp dispatches sockets across a configurable worker thread pool using the operating system's native event mechanism:");
    }

    Ul() {
        Li() { Text("Linux: edge-triggered epoll (EPOLLET) with SO_REUSEPORT listener sockets per thread."); }
        Li() { Text("macOS / *BSD: kqueue."); }
        Li() { Text("POSIX fallback: poll()."); }
    }

    Pre() {
        Code() {
            Text("MzApp app;\n"
                 "mz_app_init(&app);\n"
                 "mz_app_set_workers(&app, 4);\n\n"
                 "mz_app_get(&app, \"/users/:id\", handle_user);\n"
                 "mz_app_listen(&app, 3000);\n");
        }
    }

    H2() { Text("Path Parameters and Query Strings"); }
    Pre() {
        Code() {
            Text("%s",
                 "static void handle_user(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    const char *user_id = mz_req_param(req, \"id\");\n"
                 "    const char *sort = mz_req_query(req, \"sort\");\n"
                 "    mz_res_text(res, \"User ID: %s, Sort: %s\", user_id, sort ? sort : \"asc\");\n"
                 "}\n");
        }
    }

    H2() { Text("Returning JSON and Setting Status Codes"); }
    P() {
        Text("In addition to HTML, MzResponse provides helpers for raw status codes, JSON payloads, and headers:");
    }

    Pre() {
        Code() {
            Text("static void handle_api_status(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    mz_res_status(res, 200);\n"
                 "    mz_res_header(res, \"Cache-Control\", \"no-cache\");\n"
                 "    mz_res_json(res, \"{\\\"status\\\": \\\"healthy\\\", \\\"uptime_sec\\\": 3600}\");\n"
                 "}\n");
        }
    }
}

static void page_server(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "HTTP Server Engine", "server", content_server);
}

// -----------------------------------------------------------------------------
// 8. MIDDLEWARE PIPELINE
// -----------------------------------------------------------------------------
static void content_middleware(void) {
    Header(.cls = "page-header") {
        H1() { Text("Middleware Pipeline"); }
        P(.cls = "tagline") {
            Text("Chained request interceptors with early-exit support.");
        }
    }

    H2() { Text("Middleware Signature"); }
    P() {
        Text("A middleware callback matches MzMiddlewareFn: bool (*)(const MzRequest*, MzResponse*, void*). "
             "Returning true forwards execution to the next handler; returning false terminates the request pipeline.");
    }

    Pre() {
        Code() {
            Text("bool check_bearer(const MzRequest *req, MzResponse *res, void *udata) {\n"
                 "    const char *auth = mz_req_header(req, \"Authorization\");\n"
                 "    if (!auth || strncmp(auth, \"Bearer \", 7) != 0) {\n"
                 "        mz_res_status(res, 401);\n"
                 "        mz_res_text(res, \"Unauthorized\");\n"
                 "        return false;\n"
                 "    }\n"
                 "    return true;\n"
                 "}\n\n"
                 "mz_app_use(&app, check_bearer);\n");
        }
    }

    H2() { Text("Standard Middlewares"); }
    Ul() {
        Li() {
            Code() { Text("mz_middleware_security_headers"); }
            Text(" — Sets Strict-Transport-Security, X-Content-Type-Options: nosniff, X-Frame-Options: DENY, and Referrer-Policy.");
        }
        Li() {
            Code() { Text("mz_middleware_logger"); }
            Text(" — Writes method, path, HTTP status, and duration to stderr.");
        }
        Li() {
            Code() { Text("mz_middleware_cors"); }
            Text(" — Handles Access-Control-Allow-* headers and OPTIONS preflight requests.");
        }
    }
}

static void page_middleware(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Middleware Pipeline", "middleware", content_middleware);
}

// -----------------------------------------------------------------------------
// 9. SESSION & FLASH
// -----------------------------------------------------------------------------
static void content_session(void) {
    Header(.cls = "page-header") {
        H1() { Text("Sessions and Flash Storage"); }
        P(.cls = "tagline") {
            Text("Thread-safe session storage and redirect messaging.");
        }
    }

    H2() { Text("MzSessionStore"); }
    P() {
        Text("MzSessionStore provides synchronized in-memory key-value maps indexed by 32-character token cookies:");
    }

    Pre() {
        Code() {
            Text("MzSessionStore store;\n"
                 "mz_session_store_init(&store, 3600); // 3600s expiration TTL\n\n"
                 "// Create new session\n"
                 "MzSession *s = mz_session_create(&store);\n"
                 "mz_session_set(s, \"user_id\", \"1024\");\n"
                 "mz_res_header(res, \"Set-Cookie\", mz_session_cookie_header(s));\n\n"
                 "// Read session from request\n"
                 "const char *cookie = mz_req_cookie(req, \"mizar_session\");\n"
                 "MzSession *existing = mz_session_get(&store, cookie);\n");
        }
    }

    H2() { Text("Flash Messaging"); }
    P() {
        Text("Flash messages are stored in cookies and cleared immediately on next read:");
    }

    Pre() {
        Code() {
            Text("// Handler\n"
                 "mz_flash_set(res, \"success\", \"Record updated.\");\n"
                 "mz_res_redirect(res, \"/dashboard\");\n\n"
                 "// View\n"
                 "MzFlashContainer();\n");
        }
    }
}

static void page_session(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Sessions and Flash Storage", "session", content_session);
}

// -----------------------------------------------------------------------------
// 10. BEARSSL TLS / HTTPS
// -----------------------------------------------------------------------------
static void content_tls(void) {
    Header(.cls = "page-header") {
        H1() { Text("BearSSL TLS Configuration"); }
        P(.cls = "tagline") {
            Text("Embedded TLS 1.2 / 1.3 implementation via BearSSL.");
        }
    }

    H2() { Text("Build Flags"); }
    P() {
        Text("Compile with TLS=1 to link against libbearssl:");
    }

    Pre() {
        Code() {
            Text("make TLS=1\n"
                 "sudo make install TLS=1\n");
        }
    }

    H2() { Text("Starting an HTTPS Listener"); }
    Pre() {
        Code() {
            Text("MzApp app;\n"
                 "mz_app_init(&app);\n"
                 "mz_app_get(&app, \"/\", handle_home);\n\n"
                 "// Loads PEM certificate chain and private key\n"
                 "mz_app_listen_tls(&app, 8443, \"certs/server.crt\", \"certs/server.key\");\n"
                 "mz_app_free(&app);\n");
        }
    }
}

static void page_tls(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "BearSSL TLS Configuration", "tls", content_tls);
}

// -----------------------------------------------------------------------------
// 11. STATIC SITE GENERATOR (SSG)
// -----------------------------------------------------------------------------
static void content_ssg(void) {
    Header(.cls = "page-header") {
        H1() { Text("Static Site Generator"); }
        P(.cls = "tagline") {
            Text("Batch rendering of HTML routes to static files.");
        }
    }

    H2() { Text("MizarSite"); }
    P() {
        Text("MizarSite renders registered view callbacks directly into directory structures on disk:");
    }

    Pre() {
        Code() {
            Text("MizarSite site;\n"
                 "mz_site_init(&site, \"dist\");\n"
                 "mz_site_set_static_dir(&site, \"static\");\n\n"
                 "mz_site_add_page(&site, \"/\", render_home);\n"
                 "mz_site_add_page(&site, \"/about\", render_about);\n\n"
                 "if (!mz_site_build(&site)) {\n"
                 "    fprintf(stderr, \"Build failed\\n\");\n"
                 "}\n"
                 "mz_site_free(&site);\n");
        }
    }
}

static void page_ssg(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Static Site Generator", "ssg", content_ssg);
}

// -----------------------------------------------------------------------------
// MAIN ENTRYPOINT
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    const char *subcommand = "build";
    int port = 3000;

    if (argc > 1 && argv[1][0] != '-') {
        subcommand = argv[1];
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        }
    }

    bool do_build = (strcmp(subcommand, "build") == 0 || strcmp(subcommand, "serve") == 0);
    bool do_serve = (strcmp(subcommand, "serve") == 0);

    if (!do_build && !do_serve) {
        fprintf(stderr, "Usage: %s [build|serve] [--port <port>]\n", argv[0]);
        return 1;
    }

    if (do_build) {
        MizarSite site;
        mz_site_init(&site, "build/docs");
        mz_site_set_static_dir(&site, "docs/static");

        mz_site_add_page(&site, "/", page_index);
        mz_site_add_page(&site, "/quickstart", page_quickstart);
        mz_site_add_page(&site, "/html-svg", page_html_svg);
        mz_site_add_page(&site, "/components", page_components);
        mz_site_add_page(&site, "/equations", page_equations);
        mz_site_add_page(&site, "/htmx", page_htmx);
        mz_site_add_page(&site, "/server", page_server);
        mz_site_add_page(&site, "/middleware", page_middleware);
        mz_site_add_page(&site, "/session", page_session);
        mz_site_add_page(&site, "/tls", page_tls);
        mz_site_add_page(&site, "/ssg", page_ssg);

        if (!mz_site_build(&site)) {
            fprintf(stderr, "Failed to build documentation.\n");
            mz_site_free(&site);
            return 1;
        }

        MizarBuffer css_buf;
        mz_buf_init(&css_buf, 4096);
        render_docs_stylesheet(&css_buf);
        mz_fs_write_file("build/docs/style.css", css_buf.data, css_buf.len);
        mz_buf_free(&css_buf);

        // Pre-build searchable full-text index across all generated pages
        const char *search_index_json =
        "[\n"
        "  {\"url\":\"/\",\"title\":\"Architecture & Memory\",\"body\":\"Mizar Reference Manual C23 Web Framework and Static Site Generator Design Principles Predictable memory consumption non-blocking socket IO ISO C23 Primitives nullptr compound literals typeof MzRequest borrowed state socket buffer MzResponse owned state MizarBuffer thread-local context\"},\n"
        "  {\"url\":\"/quickstart/\",\"title\":\"Quickstart\",\"body\":\"Quickstart Building and running your first Mizar application Toolchain Requirements C compiler ISO C23 GCC 14 Clang 18 POSIX pthreads make pkg-config Project Initialization mizar init myapp make run Minimal Server main.c handle_home MzApp mz_app_init mz_app_listen\"},\n"
        "  {\"url\":\"/html-svg/\",\"title\":\"HTML5 & SVG DSL\",\"body\":\"HTML5 & SVG DSL Compile-time typed elements and vector generation Paired tags Div P Span Form void elements Input_ Img_ Meta_ Link_ Svg Svg_Circle_ Svg_Rect_ Svg_Path_ Text auto-escaping XSS Raw unescaped HTML Custom reusable components\"},\n"
        "  {\"url\":\"/components/\",\"title\":\"UI Components\",\"body\":\"UI Components Pre-styled elements Buttons and Badges MizarButton MZ_BTN_PRIMARY MZ_SIZE_MD MizarBadge MZ_BADGE_SUCCESS Alerts and Cards MizarAlert MZ_ALERT_INFO MizarCard SVG Icons mz_icon_search mz_icon_user mz_icon_check mz_icon_trash mz_icon_settings\"},\n"
        "  {\"url\":\"/equations/\",\"title\":\"Equations & MathML\",\"body\":\"Equations & MathML LaTeX text processing and W3C MathML generation LaTeX Delimiter Parsing EqText inline block math delimiters normalize escape sequences KaTeX MathML Math Math_Mrow Math_Mi Math_Mo Math_Mn\"},\n"
        "  {\"url\":\"/htmx/\",\"title\":\"Pages, Fragments & HTMX 4\",\"body\":\"Pages Fragments & HTMX 4 Hypermedia mental model returning full documents vs isolated HTML fragments Full Page Request initial visit MzPage Html Fragment Partial Swap hx-get hx-post counter-box HTMX 4 Attribute DSL HxGet HxPost HxTarget HxSwap HxPending HxDisable HxStatus200 MzSearch MzClickToEdit MzInfiniteScroll MzInlineEditRow\"},\n"
        "  {\"url\":\"/server/\",\"title\":\"HTTP Server Engine\",\"body\":\"HTTP Server Engine Non-blocking event loop with SO_REUSEPORT worker threads IO Multiplexing Linux epoll EPOLLET macOS BSD kqueue poll fallback MzApp mz_app_set_workers mz_app_listen Path Parameters Query Strings mz_req_param mz_req_query mz_res_text mz_res_json mz_res_status mz_res_header\"},\n"
        "  {\"url\":\"/middleware/\",\"title\":\"Middleware Pipeline\",\"body\":\"Middleware Pipeline Chained request interceptors early-exit support MzMiddlewareFn bool return false terminates pipeline mz_middleware_security_headers HSTS nosniff DENY Referrer-Policy mz_middleware_logger mz_middleware_cors preflight OPTIONS\"},\n"
        "  {\"url\":\"/session/\",\"title\":\"Sessions & Flash\",\"body\":\"Sessions and Flash Storage Thread-safe session storage redirect messaging MzSessionStore 32-character token cookies TTL expiration mz_session_create mz_session_set mz_session_get Flash Messaging mz_flash_set MzFlashContainer\"},\n"
        "  {\"url\":\"/tls/\",\"title\":\"BearSSL TLS Configuration\",\"body\":\"BearSSL TLS Configuration Embedded TLS 1.2 1.3 implementation via BearSSL Build Flags make TLS=1 mz_app_listen_tls PEM certificate private key\"},\n"
        "  {\"url\":\"/ssg/\",\"title\":\"Static Site Generator\",\"body\":\"Static Site Generator Batch rendering of HTML routes to static files MizarSite mz_site_init mz_site_set_static_dir mz_site_add_page mz_site_build mz_site_serve dist directory\"}\n"
        "]";
        mz_fs_write_file("build/docs/search-index.json", search_index_json, strlen(search_index_json));

        mz_site_free(&site);
        printf("Documentation built in build/docs/\n");
    }

    if (do_serve) {
        printf("Serving documentation on http://localhost:%d\n", port);
        mz_site_serve("build/docs", port);
    }

    return 0;
}
