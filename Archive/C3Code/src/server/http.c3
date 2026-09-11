module mizar::server::http;
import mizar::core::buffer;
import mizar::algo::arena;
import mizar::algo::hashmap;

const usz HTTP_MAX_HEADERS = 64;
const usz HTTP_MAX_PARAMS  = 32;
const usz HTTP_MAX_COOKIES = 32;

struct Header {
    String key;
    String value;
}

struct Cookie {
    String name;
    String value;
}

struct StringEntry {
    String s;
}

struct Request {
    arena::Arena arena;
    hashmap::HashMap header_map;
    hashmap::HashMap cookie_map;
    hashmap::HashMap query_map;
    hashmap::HashMap form_map;
    hashmap::HashMap param_map;

    String method;
    String path;
    String query_string;

    Header[HTTP_MAX_HEADERS] headers;
    usz header_count;

    Cookie[HTTP_MAX_COOKIES] cookies;
    usz cookie_count;

    String body;
    void* user_data;
}

struct Response {
    int status_code;
    String status_text;

    Header[HTTP_MAX_HEADERS] headers;
    usz header_count;

    buffer::Buffer body;
}

fn void Request.init(&this) {
    this.arena.init(4096);
    this.header_map.init(32, true, &this.arena);
    this.cookie_map.init(16, false, &this.arena);
    this.query_map.init(16, false, &this.arena);
    this.form_map.init(16, false, &this.arena);
    this.param_map.init(16, false, &this.arena);

    this.method = "";
    this.path = "";
    this.query_string = "";
    this.header_count = 0;
    this.cookie_count = 0;
    this.body = "";
    this.user_data = null;
}

fn void Request.free(&this) {
    this.header_map.free();
    this.cookie_map.free();
    this.query_map.free();
    this.form_map.free();
    this.param_map.free();
    this.arena.free();
}

fn String Request.header(&this, String key) {
    StringEntry* v = (StringEntry*)this.header_map.get(key);
    if (v != null) return v.s;
    return "";
}

fn String Request.query(&this, String key) {
    StringEntry* v = (StringEntry*)this.query_map.get(key);
    if (v != null) return v.s;
    return "";
}

fn String Request.param(&this, String key) {
    StringEntry* v = (StringEntry*)this.param_map.get(key);
    if (v != null) return v.s;
    return "";
}

fn String Request.form(&this, String key) {
    StringEntry* v = (StringEntry*)this.form_map.get(key);
    if (v != null) return v.s;
    return "";
}

fn String Request.cookie(&this, String key) {
    StringEntry* v = (StringEntry*)this.cookie_map.get(key);
    if (v != null) return v.s;
    return "";
}

fn bool Request.is_htmx(&this) {
    return this.header("HX-Request") == "true";
}

fn bool Request.is_htmx_boosted(&this) {
    return this.header("HX-Boosted") == "true";
}

fn String Request.htmx_target(&this) {
    return this.header("HX-Target");
}

fn String Request.htmx_trigger(&this) {
    return this.header("HX-Trigger");
}

fn void Response.init(&this) {
    this.status_code = 200;
    this.status_text = "OK";
    this.header_count = 0;
    this.body.init(1024);
}

fn void Response.free(&this) {
    this.body.free();
}

fn void Response.status(&this, int code, String text = "OK") {
    this.status_code = code;
    this.status_text = text;
}

fn void Response.header(&this, String key, String val) {
    if (this.header_count < HTTP_MAX_HEADERS) {
        this.headers[this.header_count++] = { .key = key, .value = val };
    }
}

fn void Response.content_type(&this, String mime) {
    this.header("Content-Type", mime);
}

fn void Response.html(&this) {
    this.content_type("text/html; charset=utf-8");
}

fn void Response.json(&this) {
    this.content_type("application/json");
}

fn void Response.retarget(&this, String target) {
    this.header("HX-Retarget", target);
}

fn void Response.reswap(&this, String swap) {
    this.header("HX-Reswap", swap);
}

fn void Response.redirect(&this, String url, bool htmx_aware = true) {
    if (htmx_aware) {
        this.header("HX-Redirect", url);
    }
    this.header("Location", url);
    this.status(302, "Found");
}

fn int find_char(String s, char c, int start = 0) {
    for (int i = start; i < s.len; i++) {
        if (s[i] == c) return i;
    }
    return -1;
}

fn int find_str(String s, String target, int start = 0) {
    if (target.len == 0) return start;
    if (s.len < target.len) return -1;
    for (int i = start; i <= s.len - target.len; i++) {
        bool match = true;
        for (int j = 0; j < target.len; j++) {
            if (s[i + j] != target[j]) {
                match = false;
                break;
            }
        }
        if (match) return i;
    }
    return -1;
}

fn StringEntry* alloc_entry(arena::Arena* ar, String s) {
    StringEntry* e = (StringEntry*)ar.alloc(StringEntry::size);
    if (e != null) {
        e.s = s;
    }
    return e;
}

