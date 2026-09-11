module mizar::server::flash;
import mizar::server::http;
import mizar::server::session;
import mizar::view::html;

struct FlashMessage {
    String category; // "success", "error", "info", "warning"
    String message;
}

fn void set_flash(http::Response* res, session::SessionManager* sm, http::Request* req, String category, String message) {
    char[512] raw;
    int n = 0;
    for (int i = 0; i < category.len && n < 500; i++) raw[n++] = category[i];
    raw[n++] = ':';
    for (int i = 0; i < message.len && n < 500; i++) raw[n++] = message[i];
    raw[n] = '\0';

    String signed_flash = sm.sign(req, (String)raw[0..n - 1]);

    char[1024] c_buf;
    int cn = 0;
    String prefix = "mizar_flash=";
    for (int i = 0; i < prefix.len; i++) c_buf[cn++] = prefix[i];
    for (int i = 0; i < signed_flash.len; i++) c_buf[cn++] = signed_flash[i];
    String suffix = "; Path=/; HttpOnly; SameSite=Lax";
    for (int i = 0; i < suffix.len; i++) c_buf[cn++] = suffix[i];
    c_buf[cn] = '\0';

    res.header("Set-Cookie", (String)c_buf[0..cn - 1]);
}

fn FlashMessage get_flash(http::Request* req, http::Response* res, session::SessionManager* sm) {
    FlashMessage empty = { .category = "", .message = "" };
    String raw_cookie = req.cookie("mizar_flash");
    if (raw_cookie.len == 0) return empty;

    String verified = sm.verify(raw_cookie);
    if (verified.len == 0) return empty;

    // Immediately expire cookie on read
    if (res != null) {
        res.header("Set-Cookie", "mizar_flash=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax");
    }

    int colon = -1;
    for (int i = 0; i < verified.len; i++) {
        if (verified[i] == ':') {
            colon = i;
            break;
        }
    }
    if (colon == -1) return empty;

    return {
        .category = verified[0..colon - 1],
        .message = verified[colon + 1..verified.len - 1]
    };
}

macro void @flash_container(http::Request* req, http::Response* res, session::SessionManager* sm) {
    FlashMessage fl = get_flash(req, res, sm);
    if (fl.message.len > 0) {
        String border = "#38bdf8";
        String bg = "#f0f9ff";
        if (fl.category == "success") {
            border = "#4ade80";
            bg = "#f0fdf4";
        } else if (fl.category == "error") {
            border = "#f87171";
            bg = "#fef2f2";
        } else if (fl.category == "warning") {
            border = "#facc15";
            bg = "#fefce8";
        }

        char[256] sbuf;
        int sn = 0;
        String s1 = "padding: 12px 16px; border-radius: 6px; margin-bottom: 16px; border-left: 4px solid ";
        for (int i = 0; i < s1.len; i++) sbuf[sn++] = s1[i];
        for (int i = 0; i < border.len; i++) sbuf[sn++] = border[i];
        String s2 = "; background: ";
        for (int i = 0; i < s2.len; i++) sbuf[sn++] = s2[i];
        for (int i = 0; i < bg.len; i++) sbuf[sn++] = bg[i];
        sbuf[sn++] = ';';
        sbuf[sn] = '\0';

        html::@div({ .style = (String)sbuf[0..sn - 1] }) {
            html::text(fl.message);
        };
    }
}
