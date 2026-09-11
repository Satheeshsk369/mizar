#ifndef MIZAR_SERVER_RADIX_H
#define MIZAR_SERVER_RADIX_H

#include "server/http.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef void (*MzHandlerFn)(const MzRequest *req, MzResponse *res, void *user_data);

typedef enum {
    MZ_NODE_STATIC,
    MZ_NODE_PARAM, // :param
    MZ_NODE_CATCHALL // * or *wildcard
} MzRadixNodeType;

typedef struct MzRadixNode MzRadixNode;

struct MzRadixNode {
    char *prefix;
    MzRadixNodeType type;
    char *param_name; // for :param

    // Handlers per method
    char **methods;
    MzHandlerFn *handlers;
    void **user_datas;
    size_t handler_count;

    MzRadixNode **children;
    size_t child_count;
};

static inline MzRadixNode *mz_radix_node_create(const char *prefix, MzRadixNodeType type, const char *param_name) {
    MzRadixNode *node = (MzRadixNode *)calloc(1, sizeof(MzRadixNode));
    if (!node) return nullptr;
    node->prefix = prefix ? strdup(prefix) : strdup("");
    node->type = type;
    node->param_name = param_name ? strdup(param_name) : nullptr;
    return node;
}

static inline void mz_radix_node_free(MzRadixNode *node) {
    if (!node) return;
    free(node->prefix);
    free(node->param_name);
    for (size_t i = 0; i < node->handler_count; i++) {
        free(node->methods[i]);
    }
    free(node->methods);
    free(node->handlers);
    free(node->user_datas);

    for (size_t i = 0; i < node->child_count; i++) {
        mz_radix_node_free(node->children[i]);
    }
    free(node->children);
    free(node);
}

static inline void mz_radix_add_handler(MzRadixNode *node, const char *method, MzHandlerFn handler, void *user_data) {
    for (size_t i = 0; i < node->handler_count; i++) {
        if (strcmp(node->methods[i], method) == 0) {
            node->handlers[i] = handler;
            node->user_datas[i] = user_data;
            return;
        }
    }
    size_t nc = node->handler_count + 1;
    char **nm = (char **)realloc(node->methods, nc * sizeof(char *));
    MzHandlerFn *nh = (MzHandlerFn *)realloc(node->handlers, nc * sizeof(MzHandlerFn));
    void **nu = (void **)realloc(node->user_datas, nc * sizeof(void *));
    if (!nm || !nh || !nu) return;
    node->methods = nm;
    node->handlers = nh;
    node->user_datas = nu;

    node->methods[node->handler_count] = strdup(method);
    node->handlers[node->handler_count] = handler;
    node->user_datas[node->handler_count] = user_data;
    node->handler_count = nc;
}

