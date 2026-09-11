module mizar::view::css;
import mizar::core::buffer;

struct CssDecl {
    String prop;
    String val;
}

fn void rule_open(String selector) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str(selector);
    buf.append_str(" {\n");
}

fn void rule_close() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("}\n");
}

fn void prop(String property_name, String value) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("  ");
    buf.append_str(property_name);
    buf.append_str(": ");
    buf.append_str(value);
    buf.append_str(";\n");
}

macro void @rule(String selector; @body()) {
    rule_open(selector);
    @body();
    rule_close();
}
