#include "docs_shared.h"

// -----------------------------------------------------------------------------
// EXAMPLE 01: MINIMAL SERVER
// -----------------------------------------------------------------------------
static void content_ex_minimal(void) {
    Header(.cls = "page-header") {
        H1() { Text("01. Minimal Server Architecture"); }
        P(.cls = "tagline") {
            Text("Dissecting entrypoints, workers, routes, and lifecycle management in main.c.");
        }
    }

    H2() { Text("The Concept"); }
    P() {
        Text("Every Mizar application starts with an MzApp instance. Unlike heavy event frameworks that mandate complex "
             "asynchronous callback chains, Mizar uses synchronous handler functions executed across a worker thread pool "
             "backed by SO_REUSEPORT and OS multiplexers (epoll on Linux, kqueue on BSD/macOS).");
    }

    H2() { Text("Full main.c Walkthrough"); }
    Pre() {
        Code() {
            Text("#include <mizar.h>\n\n"
                 "// 1. Route Handler Signature\n"
                 "// Handlers take (const MzRequest *req, MzResponse *res, void *user_data)\n"
                 "static void handle_hello(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    (void)req;\n"
                 "    (void)user_data;\n\n"
                 "    // Set content type to text/html; charset=utf-8\n"
                 "    mz_res_html(res);\n\n"
                 "    // Write HTML content directly into res->body\n"
                 "    mz_context_push(&res->body);\n"
                 "    H1() { Text(\"Hello World\"); }\n"
                 "    P() { Text(\"Rendered in C23.\"); }\n"
                 "    mz_context_pop();\n"
                 "}\n\n"
                 "int main(void) {\n"
                 "    // 2. Initialize application state and O(k) radix router\n"
                 "    MzApp app;\n"
                 "    mz_app_init(&app);\n\n"
                 "    // 3. Configure thread pool concurrency (e.g. 4 worker threads)\n"
                 "    mz_app_set_workers(&app, 4);\n\n"
                 "    // 4. Register routes\n"
                 "    mz_app_get(&app, \"/\", handle_hello);\n\n"
                 "    // 5. Start HTTP server (blocks until SIGINT / SIGTERM)\n"
                 "    printf(\"Listening on http://localhost:3000\\n\");\n"
                 "    mz_app_listen(&app, 3000);\n\n"
                 "    // 6. Clean up radix tree, LRU caches, and middlewares\n"
                 "    mz_app_free(&app);\n"
                 "    return 0;\n"
                 "}\n");
        }
    }
}

void page_example_minimal(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "01. Minimal Server", "ex-minimal", content_ex_minimal);
}

// -----------------------------------------------------------------------------
// EXAMPLE 02: ROUTING & PARAMETERS
// -----------------------------------------------------------------------------
static void content_ex_routing(void) {
    Header(.cls = "page-header") {
        H1() { Text("02. Routing, Queries & Forms"); }
        P(.cls = "tagline") {
            Text("Extracting path tokens (:id), query strings, and form bodies with O(1) hash maps.");
        }
    }

    H2() { Text("Parameterized URL Routing"); }
    P() {
        Text("Paths support parameterized tokens such as :id or :slug. The radix router extracts tokens and stores them "
             "in the request arena:");
    }

    Pre() {
        Code() {
            Text("%s",
                 "static void handle_user(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    // 1. Path parameters (:id)\n"
                 "    const char *id = mz_req_param(req, \"id\");\n\n"
                 "    // Tagged type-safe integer parser\n"
                 "    MzIntResult id_res = mz_req_param_int(req, \"id\");\n"
                 "    if (!id_res.ok) {\n"
                 "        mz_res_status(res, 400, \"Bad Request\");\n"
                 "        return;\n"
                 "    }\n\n"
                 "    // 2. Query string parameters (?tab=profile&page=1)\n"
                 "    const char *tab = mz_req_query(req, \"tab\");\n"
                 "    int64_t page = mz_req_query_int_or(req, \"page\", 1);\n\n"
                 "    mz_res_html(res);\n"
                 "    mz_context_push(&res->body);\n"
                 "    H2() { Text(\"User #%lld (Tab: %s, Page: %lld)\", id_res.val, tab ? tab : \"default\", page); }\n"
                 "    mz_context_pop();\n"
                 "}\n\n"
                 "// Registration:\n"
                 "mz_app_get(&app, \"/users/:id\", handle_user);\n");
        }
    }

    H2() { Text("POST / Form Submission"); }
    P() {
        Text("Mizar automatically parses application/x-www-form-urlencoded bodies into O(1) hash tables:");
    }

    Pre() {
        Code() {
            Text("static void handle_login(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    const char *username = mz_req_form(req, \"username\");\n"
                 "    const char *password = mz_req_form(req, \"password\");\n\n"
                 "    if (username && password && strcmp(username, \"admin\") == 0) {\n"
                 "        mz_res_redirect(res, \"/dashboard\");\n"
                 "    } else {\n"
                 "        mz_res_status(res, 401, \"Unauthorized\");\n"
                 "    }\n"
                 "}\n\n"
                 "mz_app_post(&app, \"/login\", handle_login);\n");
        }
    }
}

