#define _DEFAULT_SOURCE
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mizar.h"

#define TEST_PORT 9876

static void handle_ping(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req;
    (void)user_data;
    mz_res_status(res, 200, "OK");
    mz_buf_append_str(&res->body, "pong");
}

static void *run_server(void *arg) {
    MzApp *app = (MzApp *)arg;
    mz_app_listen(app, TEST_PORT);
    return nullptr;
}

int main(void) {
    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 2);
    mz_app_get(&app, "/ping", handle_ping);

    pthread_t tid;
    pthread_create(&tid, nullptr, run_server, &app);
    usleep(50000); // 50ms wait for bind

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(TEST_PORT);
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);

    assert(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == 0);

    // Keep-alive request 1
    const char *req1 = "GET /ping HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
    send(sock, req1, strlen(req1), 0);

    char resp[1024];
    ssize_t n = recv(sock, resp, sizeof(resp) - 1, 0);
    assert(n > 0);
    resp[n] = '\0';
    assert(strstr(resp, "200 OK") != NULL);
    assert(strstr(resp, "pong") != NULL);

    // Keep-alive request 2 on the same socket
    const char *req2 = "GET /ping HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(sock, req2, strlen(req2), 0);

    n = recv(sock, resp, sizeof(resp) - 1, 0);
    assert(n > 0);
    resp[n] = '\0';
    assert(strstr(resp, "200 OK") != NULL);
    assert(strstr(resp, "pong") != NULL);

    close(sock);
    pthread_cancel(tid);
    pthread_join(tid, nullptr);

    mz_app_free(&app);
    return 0;
}
