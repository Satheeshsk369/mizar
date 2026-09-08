#define _DEFAULT_SOURCE
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "mizar.h"

#define FUZZ_PORT 9879
#define NUM_CLIENTS 10

static void handle_echo(const MzRequest *req, MzResponse *res, void *user_data) {
    (void)user_data;
    mz_res_status(res, 200, "OK");
    if (req->body && req->body_len > 0) {
        mz_buf_append(&res->body, req->body, req->body_len);
    } else {
        mz_buf_append_str(&res->body, "empty");
    }
}

static void *run_fuzz_server(void *arg) {
    MzApp *app = (MzApp *)arg;
    mz_app_listen(app, FUZZ_PORT);
    return nullptr;
}

static void *slow_client_worker(void *arg) {
    int id = (int)(intptr_t)arg;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(FUZZ_PORT);
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);

    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) {
        close(sock);
        return nullptr;
    }

    // Send headers piece-by-piece to simulate network fragmentation & Slowloris
    const char *p1 = "POST /echo HTTP/1.1\r\nHost: localhost\r\n";
    send(sock, p1, strlen(p1), 0);
    usleep(10000); // 10ms delay

    char p2[128];
    snprintf(p2, sizeof(p2), "Content-Length: 12\r\nConnection: close\r\n\r\n");
    send(sock, p2, strlen(p2), 0);
    usleep(10000); // 10ms delay

    // Send payload byte-by-byte
    char body[32];
    snprintf(body, sizeof(body), "client-%04d!", id);
    for (size_t i = 0; i < strlen(body); i++) {
        send(sock, &body[i], 1, 0);
        usleep(1000);
    }

    char resp[1024];
    ssize_t n = recv(sock, resp, sizeof(resp) - 1, 0);
    assert(n > 0);
    resp[n] = '\0';
    assert(strstr(resp, "200 OK") != NULL);
    assert(strstr(resp, body) != NULL);

    close(sock);
    return nullptr;
}

int main(void) {
    MzApp app;
    mz_app_init(&app);
    mz_app_set_workers(&app, 4);
    mz_app_post(&app, "/echo", handle_echo);

    pthread_t s_tid;
    pthread_create(&s_tid, nullptr, run_fuzz_server, &app);
    usleep(50000); // 50ms wait for server startup

    pthread_t clients[NUM_CLIENTS];
    for (intptr_t i = 0; i < NUM_CLIENTS; i++) {
        pthread_create(&clients[i], nullptr, slow_client_worker, (void *)i);
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], nullptr);
    }

    pthread_cancel(s_tid);
    pthread_join(s_tid, nullptr);

    mz_app_free(&app);
    printf("Multi-client slowloris & TCP fragmentation fuzz test passed cleanly!\n");
    return 0;
}
