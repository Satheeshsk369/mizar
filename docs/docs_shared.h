#ifndef DOCS_SHARED_H
#define DOCS_SHARED_H

#include "mizar.h"
#include "style.h"
#include <stdio.h>
#include <string.h>

// Layout & Style helpers
void docs_layout(MizarBuffer *buf, const char *title, const char *slug, void (*content_fn)(void));
void docs_register_search_item(const char *url, const char *title, const char *category, const char *body);
void docs_export_search_index(const char *filepath);

// Page render functions
// 1. Fundamentals
void page_index(MizarBuffer *buf, void *data);
void page_intro(MizarBuffer *buf, void *data);
void page_quickstart(MizarBuffer *buf, void *data);

// 2. Progressive Examples
void page_example_minimal(MizarBuffer *buf, void *data);
void page_example_routing(MizarBuffer *buf, void *data);
void page_example_components(MizarBuffer *buf, void *data);
void page_example_htmx(MizarBuffer *buf, void *data);
void page_example_middleware(MizarBuffer *buf, void *data);
void page_example_ssg(MizarBuffer *buf, void *data);

// 3. Source Internals
void page_source_overview(MizarBuffer *buf, void *data);
void page_source_algo(MizarBuffer *buf, void *data);
void page_source_server(MizarBuffer *buf, void *data);
void page_source_view(MizarBuffer *buf, void *data);
void page_source_ssg(MizarBuffer *buf, void *data);

#endif
