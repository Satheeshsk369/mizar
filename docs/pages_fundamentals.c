#include "docs_shared.h"

// -----------------------------------------------------------------------------
// 1. OVERVIEW / HOME
// -----------------------------------------------------------------------------
static void content_index(void) {
    Header(.cls = "page-header") {
        H1() { Text("Mizar Web Framework"); }
        P(.cls = "tagline") {
            Text("Modern, high-performance web development and static site generation in ISO C23.");
        }
    }

    H2() { Text("Why Mizar?"); }
    P() {
        Text("Mizar proves that modern full-stack web development does not require bloated virtual machines, "
             "fragile JavaScript build pipelines, or gigabytes of dependencies. By combining ISO C23 designated compound "
             "literals, thread-local buffer context streaming, and an event-driven non-blocking socket engine, Mizar delivers "
             "sub-millisecond HTTP response times with less than 2 MB idle memory.");
    }

    Div(.cls = "callout") {
        H4() { Text("Core Pillars"); }
        Ul() {
            Li() { Text("Zero-dependency ISO C23 codebase compiling to native machine code in <0.5 seconds."); }
            Li() { Text("JSX-like declarative DSL for HTML5, SVG vectors, MathML equations, and client-side scripts."); }
            Li() { Text("Battle-tested algorithmic core in src/algo/: Arenas, SipHash, Radix tree, Ring buffers, and LRU cache."); }
            Li() { Text("Cryptographic RFC 2104 HMAC-SHA256 tamper-proof sessions with constant-time verification."); }
            Li() { Text("Native HTMX 4 & 2 integration for reactive server-driven UI without writing custom frontend JS."); }
        }
    }

    H2() { Text("Navigating This Manual"); }
    P() {
        Text("This documentation is structured into two complementary tracks:");
    }
    Ul() {
        Li() {
            Code() { Text("Progressive Examples"); }
            Text(" — For application developers. A progressive, file-by-file journey through building real-world web apps, APIs, and static sites with Mizar.");
        }
        Li() {
            Code() { Text("Source Architecture"); }
            Text(" — For contributors and systems developers. Detailed explanations of the internal memory models, algorithm choices, epoll/kqueue event loops, and how to extend Mizar.");
        }
    }
}

void page_index(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Mizar Overview", "index", content_index);
}

// -----------------------------------------------------------------------------
// 2. INTRODUCTION
// -----------------------------------------------------------------------------
static void content_intro(void) {
    Header(.cls = "page-header") {
        H1() { Text("Introduction & Design Principles"); }
        P(.cls = "tagline") {
            Text("Predictable memory models, non-blocking I/O, and type-checked C23 composition.");
        }
    }

    H2() { Text("The C23 Advantage"); }
    P() {
        Text("Mizar leverages features introduced in ISO C23 to make C web programming expressive and safe:");
    }
    Ul() {
        Li() { Text("Standardized nullptr eliminates NULL pointer ambiguities."); }
        Li() { Text("Compound literal designated initializers enable declarative JSX-style parameter passing: Div(.id = \"box\", .cls = \"flex\")"); }
        Li() { Text("Thread-local storage (thread_local) eliminates global locks and enables lock-free request rendering."); }
    }

    H2() { Text("Explicit Ownership Contracts"); }
    P() {
        Text("Unlike traditional C servers that scatter malloc and free across dozens of functions, Mizar enforces clear lifecycle contracts:");
    }
    Ul() {
        Li() {
            Code() { Text("MzRequest"); }
            Text(" — Borrowed state. Backed by an MzArena that allocates all headers, parameters, and cookies sequentially. Freed in one single bulk deallocation when the request completes.");
        }
        Li() {
            Code() { Text("MzResponse"); }
            Text(" — Owned state. Handlers write directly into res->body (a dynamic MizarBuffer). Headers are tracked in a fixed array and flushed directly into the socket.");
        }
    }
}

void page_intro(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Introduction", "intro", content_intro);
}

// -----------------------------------------------------------------------------
// 3. QUICKSTART
// -----------------------------------------------------------------------------
static void content_quickstart(void) {
    Header(.cls = "page-header") {
        H1() { Text("Quickstart Guide"); }
        P(.cls = "tagline") {
            Text("Get your first Mizar web application running in 60 seconds.");
        }
    }

    H2() { Text("Requirements"); }
    Ul() {
        Li() { Text("A C23-capable compiler: GCC 14+ or Clang 18+"); }
        Li() { Text("GNU Make and pkg-config"); }
        Li() { Text("Linux, macOS, or *BSD"); }
    }

    H2() { Text("1. Installation"); }
    Pre() {
        Code() {
            Text("git clone https://github.com/Satheeshsk369/mizar.git\n"
                 "cd mizar\n"
                 "make\n"
                 "sudo make install\n");
        }
    }

    H2() { Text("2. Scaffold a Project"); }
    Pre() {
        Code() {
            Text("mizar init my-app\n"
                 "cd my-app\n"
                 "make run\n");
        }
    }

    H2() { Text("3. Minimal main.c"); }
    Pre() {
        Code() {
            Text("#include <mizar.h>\n\n"
                 "static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {\n"
                 "    (void)req; (void)user_data;\n"
                 "    mz_res_html(res);\n"
                 "    MzPage(&res->body, .title = \"Welcome\") {\n"
                 "        H1() { Text(\"Hello from Mizar!\"); }\n"
                 "        P() { Text(\"Native server-rendered C23 web application.\"); }\n"
                 "    }\n"
                 "}\n\n"
                 "int main(void) {\n"
                 "    MzApp app;\n"
                 "    mz_app_init(&app);\n"
                 "    mz_app_get(&app, \"/\", handle_home);\n"
                 "    mz_app_listen(&app, 3000);\n"
                 "    mz_app_free(&app);\n"
                 "    return 0;\n"
                 "}\n");
        }
    }
}

void page_quickstart(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Quickstart", "quickstart", content_quickstart);
}
