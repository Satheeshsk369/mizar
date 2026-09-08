#define _GNU_SOURCE
#define _DEFAULT_SOURCE
#include "server/app.h"
#include "server/tls.h"
#include "server/radix.h"
#include "ssg/fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>

// Platform multiplexer selection
#if defined(__linux__)
  #define MZ_USE_EPOLL 1
  #include <sys/epoll.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
  #define MZ_USE_KQUEUE 1
  #include <sys/event.h>
  #include <sys/time.h>
#else
  #define MZ_USE_POLL 1
  #include <poll.h>
#endif

#define MZ_MAX_EVENTS 512
#define MZ_BUFFER_SIZE 8192
#define MZ_MAX_REQUEST_SIZE (10 * 1024 * 1024) // 10MB maximum request size
#define MZ_MAX_HEADER_SIZE  (64 * 1024)        // 64KB maximum header size

typedef struct {
    MzApp *app;
    int port;
    int worker_id;
} MzWorkerContext;

static int mz_set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static inline void mz_configure_client_socket(int client_fd) {
    mz_set_nonblocking(client_fd);
    int nodelay = 1;
    setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    // Defense: 10-second socket timeout against Slowloris attacks
    struct timeval timeout = { .tv_sec = 10, .tv_usec = 0 };
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

static int mz_create_listener_socket(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return -1;

    mz_set_nonblocking(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_REUSEPORT
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#endif

    // Disable Nagle's algorithm for low-latency response delivery
    setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 1024) < 0) {
        close(server_fd);
        return -1;
    }

    return server_fd;
}

void mz_app_init(MzApp *app) {
    if (!app) return;
    memset(app, 0, sizeof(MzApp));
    app->worker_threads = 4;
    app->radix_tree = mz_radix_node_create("", MZ_NODE_STATIC, nullptr);
}

void mz_app_set_workers(MzApp *app, int num_threads) {
    if (!app) return;
    app->worker_threads = num_threads > 0 ? num_threads : 1;
}

void mz_app_free(MzApp *app) {
    if (!app) return;

    for (size_t i = 0; i < app->route_count; i++) {
        free(app->routes[i].method);
        free(app->routes[i].pattern);
    }
    free(app->routes);

    if (app->radix_tree) {
        mz_radix_node_free((MzRadixNode *)app->radix_tree);
        app->radix_tree = nullptr;
    }

    free(app->middlewares);
    free(app->static_dir);
    free(app->static_prefix);

    memset(app, 0, sizeof(MzApp));
}

void mz_app_use_impl(MzApp *app, MzMiddlewareFn fn, void *user_data) {
    if (!app || !fn) return;
    if (app->middleware_count >= app->middleware_capacity) {
        size_t new_cap = app->middleware_capacity ? app->middleware_capacity * 2 : 8;
        MzMiddlewareEntry *new_arr = (MzMiddlewareEntry *)realloc(app->middlewares, new_cap * sizeof(MzMiddlewareEntry));
        if (!new_arr) return;
        app->middlewares = new_arr;
        app->middleware_capacity = new_cap;
    }
    app->middlewares[app->middleware_count].fn = fn;
    app->middlewares[app->middleware_count].user_data = user_data;
    app->middleware_count++;
}

void mz_app_static(MzApp *app, const char *url_prefix, const char *dir_path) {
    if (!app || !dir_path) return;
    free(app->static_prefix);
    free(app->static_dir);
    app->static_prefix = url_prefix ? strdup(url_prefix) : strdup("/static");
    app->static_dir = strdup(dir_path);
}

void mz_app_route_impl(MzApp *app, const char *method, const char *pattern, MzHandlerFn handler, void *user_data) {
    if (!app || !method || !pattern || !handler) return;

    if (app->route_count >= app->route_capacity) {
        size_t new_cap = app->route_capacity ? app->route_capacity * 2 : 16;
        MzRouteEntry *new_routes = (MzRouteEntry *)realloc(app->routes, new_cap * sizeof(MzRouteEntry));
        if (!new_routes) return;
        app->routes = new_routes;
        app->route_capacity = new_cap;
    }

    app->routes[app->route_count].method = strdup(method);
    app->routes[app->route_count].pattern = strdup(pattern);
    app->routes[app->route_count].handler = handler;
    app->routes[app->route_count].user_data = user_data;
    app->route_count++;

    if (!app->radix_tree) {
        app->radix_tree = mz_radix_node_create("", MZ_NODE_STATIC, nullptr);
    }
    mz_radix_insert((MzRadixNode *)app->radix_tree, method, pattern, handler, user_data);
}

