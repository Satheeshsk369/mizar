#define _DEFAULT_SOURCE
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "algo/arena.h"
#include "algo/strview.h"
#include "algo/hashmap.h"
#include "algo/vec.h"
#include "algo/siphash.h"
#include "algo/urlcodec.h"
#include "algo/ringbuf.h"
#include "algo/utf8.h"
#include "algo/lru.h"

static void test_siphash(void) {
    const char *msg = "GET /api/v1/users HTTP/1.1";
    uint64_t h1 = mz_siphash13(msg, strlen(msg), nullptr);
    uint64_t h2 = mz_siphash13(msg, strlen(msg), nullptr);
    assert(h1 == h2);

    uint64_t h_upper = mz_siphash13_ci("Content-Type", 12, nullptr);
    uint64_t h_lower = mz_siphash13_ci("content-type", 12, nullptr);
    assert(h_upper == h_lower);
    printf("  ✓ SipHash-1-3 collision resistance tests passed\n");
}

static void test_urlcodec(void) {
    const char *raw = "hello world & foo=bar/100%";
    char *enc = mz_urlcodec_encode(raw, strlen(raw), nullptr);
    assert(enc != nullptr);

    char *dec = mz_urlcodec_decode(enc, strlen(enc), true, nullptr);
    assert(dec != nullptr);
    assert(strcmp(raw, dec) == 0);

    free(enc);
    free(dec);
    printf("  ✓ Fast URL Codec tests passed\n");
}

static void test_ringbuf(void) {
    MzRingBuf rb;
    assert(mz_ringbuf_init(&rb, 64));

    const char *chunk1 = "HTTP/1.1 200 OK\r\n";
    size_t w1 = mz_ringbuf_write(&rb, chunk1, strlen(chunk1));
    assert(w1 == strlen(chunk1));

    char buf[128];
    size_t r1 = mz_ringbuf_read(&rb, buf, w1);
    assert(r1 == w1);
    buf[r1] = '\0';
    assert(strcmp(buf, chunk1) == 0);

    mz_ringbuf_free(&rb);
    printf("  ✓ Ring Buffer tests passed\n");
}

static void test_utf8(void) {
    const char *valid = "Hello 世界 🚀 Mizar!";
    assert(mz_utf8_validate(valid, strlen(valid)));
    assert(mz_utf8_strlen(valid, strlen(valid)) == 17);

    const char invalid[] = { (char)0xF0, (char)0x28, (char)0x8C, (char)0x28 };
    assert(!mz_utf8_validate(invalid, sizeof(invalid)));
    printf("  ✓ UTF-8 Validator and Code Point tests passed\n");
}

static void test_lru(void) {
    MzLruCache cache;
    assert(mz_lru_init(&cache, 3, nullptr));

    assert(mz_lru_put(&cache, "a", (void *)1));
    assert(mz_lru_put(&cache, "b", (void *)2));
    assert(mz_lru_put(&cache, "c", (void *)3));

    // Access 'a' to promote to MRU
    assert((intptr_t)mz_lru_get(&cache, "a") == 1);

    // Insert 'd', should evict 'b' (LRU)
    assert(mz_lru_put(&cache, "d", (void *)4));
    assert(mz_lru_get(&cache, "b") == nullptr);
    assert((intptr_t)mz_lru_get(&cache, "a") == 1);
    assert((intptr_t)mz_lru_get(&cache, "c") == 3);
    assert((intptr_t)mz_lru_get(&cache, "d") == 4);

    mz_lru_free(&cache);
    printf("  ✓ LRU Cache tests passed\n");
}

static void test_arena(void) {
    MzArena arena;
    mz_arena_init(&arena, 1024);

    void *p1 = mz_arena_alloc(&arena, 64);
    assert(p1 != nullptr);
    memset(p1, 0xAA, 64);

    char *s1 = mz_arena_strdup(&arena, "Hello Mizar Arena");
    assert(s1 != nullptr);
    assert(strcmp(s1, "Hello Mizar Arena") == 0);

    // Test large allocation exceeding default block size
    void *large = mz_arena_alloc(&arena, 8192);
    assert(large != nullptr);
    memset(large, 0x55, 8192);

    mz_arena_reset(&arena);

    // Reuse after reset
    char *s2 = mz_arena_strdup(&arena, "After Reset");
    assert(strcmp(s2, "After Reset") == 0);

    mz_arena_free(&arena);
    printf("  ✓ Arena allocator tests passed\n");
}