fn void parse_kv(arena::Arena* ar, hashmap::HashMap* map, String qs) {
    if (qs.len == 0) return;
    int pos = 0;
    while (pos < qs.len) {
        int next_amp = find_char(qs, '&', pos);
        String pair = next_amp != -1 ? qs[pos..next_amp - 1] : qs[pos..qs.len - 1];
        if (pair.len > 0) {
            int eq = find_char(pair, '=');
            if (eq != -1) {
                String k = ar.strdup(pair[0..eq - 1]);
                String v = ar.strdup(pair[eq + 1..pair.len - 1]);
                map.set(k, (void*)alloc_entry(ar, v));
            } else {
                String k = ar.strdup(pair);
                map.set(k, (void*)alloc_entry(ar, ""));
            }
        }
        if (next_amp == -1) break;
        pos = next_amp + 1;
    }
}

fn bool Request.parse(&this, String raw) {
    if (raw.len < 10) return false;

    // 1. Request line: METHOD PATH[?QUERY] HTTP/1.1\r\n
    int crlf1 = find_str(raw, "\r\n");
    if (crlf1 == -1) return false;

    String line = raw[0..crlf1 - 1];
    int sp1 = find_char(line, ' ');
    if (sp1 == -1) return false;

    int sp2 = find_char(line, ' ', sp1 + 1);
    if (sp2 == -1) return false;

    this.method = this.arena.strdup(line[0..sp1 - 1]);
    String full_path = line[sp1 + 1..sp2 - 1];

    int qm = find_char(full_path, '?');
    if (qm != -1) {
        this.path = this.arena.strdup(full_path[0..qm - 1]);
        this.query_string = this.arena.strdup(full_path[qm + 1..full_path.len - 1]);
        parse_kv(&this.arena, &this.query_map, this.query_string);
    } else {
        this.path = this.arena.strdup(full_path);
        this.query_string = "";
    }

    // 2. Parse Headers until \r\n\r\n
    int header_end = find_str(raw, "\r\n\r\n", crlf1 + 2);
    if (header_end == -1) return false;

    int pos = crlf1 + 2;
    while (pos < header_end && this.header_count < HTTP_MAX_HEADERS) {
        int next_crlf = find_str(raw, "\r\n", pos);
        if (next_crlf == -1 || next_crlf > header_end) break;

        String h_line = raw[pos..next_crlf - 1];
        int colon = find_char(h_line, ':');
        if (colon != -1) {
            String k = this.arena.strdup(h_line[0..colon - 1]);
            int v_start = colon + 1;
            while (v_start < h_line.len && (h_line[v_start] == ' ' || h_line[v_start] == '\t')) {
                v_start++;
            }
            String v = this.arena.strdup(h_line[v_start..h_line.len - 1]);
            this.headers[this.header_count++] = { .key = k, .value = v };
            this.header_map.set(k, (void*)alloc_entry(&this.arena, v));

            // Cookies
            if (k == "Cookie" || k == "cookie") {
                parse_kv(&this.arena, &this.cookie_map, v);
            }
        }
        pos = next_crlf + 2;
    }

    // 3. Body
    int body_start = header_end + 4;
    if (body_start < raw.len) {
        this.body = this.arena.strdup(raw[body_start..raw.len - 1]);
        String ct = this.header("Content-Type");
        if (find_str(ct, "application/x-www-form-urlencoded") != -1) {
            parse_kv(&this.arena, &this.form_map, this.body);
        }
    } else {
        this.body = "";
    }

    return true;
}

fn void Response.serialize(&this, buffer::Buffer* out) {
    out.append_str("HTTP/1.1 ");
    // Convert status_code to string
    char[16] code_buf;
    int n = 0;
    int c = this.status_code;
    if (c == 0) c = 200;
    char[16] rev;
    int rlen = 0;
    while (c > 0) {
        rev[rlen++] = (char)('0' + (c % 10));
        c /= 10;
    }
    for (int i = rlen - 1; i >= 0; i--) {
        code_buf[n++] = rev[i];
    }
    code_buf[n] = '\0';
    out.append_str((String)code_buf[0..n - 1]);
    out.append_char(' ');
    out.append_str(this.status_text);
    out.append_str("\r\n");

    bool has_len = false;
    for (usz i = 0; i < this.header_count; i++) {
        if (this.headers[i].key == "Content-Length") has_len = true;
        out.append_str(this.headers[i].key);
        out.append_str(": ");
        out.append_str(this.headers[i].value);
        out.append_str("\r\n");
    }

    if (!has_len) {
        out.append_str("Content-Length: ");
        usz len = this.body.len;
        if (len == 0) {
            out.append_str("0");
        } else {
            char[32] lrev;
            int llen = 0;
            usz temp = len;
            while (temp > 0) {
                lrev[llen++] = (char)('0' + (temp % 10));
                temp /= 10;
            }
            char[32] lbuf;
            int ln = 0;
            for (int i = llen - 1; i >= 0; i--) {
                lbuf[ln++] = lrev[i];
            }
            out.append_str((String)lbuf[0..ln - 1]);
        }
        out.append_str("\r\n");
    }

    out.append_str("Connection: close\r\n\r\n");

    if (this.body.len > 0) {
        out.append_str(this.body.as_string());
    }
}
