#include "docs_shared.h"

// -----------------------------------------------------------------------------
// SOURCE 01: OVERVIEW & MEMORY MODEL
// -----------------------------------------------------------------------------
static void content_source_overview(void) {
    Header(.cls = "page-header") {
        H1() { Text("Source Architecture & Memory Model"); }
        P(.cls = "tagline") {
            Text("Complete breakdown of Mizar's internal subsystems and memory contracts.");
        }
    }

    H2() { Text("Repository Layout"); }
    Pre() {
        Code() {
            Text("src/\n"
                 "├── algo/      # Pure, self-contained algorithms & data structures\n"
                 "│   ├── arena.h/.c    # Chunk-based bump allocator\n"
                 "│   ├── strview.h     # Zero-copy bounds-checked string slices\n"
                 "│   ├── siphash.h/.c  # 64-bit SipHash-1-3 collision defense\n"
                 "│   ├── hashmap.h/.c  # Open-addressing hash table with arena support\n"
                 "│   ├── radix.h       # O(k) prefix tree router\n"
                 "│   ├── ringbuf.h/.c  # SPSC circular byte stream buffer\n"
                 "│   ├── utf8.h/.c     # DFA UTF-8 decoder & code point validator\n"
                 "│   └── lru.h/.c      # O(1) LRU cache with SipHash & DLL\n"
                 "├── server/    # Non-blocking HTTP socket engine & router\n"
                 "├── core/      # Buffers, URL helpers, tagged results, SHA256\n"
                 "├── view/      # Declarative HTML5, SVG, MathML, and JS DSL\n"
                 "├── ssg/       # Static site generator engine\n"
                 "└── ui/        # Standard pre-styled UI components and icons\n");
        }
    }

    H2() { Text("The Per-Request Arena Contract"); }
    P() {
        Text("Traditional C web servers suffer from memory fragmentation and leaks due to thousands of individual malloc "
             "calls for headers, cookies, and parameters. In Mizar, every incoming request initializes a dedicated MzArena. "
             "All request strings and hash map bucket arrays allocate contiguously inside this arena. "
             "When the request completes, mz_req_free() destroys the entire arena in a single pointer adjustment. Zero leaks.");
    }
}

void page_source_overview(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "Source & Memory Model", "src-overview", content_source_overview);
}

// -----------------------------------------------------------------------------
// SOURCE 02: ALGO/ DEEP DIVE
// -----------------------------------------------------------------------------
static void content_source_algo(void) {
    Header(.cls = "page-header") {
        H1() { Text("algo/ Data Structures & Algorithms"); }
        P(.cls = "tagline") {
            Text("Deep dive into the 8 core algorithm primitives underpinning the framework.");
        }
    }

    H2() { Text("1. MzArena (arena.h / arena.c)"); }
    P() {
        Text("Monotonic bump allocator supporting aligned allocations, zero-initialized memory (mz_arena_calloc), and bulk reset. "
             "Guarantees max_align_t alignment on all platforms.");
    }

    H2() { Text("2. MzStrView (strview.h)"); }
    P() {
        Text("Immutable string slice consisting of (const char *data, size_t len). "
             "Provides constant-time length lookups, case-insensitive comparison (mz_sv_eq_ci), and delimiter splitting without modifying the source buffer.");
    }

    H2() { Text("3. SipHash-1-3 (siphash.h / siphash.c)"); }
    P() {
        Text("Cryptographically hardened 64-bit hash algorithm seeded from /dev/urandom at startup. "
             "Protects HTTP header, query, and form hash maps against algorithmic Hash DoS collision attacks.");
    }

    H2() { Text("4. MzHashMap (hashmap.h / hashmap.c)"); }
    P() {
        Text("Power-of-two open-addressing hash table with linear probing and tombstone markers. "
             "Supports arena-backed allocation (mz_map_init_arena) and lazy capacity expansion.");
    }

    H2() { Text("5. Radix Tree (radix.h)"); }
    P() {
        Text("Prefix tree routing engine matching static paths, parameterized tokens (:id), and wildcards (*) in O(k) time where k is path length.");
    }

    H2() { Text("6. MzRingBuf (ringbuf.h / ringbuf.c)"); }
    P() {
        Text("Power-of-two circular byte buffer for non-blocking stream processing with non-consuming peek operations.");
    }

    H2() { Text("7. DFA UTF-8 Engine (utf8.h / utf8.c)"); }
    P() {
        Text("State-machine based UTF-8 validator and code point decoder based on Bjoern Hoehrmann's DFA algorithm.");
    }

    H2() { Text("8. MzLruCache (lru.h / lru.c)"); }
    P() {
        Text("O(1) Least Recently Used cache combining SipHash hash indexing with an intrusive doubly-linked list for automatic tail eviction.");
    }
}

