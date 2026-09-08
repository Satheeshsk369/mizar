#define _DEFAULT_SOURCE
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mizar.h"

#define TEST_TLS_PORT 9443

#ifdef MIZAR_ENABLE_TLS
static void handle_hello_tls(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req; (void)user_data;
    mz_res_html(res);
    mz_buf_append_str(&res->body, "<h1>Hello from BearSSL HTTPS!</h1>");
}

static void *run_tls_server(void *arg) {
    MzApp *app = (MzApp *)arg;
    mz_app_listen_tls(app, TEST_TLS_PORT, "build/cert.pem", "build/key.pem");
    return nullptr;
}
#endif

int main(void) {
#ifdef MIZAR_ENABLE_TLS
    // Generate self-signed test cert & key
    int sys_res = system("openssl req -x509 -newkey rsa:2048 -keyout build/key.pem -out build/cert.pem -days 1 -nodes -subj \"/CN=localhost\" 2>/dev/null");
    assert(sys_res == 0);

    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 1);
    mz_app_get(&app, "/secure", handle_hello_tls);

    pthread_t tid;
    pthread_create(&tid, nullptr, run_tls_server, &app);
    usleep(80000); // 80ms wait for bind

    // Issue curl request verifying HTTPS response
    FILE *fp = popen("curl -k -s https://127.0.0.1:9443/secure", "r");
    assert(fp != NULL);
    char buf[512] = {0};
    size_t rd = fread(buf, 1, sizeof(buf) - 1, fp);
    pclose(fp);

    assert(rd > 0);
    assert(strstr(buf, "Hello from BearSSL HTTPS!") != NULL);

    pthread_cancel(tid);
    pthread_join(tid, nullptr);

    mz_app_free(&app);
    remove("build/key.pem");
    remove("build/cert.pem");
    printf("BearSSL End-to-End HTTPS request test passed cleanly!\n");
#else
    printf("Skipping HTTPS test (TLS not enabled in build)\n");
#endif
    return 0;
}
