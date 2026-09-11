module mizar::ui::icons;
import mizar::core::buffer;

struct IconProps {
    int size;                // px, default 24
    String color;            // default "currentColor"
    String stroke_width;     // default "2"
    String class;
    String style;
}

fn void render_svg_open(IconProps p) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;

    int icon_size = p.size > 0 ? p.size : 24;
    String col = p.color.len > 0 ? p.color : "currentColor";
    String sw = p.stroke_width.len > 0 ? p.stroke_width : "2";

    buf.append_str("<svg width=\"");
    char[16] sz_str;
    int n = 0;
    int temp = icon_size;
    char[16] rev;
    int rlen = 0;
    while (temp > 0) {
        rev[rlen++] = (char)('0' + (temp % 10));
        temp /= 10;
    }
    for (int i = rlen - 1; i >= 0; i--) {
        sz_str[n++] = rev[i];
    }
    sz_str[n] = '\0';
    buf.append_str((String)sz_str[0..n - 1]);

    buf.append_str("\" height=\"");
    buf.append_str((String)sz_str[0..n - 1]);
    buf.append_str("\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"");
    buf.append_str(col);
    buf.append_str("\" stroke-width=\"");
    buf.append_str(sw);
    buf.append_str("\" stroke-linecap=\"round\" stroke-linejoin=\"round\"");

    if (p.class.len > 0) {
        buf.append_str(" class=\"");
        buf.append_str(p.class);
        buf.append_str("\"");
    }
    if (p.style.len > 0) {
        buf.append_str(" style=\"");
        buf.append_str(p.style);
        buf.append_str("\"");
    }
    buf.append_char('>');
}

fn void render_path(String d) {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("<path d=\"");
    buf.append_str(d);
    buf.append_str("\" />");
}

fn void render_svg_close() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str("</svg>");
}

// -----------------------------------------------------------------------------
// Complete 28 SVG Icons Suite
// -----------------------------------------------------------------------------

// 1. Navigation & Directional
macro void @icon_chevron_right(IconProps p = {}) { render_svg_open(p); render_path("m9 18 6-6-6-6"); render_svg_close(); }
macro void @icon_chevron_left(IconProps p = {})  { render_svg_open(p); render_path("m15 18-6-6 6-6"); render_svg_close(); }
macro void @icon_chevron_up(IconProps p = {})    { render_svg_open(p); render_path("m18 15-6-6-6 6"); render_svg_close(); }
macro void @icon_chevron_down(IconProps p = {})  { render_svg_open(p); render_path("m6 9 6 6 6-6"); render_svg_close(); }
macro void @icon_arrow_right(IconProps p = {})   { render_svg_open(p); render_path("M5 12h14m-7-7 7 7-7 7"); render_svg_close(); }
macro void @icon_arrow_left(IconProps p = {})    { render_svg_open(p); render_path("m12 19-7-7 7-7m7 7H5"); render_svg_close(); }

// 2. Common Actions & Controls
macro void @icon_search(IconProps p = {}) { render_svg_open(p); render_path("m21 21-4.3-4.3M11 19a8 8 0 1 0 0-16 8 8 0 0 0 0 16z"); render_svg_close(); }
macro void @icon_check(IconProps p = {})  { render_svg_open(p); render_path("M20 6 9 17l-5-5"); render_svg_close(); }
macro void @icon_close(IconProps p = {})  { render_svg_open(p); render_path("M18 6 6 18M6 6l12 12"); render_svg_close(); }
macro void @icon_plus(IconProps p = {})   { render_svg_open(p); render_path("M12 5v14m-7-7h14"); render_svg_close(); }
macro void @icon_minus(IconProps p = {})  { render_svg_open(p); render_path("M5 12h14"); render_svg_close(); }
macro void @icon_trash(IconProps p = {})  { render_svg_open(p); render_path("M3 6h18m-2 0v14c0 1-1 2-2 2H7c-1 0-2-1-2-2V6m3 0V4c0-1 1-2 2-2h4c1 0 2 1 2 2v2"); render_svg_close(); }
macro void @icon_edit(IconProps p = {})   { render_svg_open(p); render_path("M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"); render_path("M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"); render_svg_close(); }
macro void @icon_refresh(IconProps p = {}){ render_svg_open(p); render_path("M21 12a9 9 0 0 0-9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"); render_path("M3 3v5h5"); render_svg_close(); }

// 3. User & Social
macro void @icon_user(IconProps p = {})   { render_svg_open(p); render_path("M19 21v-2a4 4 0 0 0-4-4H9a4 4 0 0 0-4 4v2"); render_path("M12 7a4 4 0 1 0 0-8 4 4 0 0 0 0 8z"); render_svg_close(); }
macro void @icon_users(IconProps p = {})  { render_svg_open(p); render_path("M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2"); render_path("M9 7a4 4 0 1 0 0-8 4 4 0 0 0 0 8z"); render_svg_close(); }
macro void @icon_mail(IconProps p = {})   { render_svg_open(p); render_path("M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2z"); render_path("m22 6-10 7L2 6"); render_svg_close(); }
macro void @icon_lock(IconProps p = {})   { render_svg_open(p); render_path("M7 11V7a5 5 0 0 1 10 0v4"); render_path("M5 11h14a2 2 0 0 1 2 2v7a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-7a2 2 0 0 1 2-2z"); render_svg_close(); }

// 4. System & Notifications
macro void @icon_info(IconProps p = {})   { render_svg_open(p); render_path("M12 16v-4m0-4h.01M22 12a10 10 0 1 1-20 0 10 10 0 0 1 20 0z"); render_svg_close(); }
macro void @icon_alert(IconProps p = {})  { render_svg_open(p); render_path("m21.73 18-8-14a2 2 0 0 0-3.48 0l-8 14A2 2 0 0 0 4 21h16a2 2 0 0 0 1.73-3zM12 9v4m0 4h.01"); render_svg_close(); }
macro void @icon_bell(IconProps p = {})   { render_svg_open(p); render_path("M6 8a6 6 0 0 1 12 0c0 7 3 9 3 9H3s3-2 3-9m7.3 13a2 2 0 0 1-3.6 0"); render_svg_close(); }
macro void @icon_settings(IconProps p = {}){render_svg_open(p); render_path("M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z"); render_svg_close(); }

// 5. Files & Data
macro void @icon_file(IconProps p = {})   { render_svg_open(p); render_path("M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"); render_path("M14 2v6h6"); render_svg_close(); }
macro void @icon_folder(IconProps p = {}) { render_svg_open(p); render_path("M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"); render_svg_close(); }
macro void @icon_database(IconProps p = {}){render_svg_open(p); render_path("M12 3c-4.97 0-9 1.34-9 3v12c0 1.66 4.03 3 9 3s9-1.34 9-3V6c0-1.66-4.03-3-9-3z"); render_path("M3 12c0 1.66 4.03 3 9 3s9-1.34 9-3"); render_svg_close(); }
macro void @icon_download(IconProps p = {}){render_svg_open(p); render_path("M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4m4-5 5 5 5-5m-5 5V3"); render_svg_close(); }
macro void @icon_upload(IconProps p = {})  {render_svg_open(p); render_path("M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4m14-7-5-5-5 5m5-5v12"); render_svg_close(); }
macro void @icon_copy(IconProps p = {})    {render_svg_open(p); render_path("M8 4h10a2 2 0 0 1 2 2v10m-4 4H4a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2v10a2 2 0 0 1-2 2z"); render_svg_close(); }
