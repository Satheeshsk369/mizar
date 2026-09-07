#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mizar.h"

// Middleware example: adds custom security header
static bool security_headers_middleware(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req;
    (void)user_data;
    mz_res_header(res, "X-Frame-Options", "DENY");
    mz_res_header(res, "X-Content-Type-Options", "nosniff");
    return true;
}

// Route 1: Home page (SSR using Mizar DSL + LiveReload)
static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req;
    (void)user_data;
    mz_res_html(res);

    Html(&res->body, .lang = "en") {
        Head() {
            Title() { Text("Mizar Dynamic Server"); }
        }
        Body() {
            H1() { Text("Dynamic SSR with C23"); }
            P() { Text("Welcome to Mizar full web framework."); }
            Button(HxGet("/api/time"), HxTarget("#time-box")) {
                Text("Fetch Time via HTMX");
            }
            Div(.id = "time-box") {
                Text("Click button to load...");
            }
            MzLiveReloadScript();
        }
    }
}

// Route 2: HTMX Partial with Trigger & Retarget & OOB Swap
static void handle_api_time(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req;
    (void)user_data;
    mz_res_html(res);

    // First-class HTMX response modifiers
    mz_res_trigger(res, "timeUpdated");
    mz_res_reswap(res, "innerHTML");

    mz_context_push(&res->body);
    // Primary fragment
    Span(.cls = "badge-success") {
        Text("Current Server Time: 12:00:00 UTC (HTMX fragment)");
    }
    // Out-of-band swap fragment to update notification counter elsewhere on page
    Div(HxOob("notif-counter", "innerHTML")) {
        Text("1 new notification");
    }
    mz_context_pop();
}

// Route 3: Form submission with url-encoded body parsing
static void handle_form_submit(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    const char *username = mz_req_form(req, "username");
    const char *email = mz_req_form(req, "email");

    mz_res_html(res);
    mz_context_push(&res->body);
    Div(.cls = "alert alert-success") {
        H3() { Text("User created successfully!"); }
        P() { Text("Username: %s, Email: %s", username, email); }
    }
    mz_context_pop();
}

// Route 4: Dynamic Path Parameter (:id) + Query Parameter (?format=json)
static void handle_user_profile(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    const char *user_id = mz_req_param(req, "id");
    const char *format = mz_req_query(req, "format");

    if (format && strcmp(format, "json") == 0) {
        mz_res_json(res);
        mz_buf_printf(&res->body, "{\"id\": \"%s\", \"name\": \"User %s\"}", user_id, user_id);
    } else {
        mz_res_html(res);
        mz_context_push(&res->body);
        Div(.cls = "user-profile") {
            H2() { Text("User Profile: %s", user_id); }
        }
        mz_context_pop();
    }
}

int main(void) {
    printf("Testing Mizar Dynamic Web Framework & HTMX Engine...\n");

    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 4);

    // Register middleware
    mz_app_use(&app, security_headers_middleware, nullptr);

    // Register routes
    mz_app_get(&app, "/", handle_home, nullptr);
    mz_app_get(&app, "/api/time", handle_api_time, nullptr);
    mz_app_post(&app, "/api/users", handle_form_submit, nullptr);
    mz_app_get(&app, "/users/:id", handle_user_profile, nullptr);

    // 1. Simulate GET /
    {
        const char *raw_get = "GET / HTTP/1.1\r\nHost: localhost:3000\r\nUser-Agent: test\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw_get, strlen(raw_get), &req));
        assert(strcmp(req.method, "GET") == 0);
        assert(strcmp(req.path, "/") == 0);

        MzResponse res;
        mz_res_init(&res);

        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strstr(res.body.data, "<title>Mizar Dynamic Server</title>") != NULL);
        assert(strstr(res.body.data, "/__mizar_reload") != NULL);

        MizarBuffer out;
        mz_buf_init(&out, 1024);
        mz_res_serialize(&res, &out);
        assert(strstr(out.data, "X-Frame-Options: DENY") != NULL);
        assert(strstr(out.data, "Content-Type: text/html") != NULL);

        mz_buf_free(&out);
        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 2. Simulate HTMX GET /api/time
    {
        const char *raw_htmx = "GET /api/time HTTP/1.1\r\nHost: localhost\r\nHX-Request: true\r\nHX-Target: #time-box\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw_htmx, strlen(raw_htmx), &req));
        assert(mz_req_is_htmx(&req));
        assert(strcmp(mz_req_htmx_target(&req), "#time-box") == 0);

        MzResponse res;
        mz_res_init(&res);

        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strstr(res.body.data, "Current Server Time: 12:00:00 UTC") != NULL);
        assert(strstr(res.body.data, "hx-swap-oob=\"innerHTML\"") != NULL);
        assert(strstr(res.body.data, "1 new notification") != NULL);

        MizarBuffer out;
        mz_buf_init(&out, 1024);
        mz_res_serialize(&res, &out);
        assert(strstr(out.data, "HX-Trigger: timeUpdated") != NULL);
        assert(strstr(out.data, "HX-Reswap: innerHTML") != NULL);

        mz_buf_free(&out);
        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 3. Simulate POST /api/users (form-urlencoded with url-decoding)
    {
        const char *body_str = "username=Jane+Doe&email=jane%40example.com";
        char raw_post[1024];
        snprintf(raw_post, sizeof(raw_post),
                 "POST /api/users HTTP/1.1\r\n"
                 "Host: localhost\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "Content-Length: %zu\r\n\r\n%s",
                 strlen(body_str), body_str);

        MzRequest req;
        assert(mz_http_parse_request(raw_post, strlen(raw_post), &req));
        assert(strcmp(mz_req_form(&req, "username"), "Jane Doe") == 0);
        assert(strcmp(mz_req_form(&req, "email"), "jane@example.com") == 0);

        MzResponse res;
        mz_res_init(&res);

        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strstr(res.body.data, "Username: Jane Doe, Email: jane@example.com") != NULL);

        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 4. Simulate GET /users/42?format=json
    {
        const char *raw_get = "GET /users/42?format=json HTTP/1.1\r\nHost: localhost\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw_get, strlen(raw_get), &req));
        assert(strcmp(req.path, "/users/42") == 0);

        MzResponse res;
        mz_res_init(&res);

        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strcmp(mz_req_param(&req, "id"), "42") == 0);
        assert(strcmp(mz_req_query(&req, "format"), "json") == 0);
        assert(strstr(res.body.data, "{\"id\": \"42\", \"name\": \"User 42\"}") != NULL);

        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 5. Test URL Encode / Decode utilities directly
    {
        const char *original = "Hello World! @ 2026 / test";
        char *encoded = mz_url_encode(original, strlen(original));
        char *decoded = mz_url_decode(encoded, strlen(encoded), false);
        assert(strcmp(original, decoded) == 0);
        free(encoded);
        free(decoded);
    }

    mz_app_free(&app);
    printf(">>> ALL ADVANCED HYPERMEDIA & SERVER TESTS PASSED! <<<\n");
    return 0;
}
