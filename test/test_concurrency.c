#define _DEFAULT_SOURCE
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mizar.h"

#define TEST_PORT 9876

static void handle_bench_ping(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)req;
    (void)user_data;
    mz_res_status(res, 200, "OK");
    mz_res_html(res);
    mz_buf_append_str(&res->body, "pong");
}

static void *run_server(void *arg) {
    MzApp *app = (MzApp *)arg;
    mz_app_listen(app, TEST_PORT);
    return nullptr;
}

int main(void) {
    printf("Testing Mizar High-Concurrency epoll + SO_REUSEPORT Server...\n");

    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 2);
    mz_app_get(&app, "/ping", handle_bench_ping, nullptr);

    pthread_t server_tid;
    pthread_create(&server_tid, nullptr, run_server, &app);

    // Wait 100ms for workers to bind
    usleep(100000);

    // Test multiple concurrent keep-alive requests on the epoll worker
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(client_sock >= 0);

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(TEST_PORT);
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);

    assert(connect(client_sock, (struct sockaddr *)&saddr, sizeof(saddr)) == 0);

    // Request 1 over persistent Keep-Alive
    const char *req1 = "GET /ping HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
    send(client_sock, req1, strlen(req1), 0);

    char resp1[1024];
    ssize_t n1 = recv(client_sock, resp1, sizeof(resp1) - 1, 0);
    assert(n1 > 0);
    resp1[n1] = '\0';
    assert(strstr(resp1, "HTTP/1.1 200 OK") != NULL);
    assert(strstr(resp1, "Connection: keep-alive") != NULL);
    assert(strstr(resp1, "pong") != NULL);

    // Request 2 on the EXACT SAME TCP socket (Edge-triggered oneshot re-arm verification)
    const char *req2 = "GET /ping HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(client_sock, req2, strlen(req2), 0);

    char resp2[1024];
    ssize_t n2 = recv(client_sock, resp2, sizeof(resp2) - 1, 0);
    assert(n2 > 0);
    resp2[n2] = '\0';
    assert(strstr(resp2, "HTTP/1.1 200 OK") != NULL);
    assert(strstr(resp2, "Connection: close") != NULL);
    assert(strstr(resp2, "pong") != NULL);

    close(client_sock);

    // Clean cancellation of server thread for graceful test exit
    pthread_cancel(server_tid);
    pthread_join(server_tid, nullptr);

    mz_app_free(&app);
    printf(">>> ALL EPOLL & REUSEPORT CONCURRENCY TESTS PASSED! <<<\n");
    return 0;
}