void page_example_routing(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "02. Routing & Params", "ex-routing", content_ex_routing);
}

// -----------------------------------------------------------------------------
// EXAMPLE 03: VIEW DSL
// -----------------------------------------------------------------------------
static void content_ex_components(void) {
    Header(.cls = "page-header") {
        H1() { Text("03. HTML5, SVG & MathML Composition"); }
        P(.cls = "tagline") {
            Text("Compile-time checked JSX-style macro DSL in ISO C23.");
        }
    }

    H2() { Text("JSX-like C23 Macro DSL"); }
    P() {
        Text("Every view macro expands into a scoped C for-loop that pushes to a thread-local MizarBuffer. "
             "Attributes are compile-time type-checked using compound designated initializers:");
    }

    Pre() {
        Code() {
            Text("%s",
                 "Div(.id = \"dashboard\", .cls = \"grid grid-cols-2 gap-4\") {\n"
                 "    Section(.cls = \"card\") {\n"
                 "        H3() { Text(\"Analytics\"); }\n"
                 "        P() { Text(\"Active users: %d\", 128); }\n"
                 "    }\n"
                 "    Section(.cls = \"card\") {\n"
                 "        H3() { Text(\"Status\"); }\n"
                 "        Span(.cls = \"badge success\") { Text(\"Healthy\"); }\n"
                 "    }\n"
                 "}\n");
        }
    }

    H2() { Text("Vector Graphics (SVG)"); }
    Pre() {
        Code() {
            Text("Svg(.width = \"48\", .height = \"48\", .viewBox = \"0 0 24 24\", .fill = \"none\", .stroke = \"currentColor\") {\n"
                 "    Svg_Circle_(.cx = \"12\", .cy = \"12\", .r = \"10\", .stroke_width = \"2\");\n"
                 "    Svg_Path_(.d = \"M12 6v6l4 2\", .stroke_width = \"2\", .stroke_linecap = \"round\");\n"
                 "}\n");
        }
    }

    H2() { Text("W3C MathML Equations"); }
    Pre() {
        Code() {
            Text("Math(.display = \"block\") {\n"
                 "    Math_Mrow() {\n"
                 "        Math_Mi() { Text(\"E\"); }\n"
                 "        Math_Mo() { Text(\"=\"); }\n"
                 "        Math_Mi() { Text(\"m\"); }\n"
                 "        Math_Msup() {\n"
                 "            Math_Mi() { Text(\"c\"); }\n"
                 "            Math_Mn() { Text(\"2\"); }\n"
                 "        }\n"
                 "    }\n"
                 "}\n");
        }
    }
}

void page_example_components(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "03. HTML, SVG & Math", "ex-components", content_ex_components);
}

// -----------------------------------------------------------------------------
// EXAMPLE 04: HTMX 4 HYPERMEDIA
// -----------------------------------------------------------------------------
static void content_ex_htmx(void) {
    Header(.cls = "page-header") {
        H1() { Text("04. HTMX 4 Hypermedia Integration"); }
        P(.cls = "tagline") {
            Text("Building reactive single-page experiences without writing frontend JavaScript.");
        }
    }

    H2() { Text("Returning Fragments vs Full Pages"); }
    P() {
        Text("With HTMX, initial page requests render the complete HTML shell, while subsequent interactions swap "
             "targeted fragments into place:");
    }

    Pre() {
        Code() {
            Text("%s",
                 "static void handle_feed(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    mz_res_html(res);\n"
                 "    mz_context_push(&res->body);\n\n"
                 "    if (mz_req_is_htmx(req)) {\n"
                 "        // Render only the partial fragment\n"
                 "        Div(.id = \"feed-item\") { Text(\"New item loaded at %s\", timestamp); }\n"
                 "    } else {\n"
                 "        // Render full page shell\n"
                 "        MzPage(&res->body, .title = \"Live Feed\") {\n"
                 "            Button(HxGet(\"/feed\"), HxTarget(\"#feed-list\"), HxSwap(\"beforeend\")) {\n"
                 "                Text(\"Load More\");\n"
                 "            }\n"
                 "            Div(.id = \"feed-list\") {}\n"
                 "        }\n"
                 "    }\n"
                 "    mz_context_pop();\n"
                 "}\n");
        }
    }
}

