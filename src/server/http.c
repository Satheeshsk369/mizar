#include "server/http.h"
#include "core/url.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static char *mz_strndup(const char *s, size_t n) {
    char *p = (char *)malloc(n + 1);
    if (!p) return nullptr;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

static int mz_strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

const char *mz_req_header(const MzRequest *req, const char *key) {
    if (!req || !key) return nullptr;
    if (req->header_map.capacity > 0) {
        return (const char *)mz_map_get_cstr(&req->header_map, key);
    }
    for (size_t i = 0; i < req->header_count; i++) {
        if (mz_strcasecmp(req->headers[i].key, key) == 0) {
            return req->headers[i].value;
        }
    }
    return nullptr;
}

const char *mz_req_param(const MzRequest *req, const char *key) {
    if (!req || !key) return nullptr;
    for (size_t i = 0; i < req->param_count; i++) {
        if (strcmp(req->params[i].key, key) == 0) {
            return req->params[i].value;
        }
    }
    return nullptr;
}

const char *mz_req_query(const MzRequest *req, const char *key) {
    if (!req || !key) return nullptr;
    if (req->query_map.capacity > 0) {
        return (const char *)mz_map_get_cstr(&req->query_map, key);
    }
    for (size_t i = 0; i < req->query_count; i++) {
        if (strcmp(req->queries[i].key, key) == 0) {
            return req->queries[i].value;
        }
    }
    return nullptr;
}

const char *mz_req_form(const MzRequest *req, const char *key) {
    if (!req || !key) return nullptr;
    if (req->form_map.capacity > 0) {
        return (const char *)mz_map_get_cstr(&req->form_map, key);
    }
    for (size_t i = 0; i < req->form_count; i++) {
        if (strcmp(req->forms[i].key, key) == 0) {
            return req->forms[i].value;
        }
    }
    return nullptr;
}

static MzIntResult parse_int_str(const char *raw) {
    if (!raw || !*raw) return (MzIntResult){ .ok = false, .val = 0 };
    char *end = nullptr;
    errno = 0;
    int64_t v = strtoll(raw, &end, 10);
    if (errno != 0 || end == raw || *end != '\0') {
        return (MzIntResult){ .ok = false, .val = 0 };
    }
    return (MzIntResult){ .ok = true, .val = v };
}

static MzUintResult parse_uint_str(const char *raw) {
    if (!raw || !*raw) return (MzUintResult){ .ok = false, .val = 0 };
    while (*raw == ' ') raw++;
    if (*raw == '-') return (MzUintResult){ .ok = false, .val = 0 };
    char *end = nullptr;
    errno = 0;
    uint64_t v = strtoull(raw, &end, 10);
    if (errno != 0 || end == raw || *end != '\0') {
        return (MzUintResult){ .ok = false, .val = 0 };
    }
    return (MzUintResult){ .ok = true, .val = v };
}

static MzFloatResult parse_float_str(const char *raw) {
    if (!raw || !*raw) return (MzFloatResult){ .ok = false, .val = 0.0 };
    char *end = nullptr;
    errno = 0;
    double v = strtod(raw, &end);
    if (errno != 0 || end == raw || *end != '\0') {
        return (MzFloatResult){ .ok = false, .val = 0.0 };
    }
    return (MzFloatResult){ .ok = true, .val = v };
}

static MzBoolResult parse_bool_str(const char *raw) {
    if (!raw || !*raw) return (MzBoolResult){ .ok = false, .val = false };
    if (mz_strcasecmp(raw, "true") == 0 ||
        mz_strcasecmp(raw, "1") == 0 ||
        mz_strcasecmp(raw, "on") == 0 ||
        mz_strcasecmp(raw, "yes") == 0) {
        return (MzBoolResult){ .ok = true, .val = true };
    }
    if (mz_strcasecmp(raw, "false") == 0 ||
        mz_strcasecmp(raw, "0") == 0 ||
        mz_strcasecmp(raw, "off") == 0 ||
        mz_strcasecmp(raw, "no") == 0) {
        return (MzBoolResult){ .ok = true, .val = false };
    }
    return (MzBoolResult){ .ok = false, .val = false };
}

MzIntResult mz_req_param_int(const MzRequest *req, const char *key) {
    return parse_int_str(mz_req_param(req, key));
}

MzUintResult mz_req_param_uint(const MzRequest *req, const char *key) {
    return parse_uint_str(mz_req_param(req, key));
}

