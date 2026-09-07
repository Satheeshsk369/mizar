#include <stdio.h>
#include <string.h>
#include "mizar.h"
#include "layout.h"
#include "style.h"

// -----------------------------------------------------------------------------
// 1. INDEX / OVERVIEW
// -----------------------------------------------------------------------------
static void content_index(void) {
    Header(.cls = "page-header") {
        H1() { Text("Mizar Web Framework"); }
        P(.cls = "tagline") {
            Text("A declarative, high-performance web framework and static site generator written in ISO C23.");
        }
    }

    H2() { Text("The Mental Model: Web in Pure C"); }
    P() {
        Text("Web development in C has historically suffered from messy string concatenations, memory leaks, and fragmented tooling. "
             "Mizar solves this by bringing modern declarative syntax directly to C23 using macro-driven block scoping and RAII patterns.");
    }

    Div(.cls = "callout") {
        H4() { Text("Why C23?"); }
        P() {
            Text("C23 introduces standardized nullptr, enhanced compound literals, typeof, and improved attribute syntax. "
                 "Mizar leverages compound literals (Attrs){ .id = \"foo\" } to give C developer ergonomics comparable to JSX or Swift, "
                 "without runtime overhead or garbage collection.");
        }
    }

    H2() { Text("How the DSL Works"); }
    P() {
        Text("Every HTML element in Mizar is a macro wrapping a C for-loop. The loop opens the tag, pushes the active buffer into thread-local storage, executes your child block, closes the tag, and pops the buffer context:");
    }

    Pre() {
        Code() {
            Text("// What you write:\n"
                 "Div(.id = \"card\", .cls = \"p-4\") {\n"
                 "    H2() { Text(\"Hello C23\"); }\n"
                 "    P() { Text(\"Zero runtime memory leaks.\"); }\n"
                 "}\n");
        }
    }

    H2() { Text("Core Architecture Pillars"); }
    Ul() {
        Li() {
            Code() { Text("Declarative Views"); }
            Text(" — HTML5, SVG, and MathML with full attribute validation and auto-escaping.");
        }
        Li() {
            Code() { Text("Math & LaTeX (EqText)"); }
            Text(" — Write $E = mc^2$ and $$\\frac{a}{b}$$ naturally with automated escape repairs.");
        }
        Li() {
            Code() { Text("Static Site Engine"); }
            Text(" — Multi-page site generation with asset copying and layout composition.");
        }
        Li() {
            Code() { Text("High-Concurrency Server"); }
            Text(" — Worker-pool architecture backed by Linux epoll and BSD/macOS kqueue.");
        }
    }
}

static void page_index(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Overview & Architecture", "index", content_index);
}

// -----------------------------------------------------------------------------
// 2. QUICKSTART
// -----------------------------------------------------------------------------
static void content_quickstart(void) {
    Header(.cls = "page-header") {
        H1() { Text("5-Minute Quickstart"); }
        P(.cls = "tagline") {
            Text("From zero to running a reactive C23 web server in less than 5 minutes.");
        }
    }

    H2() { Text("1. Requirements"); }
    P() {
        Text("You need any modern C23-compliant compiler: GCC 14+ or Clang 18+.");
    }

    H2() { Text("2. Minimal Server Example"); }
    P() {
        Text("Create a file named main.c. Notice how Mizar integrates first-class HTMX attributes for interactive UIs:");
    }

    Pre() {
        Code() {
            Text("#include <mizar.h>\n\n"
                 "static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    (void)req; (void)user_data;\n"
                 "    mz_res_html(res);\n"
                 "    Html(&res->body, .lang = \"en\") {\n"
                 "        Head() {\n"
                 "            Title() { Text(\"My C Web App\"); }\n"
                 "            Script(.src = \"https://unpkg.com/htmx.org@2.0.4\") {}\n"
                 "        }\n"
                 "        Body() {\n"
                 "            H1() { Text(\"Welcome to Mizar\"); }\n"
                 "            Button(HxGet(\"/api/hello\"), HxTarget(\"#out\")) {\n"
                 "                Text(\"Click Me\");\n"
                 "            }\n"
                 "            Div(.id = \"out\") {}\n"
                 "        }\n"
                 "    }\n"
                 "}\n\n"
                 "int main(void) {\n"
                 "    MzApp app;\n"
                 "    mz_app_init(&app);\n"
                 "    mz_app_get(&app, \"/\", handle_home);\n"
                 "    mz_app_listen(&app, 8080);\n"
                 "    mz_app_free(&app);\n"
                 "    return 0;\n"
                 "}\n");
        }
    }

    H2() { Text("3. Compilation"); }
    P() {
        Text("Compile directly using pkg-config or standard flags:");
    }
    Pre() {
        Code() {
            Text("gcc -std=c23 $(pkg-config --cflags mizar) main.c $(pkg-config --libs mizar) -o myapp\n"
                 "./myapp\n");
        }
    }
}