void page_example_htmx(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "04. HTMX 4 Hypermedia", "ex-htmx", content_ex_htmx);
}

// -----------------------------------------------------------------------------
// EXAMPLE 05: MIDDLEWARE & AUTH
// -----------------------------------------------------------------------------
static void content_ex_middleware(void) {
    Header(.cls = "page-header") {
        H1() { Text("05. Middlewares, Sessions & Toasts"); }
        P(.cls = "tagline") {
            Text("Chained request interceptors, cryptographic HMAC sessions, and flash notifications.");
        }
    }

    H2() { Text("Cryptographic HMAC-SHA256 Sessions"); }
    P() {
        Text("Sessions are signed with a secret key using RFC 2104 HMAC-SHA256 and verified in constant time:");
    }

    Pre() {
        Code() {
            Text("static void handle_login_success(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    const char *secret = \"super-secret-key-32-bytes-long!\";\n\n"
                 "    MzSession sess;\n"
                 "    mz_session_clear(&sess);\n"
                 "    mz_session_set(&sess, \"user_id\", \"42\");\n"
                 "    mz_session_set(&sess, \"role\", \"admin\");\n\n"
                 "    // Set signed cookie\n"
                 "    mz_session_write(res, &sess, secret, (MzCookieOpts){\n"
                 "        .max_age = 86400, // 24 hours\n"
                 "        .http_only = true,\n"
                 "        .same_site = \"Lax\"\n"
                 "    });\n\n"
                 "    // Trigger client-side flash toast\n"
                 "    mz_res_flash(res, \"Login successful! Welcome back.\", MZ_FLASH_SUCCESS);\n"
                 "    mz_res_redirect(res, \"/dashboard\");\n"
                 "}\n");
        }
    }
}

void page_example_middleware(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "05. Middleware & Auth", "ex-middleware", content_ex_middleware);
}

// -----------------------------------------------------------------------------
// EXAMPLE 06: STATIC SITE GENERATION (SSG)
// -----------------------------------------------------------------------------
static void content_ex_ssg(void) {
    Header(.cls = "page-header") {
        H1() { Text("06. Static Site Generation Pipeline"); }
        P(.cls = "tagline") {
            Text("Pre-rendering documentation, blogs, and marketing sites to static files.");
        }
    }

    H2() { Text("SSG Architecture"); }
    P() {
        Text("The MizarSite engine renders registered pages into an output directory, copies static assets recursively, "
             "and includes a built-in development preview server:");
    }

    Pre() {
        Code() {
            Text("#include <mizar.h>\n\n"
                 "static void page_home(MizarBuffer *buf, void *data) {\n"
                 "    Html(buf, .lang = \"en\") {\n"
                 "        Head() { Title() { Text(\"My Static Site\"); } }\n"
                 "        Body() { H1() { Text(\"Welcome\"); } }\n"
                 "    }\n"
                 "}\n\n"
                 "int main(void) {\n"
                 "    MizarSite site;\n"
                 "    mz_site_init(&site, \"dist\");\n"
                 "    mz_site_set_static_dir(&site, \"assets\");\n\n"
                 "    mz_site_add_page(&site, \"/\", page_home);\n\n"
                 "    // Builds HTML pages into dist/\n"
                 "    mz_site_build(&site);\n\n"
                 "    // Preview on http://localhost:8080\n"
                 "    mz_site_serve(\"dist\", 8080);\n\n"
                 "    mz_site_free(&site);\n"
                 "    return 0;\n"
                 "}\n");
        }
    }
}

void page_example_ssg(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "06. Static Site Pipeline", "ex-ssg", content_ex_ssg);
}