MzIntResult mz_req_query_int(const MzRequest *req, const char *key) {
    return parse_int_str(mz_req_query(req, key));
}

MzUintResult mz_req_query_uint(const MzRequest *req, const char *key) {
    return parse_uint_str(mz_req_query(req, key));
}

MzFloatResult mz_req_query_float(const MzRequest *req, const char *key) {
    return parse_float_str(mz_req_query(req, key));
}

MzBoolResult mz_req_query_bool(const MzRequest *req, const char *key) {
    return parse_bool_str(mz_req_query(req, key));
}

MzSliceResult mz_req_query_slice(const MzRequest *req, const char *key) {
    const char *raw = mz_req_query(req, key);
    if (!raw) return (MzSliceResult){ .ok = false, .val = MZ_SLICE_NULL };
    return (MzSliceResult){ .ok = true, .val = mz_slice_from_cstr(raw) };
}

MzIntResult mz_req_form_int(const MzRequest *req, const char *key) {
    return parse_int_str(mz_req_form(req, key));
}

MzUintResult mz_req_form_uint(const MzRequest *req, const char *key) {
    return parse_uint_str(mz_req_form(req, key));
}

MzFloatResult mz_req_form_float(const MzRequest *req, const char *key) {
    return parse_float_str(mz_req_form(req, key));
}

MzBoolResult mz_req_form_bool(const MzRequest *req, const char *key) {
    return parse_bool_str(mz_req_form(req, key));
}

MzSliceResult mz_req_form_slice(const MzRequest *req, const char *key) {
    const char *raw = mz_req_form(req, key);
    if (!raw) return (MzSliceResult){ .ok = false, .val = MZ_SLICE_NULL };
    return (MzSliceResult){ .ok = true, .val = mz_slice_from_cstr(raw) };
}

int64_t mz_req_query_int_or(const MzRequest *req, const char *key, int64_t default_val) {
    MzIntResult r = mz_req_query_int(req, key);
    return r.ok ? r.val : default_val;
}

int64_t mz_req_query_int_bounded(const MzRequest *req, const char *key, int64_t default_val, int64_t min_val, int64_t max_val) {
    MzIntResult r = mz_req_query_int(req, key);
    int64_t v = r.ok ? r.val : default_val;
    if (v < min_val) v = min_val;
    if (v > max_val) v = max_val;
    return v;
}

bool mz_req_query_bool_or(const MzRequest *req, const char *key, bool default_val) {
    MzBoolResult r = mz_req_query_bool(req, key);
    return r.ok ? r.val : default_val;
}

int64_t mz_req_form_int_or(const MzRequest *req, const char *key, int64_t default_val) {
    MzIntResult r = mz_req_form_int(req, key);
    return r.ok ? r.val : default_val;
}

int64_t mz_req_form_int_bounded(const MzRequest *req, const char *key, int64_t default_val, int64_t min_val, int64_t max_val) {
    MzIntResult r = mz_req_form_int(req, key);
    int64_t v = r.ok ? r.val : default_val;
    if (v < min_val) v = min_val;
    if (v > max_val) v = max_val;
    return v;
}

bool mz_req_form_bool_or(const MzRequest *req, const char *key, bool default_val) {
    MzBoolResult r = mz_req_form_bool(req, key);
    return r.ok ? r.val : default_val;
}

size_t mz_req_query_all(const MzRequest *req, const char *key, const char *out[], size_t max_items) {
    if (!req || !key || !out || max_items == 0) return 0;
    size_t count = 0;
    for (size_t i = 0; i < req->query_count && count < max_items; i++) {
        if (strcmp(req->queries[i].key, key) == 0) {
            out[count++] = req->queries[i].value;
        }
    }
    return count;
}

size_t mz_req_form_all(const MzRequest *req, const char *key, const char *out[], size_t max_items) {
    if (!req || !key || !out || max_items == 0) return 0;
    size_t count = 0;
    for (size_t i = 0; i < req->form_count && count < max_items; i++) {
        if (strcmp(req->forms[i].key, key) == 0) {
            out[count++] = req->forms[i].value;
        }
    }
    return count;
}