static void test_strview(void) {
    MzStrView s1 = MZ_SV_LIT("Authorization: Bearer token123");
    assert(s1.len == strlen("Authorization: Bearer token123"));
    assert(mz_sv_starts_with(s1, MZ_SV_LIT("Authorization:")));
    assert(!mz_sv_starts_with(s1, MZ_SV_LIT("Content-Type:")));
    assert(mz_sv_ends_with(s1, MZ_SV_LIT("token123")));

    MzStrView left, right;
    bool split = mz_sv_split_char(s1, ':', &left, &right);
    assert(split);
    assert(mz_sv_eq(left, MZ_SV_LIT("Authorization")));
    
    right = mz_sv_trim(right);
    assert(mz_sv_eq(right, MZ_SV_LIT("Bearer token123")));

    // Case-insensitive comparison
    assert(mz_sv_eq_ci(MZ_SV_LIT("Content-Type"), MZ_SV_LIT("content-type")));
    assert(mz_sv_eq_ci(MZ_SV_LIT("HOST"), MZ_SV_LIT("host")));
    assert(!mz_sv_eq_ci(MZ_SV_LIT("Host"), MZ_SV_LIT("Post")));

    printf("  ✓ StrView tests passed\n");
}

static void test_hashmap(void) {
    MzHashMap map;
    mz_map_init(&map, 16, true); // Case-insensitive for HTTP headers

    assert(mz_map_set_cstr(&map, "Content-Type", (void *)"text/html"));
    assert(mz_map_set_cstr(&map, "Content-Length", (void *)"42"));
    assert(mz_map_set_cstr(&map, "Host", (void *)"localhost:3000"));

    // Case-insensitive lookup
    assert(strcmp((const char *)mz_map_get_cstr(&map, "content-type"), "text/html") == 0);
    assert(strcmp((const char *)mz_map_get_cstr(&map, "CONTENT-LENGTH"), "42") == 0);
    assert(strcmp((const char *)mz_map_get_cstr(&map, "host"), "localhost:3000") == 0);
    assert(mz_map_get_cstr(&map, "non-existent") == nullptr);

    // Overwrite value
    assert(mz_map_set_cstr(&map, "host", (void *)"example.com"));
    assert(strcmp((const char *)mz_map_get_cstr(&map, "HOST"), "example.com") == 0);

    // Remove
    assert(mz_map_remove(&map, mz_sv_from_cstr("content-length")));
    assert(mz_map_get_cstr(&map, "Content-Length") == nullptr);

    // Insertion stress test for resize
    char keys[100][16];
    for (int i = 0; i < 100; i++) {
        snprintf(keys[i], sizeof(keys[i]), "key-%d", i);
        assert(mz_map_set_cstr(&map, keys[i], (void *)(intptr_t)(i + 1000)));
    }
    for (int i = 0; i < 100; i++) {
        intptr_t val = (intptr_t)mz_map_get_cstr(&map, keys[i]);
        assert(val == i + 1000);
    }

    mz_map_free(&map);
    printf("  ✓ HashMap tests passed\n");
}

static void test_vec(void) {
    MzVec v;
    mz_vec_init(&v, 4);

    for (intptr_t i = 0; i < 20; i++) {
        mz_vec_push(&v, (void *)i);
    }
    assert(v.count == 20);
    for (intptr_t i = 0; i < 20; i++) {
        assert((intptr_t)mz_vec_get(&v, i) == i);
    }
    assert((intptr_t)mz_vec_pop(&v) == 19);
    assert(v.count == 19);

    mz_vec_free(&v);
    printf("  ✓ Vec tests passed\n");
}

int main(void) {
    printf("Testing Core Algorithm Primitives (src/algo/):\n");
    test_arena();
    test_strview();
    test_siphash();
    test_hashmap();
    test_vec();
    test_urlcodec();
    test_ringbuf();
    test_utf8();
    test_lru();
    printf("All Core Algorithm Primitives tested and passed cleanly!\n");
    return 0;
}
