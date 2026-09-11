module mizar::core::json;
import mizar::core::buffer;

fn void strip_trailing_comma(buffer::Buffer* buf) {
    if (buf.len > 0 && buf.data[buf.len - 1] == ',') {
        buf.len--;
        buf.data[buf.len] = '\0';
    }
}

fn void json_open_obj() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('{');
}

fn void json_close_obj() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    strip_trailing_comma(buf);
    buf.append_str("}");
}

fn void json_open_arr() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('[');
}

fn void json_close_arr() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    strip_trailing_comma(buf);
    buf.append_str("]");
}

macro void @json_obj(; @body()) {
    json_open_obj();
    @body();
    json_close_obj();
}

macro void @json_arr(; @body()) {
    json_open_arr();
    @body();
    json_close_arr();
}

fn void json_str(String key, String val) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('"');
    buf.append_str(key);
    buf.append_str("\":\"");
    buf.append_escaped(val);
    buf.append_str("\",");
}

fn void json_int(String key, long val) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('"');
    buf.append_str(key);
    buf.append_str("\":");

    if (val == 0) {
        buf.append_str("0,");
        return;
    }

    char[32] rev;
    int rlen = 0;
    long temp = val < 0 ? -val : val;
    while (temp > 0) {
        rev[rlen++] = (char)('0' + (temp % 10));
        temp /= 10;
    }
    if (val < 0) buf.append_char('-');
    for (int i = rlen - 1; i >= 0; i--) {
        buf.append_char(rev[i]);
    }
    buf.append_char(',');
}

fn void json_bool(String key, bool val) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('"');
    buf.append_str(key);
    buf.append_str(val ? "\":true," : "\":false,");
}
