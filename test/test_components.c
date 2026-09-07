#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 4096);
    mz_context_push(&buf);

    // 1. Core: UiCard & UiBtn
    UiCard(.title = "Server Metrics", .desc = "Realtime status") {
        P() { Text("CPU: 4.2%%"); }
        UiBtn("Deploy App", .variant = BTN_PRIMARY, HxPost("/api/deploy"), HxTarget("#status"));
    }

    // 2. Core: UiBadge & UiAlert
    UiBadge("PRO", .variant = BADGE_SUCCESS);
    UiAlert(.variant = ALERT_WARNING, .title = "High Load") {
        Text("Memory usage exceeds 85%%.");
    }

    // 3. Core: UiField & UiModal
    UiField(.label = "Username", .name = "username", .placeholder = "john_doe", .required = true);
    UiModal(.id = "delete-modal", .title = "Confirm Deletion") {
        P() { Text("Are you sure?"); }
        UiBtn("Delete", .variant = BTN_DANGER, HxDelete("/api/resource"));
    }

    // 4. Layout: UiContainer, UiStack, UiGrid, UiPaper, UiDivider, UiCollapsible
    UiContainer(.max_width_px = 960) {
        UiStack(.horizontal = true, .gap_px = 24) {
            UiPaper(.elevation = 2) {
                Text("Elevated Card");
            }
        }
        UiDivider(.label = "Or continue with");
        UiGrid(.cols = 3, .gap_px = 16) {
            Div() { Text("Col 1"); }
            Div() { Text("Col 2"); }
            Div() { Text("Col 3"); }
        }
        UiCollapsible(.title = "View FAQ") {
            P() { Text("Frequently asked answer."); }
        }
    }

    // 5. Typography: UiKbd, UiQuote, UiMark, UiCodeBlock
    UiKbd("Ctrl + Shift + P");
    UiMark("important note");
    UiQuote() { Text("Design is how it works."); }
    UiCodeBlock(.filename = "main.c", .lang = "C23") {
        Text("int main(void) { return 0; }");
    }

    // 6. Data: UiAvatar, UiMetric, UiProgress, UiEmpty, UiTable
    UiAvatar(.initials = "SK", .size_px = 48);
    UiMetric(.label = "Total Revenue", .value = "$48,200", .change = "+14.2%", .positive = true);
    UiProgress(.percent = 75);
    UiEmpty(.title = "No Orders", .desc = "You have not placed any orders yet.") {
        UiBtn("Start Shopping", .variant = BTN_PRIMARY);
    }
    UiTable() {
        Thead() {
            Tr() { Th() { Text("ID"); } Th() { Text("Name"); } }
        }
        Tbody() {
            Tr() { Td() { Text("1"); } Td() { Text("Alice"); } }
        }
    }

    // 7. Feedback: UiSpinner, UiSkeleton, UiStatus
    UiSpinner(.size_px = 32);
    UiSkeleton(.height_px = 20);
    UiStatus(.status = STATUS_ONLINE, .label = "Operational");

    // 8. Nav: UiBreadcrumbs, UiPagination, UiNavbar
    UiBreadcrumbItem crumbs[] = {
        { .label = "Home", .href = "/" },
        { .label = "Settings", .href = "/settings" },
        { .label = "Security", .href = nullptr }
    };
    UiBreadcrumbs(crumbs, 3);
    UiPagination(.current_page = 2, .total_pages = 5, .base_url = "/items?page=");
    UiNavbar(.brand = "Mizar Cloud", .brand_href = "/") {
        A(.href = "/docs") { Text("Docs"); }
    }

    // 9. Extended: UiCheckbox, UiSwitch, UiTextarea, UiSelect, UiTooltip, UiToast, UiHero, UiTestimonial, UiPricing, UiLiveSearch
    UiCheckbox(.label = "Remember me", .name = "remember", .checked = true);
    UiSwitch(.label = "Dark Mode", .name = "dark_mode");
    UiTextarea(.label = "Bio", .name = "bio", .placeholder = "About yourself...");

    UiSelectOption options[] = {
        { .label = "Admin", .val = "admin" },
        { .label = "Member", .val = "member", .selected = true }
    };
    UiSelect(.label = "Role", .name = "role", .options = options, .option_count = 2);

    UiTooltip("Detailed explanation here") {
        Text("Hover for info");
    }
    UiToast(.title = "Saved", .message = "Preferences updated.", .success = true);

    UiHero(.badge = "New Release", .title = "Build Fast Web in C23", .tagline = "The declarative C framework.") {
        UiBtn("Get Started", .variant = BTN_PRIMARY);
    }

    UiTestimonial(.quote = "Mizar changed how we view backend web performance in C.", .author = "Jane Doe", .role = "Principal Architect", .avatar_initials = "JD");

    UiPricing(.plan = "Pro Plan", .price = "$29", .period = "/mo", .popular = true) {
        Li() { Text("Unlimited requests"); }
        Li() { Text("24/7 Support"); }
    }

    UiLiveSearch(.placeholder = "Search users...", .hx_post_url = "/api/search", .hx_target = "#search-results");

    mz_context_pop();

    // Assertions across all component categories
    assert(strstr(buf.data, "class=\"mz-card\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-badge\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-btn\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-alert\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-form-field\"") != NULL);
    assert(strstr(buf.data, "<dialog id=\"delete-modal\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-container\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-stack\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-grid\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-paper\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-divider-labeled\"") != NULL);
    assert(strstr(buf.data, "<details class=\"mz-collapsible\"") != NULL);
    assert(strstr(buf.data, "<kbd style=") != NULL);
    assert(strstr(buf.data, "<mark style=") != NULL);
    assert(strstr(buf.data, "class=\"mz-avatar-fallback\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-metric\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-progress\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-empty-state\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-table\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-spinner\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-skeleton\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-status\"") != NULL);
    assert(strstr(buf.data, "aria-label=\"Breadcrumb\"") != NULL);
    assert(strstr(buf.data, "aria-label=\"Pagination\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-navbar\"") != NULL);

    // Extended assertions
    assert(strstr(buf.data, "type=\"checkbox\"") != NULL);
    assert(strstr(buf.data, "Remember me") != NULL);
    assert(strstr(buf.data, "class=\"mz-toast\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-hero\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-testimonial\"") != NULL);
    assert(strstr(buf.data, "class=\"mz-pricing-card\"") != NULL);
    assert(strstr(buf.data, "type=\"search\"") != NULL);
    assert(strstr(buf.data, "keyup changed delay:300ms") != NULL);

    mz_buf_free(&buf);
    printf("Complete UI Component Suite (all categories) tested and passed cleanly!\n");
    return 0;
}
