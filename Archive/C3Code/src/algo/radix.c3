module mizar::algo::radix;
import mizar::algo::hashmap;
import mizar::server::http;

alias HandlerFn = fn void(void* req, void* res, void* user_data);

enum NodeType {
    STATIC,
    PARAM,     // :param
    CATCHALL   // *
}

struct MethodHandler {
    String method;
    HandlerFn handler;
    void* user_data;
}

struct RadixNode {
    String prefix;
    NodeType type;
    String param_name;

    MethodHandler* handlers;
    usz handler_count;

    RadixNode** children;
    usz child_count;
}

struct RouteMatch {
    HandlerFn handler;
    void* user_data;
    bool found;
}

fn RadixNode* new_node(String prefix, NodeType type, String param_name = "") {
    RadixNode* n = (RadixNode*)mem::malloc(RadixNode::size);
    if (n == null) return null;
    n.prefix = prefix;
    n.type = type;
    n.param_name = param_name;
    n.handlers = null;
    n.handler_count = 0;
    n.children = null;
    n.child_count = 0;
    return n;
}

fn void free_node(RadixNode* n) {
    if (n == null) return;
    if (n.handlers != null) {
        mem::free(n.handlers);
        n.handlers = null;
    }
    for (usz i = 0; i < n.child_count; i++) {
        free_node(n.children[i]);
    }
    if (n.children != null) {
        mem::free(n.children);
        n.children = null;
    }
    mem::free(n);
}

fn void RadixNode.add_handler(&this, String method, HandlerFn handler, void* user_data) {
    for (usz i = 0; i < this.handler_count; i++) {
        if (this.handlers[i].method == method) {
            this.handlers[i].handler = handler;
            this.handlers[i].user_data = user_data;
            return;
        }
    }
    usz nc = this.handler_count + 1;
    MethodHandler* nh = (MethodHandler*)mem::realloc(this.handlers, (sz)(nc * MethodHandler::size));
    if (nh == null) return;
    this.handlers = nh;
    this.handlers[this.handler_count] = {
        .method = method,
        .handler = handler,
        .user_data = user_data
    };
    this.handler_count = nc;
}

fn void RadixNode.add_child(&this, RadixNode* child) {
    usz nc = this.child_count + 1;
    RadixNode** nch = (RadixNode**)mem::realloc(this.children, (sz)(nc * uptr::size));
    if (nch == null) return;
    this.children = nch;
    this.children[this.child_count++] = child;
}

struct RadixRouter {
    RadixNode* root;
}

fn void RadixRouter.init(&this) {
    this.root = new_node("/", NodeType.STATIC);
}

fn void RadixRouter.free(&this) {
    if (this.root != null) {
        free_node(this.root);
        this.root = null;
    }
}