static void page_quickstart(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "5-Minute Quickstart", "quickstart", content_quickstart);
}

// -----------------------------------------------------------------------------
// 3. HTML & SVG DSL
// -----------------------------------------------------------------------------
static void content_html_svg(void) {
    Header(.cls = "page-header") {
        H1() { Text("HTML5 & SVG DSL"); }
        P(.cls = "tagline") {
            Text("Declarative vector graphics and semantic markup in pure C.");
        }
    }

    H2() { Text("HTML Tags and Named Attributes"); }
    P() {
        Text("All standard HTML5 tags are capitalized macros: Div, P, Span, Table, Form, Input_, etc. "
             "Tags ending with an underscore (like Input_() or Img_()) are self-closing void elements.");
    }

    Pre() {
        Code() {
            Text("Form(.method = \"POST\", .action = \"/submit\") {\n"
                 "    Label(.for_id = \"name\") { Text(\"Your Name:\"); }\n"
                 "    Input_(.type = \"text\", .id = \"name\", .placeholder = \"Jane Doe\", .required = true);\n"
                 "    Button(.type = \"submit\") { Text(\"Send\"); }\n"
                 "}\n");
        }
    }

    H2() { Text("Declarative SVG in C"); }
    P() {
        Text("Generate SVG graphics with zero external image processing libraries. "
             "Mizar provides Svg(), Svg_Circle_(), Svg_Rect_(), Svg_Path_(), and more:");
    }

    Pre() {
        Code() {
            Text("Svg(.width = \"120\", .height = \"120\", .viewBox = \"0 0 100 100\") {\n"
                 "    Svg_Circle_(.cx = \"50\", .cy = \"50\", .r = \"40\", .fill = \"#3b82f6\", .stroke = \"#1d4ed8\", .stroke_width = \"3\");\n"
                 "}\n");
        }
    }

    P() { Text("Live rendered result directly from C:"); }
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
// 4. EQUATIONS & MATHML
// -----------------------------------------------------------------------------
static void content_equations(void) {
    Header(.cls = "page-header") {
        H1() { Text("Equations & MathML"); }
        P(.cls = "tagline") {
            Text("Typesetting scientific formulas and equations with native MathML and KaTeX/LaTeX.");
        }
    }

    H2() { Text("Natural LaTeX with EqText()"); }
    P() {
        Text("Writing complex formulas in C strings often leads to accidental escape character traps (like \\f turning into a form-feed). "
             "Mizar's EqText() solves this by scanning your text for $inline$ and $$block$$ math while automatically repairing C literal escapes:");
    }

    Pre() {
        Code() {
            Text("EqText(\"Einstein discovered that $E = mc^2$, which explains energy.\");\n"
                 "EqText(\"The quadratic root is given by: $$x = \\frac{-b \\pm \\sqrt{b^2 - 4ac}}{2a}$$\");\n");
        }
    }

    H2() { Text("Live Equation Rendering"); }
    Div(.cls = "math-display") {
        EqText("$$x = \\frac{-b \\pm \\sqrt{b^2 - 4ac}}{2a}$$");
    }

    Div(.cls = "math-display") {
        EqText("$$\\int_0^\\infty e^{-x^2} dx = \\frac{\\sqrt{\\pi}}{2}$$");
    }

    H2() { Text("Native MathML (Zero JavaScript)"); }
    P() {
        Text("For environments where client-side JavaScript is forbidden, Mizar supports pure W3C MathML out of the box:");
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
// 5. STATIC SITE GENERATOR (SSG)
// -----------------------------------------------------------------------------
static void content_ssg(void) {
    Header(.cls = "page-header") {
        H1() { Text("Static Site Generator"); }
        P(.cls = "tagline") {
            Text("Lightning fast, multi-threaded static website builds written in C.");
        }
    }

    H2() { Text("How MizarSite Works"); }
    P() {
        Text("Mizar includes a built-in SSG engine (MizarSite). It accepts page routes, recursive static asset folders, "
             "and compiles hundreds of HTML pages directly to disk in milliseconds.");
    }

    Pre() {
        Code() {
            Text("MizarSite site;\n"
                 "mz_site_init(&site, \"dist\");\n"
                 "mz_site_set_static_dir(&site, \"static\");\n\n"
                 "// Register pages\n"
                 "mz_site_add_page(&site, \"/\", render_home);\n"
                 "mz_site_add_page(&site, \"/about\", render_about);\n\n"
                 "// Build site\n"
                 "if (mz_site_build(&site)) {\n"
                 "    printf(\"Site built successfully to dist/\\n\");\n"
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
// 6. HTTP SERVER ENGINE
// -----------------------------------------------------------------------------
static void content_server(void) {
    Header(.cls = "page-header") {
        H1() { Text("HTTP Server Engine"); }
        P(.cls = "tagline") {
            Text("Event-driven epoll/kqueue multi-threaded HTTP server.");
        }
    }

    H2() { Text("Architecture & Concurrency"); }
    P() {
        Text("MzApp does not spawn a new thread per request. Instead, it utilizes an asynchronous event loop with an OS-native multiplexer:");
    }

    Ul() {
        Li() { Text("Linux: edge-triggered epoll (EPOLLET) with thread-safe SO_REUSEPORT listeners."); }
        Li() { Text("macOS / FreeBSD: kernel event queue (kqueue)."); }
        Li() { Text("Other POSIX: portable poll() fallback."); }
    }

    Pre() {
        Code() {
            Text("MzApp app;\n"
                 "mz_app_init(&app);\n"
                 "mz_app_set_workers(&app, 8); // Spin 8 event-loop worker threads\n\n"
                 "mz_app_get(&app, \"/api/health\", handle_health);\n"
                 "mz_app_listen(&app, 3000);\n");
        }
    }
}

static void page_server(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "HTTP Server Engine", "server", content_server);
}

// -----------------------------------------------------------------------------
// MAIN ENTRYPOINT (subcommands: "build" and "serve")
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
        printf("Building Mizar documentation site with MizarSite...\n");

        MizarSite site;
        mz_site_init(&site, "build/docs");
        mz_site_set_static_dir(&site, "docs/static");

        // Clean 3-argument API (user_data defaults to nullptr)
        mz_site_add_page(&site, "/", page_index);
        mz_site_add_page(&site, "/quickstart", page_quickstart);
        mz_site_add_page(&site, "/html-svg", page_html_svg);
        mz_site_add_page(&site, "/equations", page_equations);
        mz_site_add_page(&site, "/ssg", page_ssg);
        mz_site_add_page(&site, "/server", page_server);

        if (!mz_site_build(&site)) {
            fprintf(stderr, "Failed to build documentation site.\n");
            mz_site_free(&site);
            return 1;
        }

        // Generate the entire stylesheet using the type-safe C23 CSS DSL
        MizarBuffer css_buf;
        mz_buf_init(&css_buf, 4096);
        render_docs_stylesheet(&css_buf);
        mz_fs_write_file("build/docs/style.css", css_buf.data, css_buf.len);
        mz_buf_free(&css_buf);

        mz_site_free(&site);
        printf("Documentation successfully generated in build/docs/\n");
    }

    if (do_serve) {
        printf("Serving docs on http://localhost:%d (Ctrl+C to stop)...\n", port);
        mz_site_serve("build/docs", port);
    }

    return 0;
}