void mz_app_get_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route_impl(app, "GET", pattern, handler, user_data);
}

void mz_app_post_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route_impl(app, "POST", pattern, handler, user_data);
}

void mz_app_put_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route_impl(app, "PUT", pattern, handler, user_data);
}

void mz_app_delete_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route_impl(app, "DELETE", pattern, handler, user_data);
}

void mz_app_patch_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route_impl(app, "PATCH", pattern, handler, user_data);
}

static bool mz_match_path(const char *pattern, const char *path, MzRequest *req) {
    const char *pat = pattern;
    const char *p = path;

    for (size_t i = 0; i < req->param_count; i++) {
        free(req->params[i].key);
        free(req->params[i].value);
    }
    req->param_count = 0;

    while (*pat && *p) {
        if (*pat == ':') {
            pat++;
            const char *key_start = pat;
            while (*pat && *pat != '/') pat++;
            size_t key_len = pat - key_start;

            const char *val_start = p;
            while (*p && *p != '/') p++;
            size_t val_len = p - val_start;

            if (req->param_count < MZ_HTTP_MAX_PARAMS) {
                char *k = (char *)malloc(key_len + 1);
                char *v = (char *)malloc(val_len + 1);
                memcpy(k, key_start, key_len);
                k[key_len] = '\0';
                memcpy(v, val_start, val_len);
                v[val_len] = '\0';

                req->params[req->param_count].key = k;
                req->params[req->param_count].value = v;
                req->param_count++;
            }
        } else if (*pat == *p) {
            pat++;
            p++;
        } else {
            return false;
        }
    }

    return (*pat == '\0' && *p == '\0');
}

static const char *mz_mime_type_for_file(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    if (strcmp(dot, ".html") == 0) return "text/html; charset=utf-8";
    if (strcmp(dot, ".css") == 0) return "text/css; charset=utf-8";
    if (strcmp(dot, ".js") == 0) return "application/javascript; charset=utf-8";
    if (strcmp(dot, ".json") == 0) return "application/json";
    if (strcmp(dot, ".svg") == 0) return "image/svg+xml";
    if (strcmp(dot, ".png") == 0) return "image/png";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(dot, ".ico") == 0) return "image/x-icon";
    return "application/octet-stream";
}

