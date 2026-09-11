module mizar::server::app;
import mizar::server::http;
import mizar::algo::radix;
import mizar::core::buffer;
import std::net::tcp;
import std::io;

alias HandlerFn = fn void(http::Request* req, http::Response* res, void* user_data);
alias MiddlewareFn = fn bool(http::Request* req, http::Response* res, void* user_data);

struct MiddlewareEntry {
    MiddlewareFn fn_ptr;
    void* user_data;
}

const usz MAX_MIDDLEWARES = 32;

struct RouteGroup {
    App* app;
    String prefix;
    MiddlewareEntry[MAX_MIDDLEWARES] middlewares;
    usz middleware_count;
}

struct App {
    radix::RadixRouter router;
    MiddlewareEntry[MAX_MIDDLEWARES] middlewares;
    usz middleware_count;
    int worker_threads;
    bool running;
}

fn void App.init(&this) {
    this.router.init();
    this.middleware_count = 0;
    this.worker_threads = 4;
    this.running = false;
}

fn void App.free(&this) {
    this.router.free();
}

fn void App.use(&this, MiddlewareFn mw, void* user_data = null) {
    if (this.middleware_count < MAX_MIDDLEWARES) {
        this.middlewares[this.middleware_count++] = {
            .fn_ptr = mw,
            .user_data = user_data
        };
    }
}

fn void App.get(&this, String pattern, HandlerFn handler, void* user_data = null) {
    this.router.insert("GET", pattern, (radix::HandlerFn)handler, user_data);
}

fn void App.post(&this, String pattern, HandlerFn handler, void* user_data = null) {
    this.router.insert("POST", pattern, (radix::HandlerFn)handler, user_data);
}

fn void App.put(&this, String pattern, HandlerFn handler, void* user_data = null) {
    this.router.insert("PUT", pattern, (radix::HandlerFn)handler, user_data);
}

fn void App.delete(&this, String pattern, HandlerFn handler, void* user_data = null) {
    this.router.insert("DELETE", pattern, (radix::HandlerFn)handler, user_data);
}

fn void App.patch(&this, String pattern, HandlerFn handler, void* user_data = null) {
    this.router.insert("PATCH", pattern, (radix::HandlerFn)handler, user_data);
}

// -----------------------------------------------------------------------------
// Route Groups
// -----------------------------------------------------------------------------
fn RouteGroup App.group(&this, String prefix) {
    RouteGroup g;
    g.app = this;
    g.prefix = prefix;
    g.middleware_count = 0;
    return g;
}

fn void RouteGroup.use(&this, MiddlewareFn mw, void* user_data = null) {
    if (this.middleware_count < MAX_MIDDLEWARES) {
        this.middlewares[this.middleware_count++] = {
            .fn_ptr = mw,
            .user_data = user_data
        };
    }
}

fn void RouteGroup.get(&this, String pattern, HandlerFn handler, void* user_data = null) {
    char[256] pbuf;
    int n = 0;
    for (int i = 0; i < this.prefix.len && n < 254; i++) pbuf[n++] = this.prefix[i];
    if (pattern.len > 0) {
        if (pattern[0] != '/' && (n == 0 || pbuf[n - 1] != '/')) {
            pbuf[n++] = '/';
        }
        for (int i = 0; i < pattern.len && n < 255; i++) {
            if (i == 0 && pattern[0] == '/' && n > 0 && pbuf[n - 1] == '/') continue;
            pbuf[n++] = pattern[i];
        }
    }
    pbuf[n] = '\0';
    char* permanent_path = (char*)mem::malloc((sz)n);
    mem::copy(permanent_path, &pbuf[0], (sz)n);
    this.app.get((String)permanent_path[0..n - 1], handler, user_data);
}

// -----------------------------------------------------------------------------
// Request Dispatching & Lifecycle
// -----------------------------------------------------------------------------
fn void App.handle(&this, http::Request* req, http::Response* res) {
    for (usz i = 0; i < this.middleware_count; i++) {
        bool cont = this.middlewares[i].fn_ptr(req, res, this.middlewares[i].user_data);
        if (!cont) return;
    }

    radix::RouteMatch match = this.router.lookup(req.method, req.path, &req.param_map);
    if (match.found && match.handler != null) {
        HandlerFn h = (HandlerFn)match.handler;
        h(req, res, match.user_data);
        return;
    }

    res.status(404, "Not Found");
    res.html();
    buffer::context_push(&res.body);
    res.body.append_str("<h1>404 Not Found</h1><p>The requested route does not exist.</p>");
    buffer::context_pop();
}

// -----------------------------------------------------------------------------
// Cross-Platform std::net::tcp Listener
// -----------------------------------------------------------------------------
fn bool App.listen(&this, int port, String host = "0.0.0.0") {
    tcp::TcpServerSocket? server_opt = tcp::listen(host, port, 128);
    if (catch err = server_opt) {
        io::printfn("Error: Failed to listen on %s:%d", host, port);
        return false;
    }
    tcp::TcpServerSocket server = server_opt;
    defer (void)server.close();

    io::printfn("=================================================");
    io::printfn("  🚀 Mizar (std::net::tcp) running at http://%s:%d", host == "0.0.0.0" ? "localhost" : host, port);
    io::printfn("  Press Ctrl+C to terminate the server.");
    io::printfn("=================================================");

    this.running = true;
    char[4096] read_buffer;

    while (this.running) {
        tcp::TcpSocket? client_opt = tcp::accept(&server);
        if (catch err = client_opt) continue;
        tcp::TcpSocket client = client_opt;
        defer (void)client.close();

        sz? bytes_read_opt = client.read(&read_buffer);
        if (catch err = bytes_read_opt) continue;
        sz bytes_read = bytes_read_opt;

        if (bytes_read > 0) {
            read_buffer[bytes_read] = '\0';
            String raw_http = (String)read_buffer[0..(usz)bytes_read - 1];

            http::Request req;
            req.init();
            defer req.free();

            http::Response res;
            res.init();
            defer res.free();

            if (req.parse(raw_http)) {
                this.handle(&req, &res);
            } else {
                res.status(400, "Bad Request");
            }

            buffer::Buffer out_buf;
            out_buf.init();
            defer out_buf.free();
            res.serialize(&out_buf);

            (void)client.write(out_buf.data[0..out_buf.len - 1]);
        }
    }

    return true;
}

// Built-in Standard Middleware
fn bool security_headers_middleware(http::Request* req, http::Response* res, void* user_data) {
    res.header("X-Content-Type-Options", "nosniff");
    res.header("X-Frame-Options", "DENY");
    res.header("X-XSS-Protection", "1; mode=block");
    res.header("Referrer-Policy", "strict-origin-when-cross-origin");
    return true;
}
