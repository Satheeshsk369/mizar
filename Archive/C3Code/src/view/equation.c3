module mizar::view::equation;
import mizar::core::buffer;

fn void eq(String expr) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("\\(");
    buf.append_str(expr);
    buf.append_str("\\)");
}

fn void eq_block(String expr) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("\\[");
    buf.append_str(expr);
    buf.append_str("\\]");
}