void mz_app_handle(MzApp *app, MzRequest *req, MzResponse *res) {
    if (!app || !req || !res) return;

    int saved_context_depth = mz_context_get_depth();

    // 1. Middleware chain
    for (size_t i = 0; i < app->middleware_count; i++) {
        bool cont = app->middlewares[i].fn(req, res, app->middlewares[i].user_data);
        if (!cont) {
            mz_context_restore_depth(saved_context_depth);
            return;
        }
    }

    // 2. Static files
    if (app->static_dir && app->static_prefix) {
        size_t prefix_len = strlen(app->static_prefix);
        if (strncmp(req->path, app->static_prefix, prefix_len) == 0) {
            const char *subpath = req->path + prefix_len;
            while (*subpath == '/') subpath++;

            // Strict path traversal defense: forbid any ".." components
            if (strstr(subpath, "..")) {
                mz_res_status(res, 403, "Forbidden");
                mz_res_html(res);
                mz_buf_append_str(&res->body, "<h1>403 Forbidden</h1><p>Path traversal detected.</p>");
                return;
            }

            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", app->static_dir, subpath);

            // Canonical path check via realpath
            char canonical_base[1024];
            char canonical_file[1024];
            if (realpath(app->static_dir, canonical_base) && realpath(filepath, canonical_file)) {
                size_t base_len = strlen(canonical_base);
                if (strncmp(canonical_file, canonical_base, base_len) != 0 ||
                    (canonical_file[base_len] != '/' && canonical_file[base_len] != '\0')) {
                    mz_res_status(res, 403, "Forbidden");
                    mz_res_html(res);
                    mz_buf_append_str(&res->body, "<h1>403 Forbidden</h1><p>Access denied.</p>");
                    return;
                }

                FILE *f = fopen(canonical_file, "rb");
                if (f) {
                    fseek(f, 0, SEEK_END);
                    long fsize = ftell(f);
                    fseek(f, 0, SEEK_SET);

                    mz_res_status(res, 200, "OK");
                    mz_res_content_type(res, mz_mime_type_for_file(canonical_file));

                    // Chunked streaming read into response buffer (prevents massive monolithic allocations)
                    if (fsize > 0 && fsize < 64 * 1024 * 1024) {
                        char chunk[16384];
                        size_t remaining = (size_t)fsize;
                        while (remaining > 0) {
                            size_t to_read = remaining < sizeof(chunk) ? remaining : sizeof(chunk);
                            size_t rd = fread(chunk, 1, to_read, f);
                            if (rd == 0) break;
                            mz_buf_append(&res->body, chunk, rd);
                            remaining -= rd;
                        }
                    }
                    fclose(f);
                    return;
                }
            }
        }
    }

    // 3. Match Routes via Radix Tree (O(k) where k is path length)
    if (app->radix_tree) {
        MzHandlerFn matched_handler = nullptr;
        void *matched_udata = nullptr;
        if (mz_radix_find((MzRadixNode *)app->radix_tree, req->method, req->path, req, &matched_handler, &matched_udata)) {
            matched_handler(req, res, matched_udata);
            mz_context_restore_depth(saved_context_depth);
            return;
        }
    } else {
        // Fallback linear route matching
        for (size_t i = 0; i < app->route_count; i++) {
            MzRouteEntry *r = &app->routes[i];
            if (strcmp(r->method, "*") != 0 && strcmp(r->method, req->method) != 0) {
                continue;
            }

            if (mz_match_path(r->pattern, req->path, req)) {
                r->handler(req, res, r->user_data);
                mz_context_restore_depth(saved_context_depth);
                return;
            }
        }
    }

    // 4. Default 404
    mz_res_status(res, 404, "Not Found");
    mz_res_html(res);
    mz_buf_append_str(&res->body, "<h1>404 Not Found</h1><p>The requested route does not exist.</p>");
    mz_context_restore_depth(saved_context_depth);
}

// Robust write loop: sends entire buffer handling partial writes and EAGAIN/EWOULDBLOCK
static bool mz_socket_write_all(int fd, const char *data, size_t total_len) {
    size_t sent = 0;
    while (sent < total_len) {
#if defined(__linux__)
        ssize_t n = send(fd, data + sent, total_len - sent, MSG_NOSIGNAL);
#else
        ssize_t n = send(fd, data + sent, total_len - sent, 0);
#endif
        if (n > 0) {
            sent += (size_t)n;
            continue;
        }
        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Wait briefly for socket buffer to become writable
                struct pollfd pfd = { .fd = fd, .events = POLLOUT, .revents = 0 };
                int pr = poll(&pfd, 1, 5000); // 5-second socket write timeout
                if (pr > 0 && (pfd.revents & POLLOUT)) {
                    continue;
                }
                return false;
            }
            return false;
        }
        return false;
    }
    return true;
}