fn void RadixRouter.insert(&this, String method, String pattern, HandlerFn handler, void* user_data = null) {
    if (this.root == null || pattern.len == 0 || handler == null) return;

    RadixNode* curr = this.root;
    String p = pattern;

    if (p == "/") {
        curr.add_handler(method, handler, user_data);
        return;
    }

    while (p.len > 0) {
        if (p[0] == '/') {
            p = p[1..p.len - 1];
            if (p.len == 0) break;
        }

        if (p[0] == ':') {
            int next_slash = -1;
            for (int i = 0; i < p.len; i++) {
                if (p[i] == '/') {
                    next_slash = i;
                    break;
                }
            }
            String param_name = next_slash != -1 ? p[1..next_slash - 1] : p[1..p.len - 1];
            p = next_slash != -1 ? p[next_slash..p.len - 1] : "";

            RadixNode* param_child = null;
            for (usz i = 0; i < curr.child_count; i++) {
                if (curr.children[i].type == NodeType.PARAM && curr.children[i].param_name == param_name) {
                    param_child = curr.children[i];
                    break;
                }
            }
            if (param_child == null) {
                param_child = new_node("", NodeType.PARAM, param_name);
                curr.add_child(param_child);
            }
            curr = param_child;
        } else if (p[0] == '*') {
            RadixNode* catch_child = null;
            for (usz i = 0; i < curr.child_count; i++) {
                if (curr.children[i].type == NodeType.CATCHALL) {
                    catch_child = curr.children[i];
                    break;
                }
            }
            if (catch_child == null) {
                catch_child = new_node("*", NodeType.CATCHALL);
                curr.add_child(catch_child);
            }
            curr = catch_child;
            break;
        } else {
            int next_slash = -1;
            for (int i = 0; i < p.len; i++) {
                if (p[i] == '/' || p[i] == ':' || p[i] == '*') {
                    next_slash = i;
                    break;
                }
            }
            String segment = next_slash != -1 ? p[0..next_slash - 1] : p;
            p = next_slash != -1 ? p[next_slash..p.len - 1] : "";

            RadixNode* static_child = null;
            for (usz i = 0; i < curr.child_count; i++) {
                if (curr.children[i].type == NodeType.STATIC && curr.children[i].prefix == segment) {
                    static_child = curr.children[i];
                    break;
                }
            }
            if (static_child == null) {
                static_child = new_node(segment, NodeType.STATIC);
                curr.add_child(static_child);
            }
            curr = static_child;
        }
    }

    curr.add_handler(method, handler, user_data);
}

fn RouteMatch match_node(RadixNode* node, String method, String path, hashmap::HashMap* params) {
    RouteMatch none = { .handler = null, .user_data = null, .found = false };
    if (node == null) return none;

    if (path.len == 0 || path == "/") {
        for (usz i = 0; i < node.handler_count; i++) {
            if (node.handlers[i].method == method) {
                return {
                    .handler = node.handlers[i].handler,
                    .user_data = node.handlers[i].user_data,
                    .found = true
                };
            }
        }
    }

    String p = path;
    if (p.len > 0 && p[0] == '/') {
        p = p[1..p.len - 1];
    }
    if (p.len == 0) return none;

    int next_slash = -1;
    for (int i = 0; i < p.len; i++) {
        if (p[i] == '/') {
            next_slash = i;
            break;
        }
    }
    String segment = next_slash != -1 ? p[0..next_slash - 1] : p;
    String rest = next_slash != -1 ? p[next_slash..p.len - 1] : "";

    for (usz i = 0; i < node.child_count; i++) {
        RadixNode* c = node.children[i];
        if (c.type == NodeType.STATIC && c.prefix == segment) {
            RouteMatch m = match_node(c, method, rest, params);
            if (m.found) return m;
        }
    }

    for (usz i = 0; i < node.child_count; i++) {
        RadixNode* c = node.children[i];
        if (c.type == NodeType.PARAM) {
            if (params != null) {
                http::StringEntry* se = http::alloc_entry(params.arena, segment);
                params.set(c.param_name, (void*)se);
            }
            RouteMatch m = match_node(c, method, rest, params);
            if (m.found) return m;
        }
    }

    for (usz i = 0; i < node.child_count; i++) {
        RadixNode* c = node.children[i];
        if (c.type == NodeType.CATCHALL) {
            for (usz h = 0; h < c.handler_count; h++) {
                if (c.handlers[h].method == method) {
                    return {
                        .handler = c.handlers[h].handler,
                        .user_data = c.handlers[h].user_data,
                        .found = true
                    };
                }
            }
        }
    }

    return none;
}

fn RouteMatch RadixRouter.lookup(&this, String method, String path, hashmap::HashMap* params = null) {
    if (this.root == null) return { .found = false };
    if (path == "/") {
        for (usz i = 0; i < this.root.handler_count; i++) {
            if (this.root.handlers[i].method == method) {
                return {
                    .handler = this.root.handlers[i].handler,
                    .user_data = this.root.handlers[i].user_data,
                    .found = true
                };
            }
        }
        return { .found = false };
    }
    return match_node(this.root, method, path, params);
}