const char *mz_req_cookie(const MzRequest *req, const char *key) {
    if (!req || !key) return nullptr;
    if (req->cookie_map.capacity > 0) {
        return (const char *)mz_map_get_cstr(&req->cookie_map, key);
    }
    for (size_t i = 0; i < req->cookie_count; i++) {
        if (strcmp(req->cookies[i].name, key) == 0) {
            return req->cookies[i].value;
        }
    }
    return nullptr;
}

void mz_res_set_cookie(MzResponse *res, const char *name, const char *val, MzCookieOpts opts) {
    if (!res || !name || !val) return;
    char cookie_str[512];
    int n = snprintf(cookie_str, sizeof(cookie_str), "%s=%s", name, val);
    if (opts.path && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; Path=%s", opts.path);
    }
    if (opts.max_age > 0 && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; Max-Age=%d", opts.max_age);
    } else if (opts.max_age < 0 && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; Max-Age=0; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
    }
    if (opts.http_only && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; HttpOnly");
    }
    if (opts.secure && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; Secure");
    }
    if (opts.partitioned && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; Partitioned");
    }
    if (opts.same_site && n < (int)sizeof(cookie_str)) {
        n += snprintf(cookie_str + n, sizeof(cookie_str) - n, "; SameSite=%s", opts.same_site);
    }
    mz_res_header(res, "Set-Cookie", cookie_str);
}

bool mz_req_is_htmx(const MzRequest *req) {
    const char *val = mz_req_header(req, "HX-Request");
    return (val != nullptr && strcmp(val, "true") == 0);
}

bool mz_req_is_htmx_boosted(const MzRequest *req) {
    const char *val = mz_req_header(req, "HX-Boosted");
    return (val != nullptr && strcmp(val, "true") == 0);
}

bool mz_req_is_htmx_history_restore(const MzRequest *req) {
    const char *val = mz_req_header(req, "HX-History-Restore-Request");
    return (val != nullptr && strcmp(val, "true") == 0);
}

const char *mz_req_htmx_target(const MzRequest *req) {
    return mz_req_header(req, "HX-Target");
}

const char *mz_req_htmx_source(const MzRequest *req) {
    return mz_req_header(req, "HX-Source");
}

const char *mz_req_htmx_trigger(const MzRequest *req) {
    return mz_req_header(req, "HX-Trigger");
}

const char *mz_req_htmx_trigger_name(const MzRequest *req) {
    // HTMX 2 used HX-Trigger-Name; HTMX 4 uses HX-Source
    const char *v = mz_req_header(req, "HX-Trigger-Name");
    return v ? v : mz_req_header(req, "HX-Source");
}

const char *mz_req_htmx_request_type(const MzRequest *req) {
    return mz_req_header(req, "HX-Request-Type");
}

const char *mz_req_htmx_current_url(const MzRequest *req) {
    return mz_req_header(req, "HX-Current-URL");
}

const char *mz_req_htmx_prompt(const MzRequest *req) {
    return mz_req_header(req, "HX-Prompt");
}

void mz_res_init(MzResponse *res) {
    if (!res) return;
    res->status_code = 200;
    res->status_text = "OK";
    res->header_count = 0;
    mz_buf_init(&res->body, 1024);
}

void mz_res_free(MzResponse *res) {
    if (!res) return;
    for (size_t i = 0; i < res->header_count; i++) {
        free(res->headers[i].key);
        free(res->headers[i].value);
    }
    res->header_count = 0;
    mz_buf_free(&res->body);
}

void mz_res_status(MzResponse *res, int code, const char *text) {
    if (!res) return;
    res->status_code = code;
    res->status_text = text ? text : "OK";
}

void mz_res_header(MzResponse *res, const char *key, const char *val) {
    if (!res || !key || !val || res->header_count >= MZ_HTTP_MAX_HEADERS) return;
    res->headers[res->header_count].key = strdup(key);
    res->headers[res->header_count].value = strdup(val);
    res->header_count++;
}

void mz_res_content_type(MzResponse *res, const char *mime) {
    mz_res_header(res, "Content-Type", mime);
}

void mz_res_html(MzResponse *res) {
    mz_res_content_type(res, "text/html; charset=utf-8");
}

void mz_res_json(MzResponse *res) {
    mz_res_content_type(res, "application/json");
}

// HTMX Response Modifiers
void mz_res_retarget(MzResponse *res, const char *target_selector) {
    mz_res_header(res, "HX-Retarget", target_selector);
}

void mz_res_reswap(MzResponse *res, const char *swap_style) {
    mz_res_header(res, "HX-Reswap", swap_style);
}