// Processes a client request, returns true if connection should remain alive
static bool mz_process_client(MzApp *app, int client_fd) {
    // Dynamic accumulator buffer to safely handle partial packets, bodies > 8KB, and slowloris
    size_t cap = 8192;
    size_t len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) return false;

    size_t expected_total_len = 0;
    bool headers_complete = false;

    while (1) {
        if (len + MZ_BUFFER_SIZE + 1 > cap) {
            if (cap * 2 > MZ_MAX_REQUEST_SIZE) {
                free(buf);
                return false; // Request too large
            }
            cap *= 2;
            char *nb = (char *)realloc(buf, cap);
            if (!nb) {
                free(buf);
                return false;
            }
            buf = nb;
        }

        ssize_t bytes_read = recv(client_fd, buf + len, MZ_BUFFER_SIZE, 0);
        if (bytes_read < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if (len == 0) {
                    free(buf);
                    return false;
                }
                // If we've started reading, wait for remaining data
                struct pollfd pfd = { .fd = client_fd, .events = POLLIN, .revents = 0 };
                int pr = poll(&pfd, 1, 3000); // 3-second read window
                if (pr > 0 && (pfd.revents & POLLIN)) {
                    continue;
                }
                free(buf);
                return false;
            }
            free(buf);
            return false;
        }
        if (bytes_read == 0) {
            // Client closed connection
            if (len == 0) {
                free(buf);
                return false;
            }
            break;
        }

        len += (size_t)bytes_read;
        buf[len] = '\0';

        if (!headers_complete) {
            const char *hdr_end = strstr(buf, "\r\n\r\n");
            if (hdr_end) {
                headers_complete = true;
                size_t header_len = (size_t)(hdr_end + 4 - buf);
                if (header_len > MZ_MAX_HEADER_SIZE) {
                    free(buf);
                    return false;
                }

                // Check for Content-Length
                size_t content_length = 0;
                const char *cl_ptr = strcasestr(buf, "Content-Length:");
                if (cl_ptr && cl_ptr < hdr_end) {
                    cl_ptr += 15;
                    while (*cl_ptr == ' ' || *cl_ptr == '\t') cl_ptr++;
                    content_length = (size_t)strtoull(cl_ptr, nullptr, 10);
                }

                expected_total_len = header_len + content_length;
                if (expected_total_len > MZ_MAX_REQUEST_SIZE) {
                    free(buf);
                    return false;
                }
            } else if (len > MZ_MAX_HEADER_SIZE) {
                free(buf);
                return false;
            }
        }

        if (headers_complete && len >= expected_total_len) {
            break; // Full request received
        }
    }

    MzRequest req;
    if (!mz_http_parse_request(buf, len, &req)) {
        free(buf);
        return false;
    }

    MzResponse res;
    mz_res_init(&res);

    const char *conn_hdr = mz_req_header(&req, "Connection");
    bool keep_alive = (conn_hdr && strcasecmp(conn_hdr, "keep-alive") == 0);

    mz_app_handle(app, &req, &res);

    MizarBuffer out;
    mz_buf_init(&out, 2048);

    mz_buf_printf(&out, "HTTP/1.1 %d %s\r\n", res.status_code, res.status_text);
    bool has_content_length = false;
    for (size_t h = 0; h < res.header_count; h++) {
        if (strcasecmp(res.headers[h].key, "Content-Length") == 0) has_content_length = true;
        mz_buf_printf(&out, "%s: %s\r\n", res.headers[h].key, res.headers[h].value);
    }
    if (!has_content_length) {
        mz_buf_printf(&out, "Content-Length: %zu\r\n", res.body.len);
    }
    if (keep_alive) {
        mz_buf_append_str(&out, "Connection: keep-alive\r\n\r\n");
    } else {
        mz_buf_append_str(&out, "Connection: close\r\n\r\n");
    }
    if (res.body.len > 0) {
        mz_buf_append(&out, res.body.data, res.body.len);
    }

    bool write_ok = mz_socket_write_all(client_fd, out.data, out.len);

    mz_buf_free(&out);
    mz_res_free(&res);
    mz_req_free(&req);
    free(buf);

    return keep_alive && write_ok;
}

