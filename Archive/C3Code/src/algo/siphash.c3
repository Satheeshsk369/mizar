module mizar::algo::siphash;

struct SipKey {
    ulong k0;
    ulong k1;
}

SipKey s_global_sip_key;
bool s_sip_key_initialized = false;

fn ulong rotl64(ulong x, uint b) @inline {
    return (x << b) | (x >> (64 - b));
}

macro void @sipround(ulong #v0, ulong #v1, ulong #v2, ulong #v3) {
    #v0 += #v1;
    #v1 = rotl64(#v1, 13);
    #v1 ^= #v0;
    #v0 = rotl64(#v0, 32);

    #v2 += #v3;
    #v3 = rotl64(#v3, 16);
    #v3 ^= #v2;

    #v0 += #v3;
    #v3 = rotl64(#v3, 21);
    #v3 ^= #v0;

    #v2 += #v1;
    #v1 = rotl64(#v1, 17);
    #v1 ^= #v2;
    #v2 = rotl64(#v2, 32);
}

fn SipKey* get_global_key() {
    if (!s_sip_key_initialized) {
        s_global_sip_key.k0 = 0x0706050403020100;
        s_global_sip_key.k1 = 0x0f0e0d0c0b0a0908;
        s_sip_key_initialized = true;
    }
    return &s_global_sip_key;
}

fn ulong read_u64_le(char* p) @inline {
    return ((ulong)(char)p[0]) |
           (((ulong)(char)p[1]) << 8) |
           (((ulong)(char)p[2]) << 16) |
           (((ulong)(char)p[3]) << 24) |
           (((ulong)(char)p[4]) << 32) |
           (((ulong)(char)p[5]) << 40) |
           (((ulong)(char)p[6]) << 48) |
           (((ulong)(char)p[7]) << 56);
}

fn ulong read_u64_le_ci(char* p) @inline {
    ulong v = 0;
    for (uint i = 0; i < 8; i++) {
        char c = p[i];
        if (c >= 'A' && c <= 'Z') c += 32;
        v |= (((ulong)c) << (i * 8));
    }
    return v;
}

fn ulong hash(String data, SipKey* key = null) {
    if (key == null) key = get_global_key();

    ulong v0 = 0x736f6d6570736575 ^ key.k0;
    ulong v1 = 0x646f72616e646f6d ^ key.k1;
    ulong v2 = 0x6c65747371756174 ^ key.k0;
    ulong v3 = 0x7465646279746573 ^ key.k1;

    char* p = data.ptr;
    usz left = (usz)data.len;

    while (left >= 8) {
        ulong m = read_u64_le(p);
        v3 ^= m;
        @sipround(v0, v1, v2, v3);
        v0 ^= m;
        p += 8;
        left -= 8;
    }

    ulong b = ((ulong)data.len) << 56;
    switch (left) {
        case 7: b |= ((ulong)(char)p[6]) << 48; nextcase;
        case 6: b |= ((ulong)(char)p[5]) << 40; nextcase;
        case 5: b |= ((ulong)(char)p[4]) << 32; nextcase;
        case 4: b |= ((ulong)(char)p[3]) << 24; nextcase;
        case 3: b |= ((ulong)(char)p[2]) << 16; nextcase;
        case 2: b |= ((ulong)(char)p[1]) << 8;  nextcase;
        case 1: b |= ((ulong)(char)p[0]);
        case 0: break;
    }

    v3 ^= b;
    @sipround(v0, v1, v2, v3);
    v0 ^= b;

    v2 ^= 0xff;
    @sipround(v0, v1, v2, v3);
    @sipround(v0, v1, v2, v3);
    @sipround(v0, v1, v2, v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

fn ulong hash_ci(String data, SipKey* key = null) {
    if (key == null) key = get_global_key();

    ulong v0 = 0x736f6d6570736575 ^ key.k0;
    ulong v1 = 0x646f72616e646f6d ^ key.k1;
    ulong v2 = 0x6c65747371756174 ^ key.k0;
    ulong v3 = 0x7465646279746573 ^ key.k1;

    char* p = data.ptr;
    usz left = (usz)data.len;

    while (left >= 8) {
        ulong m = read_u64_le_ci(p);
        v3 ^= m;
        @sipround(v0, v1, v2, v3);
        v0 ^= m;
        p += 8;
        left -= 8;
    }

    ulong b = ((ulong)data.len) << 56;
    for (uint i = 0; i < left; i++) {
        char c = p[i];
        if (c >= 'A' && c <= 'Z') c += 32;
        b |= (((ulong)c) << (i * 8));
    }

    v3 ^= b;
    @sipround(v0, v1, v2, v3);
    v0 ^= b;

    v2 ^= 0xff;
    @sipround(v0, v1, v2, v3);
    @sipround(v0, v1, v2, v3);
    @sipround(v0, v1, v2, v3);

    return v0 ^ v1 ^ v2 ^ v3;
}
