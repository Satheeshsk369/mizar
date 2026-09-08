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

    void *radix_tree;    // O(k) prefix tree routing engine

    int worker_threads;  // 0 = default (4 threads), or N threads
};

// Application lifecycle
void mz_app_init(MzApp *app);
void mz_app_free(MzApp *app);

// Configuration
void mz_app_set_workers(MzApp *app, int num_threads);

// Route registration base functions
void mz_app_route_impl(MzApp *app, const char *method, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_get_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_post_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_put_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_delete_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_patch_impl(MzApp *app, const char *pattern, MzHandlerFn handler, void *user_data);
void mz_app_use_impl(MzApp *app, MzMiddlewareFn fn, void *user_data);

// Macro overloads supporting optional user_data (defaults to nullptr)
#define _MZ_ARG4(_1, _2, _3, _4, NAME, ...) NAME
#define _MZ_ARG3(_1, _2, _3, NAME, ...) NAME

#define mz_app_use(...) _MZ_ARG3(__VA_ARGS__, _mz_app_use_2, _mz_app_use_1)(__VA_ARGS__)
#define _mz_app_use_1(app, fn) mz_app_use_impl((app), (fn), nullptr)
#define _mz_app_use_2(app, fn, udata) mz_app_use_impl((app), (fn), (udata))

#define mz_app_get(...) _MZ_ARG4(__VA_ARGS__, _mz_app_get_4, _mz_app_get_3)(__VA_ARGS__)
#define _mz_app_get_3(app, pat, fn) mz_app_get_impl((app), (pat), (fn), nullptr)
#define _mz_app_get_4(app, pat, fn, udata) mz_app_get_impl((app), (pat), (fn), (udata))

#define mz_app_post(...) _MZ_ARG4(__VA_ARGS__, _mz_app_post_4, _mz_app_post_3)(__VA_ARGS__)
#define _mz_app_post_3(app, pat, fn) mz_app_post_impl((app), (pat), (fn), nullptr)
#define _mz_app_post_4(app, pat, fn, udata) mz_app_post_impl((app), (pat), (fn), (udata))

#define mz_app_put(...) _MZ_ARG4(__VA_ARGS__, _mz_app_put_4, _mz_app_put_3)(__VA_ARGS__)
#define _mz_app_put_3(app, pat, fn) mz_app_put_impl((app), (pat), (fn), nullptr)
#define _mz_app_put_4(app, pat, fn, udata) mz_app_put_impl((app), (pat), (fn), (udata))

#define mz_app_delete(...) _MZ_ARG4(__VA_ARGS__, _mz_app_delete_4, _mz_app_delete_3)(__VA_ARGS__)
#define _mz_app_delete_3(app, pat, fn) mz_app_delete_impl((app), (pat), (fn), nullptr)
#define _mz_app_delete_4(app, pat, fn, udata) mz_app_delete_impl((app), (pat), (fn), (udata))

#define mz_app_patch(...) _MZ_ARG4(__VA_ARGS__, _mz_app_patch_4, _mz_app_patch_3)(__VA_ARGS__)
#define _mz_app_patch_3(app, pat, fn) mz_app_patch_impl((app), (pat), (fn), nullptr)
#define _mz_app_patch_4(app, pat, fn, udata) mz_app_patch_impl((app), (pat), (fn), (udata))

#define _MZ_ARG5(_1, _2, _3, _4, _5, NAME, ...) NAME
#define mz_app_route(...) _MZ_ARG5(__VA_ARGS__, _mz_app_route_5, _mz_app_route_4)(__VA_ARGS__)
#define _mz_app_route_4(app, meth, pat, fn) mz_app_route_impl((app), (meth), (pat), (fn), nullptr)
#define _mz_app_route_5(app, meth, pat, fn, udata) mz_app_route_impl((app), (meth), (pat), (fn), (udata))

// Request dispatch (called by server worker or unit test)
void mz_app_handle(MzApp *app, MzRequest *req, MzResponse *res);

// High-concurrency worker pool HTTP Server (blocks until interrupted)
bool mz_app_listen(MzApp *app, int port);

// High-concurrency worker pool HTTPS Server with BearSSL (blocks until interrupted)
bool mz_app_listen_tls(MzApp *app, int port, const char *cert_file, const char *key_file);

#endif
