module mizar::server::session;
import mizar::server::http;
import std::hash::sha256;

struct SessionManager {
    String secret_key;
}

fn void SessionManager.init(&this, String secret_key = "mizar-default-secret-change-me") {
    this.secret_key = secret_key;
}

fn void hmac_hex(String key, String msg, char* out_hex64) {
    sha256::HmacSha256 ctx;
    ctx.init((char[])key);
    ctx.update((char[])msg);
    char[32] digest = ctx.final();

    char[16] hex_chars = "0123456789abcdef";
    for (int i = 0; i < 32; i++) {
        uint byte_val = (uint)digest[i] & 0xFF;
        out_hex64[i * 2]     = hex_chars[(int)((byte_val >> 4) & 0x0F)];
        out_hex64[i * 2 + 1] = hex_chars[(int)(byte_val & 0x0F)];
    }
    out_hex64[64] = '\0';
}

// Sign a session value into "value.signature"
fn String SessionManager.sign(&this, http::Request* req, String value) {
    char[65] sig;
    hmac_hex(this.secret_key, value, &sig[0]);
    String hex_sig = (String)sig[0..63];

    char[256] buf;
    int n = 0;
    for (int i = 0; i < value.len && n < 254; i++) buf[n++] = value[i];
    buf[n++] = '.';
    for (int i = 0; i < hex_sig.len && n < 255; i++) buf[n++] = hex_sig[i];
    buf[n] = '\0';

    return req.arena.strdup((String)buf[0..n - 1]);
}

// Verify and extract signed session value (returns empty string if tampered)
fn String SessionManager.verify(&this, String signed_val) {
    if (signed_val.len < 66) return ""; // Minimum: 1 char + '.' + 64 hex sig

    int dot = -1;
    for (int i = signed_val.len - 65; i >= 0; i--) {
        if (signed_val[i] == '.') {
            dot = i;
            break;
        }
    }
    if (dot == -1) return "";

    String raw_val = signed_val[0..dot - 1];
    String given_sig = signed_val[dot + 1..signed_val.len - 1];

    char[65] expected_sig;
    hmac_hex(this.secret_key, raw_val, &expected_sig[0]);
    String exp_str = (String)expected_sig[0..63];

    if (given_sig == exp_str) {
        return raw_val;
    }
    return "";
}

// Helper to set signed session cookie
fn void set_session_cookie(http::Response* res, String name, String signed_value) {
    char[512] c_buf;
    int n = 0;
    String prefix = name;
    for (int i = 0; i < prefix.len; i++) c_buf[n++] = prefix[i];
    c_buf[n++] = '=';
    for (int i = 0; i < signed_value.len; i++) c_buf[n++] = signed_value[i];
    String suffix = "; Path=/; HttpOnly; SameSite=Lax";
    for (int i = 0; i < suffix.len; i++) c_buf[n++] = suffix[i];
    c_buf[n] = '\0';

    res.header("Set-Cookie", (String)c_buf[0..n - 1]);
}