static inline void mz_radix_insert(MzRadixNode *root, const char *method, const char *pattern, MzHandlerFn handler, void *user_data) {
    if (!root || !pattern || !handler) return;
    MzRadixNode *current = root;
    const char *p = pattern;

    while (*p) {
        if (*p == ':') {
            p++;
            const char *end = strchr(p, '/');
            size_t name_len = end ? (size_t)(end - p) : strlen(p);
            char *param_name = (char *)malloc(name_len + 1);
            memcpy(param_name, p, name_len);
            param_name[name_len] = '\0';
            p += name_len;

            MzRadixNode *param_child = nullptr;
            for (size_t i = 0; i < current->child_count; i++) {
                if (current->children[i]->type == MZ_NODE_PARAM &&
                    strcmp(current->children[i]->param_name, param_name) == 0) {
                    param_child = current->children[i];
                    break;
                }
            }
            if (!param_child) {
                param_child = mz_radix_node_create("", MZ_NODE_PARAM, param_name);
                current->children = (MzRadixNode **)realloc(current->children, (current->child_count + 1) * sizeof(MzRadixNode *));
                current->children[current->child_count++] = param_child;
            }
            free(param_name);
            current = param_child;
        } else if (*p == '*' && *(p + 1) == '\0') {
            p++;
            MzRadixNode *catch_child = nullptr;
            for (size_t i = 0; i < current->child_count; i++) {
                if (current->children[i]->type == MZ_NODE_CATCHALL) {
                    catch_child = current->children[i];
                    break;
                }
            }
            if (!catch_child) {
                catch_child = mz_radix_node_create("*", MZ_NODE_CATCHALL, nullptr);
                current->children = (MzRadixNode **)realloc(current->children, (current->child_count + 1) * sizeof(MzRadixNode *));
                current->children[current->child_count++] = catch_child;
            }
            current = catch_child;
        } else {
            // Static segment up to next ':' or '*'
            const char *seg_end = p;
            while (*seg_end && *seg_end != ':' && *seg_end != '*') {
                seg_end++;
            }
            size_t seg_len = (size_t)(seg_end - p);
            char *seg = (char *)malloc(seg_len + 1);
            memcpy(seg, p, seg_len);
            seg[seg_len] = '\0';
            p = seg_end;

            MzRadixNode *static_child = nullptr;
            for (size_t i = 0; i < current->child_count; i++) {
                if (current->children[i]->type == MZ_NODE_STATIC &&
                    strcmp(current->children[i]->prefix, seg) == 0) {
                    static_child = current->children[i];
                    break;
                }
            }
            if (!static_child) {
                static_child = mz_radix_node_create(seg, MZ_NODE_STATIC, nullptr);
                current->children = (MzRadixNode **)realloc(current->children, (current->child_count + 1) * sizeof(MzRadixNode *));
                current->children[current->child_count++] = static_child;
            }
            free(seg);
            current = static_child;
        }
    }

    mz_radix_add_handler(current, method, handler, user_data);
}

static inline bool mz_radix_match_node(MzRadixNode *node, const char *path, const char *method,
                                       MzRequest *req, MzHandlerFn *out_handler, void **out_udata) {
    if (!node) return false;

    if (*path == '\0') {
        // Matched end of path; check handlers
        for (size_t i = 0; i < node->handler_count; i++) {
            if (strcmp(node->methods[i], "*") == 0 || strcmp(node->methods[i], method) == 0) {
                *out_handler = node->handlers[i];
                *out_udata = node->user_datas[i];
                return true;
            }
        }
    }

    // Try children
    for (size_t i = 0; i < node->child_count; i++) {
        MzRadixNode *child = node->children[i];
        if (child->type == MZ_NODE_STATIC) {
            size_t plen = strlen(child->prefix);
            if (strncmp(path, child->prefix, plen) == 0) {
                if (mz_radix_match_node(child, path + plen, method, req, out_handler, out_udata)) {
                    return true;
                }
            }
        } else if (child->type == MZ_NODE_PARAM) {
            const char *slash = strchr(path, '/');
            size_t vlen = slash ? (size_t)(slash - path) : strlen(path);
            if (vlen > 0) {
                size_t saved_param_count = req->param_count;
                if (req->param_count < MZ_HTTP_MAX_PARAMS) {
                    char *val = (char *)malloc(vlen + 1);
                    memcpy(val, path, vlen);
                    val[vlen] = '\0';
                    req->params[req->param_count].key = strdup(child->param_name);
                    req->params[req->param_count].value = val;
                    req->param_count++;
                }

                if (mz_radix_match_node(child, path + vlen, method, req, out_handler, out_udata)) {
                    return true;
                }

                // Backtrack params
                while (req->param_count > saved_param_count) {
                    req->param_count--;
                    free(req->params[req->param_count].key);
                    free(req->params[req->param_count].value);
                }
            }
        } else if (child->type == MZ_NODE_CATCHALL) {
            for (size_t h = 0; h < child->handler_count; h++) {
                if (strcmp(child->methods[h], "*") == 0 || strcmp(child->methods[h], method) == 0) {
                    *out_handler = child->handlers[h];
                    *out_udata = child->user_datas[h];
                    return true;
                }
            }
        }
    }

    return false;
}

static inline bool mz_radix_find(MzRadixNode *root, const char *method, const char *path,
                                 MzRequest *req, MzHandlerFn *out_handler, void **out_udata) {
    if (!root || !path || !method) return false;
    for (size_t i = 0; i < req->param_count; i++) {
        free(req->params[i].key);
        free(req->params[i].value);
    }
    req->param_count = 0;

    return mz_radix_match_node(root, path, method, req, out_handler, out_udata);
}

#endif
