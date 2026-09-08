#ifndef MIZAR_SERVER_TLS_H
#define MIZAR_SERVER_TLS_H

#ifdef MIZAR_ENABLE_TLS
#include <bearssl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

typedef struct {
    br_x509_certificate *chain;
    size_t chain_len;
    int key_type; // BR_KEYTYPE_RSA or BR_KEYTYPE_EC
    union {
        br_rsa_private_key rsa;
        br_ec_private_key ec;
    } key;
    unsigned char *key_mem; // memory arena preserving key byte buffers
} MzTlsCertKey;

typedef struct {
    unsigned char *data;
    size_t len;
    size_t cap;
} MzTlsBlob;

static inline void mz_tls_blob_append(void *ctx, const void *src, size_t len) {
    MzTlsBlob *b = (MzTlsBlob *)ctx;
    if (b->len + len > b->cap) {
        size_t ncap = b->cap ? b->cap * 2 : 1024;
        while (ncap < b->len + len) ncap *= 2;
        b->data = (unsigned char *)realloc(b->data, ncap);
        b->cap = ncap;
    }
    memcpy(b->data + b->len, src, len);
    b->len += len;
}

// Loads certificate chain and private key from PEM files
static inline bool mz_tls_load_cert_and_key(const char *cert_path, const char *key_path, MzTlsCertKey *out) {
    if (!cert_path || !key_path || !out) return false;
    memset(out, 0, sizeof(MzTlsCertKey));

    // 1. Load Certificate Chain
    FILE *fc = fopen(cert_path, "rb");
    if (!fc) return false;
    fseek(fc, 0, SEEK_END);
    long csize = ftell(fc);
    fseek(fc, 0, SEEK_SET);
    if (csize <= 0) { fclose(fc); return false; }
    char *cbuf = (char *)malloc(csize);
    if (!cbuf) { fclose(fc); return false; }
    fread(cbuf, 1, csize, fc);
    fclose(fc);

    br_pem_decoder_context pc;
    br_pem_decoder_init(&pc);
    MzTlsBlob cur_blob = {0};
    br_pem_decoder_setdest(&pc, mz_tls_blob_append, &cur_blob);

    size_t off = 0;
    while (off < (size_t)csize) {
        off += br_pem_decoder_push(&pc, cbuf + off, (size_t)csize - off);
        int ev = br_pem_decoder_event(&pc);
        if (ev == BR_PEM_BEGIN_OBJ) {
            cur_blob.len = 0;
        } else if (ev == BR_PEM_END_OBJ) {
            if (cur_blob.len > 0) {
                out->chain = (br_x509_certificate *)realloc(out->chain, (out->chain_len + 1) * sizeof(br_x509_certificate));
                out->chain[out->chain_len].data = (unsigned char *)malloc(cur_blob.len);
                memcpy(out->chain[out->chain_len].data, cur_blob.data, cur_blob.len);
                out->chain[out->chain_len].data_len = cur_blob.len;
                out->chain_len++;
                cur_blob.len = 0;
            }
        }
    }
    free(cur_blob.data);
    free(cbuf);

    if (out->chain_len == 0) return false;

    // 2. Load Private Key
    FILE *fk = fopen(key_path, "rb");
    if (!fk) return false;
    fseek(fk, 0, SEEK_END);
    long ksize = ftell(fk);
    fseek(fk, 0, SEEK_SET);
    if (ksize <= 0) { fclose(fk); return false; }
    char *kbuf = (char *)malloc(ksize);
    if (!kbuf) { fclose(fk); return false; }
    fread(kbuf, 1, ksize, fk);
    fclose(fk);

    br_pem_decoder_init(&pc);
    MzTlsBlob key_blob = {0};
    br_pem_decoder_setdest(&pc, mz_tls_blob_append, &key_blob);

    off = 0;
    while (off < (size_t)ksize) {
        off += br_pem_decoder_push(&pc, kbuf + off, (size_t)ksize - off);
        int ev = br_pem_decoder_event(&pc);
        if (ev == BR_PEM_BEGIN_OBJ) {
            key_blob.len = 0;
        } else if (ev == BR_PEM_END_OBJ) {
            if (key_blob.len > 0) break;
        }
    }
    free(kbuf);

    if (key_blob.len == 0) {
        free(key_blob.data);
        return false;
    }

    br_skey_decoder_context skdc;
    br_skey_decoder_init(&skdc);
    br_skey_decoder_push(&skdc, key_blob.data, key_blob.len);
    if (br_skey_decoder_last_error(&skdc) != 0) {
        free(key_blob.data);
        return false;
    }

    out->key_type = br_skey_decoder_key_type(&skdc);
    if (out->key_type == BR_KEYTYPE_RSA) {
        const br_rsa_private_key *orig = br_skey_decoder_get_rsa(&skdc);
        size_t total = orig->plen + orig->qlen + orig->dplen + orig->dqlen + orig->iqlen;
        out->key_mem = (unsigned char *)malloc(total);
        unsigned char *p = out->key_mem;

        out->key.rsa.n_bitlen = orig->n_bitlen;
        out->key.rsa.plen = orig->plen; memcpy(p, orig->p, orig->plen); out->key.rsa.p = p; p += orig->plen;
        out->key.rsa.qlen = orig->qlen; memcpy(p, orig->q, orig->qlen); out->key.rsa.q = p; p += orig->qlen;
        out->key.rsa.dplen = orig->dplen; memcpy(p, orig->dp, orig->dplen); out->key.rsa.dp = p; p += orig->dplen;
        out->key.rsa.dqlen = orig->dqlen; memcpy(p, orig->dq, orig->dqlen); out->key.rsa.dq = p; p += orig->dqlen;
        out->key.rsa.iqlen = orig->iqlen; memcpy(p, orig->iq, orig->iqlen); out->key.rsa.iq = p;
    } else if (out->key_type == BR_KEYTYPE_EC) {
        const br_ec_private_key *orig = br_skey_decoder_get_ec(&skdc);
        out->key_mem = (unsigned char *)malloc(orig->xlen);
        memcpy(out->key_mem, orig->x, orig->xlen);
        out->key.ec.curve = orig->curve;
        out->key.ec.x = out->key_mem;
        out->key.ec.xlen = orig->xlen;
    } else {
        free(key_blob.data);
        return false;
    }

    free(key_blob.data);
    return true;
}