void mz_res_reselect(MzResponse *res, const char *select_selector) {
    mz_res_header(res, "HX-Reselect", select_selector);
}

void mz_res_push_url(MzResponse *res, const char *url) {
    mz_res_header(res, "HX-Push-Url", url);
}

void mz_res_replace_url(MzResponse *res, const char *url) {
    mz_res_header(res, "HX-Replace-Url", url);
}

void mz_res_refresh(MzResponse *res) {
    mz_res_header(res, "HX-Refresh", "true");
}

void mz_res_redirect(MzResponse *res, const char *url) {
    mz_res_header(res, "HX-Redirect", url);
}

void mz_res_location(MzResponse *res, const char *url_or_spec) {
    mz_res_header(res, "HX-Location", url_or_spec);
}

void mz_res_trigger(MzResponse *res, const char *event_name) {
    mz_res_header(res, "HX-Trigger", event_name);
}

void mz_res_trigger_after_swap(MzResponse *res, const char *event_name) {
    mz_res_header(res, "HX-Trigger-After-Swap", event_name);
}

void mz_res_trigger_after_settle(MzResponse *res, const char *event_name) {
    mz_res_header(res, "HX-Trigger-After-Settle", event_name);
}

static void mz_parse_kv_string(MzArena *arena, const char *qs, MzParam *arr, size_t *count, size_t max_count) {
    if (!qs || !*qs || !arr || !count) return;
    const char *p = qs;
    while (*p && *count < max_count) {
        const char *eq = strchr(p, '=');
        const char *amp = strchr(p, '&');
        if (!amp) amp = p + strlen(p);

        if (eq && eq < amp) {
            char *raw_k = mz_strndup(p, eq - p);
            char *raw_v = mz_strndup(eq + 1, amp - (eq + 1));
            char *dec_k = mz_url_decode(raw_k, strlen(raw_k), true);
            char *dec_v = mz_url_decode(raw_v, strlen(raw_v), true);
            if (arena) {
                arr[*count].key = mz_arena_strdup(arena, dec_k);
                arr[*count].value = mz_arena_strdup(arena, dec_v);
                free(dec_k);
                free(dec_v);
            } else {
                arr[*count].key = dec_k;
                arr[*count].value = dec_v;
            }
            free(raw_k);
            free(raw_v);
            (*count)++;
        } else {
            char *raw_k = mz_strndup(p, amp - p);
            char *dec_k = mz_url_decode(raw_k, strlen(raw_k), true);
            if (arena) {
                arr[*count].key = mz_arena_strdup(arena, dec_k);
                arr[*count].value = mz_arena_strdup(arena, "");
                free(dec_k);
            } else {
                arr[*count].key = dec_k;
                arr[*count].value = strdup("");
            }
            free(raw_k);
            (*count)++;
        }

        if (*amp == '&') p = amp + 1;
        else break;
    }
}

