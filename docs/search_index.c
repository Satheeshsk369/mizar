#include "docs_shared.h"
#include <stdlib.h>

#define MAX_SEARCH_DOCS 64

typedef struct {
    char url[128];
    char title[128];
    char category[64];
    char body[1024];
} SearchDoc;

static SearchDoc s_search_docs[MAX_SEARCH_DOCS];
static size_t s_search_doc_count = 0;

void docs_register_search_item(const char *url, const char *title, const char *category, const char *body) {
    if (s_search_doc_count >= MAX_SEARCH_DOCS) return;
    SearchDoc *d = &s_search_docs[s_search_doc_count++];
    snprintf(d->url, sizeof(d->url), "%s", url ? url : "/");
    snprintf(d->title, sizeof(d->title), "%s", title ? title : "");
    snprintf(d->category, sizeof(d->category), "%s", category ? category : "General");
    snprintf(d->body, sizeof(d->body), "%s", body ? body : "");
}

static void escape_json_str(MizarBuffer *buf, const char *s) {
    while (*s) {
        if (*s == '"') mz_buf_append_str(buf, "\\\"");
        else if (*s == '\\') mz_buf_append_str(buf, "\\\\");
        else if (*s == '\n') mz_buf_append_str(buf, " ");
        else if (*s == '\r') {}
        else if (*s == '\t') mz_buf_append_str(buf, " ");
        else mz_buf_append_char(buf, *s);
        s++;
    }
}

void docs_export_search_index(const char *filepath) {
    MizarBuffer buf;
    mz_buf_init(&buf, 16384);
    mz_buf_append_str(&buf, "[\n");

    for (size_t i = 0; i < s_search_doc_count; i++) {
        SearchDoc *d = &s_search_docs[i];
        mz_buf_append_str(&buf, "  {\n");
        mz_buf_append_str(&buf, "    \"url\": \"");
        escape_json_str(&buf, d->url);
        mz_buf_append_str(&buf, "\",\n    \"title\": \"");
        escape_json_str(&buf, d->title);
        mz_buf_append_str(&buf, "\",\n    \"category\": \"");
        escape_json_str(&buf, d->category);
        mz_buf_append_str(&buf, "\",\n    \"body\": \"");
        escape_json_str(&buf, d->body);
        mz_buf_append_str(&buf, "\"\n  }");
        if (i + 1 < s_search_doc_count) {
            mz_buf_append_str(&buf, ",");
        }
        mz_buf_append_str(&buf, "\n");
    }

    mz_buf_append_str(&buf, "]\n");
    mz_fs_write_file(filepath, buf.data, buf.len);
    mz_buf_free(&buf);
}
