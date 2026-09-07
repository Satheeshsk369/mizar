#ifndef MIZAR_SSG_SITE_H
#define MIZAR_SSG_SITE_H

#include "core/buffer.h"
#include <stddef.h>
#include <stdbool.h>

typedef void (*MizarPageRenderFn)(MizarBuffer *buf, void *user_data);

typedef struct {
    char *route;
    MizarPageRenderFn render_fn;
    void *user_data;
} MizarRoute;

typedef struct {
    char *out_dir;
    char *static_dir;
    MizarRoute *routes;
    size_t route_count;
    size_t route_capacity;
} MizarSite;

// Site lifecycle
void mz_site_init(MizarSite *site, const char *out_dir);
void mz_site_free(MizarSite *site);

// Configuration
void mz_site_set_static_dir(MizarSite *site, const char *static_dir);

// Route registration
bool mz_site_add_page_impl(MizarSite *site, const char *route, MizarPageRenderFn render_fn, void *user_data);

#define _MZ_SITE_ARG4(_1, _2, _3, _4, NAME, ...) NAME
#define mz_site_add_page(...) _MZ_SITE_ARG4(__VA_ARGS__, _mz_site_add_page_4, _mz_site_add_page_3)(__VA_ARGS__)
#define _mz_site_add_page_3(site, route, fn) mz_site_add_page_impl((site), (route), (fn), nullptr)
#define _mz_site_add_page_4(site, route, fn, udata) mz_site_add_page_impl((site), (route), (fn), (udata))

// Build execution: renders all registered routes into files in out_dir, copies static_dir
bool mz_site_build(MizarSite *site);

// Starts a local static HTTP preview server on port (blocks until interrupted)
bool mz_site_serve(const char *dir, int port);

#endif