bool mz_http_parse_request(const char *raw, size_t raw_len, MzRequest *req) {
    if (!raw || !req || raw_len == 0) return false;
    memset(req, 0, sizeof(MzRequest));

    mz_arena_init(&req->arena, 8192);
    // Arena-backed hash maps: zero individual mallocs, zero per-field free overhead
    mz_map_init_arena(&req->header_map, 32, true, &req->arena);
    mz_map_init_arena(&req->cookie_map, 0, false, &req->arena); // Lazy initial capacity
    mz_map_init_arena(&req->query_map, 0, false, &req->arena);  // Lazy initial capacity
    mz_map_init_arena(&req->form_map, 0, false, &req->arena);   // Lazy initial capacity

    const char *line_end = strstr(raw, "\r\n");
    if (!line_end) return false;

    // 1. Request Line: METHOD PATH[?QUERY] PROTO
    char method[16], url[1024];
    if (sscanf(raw, "%15s %1023s", method, url) < 2) {
        return false;
    }
    req->method = mz_arena_strdup(&req->arena, method);

    char *q = strchr(url, '?');
    if (q) {
        req->path = mz_arena_strndup(&req->arena, url, q - url);
        req->query_string = mz_arena_strdup(&req->arena, q + 1);
        mz_parse_kv_string(&req->arena, req->query_string, req->queries, &req->query_count, MZ_HTTP_MAX_PARAMS);
        for (size_t i = 0; i < req->query_count; i++) {
            mz_map_set_cstr(&req->query_map, req->queries[i].key, req->queries[i].value);
        }
    } else {
        req->path = mz_arena_strdup(&req->arena, url);
        req->query_string = mz_arena_strdup(&req->arena, "");
    }

    // 2. Headers
    const char *p = line_end + 2;
    while (*p && req->header_count < MZ_HTTP_MAX_HEADERS) {
        if (p[0] == '\r' && p[1] == '\n') {
            p += 2; // End of headers
            break;
        }
        const char *next_crlf = strstr(p, "\r\n");
        if (!next_crlf) break;

        const char *colon = strchr(p, ':');
        if (colon && colon < next_crlf) {
            req->headers[req->header_count].key = mz_arena_strndup(&req->arena, p, colon - p);
            
            // Skip colon and leading spaces
            const char *val_start = colon + 1;
            while (val_start < next_crlf && (*val_start == ' ' || *val_start == '\t')) {
                val_start++;
            }
            req->headers[req->header_count].value = mz_arena_strndup(&req->arena, val_start, next_crlf - val_start);

            // Index in O(1) header map
            mz_map_set_cstr(&req->header_map, req->headers[req->header_count].key, req->headers[req->header_count].value);

            // If Cookie header, parse cookies into request
            if (mz_strcasecmp(req->headers[req->header_count].key, "Cookie") == 0) {
                const char *cp = req->headers[req->header_count].value;
                while (*cp && req->cookie_count < MZ_HTTP_MAX_COOKIES) {
                    while (*cp == ' ' || *cp == ';') cp++;
                    if (!*cp) break;
                    const char *eq = strchr(cp, '=');
                    const char *semi = strchr(cp, ';');
                    if (!semi) semi = cp + strlen(cp);

                    if (eq && eq < semi) {
                        req->cookies[req->cookie_count].name = mz_arena_strndup(&req->arena, cp, eq - cp);
                        req->cookies[req->cookie_count].value = mz_arena_strndup(&req->arena, eq + 1, semi - (eq + 1));
                        mz_map_set_cstr(&req->cookie_map, req->cookies[req->cookie_count].name, req->cookies[req->cookie_count].value);
                        req->cookie_count++;
                    }
                    if (*semi == ';') cp = semi + 1;
                    else break;
                }
            }

            req->header_count++;
        }
        p = next_crlf + 2;
    }

    // 3. Body
    size_t header_len = p - raw;
    if (raw_len > header_len) {
        req->body_len = raw_len - header_len;
        req->body = mz_arena_strndup(&req->arena, p, req->body_len);

        // Check if application/x-www-form-urlencoded
        const char *ct = mz_req_header(req, "Content-Type");
        if (ct && strstr(ct, "application/x-www-form-urlencoded")) {
            mz_parse_kv_string(&req->arena, req->body, req->forms, &req->form_count, MZ_HTTP_MAX_PARAMS);
            for (size_t i = 0; i < req->form_count; i++) {
                mz_map_set_cstr(&req->form_map, req->forms[i].key, req->forms[i].value);
            }
        }
    } else {
        req->body = nullptr;
        req->body_len = 0;
    }

    return true;
}

void mz_req_free(MzRequest *req) {
    if (!req) return;
    for (size_t i = 0; i < req->param_count; i++) {
        free(req->params[i].key);
        free(req->params[i].value);
    }
    // Instant bulk deallocation of all request strings, hash tables, and buckets!
    mz_arena_free(&req->arena);

    memset(req, 0, sizeof(MzRequest));
}

void mz_res_serialize(const MzResponse *res, MizarBuffer *out) {
    if (!res || !out) return;

    mz_buf_printf(out, "HTTP/1.1 %d %s\r\n", res->status_code, res->status_text);

    bool has_content_length = false;
    for (size_t i = 0; i < res->header_count; i++) {
        if (mz_strcasecmp(res->headers[i].key, "Content-Length") == 0) {
            has_content_length = true;
        }
        mz_buf_printf(out, "%s: %s\r\n", res->headers[i].key, res->headers[i].value);
    }

    if (!has_content_length) {
        mz_buf_printf(out, "Content-Length: %zu\r\n", res->body.len);
    }
    mz_buf_append_str(out, "Connection: close\r\n\r\n");

    if (res->body.len > 0) {
        mz_buf_append(out, res->body.data, res->body.len);
    }
}