static inline void mz_tls_free_cert_key(MzTlsCertKey *ck) {
    if (!ck) return;
    for (size_t i = 0; i < ck->chain_len; i++) {
        free(ck->chain[i].data);
    }
    free(ck->chain);
    free(ck->key_mem);
    memset(ck, 0, sizeof(MzTlsCertKey));
}

// Low-level socket read/write callbacks for BearSSL
static inline int mz_tls_sock_read(void *ctx, unsigned char *buf, size_t len) {
    int fd = *(int *)ctx;
    for (;;) {
        ssize_t rlen = recv(fd, buf, len, 0);
        if (rlen <= 0) {
            if (rlen < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
                continue;
            }
            return -1;
        }
        return (int)rlen;
    }
}

static inline int mz_tls_sock_write(void *ctx, const unsigned char *buf, size_t len) {
    int fd = *(int *)ctx;
    for (;;) {
#if defined(__linux__)
        ssize_t wlen = send(fd, buf, len, MSG_NOSIGNAL);
#else
        ssize_t wlen = send(fd, buf, len, 0);
#endif
        if (wlen <= 0) {
            if (wlen < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
                continue;
            }
            return -1;
        }
        return (int)wlen;
    }
}

// High-level BearSSL Server Wrapper Structure
typedef struct {
    br_ssl_server_context sc;
    br_ssl_session_cache_lru lru;
    unsigned char lru_buf[2048];
    unsigned char iobuf[BR_SSL_BUFSIZE_BIDI];
    br_sslio_context ioc;
    bool active;
} MzTlsSession;

static inline bool mz_tls_session_init(MzTlsSession *sess, const MzTlsCertKey *ck, int *client_fd) {
    if (!sess || !ck || !client_fd) return false;
    memset(sess, 0, sizeof(MzTlsSession));

    br_ssl_session_cache_lru_init(&sess->lru, sess->lru_buf, sizeof(sess->lru_buf));

    if (ck->key_type == BR_KEYTYPE_RSA) {
        br_ssl_server_init_full_rsa(&sess->sc, ck->chain, ck->chain_len, &ck->key.rsa);
    } else if (ck->key_type == BR_KEYTYPE_EC) {
        br_ssl_server_init_full_ec(&sess->sc, ck->chain, ck->chain_len, BR_KEYTYPE_EC, &ck->key.ec);
    } else {
        return false;
    }

    br_ssl_server_set_cache(&sess->sc, &sess->lru.vtable);
    br_ssl_engine_set_buffer(&sess->sc.eng, sess->iobuf, sizeof(sess->iobuf), 1);
    br_ssl_server_reset(&sess->sc);

    br_sslio_init(&sess->ioc, &sess->sc.eng, mz_tls_sock_read, client_fd, mz_tls_sock_write, client_fd);
    sess->active = true;
    return true;
}

static inline int mz_tls_read(MzTlsSession *sess, void *buf, size_t len) {
    if (!sess || !sess->active) return -1;
    return br_sslio_read(&sess->ioc, buf, len);
}

static inline int mz_tls_write_all(MzTlsSession *sess, const void *buf, size_t len) {
    if (!sess || !sess->active) return -1;
    return br_sslio_write_all(&sess->ioc, buf, len);
}

static inline int mz_tls_flush(MzTlsSession *sess) {
    if (!sess || !sess->active) return -1;
    return br_sslio_flush(&sess->ioc);
}

static inline int mz_tls_close(MzTlsSession *sess) {
    if (!sess || !sess->active) return 0;
    int res = br_sslio_close(&sess->ioc);
    sess->active = false;
    return res;
}

#endif // MIZAR_ENABLE_TLS

#endif // MIZAR_SERVER_TLS_H
