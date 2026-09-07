#include <assert.h>
#include <string.h>
#include "mizar.h"

static bool auth_middleware(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    if (strcmp(req->path, "/private") == 0) {
        const char *auth = mz_req_header(req, "Authorization");
        if (!auth || strcmp(auth, "secret") != 0) {
            mz_res_status(res, 401, "Unauthorized");
            return false;
        }
    }
    return true;
}

static void handle_items(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    mz_res_html(res);

    if (mz_req_is_htmx(req)) {
        mz_res_trigger(res, "itemsLoaded");
        mz_res_reswap(res, "innerHTML");
        mz_context_push(&res->body);
        Li() { Text("Item 1"); }
        Span(HxOob("counter", "innerHTML")) { Text("1"); }
        mz_context_pop();
    } else {
        Html(&res->body) {
            Body() {
                Ul(.id = "items") { Li() { Text("Item 1"); } }
            }
        }
    }
}

static void handle_login(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    const char *user = mz_req_form(req, "user");
    mz_res_html(res);
    mz_context_push(&res->body);
    P() { Text("Hello %s", user ? user : "guest"); }
    mz_context_pop();
}

static void handle_user_by_id(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    const char *id = mz_req_param(req, "id");
    mz_res_json(res);
    mz_buf_printf(&res->body, "{\"id\":%s}", id ? id : "0");
}

int main(void) {
    MzApp app;
    mz_app_init(&app);

    mz_app_use(&app, auth_middleware);
    mz_app_get(&app, "/items", handle_items);
    mz_app_post(&app, "/login", handle_login);
    mz_app_get(&app, "/users/:id", handle_user_by_id);

    // 1. HTMX request with OOB swap and trigger
    {
        const char *raw = "GET /items HTTP/1.1\r\nHost: localhost\r\nHX-Request: true\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw, strlen(raw), &req));
        assert(mz_req_is_htmx(&req));

        MzResponse res;
        mz_res_init(&res);
        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strstr(res.body.data, "<li>Item 1</li>") != NULL);
        assert(strstr(res.body.data, "hx-swap-oob=\"innerHTML\"") != NULL);

        MizarBuffer out;
        mz_buf_init(&out, 1024);
        mz_res_serialize(&res, &out);
        assert(strstr(out.data, "HX-Trigger: itemsLoaded") != NULL);
        assert(strstr(out.data, "HX-Reswap: innerHTML") != NULL);

        mz_buf_free(&out);
        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 2. Form submission with url decoding
    {
        const char *body = "user=John+Doe";
        char raw[512];
        snprintf(raw, sizeof(raw),
                 "POST /login HTTP/1.1\r\n"
                 "Host: localhost\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "Content-Length: %zu\r\n\r\n%s",
                 strlen(body), body);

        MzRequest req;
        assert(mz_http_parse_request(raw, strlen(raw), &req));
        assert(strcmp(mz_req_form(&req, "user"), "John Doe") == 0);

        MzResponse res;
        mz_res_init(&res);
        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strstr(res.body.data, "<p>Hello John Doe</p>") != NULL);

        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 3. Dynamic path parameter
    {
        const char *raw = "GET /users/42 HTTP/1.1\r\nHost: localhost\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw, strlen(raw), &req));

        MzResponse res;
        mz_res_init(&res);
        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 200);
        assert(strcmp(res.body.data, "{\"id\":42}") == 0);

        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 4. Middleware unauthorized
    {
        const char *raw = "GET /private HTTP/1.1\r\nHost: localhost\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw, strlen(raw), &req));

        MzResponse res;
        mz_res_init(&res);
        mz_app_handle(&app, &req, &res);

        assert(res.status_code == 401);

        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 5. Query parameters & Cookies
    {
        const char *raw = "GET /search?q=c23+framework&page=2 HTTP/1.1\r\n"
                          "Host: localhost\r\n"
                          "Cookie: session_id=xyz987; theme=light\r\n\r\n";
        MzRequest req;
        assert(mz_http_parse_request(raw, strlen(raw), &req));
        assert(strcmp(mz_req_query(&req, "q"), "c23 framework") == 0);
        assert(strcmp(mz_req_query(&req, "page"), "2") == 0);
        assert(strcmp(mz_req_cookie(&req, "session_id"), "xyz987") == 0);
        assert(strcmp(mz_req_cookie(&req, "theme"), "light") == 0);

        MzResponse res;
        mz_res_init(&res);
        mz_res_set_cookie(&res, "visited", "true", (MzCookieOpts){ .path = "/", .http_only = true });

        MizarBuffer out;
        mz_buf_init(&out, 512);
        mz_res_serialize(&res, &out);
        assert(strstr(out.data, "Set-Cookie: visited=true; Path=/; HttpOnly") != NULL);

        mz_buf_free(&out);
        mz_res_free(&res);
        mz_req_free(&req);
    }

    // 6. Streaming JSON DSL
    {
        MizarBuffer jbuf;
        mz_buf_init(&jbuf, 256);
        JsonDoc(&jbuf) {
            JsonObj() {
                JsonKeyStr("status", "ok");
                JsonKeyInt("code", 200);
                JsonKeyArr("tags") {
                    JsonValStr("c23");
                    JsonValStr("web");
                }
            }
        }
        assert(strcmp(jbuf.data, "{\"status\":\"ok\",\"code\":200,\"tags\":[\"c23\",\"web\"]}") == 0);
        mz_buf_free(&jbuf);
    }

    mz_app_free(&app);
    return 0;
}
