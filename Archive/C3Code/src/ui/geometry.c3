module mizar::ui::geometry;
import mizar::core::buffer;

struct EdgeInsets {
    double top;
    double right;
    double bottom;
    double left;
}

fn EdgeInsets insets_all(double val) {
    return { .top = val, .right = val, .bottom = val, .left = val };
}

fn EdgeInsets insets_symmetric(double horizontal, double vertical) {
    return { .top = vertical, .right = horizontal, .bottom = vertical, .left = horizontal };
}

fn EdgeInsets insets_only(double top = 0.0, double right = 0.0, double bottom = 0.0, double left = 0.0) {
    return { .top = top, .right = right, .bottom = bottom, .left = left };
}

fn void append_padding_style(buffer::Buffer* buf, EdgeInsets insets) {
    buf.append_str("padding: ");
    append_px(buf, (int)insets.top);
    buf.append_char(' ');
    append_px(buf, (int)insets.right);
    buf.append_char(' ');
    append_px(buf, (int)insets.bottom);
    buf.append_char(' ');
    append_px(buf, (int)insets.left);
    buf.append_str("; ");
}

fn void append_px(buffer::Buffer* buf, int val) {
    if (val == 0) {
        buf.append_str("0px");
        return;
    }
    char[16] rev;
    int rlen = 0;
    int temp = val < 0 ? -val : val;
    while (temp > 0) {
        rev[rlen++] = (char)('0' + (temp % 10));
        temp /= 10;
    }
    for (int i = rlen - 1; i >= 0; i--) {
        buf.append_char(rev[i]);
    }
    buf.append_str("px");
}
