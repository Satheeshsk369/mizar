#include "server/app.h"
#include "ssg/fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <pthread.h>

#define MZ_MAX_QUEUE_CLIENTS 1024

typedef struct {
    int client_fds[MZ_MAX_QUEUE_CLIENTS];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    bool shutdown;
} MzConnQueue;

typedef struct {
    MzApp *app;
    MzConnQueue *queue;
} MzWorkerArgs;

static void mz_queue_init(MzConnQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->shutdown = false;
    pthread_mutex_init(&q->lock, nullptr);
    pthread_cond_init(&q->not_empty, nullptr);
    pthread_cond_init(&q->not_full, nullptr);
}

static void mz_queue_push(MzConnQueue *q, int fd) {
    pthread_mutex_lock(&q->lock);
    while (q->count == MZ_MAX_QUEUE_CLIENTS && !q->shutdown) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    if (q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        close(fd);
        return;
    }
    q->client_fds[q->tail] = fd;
    q->tail = (q->tail + 1) % MZ_MAX_QUEUE_CLIENTS;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

static int mz_queue_pop(MzConnQueue *q) {
    pthread_mutex_lock(&q->lock);
    while (q->count == 0 && !q->shutdown) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    if (q->count == 0 && q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }
    int fd = q->client_fds[q->head];
    q->head = (q->head + 1) % MZ_MAX_QUEUE_CLIENTS;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return fd;
}

void mz_app_init(MzApp *app) {
    if (!app) return;
    memset(app, 0, sizeof(MzApp));
    app->worker_threads = 4;
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

    free(app->middlewares);
    free(app->static_dir);
    free(app->static_prefix);

    memset(app, 0, sizeof(MzApp));
}

void mz_app_use(MzApp *app, MzMiddlewareFn fn, void *user_data) {
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

void mz_app_route(MzApp *app, const char *method, const char *pattern, MzHandlerFn handler, void *user_data) {
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
}

void mz_app_get(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route(app, "GET", pattern, handler, user_data);
}

void mz_app_post(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route(app, "POST", pattern, handler, user_data);
}

void mz_app_put(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route(app, "PUT", pattern, handler, user_data);
}

void mz_app_delete(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route(app, "DELETE", pattern, handler, user_data);
}

void mz_app_patch(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data) {
    mz_app_route(app, "PATCH", pattern, handler, user_data);
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

    // 1. Middleware chain
    for (size_t i = 0; i < app->middleware_count; i++) {
        bool cont = app->middlewares[i].fn(req, res, app->middlewares[i].user_data);
        if (!cont) {
            return;
        }
    }

    // 2. Static files
    if (app->static_dir && app->static_prefix) {
        size_t prefix_len = strlen(app->static_prefix);
        if (strncmp(req->path, app->static_prefix, prefix_len) == 0) {
            const char *subpath = req->path + prefix_len;
            while (*subpath == '/') subpath++;

            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", app->static_dir, subpath);

            FILE *f = fopen(filepath, "rb");
            if (f) {
                fseek(f, 0, SEEK_END);
                long fsize = ftell(f);
                fseek(f, 0, SEEK_SET);

                mz_res_status(res, 200, "OK");
                mz_res_content_type(res, mz_mime_type_for_file(filepath));

                char *buf = (char *)malloc(fsize);
                if (buf) {
                    size_t rd = fread(buf, 1, fsize, f);
                    mz_buf_append(&res->body, buf, rd);
                    free(buf);
                }
                fclose(f);
                return;
            }
        }
    }

    // 3. Match Routes
    for (size_t i = 0; i < app->route_count; i++) {
        MzRouteEntry *r = &app->routes[i];
        if (strcmp(r->method, "*") != 0 && strcmp(r->method, req->method) != 0) {
            continue;
        }

        if (mz_match_path(r->pattern, req->path, req)) {
            r->handler(req, res, r->user_data);
            return;
        }
    }

    // 4. Default 404
    mz_res_status(res, 404, "Not Found");
    mz_res_html(res);
    mz_buf_append_str(&res->body, "<h1>404 Not Found</h1><p>The requested route does not exist.</p>");
}

static void *mz_worker_thread(void *arg) {
    MzWorkerArgs *wargs = (MzWorkerArgs *)arg;
    MzApp *app = wargs->app;
    MzConnQueue *q = wargs->queue;

    while (1) {
        int client_fd = mz_queue_pop(q);
        if (client_fd < 0) break;

        char req_buf[8192];
        ssize_t bytes_read = recv(client_fd, req_buf, sizeof(req_buf) - 1, 0);
        if (bytes_read > 0) {
            req_buf[bytes_read] = '\0';

            MzRequest req;
            if (mz_http_parse_request(req_buf, bytes_read, &req)) {
                MzResponse res;
                mz_res_init(&res);

                mz_app_handle(app, &req, &res);

                MizarBuffer out;
                mz_buf_init(&out, 2048);
                mz_res_serialize(&res, &out);

                send(client_fd, out.data, out.len, 0);

                mz_buf_free(&out);
                mz_res_free(&res);
                mz_req_free(&req);
            }
        }
        close(client_fd);
    }
    return nullptr;
}

bool mz_app_listen(MzApp *app, int port) {
    if (!app) return false;
    if (port <= 0) port = 3000;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return false;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
        return false;
    }

    if (listen(server_fd, 128) < 0) {
        close(server_fd);
        return false;
    }

    int num_threads = app->worker_threads > 0 ? app->worker_threads : 4;
    printf("Mizar high-performance web server running at http://localhost:%d/ (%d worker threads)\n", port, num_threads);

    MzConnQueue queue;
    mz_queue_init(&queue);

    MzWorkerArgs wargs = { .app = app, .queue = &queue };
    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], nullptr, mz_worker_thread, &wargs);
    }

    while (1) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            mz_queue_push(&queue, client_fd);
        }
    }

    close(server_fd);
    free(threads);
    return true;
}