#if defined(MZ_USE_EPOLL)
// Linux: epoll backend
static void *mz_worker_loop(void *arg) {
    MzWorkerContext *wctx = (MzWorkerContext *)arg;
    MzApp *app = wctx->app;
    int listener_fd = mz_create_listener_socket(wctx->port);
    if (listener_fd < 0) return nullptr;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        close(listener_fd);
        return nullptr;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listener_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener_fd, &ev);

    struct epoll_event events[MZ_MAX_EVENTS];

    while (1) {
        int nready = epoll_wait(epoll_fd, events, MZ_MAX_EVENTS, -1);
        for (int i = 0; i < nready; i++) {
            if (events[i].data.fd == listener_fd) {
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        break;
                    }
                    mz_configure_client_socket(client_fd);

                    struct epoll_event client_ev;
                    client_ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                    client_ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
                }
            } else {
                int client_fd = events[i].data.fd;
                bool keep = mz_process_client(app, client_fd);
                if (keep) {
                    struct epoll_event client_ev;
                    client_ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                    client_ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &client_ev);
                } else {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                    close(client_fd);
                }
            }
        }
    }
    close(epoll_fd);
    close(listener_fd);
    return nullptr;
}

#elif defined(MZ_USE_KQUEUE)
// macOS, FreeBSD, OpenBSD, NetBSD: kqueue backend
static void *mz_worker_loop(void *arg) {
    MzWorkerContext *wctx = (MzWorkerContext *)arg;
    MzApp *app = wctx->app;
    int listener_fd = mz_create_listener_socket(wctx->port);
    if (listener_fd < 0) return nullptr;

    int kq = kqueue();
    if (kq < 0) {
        close(listener_fd);
        return nullptr;
    }

    struct kevent change;
    EV_SET(&change, listener_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
    kevent(kq, &change, 1, nullptr, 0, nullptr);

    struct kevent events[MZ_MAX_EVENTS];

    while (1) {
        int nready = kevent(kq, nullptr, 0, events, MZ_MAX_EVENTS, nullptr);
        for (int i = 0; i < nready; i++) {
            int fd = (int)events[i].ident;
            if (fd == listener_fd) {
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (client_fd < 0) break;
                    mz_configure_client_socket(client_fd);

                    struct kevent client_change;
                    EV_SET(&client_change, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, nullptr);
                    kevent(kq, &client_change, 1, nullptr, 0, nullptr);
                }
            } else {
                bool keep = mz_process_client(app, fd);
                if (keep) {
                    struct kevent client_change;
                    EV_SET(&client_change, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, nullptr);
                    kevent(kq, &client_change, 1, nullptr, 0, nullptr);
                } else {
                    close(fd);
                }
            }
        }
    }
    close(kq);
    close(listener_fd);
    return nullptr;
}

#else
// Generic POSIX: poll() fallback (Windows/WSL, Solaris, AIX, embedded POSIX)
static void *mz_worker_loop(void *arg) {
    MzWorkerContext *wctx = (MzWorkerContext *)arg;
    MzApp *app = wctx->app;
    int listener_fd = mz_create_listener_socket(wctx->port);
    if (listener_fd < 0) return nullptr;

    struct pollfd fds[MZ_MAX_EVENTS];
    fds[0].fd = listener_fd;
    fds[0].events = POLLIN;
    int nfds = 1;

    while (1) {
        int nready = poll(fds, nfds, -1);
        if (nready <= 0) continue;

        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd >= 0 && nfds < MZ_MAX_EVENTS) {
                mz_set_nonblocking(client_fd);
                fds[nfds].fd = client_fd;
                fds[nfds].events = POLLIN;
                nfds++;
            }
        }

        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                bool keep = mz_process_client(app, fds[i].fd);
                if (!keep) {
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    nfds--;
                    i--;
                }
            }
        }
    }
    close(listener_fd);
    return nullptr;
}
#endif

bool mz_app_listen(MzApp *app, int port) {
    if (!app) return false;
    if (port <= 0) port = 3000;

    int num_threads = app->worker_threads > 0 ? app->worker_threads : 4;

    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    MzWorkerContext *contexts = (MzWorkerContext *)malloc(num_threads * sizeof(MzWorkerContext));

    for (int i = 0; i < num_threads; i++) {
        contexts[i].app = app;
        contexts[i].port = port;
        contexts[i].worker_id = i;
        if (i < num_threads - 1) {
            pthread_create(&threads[i], nullptr, mz_worker_loop, &contexts[i]);
        }
    }

    mz_worker_loop(&contexts[num_threads - 1]);

    for (int i = 0; i < num_threads - 1; i++) {
        pthread_join(threads[i], nullptr);
    }

    free(contexts);
    free(threads);
    return true;
}

