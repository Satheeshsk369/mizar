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
bool mz_site_add_page(MizarSite *site, const char *route, MizarPageRenderFn render_fn, void *user_data);

// Build execution: renders all registered routes into files in out_dir, copies static_dir
bool mz_site_build(MizarSite *site);

// Starts a local static HTTP preview server on port (blocks until interrupted)
bool mz_site_serve(const char *dir, int port);

#endif
