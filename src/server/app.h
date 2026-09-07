#ifndef MIZAR_SERVER_APP_H
#define MIZAR_SERVER_APP_H

#include "server/http.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct MzApp MzApp;

// Handler function signature
typedef void (*MzHandlerFn)(const MzRequest *req, MzResponse *res, void *user_data);

// Middleware function signature: return true to continue next, false to abort chain
typedef bool (*MzMiddlewareFn)(const MzRequest *req, MzResponse *res, void *user_data);

typedef struct {
    char *method;          // "GET", "POST", etc. (or "*" for any)
    char *pattern;         // "/users/:id"
    MzHandlerFn handler;
    void *user_data;
} MzRouteEntry;

typedef struct {
    MzMiddlewareFn fn;
    void *user_data;
} MzMiddlewareEntry;

struct MzApp {
    MzRouteEntry *routes;
    size_t route_count;
    size_t route_capacity;

    MzMiddlewareEntry *middlewares;
    size_t middleware_count;
    size_t middleware_capacity;

    char *static_dir;
    char *static_prefix; // e.g. "/static"

    int worker_threads;  // 0 = default (4 threads), or N threads
};

// Application lifecycle
void mz_app_init(MzApp *app);
void mz_app_free(MzApp *app);

// Configuration
void mz_app_set_workers(MzApp *app, int num_threads);

// Middleware
void mz_app_use(MzApp *app, MzMiddlewareFn fn, void *user_data);

// Static file hosting
void mz_app_static(MzApp *app, const char *url_prefix, const char *dir_path);

// Route registration
void mz_app_route(MzApp *app, const char *method, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_get(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_post(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_put(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_delete(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_patch(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);

// Request dispatch (called by server worker or unit test)
void mz_app_handle(MzApp *app, MzRequest *req, MzResponse *res);

// High-concurrency worker pool HTTP Server (blocks until interrupted)
bool mz_app_listen(MzApp *app, int port);

#endif