#ifdef MIZAR_ENABLE_TLS
typedef struct {
    MzApp *app;
    int port;
    int worker_id;
    MzTlsCertKey *cert_key;
} MzTlsWorkerContext;

static void *mz_worker_tls_loop(void *arg) {
    MzTlsWorkerContext *wctx = (MzTlsWorkerContext *)arg;
    MzApp *app = wctx->app;
    MzTlsCertKey *ck = wctx->cert_key;

    int listener_fd = mz_create_listener_socket(wctx->port);
    if (listener_fd < 0) return nullptr;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) continue;

        mz_configure_client_socket(client_fd);

        MzTlsSession tls_sess;
        if (!mz_tls_session_init(&tls_sess, ck, &client_fd)) {
            close(client_fd);
            continue;
        }

        char req_buf[MZ_BUFFER_SIZE];
        int rlen = mz_tls_read(&tls_sess, req_buf, sizeof(req_buf) - 1);
        if (rlen > 0) {
            req_buf[rlen] = '\0';
            MzRequest req;
            if (mz_http_parse_request(req_buf, rlen, &req)) {
                MzResponse res;
                mz_res_init(&res);
                mz_app_handle(app, &req, &res);

                MizarBuffer out;
                mz_buf_init(&out, 2048);
                mz_buf_printf(&out, "HTTP/1.1 %d %s\r\n", res.status_code, res.status_text);
                bool has_cl = false;
                for (size_t h = 0; h < res.header_count; h++) {
                    if (strcasecmp(res.headers[h].key, "Content-Length") == 0) has_cl = true;
                    mz_buf_printf(&out, "%s: %s\r\n", res.headers[h].key, res.headers[h].value);
                }
                if (!has_cl) {
                    mz_buf_printf(&out, "Content-Length: %zu\r\n", res.body.len);
                }
                mz_buf_append_str(&out, "Connection: close\r\n\r\n");
                if (res.body.len > 0) {
                    mz_buf_append(&out, res.body.data, res.body.len);
                }

                mz_tls_write_all(&tls_sess, out.data, out.len);
                mz_tls_flush(&tls_sess);

                mz_buf_free(&out);
                mz_res_free(&res);
                mz_req_free(&req);
            }
        }

        mz_tls_close(&tls_sess);
        close(client_fd);
    }

    close(listener_fd);
    return nullptr;
}
#endif

bool mz_app_listen_tls(MzApp *app, int port, const char *cert_file, const char *key_file) {
#ifdef MIZAR_ENABLE_TLS
    if (!app || !cert_file || !key_file) return false;
    if (port <= 0) port = 8443;

    MzTlsCertKey ck;
    if (!mz_tls_load_cert_and_key(cert_file, key_file, &ck)) {
        return false;
    }

    int num_threads = app->worker_threads > 0 ? app->worker_threads : 4;
    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    MzTlsWorkerContext *contexts = (MzTlsWorkerContext *)malloc(num_threads * sizeof(MzTlsWorkerContext));

    for (int i = 0; i < num_threads; i++) {
        contexts[i].app = app;
        contexts[i].port = port;
        contexts[i].worker_id = i;
        contexts[i].cert_key = &ck;
        if (i < num_threads - 1) {
            pthread_create(&threads[i], nullptr, mz_worker_tls_loop, &contexts[i]);
        }
    }

    mz_worker_tls_loop(&contexts[num_threads - 1]);

    for (int i = 0; i < num_threads - 1; i++) {
        pthread_join(threads[i], nullptr);
    }

    mz_tls_free_cert_key(&ck);
    free(contexts);
    free(threads);
    return true;
#else
    (void)app; (void)port; (void)cert_file; (void)key_file;
    fprintf(stderr, "Mizar: HTTPS requested but Mizar was compiled without BearSSL support (build with make TLS=1).\n");
    return false;
#endif
}
