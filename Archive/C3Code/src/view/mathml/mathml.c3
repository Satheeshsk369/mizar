module mizar::view::mathml;
import mizar::core::buffer;

fn void math_tag_open(String name) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('<');
    buf.append_str(name);
    buf.append_char('>');
}

fn void math_tag_close(String name) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("</");
    buf.append_str(name);
    buf.append_char('>');
}

macro void @math(; @body())  { math_tag_open("math"); @body(); math_tag_close("math"); }
macro void @mrow(; @body())  { math_tag_open("mrow"); @body(); math_tag_close("mrow"); }
macro void @mi(String val) { math_tag_open("mi"); buffer::context_get().append_escaped(val); math_tag_close("mi"); }
macro void @mo(String val) { math_tag_open("mo"); buffer::context_get().append_escaped(val); math_tag_close("mo"); }
macro void @mn(String val) { math_tag_open("mn"); buffer::context_get().append_escaped(val); math_tag_close("mn"); }
macro void @mfrac(; @body()) { math_tag_open("mfrac"); @body(); math_tag_close("mfrac"); }
macro void @msqrt(; @body()) { math_tag_open("msqrt"); @body(); math_tag_close("msqrt"); }
macro void @msup(; @body())  { math_tag_open("msup"); @body(); math_tag_close("msup"); }
macro void @msub(; @body())  { math_tag_open("msub"); @body(); math_tag_close("msub"); }
