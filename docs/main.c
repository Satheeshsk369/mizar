#include "docs_shared.h"
#include "style.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        // 1. Fundamentals
        mz_site_add_page(&site, "/", page_index);
        mz_site_add_page(&site, "/intro", page_intro);
        mz_site_add_page(&site, "/quickstart", page_quickstart);

        // 2. Progressive Examples
        mz_site_add_page(&site, "/examples/minimal", page_example_minimal);
        mz_site_add_page(&site, "/examples/routing", page_example_routing);
        mz_site_add_page(&site, "/examples/components", page_example_components);
        mz_site_add_page(&site, "/examples/htmx", page_example_htmx);
        mz_site_add_page(&site, "/examples/middleware", page_example_middleware);
        mz_site_add_page(&site, "/examples/ssg", page_example_ssg);

        // 3. Source Architecture & Internals
        mz_site_add_page(&site, "/source/overview", page_source_overview);
        mz_site_add_page(&site, "/source/algo", page_source_algo);
        mz_site_add_page(&site, "/source/server", page_source_server);
        mz_site_add_page(&site, "/source/view", page_source_view);
        mz_site_add_page(&site, "/source/ssg", page_source_ssg);

        // Register searchable entries dynamically
        docs_register_search_item("/", "Mizar Overview", "Fundamentals",
            "Mizar Web Framework ISO C23 sub-millisecond HTTP response predictable memory consumption non-blocking socket IO");
        docs_register_search_item("/intro/", "Introduction & Design Principles", "Fundamentals",
            "C23 nullptr compound literals designated initializers thread-local storage MzRequest borrowed state MzResponse owned state");
        docs_register_search_item("/quickstart/", "Quickstart Guide", "Fundamentals",
            "Quickstart installation toolchain GCC 14 Clang 18 mizar init scaffold minimal main.c handle_home mz_app_init mz_app_listen");
        docs_register_search_item("/examples/minimal/", "01. Minimal Server", "Examples",
            "Minimal Server architecture main.c breakdown worker thread pool SO_REUSEPORT epoll kqueue mz_app_set_workers mz_res_html");
        docs_register_search_item("/examples/routing/", "02. Routing, Queries & Forms", "Examples",
            "URL routing parameterized tokens :id mz_req_param mz_req_param_int query strings mz_req_query POST form submission mz_req_form");
        docs_register_search_item("/examples/components/", "03. HTML5, SVG & MathML Composition", "Examples",
            "JSX-style C23 macro DSL compound literals Div Section H3 Text SVG circle path MathML Math Math_Mrow Math_Mi Math_Mo");
        docs_register_search_item("/examples/htmx/", "04. HTMX 4 Hypermedia Integration", "Examples",
            "HTMX 4 hypermedia returning fragments vs full pages mz_req_is_htmx HxGet HxTarget HxSwap beforeend MzPage partial swap");
        docs_register_search_item("/examples/middleware/", "05. Middlewares, Sessions & Toasts", "Examples",
            "Middlewares chained interceptors cryptographic HMAC-SHA256 MzSession mz_session_set mz_session_write flash toasts mz_res_flash");
        docs_register_search_item("/examples/ssg/", "06. Static Site Generation Pipeline", "Examples",
            "Static Site Generator MizarSite mz_site_init mz_site_set_static_dir mz_site_add_page mz_site_build mz_site_serve dist preview");
        docs_register_search_item("/source/overview/", "Source Architecture & Memory Model", "Source",
            "Source repository layout algo server core view ssg ui per-request arena contract single-shot bulk deallocation mz_req_free");
        docs_register_search_item("/source/algo/", "algo/ Data Structures & Algorithms", "Source",
            "MzArena monotonic bump allocator MzStrView zero-copy slices SipHash-1-3 Hash-DoS MzHashMap open-addressing Radix tree MzRingBuf UTF-8 DFA MzLruCache");
        docs_register_search_item("/source/server/", "server/ Network Engine & Multiplexers", "Source",
            "Server network multiplexers Linux epoll EPOLLET macOS BSD kqueue poll streaming request accumulator mz_socket_write_all EAGAIN");
        docs_register_search_item("/source/view/", "view/ Declarative DSL Mechanics", "Source",
            "View declarative DSL mechanics loop-macro technique scoped for-loops context stack depth recovery mz_context_restore_depth");
        docs_register_search_item("/source/ssg/", "ssg/ Static Site Generator Architecture", "Source",
            "SSG architecture zero-allocation route vectors MzVec path canonicalization realpath directory traversal defense");

        if (!mz_site_build(&site)) {
            fprintf(stderr, "Failed to build documentation.\n");
            mz_site_free(&site);
            return 1;
        }

        // Export stylesheet
        MizarBuffer css_buf;
        mz_buf_init(&css_buf, 4096);
        render_docs_stylesheet(&css_buf);
        mz_fs_write_file("build/docs/style.css", css_buf.data, css_buf.len);
        mz_buf_free(&css_buf);

        // Export dynamic search index
        docs_export_search_index("build/docs/search-index.json");

        mz_site_free(&site);
        printf("Documentation built successfully in build/docs/\n");
    }

    if (do_serve) {
        printf("Serving documentation on http://localhost:%d\n", port);
        mz_site_serve("build/docs", port);
    }

    return 0;
}
