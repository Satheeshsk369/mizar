#ifndef MIZAR_SERVER_MIDDLEWARE_H
#define MIZAR_SERVER_MIDDLEWARE_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "server/http.h"
#include <stdio.h>
#include <strings.h>
#include <string.h>

// -----------------------------------------------------------------------------
// 1. Terminal Request Logger Middleware
// -----------------------------------------------------------------------------
static inline bool mz_middleware_logger(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)res; (void)user_data;
    if (req) {
        printf("[MIZAR] %-6s %s\n", req->method ? req->method : "UNKNOWN", req->path ? req->path : "/");
    }
    return true;
}

// -----------------------------------------------------------------------------
// 2. Standard Security Headers Middleware
// -----------------------------------------------------------------------------
static inline bool mz_middleware_security_headers(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    if (res) {
        mz_res_header(res, "X-Content-Type-Options", "nosniff");
        mz_res_header(res, "X-Frame-Options", "DENY");
        mz_res_header(res, "Referrer-Policy", "strict-origin-when-cross-origin");
        mz_res_header(res, "X-XSS-Protection", "1; mode=block");
    }
    return true;
}

// -----------------------------------------------------------------------------
// 3. CORS Middleware Configuration & Handler
// -----------------------------------------------------------------------------
typedef struct {
    const char *allow_origin;      // e.g. "*" or "https://example.com" (default: "*")
    const char *allow_methods;     // e.g. "GET, POST, PUT, DELETE, PATCH, OPTIONS"
    const char *allow_headers;     // e.g. "Content-Type, Authorization, HX-Request, HX-Target, HX-Current-URL"
    bool allow_credentials;        // true -> Access-Control-Allow-Credentials: true
    int max_age;                   // preflight cache duration in seconds (e.g. 86400)
} MzCorsOpts;

static inline bool mz_middleware_cors_handler(const MzRequest *req, MzResponse *res, void *user_data) {
    if (!res) return true;
    MzCorsOpts *opts = (MzCorsOpts *)user_data;
    const char *origin = (opts && opts->allow_origin) ? opts->allow_origin : "*";
    const char *methods = (opts && opts->allow_methods) ? opts->allow_methods : "GET, POST, PUT, DELETE, PATCH, OPTIONS";
    const char *headers = (opts && opts->allow_headers) ? opts->allow_headers : "Content-Type, Authorization, HX-Request, HX-Target, HX-Current-URL, HX-Trigger";

    mz_res_header(res, "Access-Control-Allow-Origin", origin);
    mz_res_header(res, "Access-Control-Allow-Methods", methods);
    mz_res_header(res, "Access-Control-Allow-Headers", headers);

    if (opts && opts->allow_credentials) {
        mz_res_header(res, "Access-Control-Allow-Credentials", "true");
    }

    if (opts && opts->max_age > 0) {
        char max_age_str[16];
        snprintf(max_age_str, sizeof(max_age_str), "%d", opts->max_age);
        mz_res_header(res, "Access-Control-Max-Age", max_age_str);
    }

    // Intercept OPTIONS preflight requests immediately
    if (req && req->method && strcasecmp(req->method, "OPTIONS") == 0) {
        mz_res_status(res, 204, "No Content");
        return false; // Stop middleware pipeline, return 204 directly
    }

    return true;
}

#endif
