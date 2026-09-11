module mizar::view::js;
import mizar::core::buffer;

fn void script_open() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("<script>\n");
}

fn void script_close() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("</script>\n");
}

macro void @script(; @body()) {
    script_open();
    @body();
    script_close();
}

fn void js_line(String code) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("  ");
    buf.append_str(code);
    buf.append_str(";\n");
}

fn void js_const(String name, String value_expr) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("  const ");
    buf.append_str(name);
    buf.append_str(" = ");
    buf.append_str(value_expr);
    buf.append_str(";\n");
}

fn void js_let(String name, String value_expr) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("  let ");
    buf.append_str(name);
    buf.append_str(" = ");
    buf.append_str(value_expr);
    buf.append_str(";\n");
}
