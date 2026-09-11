module mizar::view::html;
import mizar::core::buffer;

struct Attrs {
    String id;
    String class;
    String style;
    String href;
    String src;
    String type;
    String name;
    String value;
    String placeholder;
    String action;
    String method;
    String rel;
    String target;
    String rows;
    String cols;
    String selected;
    String checked;
    String disabled;
    
    // HTMX Attributes
    String hx_get;
    String hx_post;
    String hx_put;
    String hx_delete;
    String hx_patch;
    String hx_target;
    String hx_swap;
    String hx_trigger;
}

fn void render_attr(buffer::Buffer* buf, String name, String val) {
    if (val.len == 0) return;
    buf.append_char(' ');
    buf.append_str(name);
    buf.append_str("=\"");
    buf.append_escaped(val);
    buf.append_char('"');
}

fn void render_attrs(buffer::Buffer* buf, Attrs a) {
    render_attr(buf, "id", a.id);
    render_attr(buf, "class", a.class);
    render_attr(buf, "style", a.style);
    render_attr(buf, "href", a.href);
    render_attr(buf, "src", a.src);
    render_attr(buf, "type", a.type);
    render_attr(buf, "name", a.name);
    render_attr(buf, "value", a.value);
    render_attr(buf, "placeholder", a.placeholder);
    render_attr(buf, "action", a.action);
    render_attr(buf, "method", a.method);
    render_attr(buf, "rel", a.rel);
    render_attr(buf, "target", a.target);
    render_attr(buf, "rows", a.rows);
    render_attr(buf, "cols", a.cols);
    render_attr(buf, "selected", a.selected);
    render_attr(buf, "checked", a.checked);
    render_attr(buf, "disabled", a.disabled);

    render_attr(buf, "hx-get", a.hx_get);
    render_attr(buf, "hx-post", a.hx_post);
    render_attr(buf, "hx-put", a.hx_put);
    render_attr(buf, "hx-delete", a.hx_delete);
    render_attr(buf, "hx-patch", a.hx_patch);
    render_attr(buf, "hx-target", a.hx_target);
    render_attr(buf, "hx-swap", a.hx_swap);
    render_attr(buf, "hx-trigger", a.hx_trigger);
}

fn void tag_open(String name, Attrs attrs) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('<');
    buf.append_str(name);
    render_attrs(buf, attrs);
    buf.append_char('>');
}

fn void tag_close(String name) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("</");
    buf.append_str(name);
    buf.append_char('>');
}

fn void void_tag(String name, Attrs attrs) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_char('<');
    buf.append_str(name);
    render_attrs(buf, attrs);
    buf.append_str(" />");
}

// -----------------------------------------------------------------------------
// Tag Macros
// -----------------------------------------------------------------------------
macro void @tag(String name, Attrs attrs; @body()) {
    tag_open(name, attrs);
    @body();
    tag_close(name);
}

// Structural
macro void @div(Attrs attrs = {}; @body())     { tag_open("div", attrs); @body(); tag_close("div"); }
macro void @span(Attrs attrs = {}; @body())    { tag_open("span", attrs); @body(); tag_close("span"); }
macro void @header(Attrs attrs = {}; @body())  { tag_open("header", attrs); @body(); tag_close("header"); }
macro void @footer(Attrs attrs = {}; @body())  { tag_open("footer", attrs); @body(); tag_close("footer"); }
macro void @nav(Attrs attrs = {}; @body())     { tag_open("nav", attrs); @body(); tag_close("nav"); }
macro void @section(Attrs attrs = {}; @body()) { tag_open("section", attrs); @body(); tag_close("section"); }
macro void @article(Attrs attrs = {}; @body()) { tag_open("article", attrs); @body(); tag_close("article"); }
macro void @aside(Attrs attrs = {}; @body())   { tag_open("aside", attrs); @body(); tag_close("aside"); }
macro void @main(Attrs attrs = {}; @body())    { tag_open("main", attrs); @body(); tag_close("main"); }

