module mizar::core::buffer;

alias FlushFn = fn bool(String chunk, void* user_data);

struct Buffer {
    char* data;
    usz len;
    usz capacity;
    bool has_error;
    usz chunk_threshold;
    FlushFn flush_fn;
    void* user_data;
}

const usz MAX_CONTEXT_DEPTH = 64;
tlocal Buffer*[MAX_CONTEXT_DEPTH] s_context_stack;
tlocal usz s_context_depth = 0;

fn void context_push(Buffer* buf) {
    if (s_context_depth < MAX_CONTEXT_DEPTH) {
        s_context_stack[s_context_depth++] = buf;
    }
}

fn void context_pop() {
    if (s_context_depth > 0) {
        s_context_depth--;
    }
}

fn Buffer* context_get() {
    if (s_context_depth > 0) {
        return s_context_stack[s_context_depth - 1];
    }
    return null;
}

fn usz context_get_depth() {
    return s_context_depth;
}

fn void context_restore_depth(usz depth) {
    if (depth <= MAX_CONTEXT_DEPTH) {
        s_context_depth = depth;
    }
}

fn void Buffer.init(&this, usz initial_cap = 256) {
    usz cap = initial_cap < 256 ? 256 : initial_cap;
    this.data = (char*)mem::malloc((sz)cap);
    this.len = 0;
    this.capacity = this.data != null ? cap : 0;
    this.has_error = this.data == null;
    this.chunk_threshold = 0;
    this.flush_fn = null;
    this.user_data = null;
    if (this.data != null) {
        this.data[0] = '\0';
    }
}

fn void Buffer.free(&this) {
    if (this.data != null) {
        mem::free(this.data);
        this.data = null;
    }
    this.len = 0;
    this.capacity = 0;
    this.has_error = false;
}

fn bool Buffer.flush(&this) {
    if (this.has_error) return false;
    if (this.len == 0) return true;
    if (this.flush_fn != null) {
        String slice = (String)this.data[0..this.len - 1];
        bool ok = this.flush_fn(slice, this.user_data);
        if (!ok) {
            this.has_error = true;
            return false;
        }
        this.len = 0;
        if (this.data != null) this.data[0] = '\0';
        return true;
    }
    return true;
}

fn bool Buffer.ensure_space(&this, usz extra) {
    if (this.has_error) return false;
    if (this.chunk_threshold > 0 && this.len >= this.chunk_threshold && this.flush_fn != null) {
        if (!this.flush()) return false;
    }
    usz needed = this.len + extra + 1;
    if (needed <= this.capacity) return true;

    usz new_cap = this.capacity > 0 ? this.capacity * 2 : 256;
    while (new_cap < needed) {
        new_cap *= 2;
    }
    char* p = (char*)mem::realloc(this.data, (sz)new_cap);
    if (p == null) {
        this.has_error = true;
        return false;
    }
    this.data = p;
    this.capacity = new_cap;
    return true;
}

fn void Buffer.append_char(&this, char c) {
    if (!this.ensure_space(1)) return;
    this.data[this.len++] = c;
    this.data[this.len] = '\0';
}

fn void Buffer.append_str(&this, String str) {
    if (str.len == 0) return;
    if (!this.ensure_space((usz)str.len)) return;
    mem::copy(&this.data[this.len], str.ptr, (sz)str.len);
    this.len += (usz)str.len;
    this.data[this.len] = '\0';
}

fn void Buffer.append_escaped(&this, String str) {
    if (str.len == 0) return;
    foreach (char c : str) {
        switch (c) {
            case '&':  this.append_str("&amp;");
            case '<':  this.append_str("&lt;");
            case '>':  this.append_str("&gt;");
            case '"':  this.append_str("&quot;");
            case '\'': this.append_str("&#39;");
            default:   this.append_char(c);
        }
    }
}

fn String Buffer.as_string(&this) {
    if (this.data == null || this.len == 0) return "";
    return (String)this.data[0..this.len - 1];
}