void page_source_algo(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "algo/ Data Structures", "src-algo", content_source_algo);
}

// -----------------------------------------------------------------------------
// SOURCE 03: SERVER/ NETWORK ENGINE
// -----------------------------------------------------------------------------
static void content_source_server(void) {
    Header(.cls = "page-header") {
        H1() { Text("server/ Network Engine & Multiplexers"); }
        P(.cls = "tagline") {
            Text("Non-blocking TCP socket event loops, worker threads, and HTTP parsing.");
        }
    }

    H2() { Text("OS Multiplexer Selection"); }
    P() {
        Text("Mizar automatically selects the optimal kernel I/O multiplexer at compile time:");
    }
    Ul() {
        Li() { Text("Linux: epoll with EPOLLET edge-triggered notifications."); }
        Li() { Text("macOS / FreeBSD / OpenBSD / NetBSD: kqueue with EVFILT_READ."); }
        Li() { Text("Generic POSIX: poll() fallback."); }
    }

    H2() { Text("Streaming Request Accumulator"); }
    P() {
        Text("To prevent partial TCP segment corruption and defend against Slowloris attacks, mz_process_client accumulates "
             "incoming bytes into a dynamic buffer until headers are complete (\\r\\n\\r\\n). It extracts Content-Length and verifies "
             "that the entire payload has arrived before dispatching to user handlers.");
    }

    H2() { Text("Non-Blocking Partial Write Loop"); }
    P() {
        Text("mz_socket_write_all sends data in a loop, gracefully handling partial writes and polling on POLLOUT when "
             "kernel socket buffers saturate (EAGAIN / EWOULDBLOCK).");
    }
}

void page_source_server(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "server/ Network Engine", "src-server", content_source_server);
}

// -----------------------------------------------------------------------------
// SOURCE 04: VIEW/ DECLARATIVE DSL
// -----------------------------------------------------------------------------
static void content_source_view(void) {
    Header(.cls = "page-header") {
        H1() { Text("view/ Declarative DSL Mechanics"); }
        P(.cls = "tagline") {
            Text("How C23 for-loops, compound literals, and thread-local stacks emulate JSX.");
        }
    }

    H2() { Text("The Loop-Macro Technique"); }
    P() {
        Text("View macros expand into single-iteration C for-loops that manage opening and closing tags automatically:");
    }

    Pre() {
        Code() {
            Text("#define _MZ_TAG(tag_name, ...) \\\n"
                 "    for (int _mz_i = (mz_tag_open(tag_name, (Attrs){ __VA_ARGS__ }), 0); \\\n"
                 "         !_mz_i; \\\n"
                 "         _mz_i = 1, mz_tag_close(tag_name))\n");
        }
    }

    H2() { Text("Context Stack Depth Recovery"); }
    P() {
        Text("Mizar tracks active render targets in thread-local storage (s_context_stack). "
             "If user handler code invokes early return;, mz_app_handle automatically detects the depth difference "
             "and restores the stack with mz_context_restore_depth(), preventing thread context corruption.");
    }
}

void page_source_view(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "view/ Declarative DSL", "src-view", content_source_view);
}

// -----------------------------------------------------------------------------
// SOURCE 05: SSG/ GENERATOR ENGINE
// -----------------------------------------------------------------------------
static void content_source_ssg(void) {
    Header(.cls = "page-header") {
        H1() { Text("ssg/ Static Site Generator Architecture"); }
        P(.cls = "tagline") {
            Text("Batch rendering, recursive asset synchronization, and preview serving.");
        }
    }

    H2() { Text("Zero-Allocation Route Vectors"); }
    P() {
        Text("The MizarSite engine stores route render callbacks in a dynamic MzVec allocated from an internal MzArena. "
             "Pages render sequentially into reusable MizarBuffer instances and flush to disk via mz_fs_write_file.");
    }

    H2() { Text("Path Canonicalization & Traversal Defense"); }
    P() {
        Text("All static asset paths are validated using realpath() against the designated root directory, "
             "completely blocking directory traversal attacks (../) and unauthorized symlink resolution.");
    }
}

void page_source_ssg(MizarBuffer *buf, void *data) {
    (void)data;
    docs_layout(buf, "ssg/ Generator Engine", "src-ssg", content_source_ssg);
}