// Typography & Text
macro void @h1(Attrs attrs = {}; @body()) { tag_open("h1", attrs); @body(); tag_close("h1"); }
macro void @h2(Attrs attrs = {}; @body()) { tag_open("h2", attrs); @body(); tag_close("h2"); }
macro void @h3(Attrs attrs = {}; @body()) { tag_open("h3", attrs); @body(); tag_close("h3"); }
macro void @h4(Attrs attrs = {}; @body()) { tag_open("h4", attrs); @body(); tag_close("h4"); }
macro void @h5(Attrs attrs = {}; @body()) { tag_open("h5", attrs); @body(); tag_close("h5"); }
macro void @h6(Attrs attrs = {}; @body()) { tag_open("h6", attrs); @body(); tag_close("h6"); }
macro void @p(Attrs attrs = {}; @body())  { tag_open("p", attrs); @body(); tag_close("p"); }
macro void @a(Attrs attrs = {}; @body())  { tag_open("a", attrs); @body(); tag_close("a"); }
macro void @b(Attrs attrs = {}; @body())  { tag_open("b", attrs); @body(); tag_close("b"); }
macro void @strong(Attrs attrs = {}; @body()) { tag_open("strong", attrs); @body(); tag_close("strong"); }
macro void @i(Attrs attrs = {}; @body())  { tag_open("i", attrs); @body(); tag_close("i"); }
macro void @em(Attrs attrs = {}; @body()) { tag_open("em", attrs); @body(); tag_close("em"); }
macro void @small(Attrs attrs = {}; @body()) { tag_open("small", attrs); @body(); tag_close("small"); }
macro void @code(Attrs attrs = {}; @body())  { tag_open("code", attrs); @body(); tag_close("code"); }
macro void @pre(Attrs attrs = {}; @body())   { tag_open("pre", attrs); @body(); tag_close("pre"); }
macro void @blockquote(Attrs attrs = {}; @body()) { tag_open("blockquote", attrs); @body(); tag_close("blockquote"); }

// Lists
macro void @ul(Attrs attrs = {}; @body()) { tag_open("ul", attrs); @body(); tag_close("ul"); }
macro void @ol(Attrs attrs = {}; @body()) { tag_open("ol", attrs); @body(); tag_close("ol"); }
macro void @li(Attrs attrs = {}; @body()) { tag_open("li", attrs); @body(); tag_close("li"); }

// Tables
macro void @table(Attrs attrs = {}; @body()) { tag_open("table", attrs); @body(); tag_close("table"); }
macro void @thead(Attrs attrs = {}; @body()) { tag_open("thead", attrs); @body(); tag_close("thead"); }
macro void @tbody(Attrs attrs = {}; @body()) { tag_open("tbody", attrs); @body(); tag_close("tbody"); }
macro void @tr(Attrs attrs = {}; @body())    { tag_open("tr", attrs); @body(); tag_close("tr"); }
macro void @th(Attrs attrs = {}; @body())    { tag_open("th", attrs); @body(); tag_close("th"); }
macro void @td(Attrs attrs = {}; @body())    { tag_open("td", attrs); @body(); tag_close("td"); }

// Forms
macro void @form(Attrs attrs = {}; @body())     { tag_open("form", attrs); @body(); tag_close("form"); }
macro void @label(Attrs attrs = {}; @body())    { tag_open("label", attrs); @body(); tag_close("label"); }
macro void @select(Attrs attrs = {}; @body())   { tag_open("select", attrs); @body(); tag_close("select"); }
macro void @option(Attrs attrs = {}; @body())   { tag_open("option", attrs); @body(); tag_close("option"); }
macro void @textarea(Attrs attrs = {}; @body()) { tag_open("textarea", attrs); @body(); tag_close("textarea"); }
macro void @button(Attrs attrs = {}; @body())   { tag_open("button", attrs); @body(); tag_close("button"); }

// Void Tags
macro void @input(Attrs attrs = {}) { void_tag("input", attrs); }
macro void @img(Attrs attrs = {})   { void_tag("img", attrs); }
macro void @br()                    { void_tag("br", {}); }
macro void @hr(Attrs attrs = {})    { void_tag("hr", attrs); }

fn void text(String str) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_escaped(str);
}

fn void raw(String str) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str(str);
}
