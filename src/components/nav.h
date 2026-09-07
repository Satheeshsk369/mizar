#ifndef MIZAR_COMPONENTS_NAV_H
#define MIZAR_COMPONENTS_NAV_H

#include "view/html/tags.h"

// 1. Breadcrumb: Page path hierarchy
typedef struct {
    const char *label;
    const char *href;
} UiBreadcrumbItem;

static inline void UiBreadcrumbs(const UiBreadcrumbItem *items, size_t count) {
    if (!items || count == 0) return;

    Nav(.aria_label = "Breadcrumb") {
        Ol(.style = "display: flex; list-style: none; padding: 0; margin: 0; gap: 8px; font-size: 0.875rem; color: #64748b;") {
            for (size_t i = 0; i < count; i++) {
                Li(.style = "display: inline-flex; align-items: center; gap: 8px;") {
                    if (items[i].href) {
                        A(.href = items[i].href, .style = "color: #0284c7; text-decoration: none;") {
                            Text(items[i].label);
                        }
                    } else {
                        Span(.style = "color: #0f172a; font-weight: 600;") {
                            Text(items[i].label);
                        }
                    }
                    if (i + 1 < count) {
                        Span(.style = "color: #cbd5e1;") { Text("/"); }
                    }
                }
            }
        }
    }
}

// 2. Pagination Controls
typedef struct {
    int current_page;
    int total_pages;
    const char *base_url; // e.g. "/items?page="
} UiPaginationProps;

static inline void mz_render_ui_pagination(UiPaginationProps p) {
    if (p.total_pages <= 1) return;

    Nav(.aria_label = "Pagination", .style = "display: flex; justify-content: center; gap: 4px; margin: 24px 0;") {
        // Prev button
        if (p.current_page > 1) {
            char prev_url[256];
            snprintf(prev_url, sizeof(prev_url), "%s%d", p.base_url ? p.base_url : "?page=", p.current_page - 1);
            A(.href = prev_url, .style = "padding: 6px 12px; border: 1px solid #cbd5e1; border-radius: 6px; text-decoration: none; color: #334155; font-size: 0.875rem;") {
                Text("« Prev");
            }
        }

        // Current page indicator
        Span(.style = "padding: 6px 12px; font-size: 0.875rem; color: #64748b; font-weight: 600;") {
            Text("Page %d of %d", p.current_page, p.total_pages);
        }

        // Next button
        if (p.current_page < p.total_pages) {
            char next_url[256];
            snprintf(next_url, sizeof(next_url), "%s%d", p.base_url ? p.base_url : "?page=", p.current_page + 1);
            A(.href = next_url, .style = "padding: 6px 12px; border: 1px solid #cbd5e1; border-radius: 6px; text-decoration: none; color: #334155; font-size: 0.875rem;") {
                Text("Next »");
            }
        }
    }
}

#define UiPagination(...) mz_render_ui_pagination((UiPaginationProps){ __VA_ARGS__ })

// 3. Navbar Layout
typedef struct {
    const char *brand;
    const char *brand_href;
    const char *cls;
} UiNavbarProps;

static inline void mz_ui_navbar_open(UiNavbarProps p) {
    mz_tag_open("nav", (Attrs){
        .cls = p.cls ? p.cls : "mz-navbar",
        .style = "display: flex; align-items: center; justify-content: space-between; padding: 12px 24px; background: #ffffff; border-bottom: 1px solid #e2e8f0;"
    });

    mz_tag_open("a", (Attrs){
        .href = p.brand_href ? p.brand_href : "/",
        .style = "font-size: 1.25rem; font-weight: 700; color: #0f172a; text-decoration: none;"
    });
    Text(p.brand ? p.brand : "Logo");
    mz_tag_close("a");

    mz_tag_open("div", (Attrs){ .style = "display: flex; align-items: center; gap: 16px;" });
}

static inline void mz_ui_navbar_close(void) {
    mz_tag_close("div");
    mz_tag_close("nav");
}

#define UiNavbar(...) \
    for (int _mz_ui_nav = (mz_ui_navbar_open((UiNavbarProps){ __VA_ARGS__ }), 0); !_mz_ui_nav; _mz_ui_nav = 1, mz_ui_navbar_close())

#endif
