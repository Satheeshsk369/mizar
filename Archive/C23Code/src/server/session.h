#ifndef MIZAR_SERVER_SESSION_H
#define MIZAR_SERVER_SESSION_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "server/http.h"
#include "core/sha256.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MZ_SESSION_MAX_ENTRIES 16
#define MZ_SESSION_COOKIE_NAME "mz_session"

typedef struct {
    char key[64];
    char val[128];
} MzSessionEntry;

typedef struct {
    MzSessionEntry entries[MZ_SESSION_MAX_ENTRIES];
    size_t count;
    bool modified;
} MzSession;

// Cryptographic HMAC-SHA256 signature generator (64-char hex output + null)
static inline void mz_session_compute_hmac_hex(const char *data, size_t len, const char *secret, char out_hex[65]) {
    uint8_t digest[32];
    size_t secret_len = secret ? strlen(secret) : 0;
    mz_hmac_sha256(secret ? secret : "", secret_len, data, len, digest);
    for (int i = 0; i < 32; i++) {
        snprintf(out_hex + (i * 2), 3, "%02x", digest[i]);
    }
    out_hex[64] = '\0';
}

// Backward-compatibility / internal 64-bit helper if needed
static inline void mz_session_sig_to_hex(uint64_t sig, char *out_hex) {
    snprintf(out_hex, 17, "%016llx", (unsigned long long)sig);
}

// Decode session cookie from request into MzSession struct
static inline bool mz_session_read(const MzRequest *req, const char *secret, MzSession *sess) {
    if (!sess) return false;
    memset(sess, 0, sizeof(MzSession));
    if (!req) return false;

    const char *cookie = mz_req_cookie(req, MZ_SESSION_COOKIE_NAME);
    if (!cookie || !*cookie) return false;

    // Expected format: <payload>.<64_hex_hmac_sha256>
    const char *dot = strrchr(cookie, '.');
    if (!dot || (size_t)(dot - cookie) == 0) return false;

    size_t payload_len = (size_t)(dot - cookie);
    const char *sig_str = dot + 1;
    if (strlen(sig_str) != 64) return false;

    char expected_hex[65];
    mz_session_compute_hmac_hex(cookie, payload_len, secret, expected_hex);

    if (!mz_crypto_timing_safe_eq(sig_str, expected_hex, 64)) {
        // Tampered session cookie or invalid secret
        return false;
    }

    // Parse payload: key1=val1&key2=val2
    char payload[1024];
    if (payload_len >= sizeof(payload)) return false;
    memcpy(payload, cookie, payload_len);
    payload[payload_len] = '\0';

    char *p = payload;
    while (*p && sess->count < MZ_SESSION_MAX_ENTRIES) {
        char *eq = strchr(p, '=');
        char *amp = strchr(p, '&');
        bool has_next = (amp != nullptr);
        if (!amp) amp = p + strlen(p);

        if (eq && eq < amp) {
            *eq = '\0';
            *amp = '\0';
            size_t klen = strlen(p);
            if (klen >= sizeof(sess->entries[0].key)) klen = sizeof(sess->entries[0].key) - 1;
            memcpy(sess->entries[sess->count].key, p, klen);
            sess->entries[sess->count].key[klen] = '\0';

            const char *val_str = eq + 1;
            size_t vlen = strlen(val_str);
            if (vlen >= sizeof(sess->entries[0].val)) vlen = sizeof(sess->entries[0].val) - 1;
            memcpy(sess->entries[sess->count].val, val_str, vlen);
            sess->entries[sess->count].val[vlen] = '\0';

            sess->count++;
        }
        if (!has_next) break;
        p = amp + 1;
    }
    return true;
}

// Read value for a key from session
static inline const char *mz_session_get(const MzSession *sess, const char *key) {
    if (!sess || !key) return nullptr;
    for (size_t i = 0; i < sess->count; i++) {
        if (strcmp(sess->entries[i].key, key) == 0) {
            return sess->entries[i].val;
        }
    }
    return nullptr;
}

// Set value for a key in session
static inline void mz_session_set(MzSession *sess, const char *key, const char *val) {
    if (!sess || !key || !val) return;
    for (size_t i = 0; i < sess->count; i++) {
        if (strcmp(sess->entries[i].key, key) == 0) {
            strncpy(sess->entries[i].val, val, sizeof(sess->entries[0].val) - 1);
            sess->entries[i].val[sizeof(sess->entries[0].val) - 1] = '\0';
            sess->modified = true;
            return;
        }
    }
    if (sess->count < MZ_SESSION_MAX_ENTRIES) {
        strncpy(sess->entries[sess->count].key, key, sizeof(sess->entries[0].key) - 1);
        sess->entries[sess->count].key[sizeof(sess->entries[0].key) - 1] = '\0';
        strncpy(sess->entries[sess->count].val, val, sizeof(sess->entries[0].val) - 1);
        sess->entries[sess->count].val[sizeof(sess->entries[0].val) - 1] = '\0';
        sess->count++;
        sess->modified = true;
    }
}

// Delete a key from session
static inline void mz_session_remove(MzSession *sess, const char *key) {
    if (!sess || !key) return;
    for (size_t i = 0; i < sess->count; i++) {
        if (strcmp(sess->entries[i].key, key) == 0) {
            for (size_t j = i; j + 1 < sess->count; j++) {
                sess->entries[j] = sess->entries[j + 1];
            }
            sess->count--;
            sess->modified = true;
            return;
        }
    }
}

// Clear all session entries
static inline void mz_session_clear(MzSession *sess) {
    if (!sess) return;
    sess->count = 0;
    sess->modified = true;
}

// Write signed session cookie into response headers
static inline void mz_session_write(MzResponse *res, const MzSession *sess, const char *secret, MzCookieOpts opts) {
    if (!res || !sess) return;
    if (sess->count == 0) {
        // Expire cookie
        opts.max_age = -1;
        mz_res_set_cookie(res, MZ_SESSION_COOKIE_NAME, "", opts);
        return;
    }

    char payload[1024];
    size_t offset = 0;
    for (size_t i = 0; i < sess->count; i++) {
        int written = snprintf(payload + offset, sizeof(payload) - offset,
                               "%s%s=%s", (i > 0 ? "&" : ""),
                               sess->entries[i].key, sess->entries[i].val);
        if (written < 0 || (size_t)written >= sizeof(payload) - offset) break;
        offset += (size_t)written;
    }

    char sig_hex[65];
    mz_session_compute_hmac_hex(payload, offset, secret, sig_hex);

    char full_cookie[1200];
    snprintf(full_cookie, sizeof(full_cookie), "%s.%s", payload, sig_hex);

    if (!opts.same_site) opts.same_site = "Lax";
    opts.http_only = true;
    mz_res_set_cookie(res, MZ_SESSION_COOKIE_NAME, full_cookie, opts);
}

#endif
