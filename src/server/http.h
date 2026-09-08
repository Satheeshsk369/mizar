#ifndef MIZAR_SERVER_HTTP_H
#define MIZAR_SERVER_HTTP_H

#include "core/buffer.h"
#include "core/result.h"
#include <stddef.h>
#include <stdbool.h>

#define MZ_HTTP_MAX_HEADERS 64
#define MZ_HTTP_MAX_PARAMS  32

typedef struct {
    char *key;
    char *value;
} MzHeader;

typedef struct {
    char *key;
    char *value;
} MzParam;

typedef struct {
    char *method;          // "GET", "POST", "PUT", "DELETE", etc.
    char *path;            // "/users/42"
    char *query_string;    // "page=1&sort=desc"
    
    MzHeader headers[MZ_HTTP_MAX_HEADERS];
    size_t header_count;

    MzParam params[MZ_HTTP_MAX_PARAMS];   // Parsed path params (e.g. :id -> 42)
    size_t param_count;

    MzParam queries[MZ_HTTP_MAX_PARAMS];  // Parsed query params (e.g. page -> 1)
    size_t query_count;

    MzParam forms[MZ_HTTP_MAX_PARAMS];    // Parsed form-urlencoded body fields
    size_t form_count;

    char *body;
    size_t body_len;

    void *user_data;
} MzRequest;

typedef struct {
    int status_code;
    const char *status_text;
    
    MzHeader headers[MZ_HTTP_MAX_HEADERS];
    size_t header_count;

    MizarBuffer body;
} MzResponse;

// Request helpers
const char *mz_req_header(const MzRequest *req, const char *key);
const char *mz_req_param(const MzRequest *req, const char *key);
const char *mz_req_query(const MzRequest *req, const char *key);
const char *mz_req_form(const MzRequest *req, const char *key);
const char *mz_req_cookie(const MzRequest *req, const char *key);

// Type-safe, validated request parsers (returning explicit tagged results)
MzIntResult mz_req_param_int(const MzRequest *req, const char *key);
MzIntResult mz_req_query_int(const MzRequest *req, const char *key);
MzFloatResult mz_req_query_float(const MzRequest *req, const char *key);
MzSliceResult mz_req_query_slice(const MzRequest *req, const char *key);

typedef struct {
    const char *path;
    int max_age;
    bool http_only;
    bool secure;
    const char *same_site; // "Strict", "Lax", "None"
} MzCookieOpts;

// Cookie helper
void mz_res_set_cookie(MzResponse *res, const char *name, const char *val, MzCookieOpts opts);

// HTMX Request Inspection
bool mz_req_is_htmx(const MzRequest *req);
bool mz_req_is_htmx_boosted(const MzRequest *req);
bool mz_req_is_htmx_history_restore(const MzRequest *req);
const char *mz_req_htmx_target(const MzRequest *req);
const char *mz_req_htmx_source(const MzRequest *req);        // HTMX 4: tag#id
const char *mz_req_htmx_trigger(const MzRequest *req);       // HTMX 2/4 trigger ID or event
const char *mz_req_htmx_trigger_name(const MzRequest *req);  // HTMX 2 compatibility alias
const char *mz_req_htmx_request_type(const MzRequest *req);  // HTMX 4: "full" | "partial"
const char *mz_req_htmx_current_url(const MzRequest *req);   // HX-Current-URL
const char *mz_req_htmx_prompt(const MzRequest *req);        // HX-Prompt

// Response helpers
void mz_res_init(MzResponse *res);
void mz_res_free(MzResponse *res);
void mz_res_status(MzResponse *res, int code, const char *text);
void mz_res_header(MzResponse *res, const char *key, const char *val);
void mz_res_content_type(MzResponse *res, const char *mime);
void mz_res_html(MzResponse *res);
void mz_res_json(MzResponse *res);

// First-Class HTMX Response Modifiers
void mz_res_retarget(MzResponse *res, const char *target_selector);
void mz_res_reswap(MzResponse *res, const char *swap_style);
void mz_res_reselect(MzResponse *res, const char *select_selector);
void mz_res_push_url(MzResponse *res, const char *url);
void mz_res_replace_url(MzResponse *res, const char *url);
void mz_res_refresh(MzResponse *res);
void mz_res_redirect(MzResponse *res, const char *url);
void mz_res_location(MzResponse *res, const char *url_or_spec);
void mz_res_trigger(MzResponse *res, const char *event_name);
void mz_res_trigger_after_swap(MzResponse *res, const char *event_name);
void mz_res_trigger_after_settle(MzResponse *res, const char *event_name);

// Out-of-Band (OOB) Swap DSL Helpers
#define HxOob(target_id, swap_mode) .id = (target_id), .custom = "hx-swap-oob=\"" swap_mode "\""
#define HxOobTrue(target_id)        .id = (target_id), .custom = "hx-swap-oob=\"true\""

// Parsing & Serializing HTTP
bool mz_http_parse_request(const char *raw, size_t raw_len, MzRequest *req);
void mz_req_free(MzRequest *req);
void mz_res_serialize(const MzResponse *res, MizarBuffer *out);

#endif
