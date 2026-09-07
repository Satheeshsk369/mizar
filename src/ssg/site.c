#include "ssg/site.h"
#include "ssg/fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

void mz_site_init(MizarSite *site, const char *out_dir) {
    if (!site) return;
    site->out_dir = out_dir ? strdup(out_dir) : strdup("dist");
    site->static_dir = nullptr;
    site->routes = nullptr;
    site->route_count = 0;
    site->route_capacity = 0;
}

void mz_site_free(MizarSite *site) {
    if (!site) return;
    free(site->out_dir);
    free(site->static_dir);
    for (size_t i = 0; i < site->route_count; i++) {
        free(site->routes[i].route);
    }
    free(site->routes);
    site->routes = nullptr;
    site->route_count = 0;
    site->route_capacity = 0;
}

void mz_site_set_static_dir(MizarSite *site, const char *static_dir) {
    if (!site) return;
    free(site->static_dir);
    site->static_dir = static_dir ? strdup(static_dir) : nullptr;
}

bool mz_site_add_page_impl(MizarSite *site, const char *route, MizarPageRenderFn render_fn, void *user_data) {
    if (!site || !route || !render_fn) return false;

    if (site->route_count >= site->route_capacity) {
        size_t new_cap = site->route_capacity ? site->route_capacity * 2 : 16;
        MizarRoute *new_routes = (MizarRoute *)realloc(site->routes, new_cap * sizeof(MizarRoute));
        if (!new_routes) return false;
        site->routes = new_routes;
        site->route_capacity = new_cap;
    }

    site->routes[site->route_count].route = strdup(route);
    site->routes[site->route_count].render_fn = render_fn;
    site->routes[site->route_count].user_data = user_data;
    site->route_count++;
    return true;
}

bool mz_site_build(MizarSite *site) {
    if (!site || !site->out_dir) return false;

    if (!mz_fs_mkdir_p(site->out_dir)) {
        fprintf(stderr, "Error: Could not create output directory '%s'\n", site->out_dir);
        return false;
    }

    // 1. Copy static assets if configured
    if (site->static_dir) {
        struct stat st;
        if (stat(site->static_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
            if (!mz_fs_copy_dir_recursive(site->static_dir, site->out_dir)) {
                fprintf(stderr, "Warning: Failed to copy static assets from '%s'\n", site->static_dir);
            }
        }
    }

    // 2. Render each route to disk
    for (size_t i = 0; i < site->route_count; i++) {
        MizarRoute *r = &site->routes[i];
        char target_filepath[1024];
        mz_fs_route_to_filepath(site->out_dir, r->route, target_filepath, sizeof(target_filepath));

        MizarBuffer buf;
        mz_buf_init(&buf, 4096);

        // Execute user's page render callback
        r->render_fn(&buf, r->user_data);

        // Write to file
        bool written = mz_fs_write_file(target_filepath, buf.data, buf.len);
        mz_buf_free(&buf);

        if (!written) {
            fprintf(stderr, "Error: Failed to write route '%s' to '%s'\n", r->route, target_filepath);
            return false;
        }
    }

    return true;
}

static const char *mz_mime_type_for_path(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0) return "text/html; charset=utf-8";
    if (strcmp(dot, ".css") == 0) return "text/css; charset=utf-8";
    if (strcmp(dot, ".js") == 0) return "application/javascript; charset=utf-8";
    if (strcmp(dot, ".json") == 0) return "application/json";
    if (strcmp(dot, ".svg") == 0) return "image/svg+xml";
    if (strcmp(dot, ".png") == 0) return "image/png";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(dot, ".gif") == 0) return "image/gif";
    if (strcmp(dot, ".ico") == 0) return "image/x-icon";
    if (strcmp(dot, ".txt") == 0) return "text/plain; charset=utf-8";
    if (strcmp(dot, ".xml") == 0) return "application/xml";
    return "application/octet-stream";
}

bool mz_site_serve(const char *dir, int port) {
    if (!dir) dir = "dist";
    if (port <= 0) port = 8080;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return false;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
        return false;
    }

    if (listen(server_fd, 10) < 0) {
        close(server_fd);
        return false;
    }

    printf("Mizar dev server running at http://localhost:%d/ (Serving: %s)\n", port, dir);

    while (1) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;

        char req_buf[2048];
        ssize_t bytes_read = recv(client_fd, req_buf, sizeof(req_buf) - 1, 0);
        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }
        req_buf[bytes_read] = '\0';

        char method[16], url_path[512];
        if (sscanf(req_buf, "%15s %511s", method, url_path) < 2) {
            close(client_fd);
            continue;
        }

        // Clean query params
        char *q = strchr(url_path, '?');
        if (q) *q = '\0';

        char file_path[1024];
        mz_fs_route_to_filepath(dir, url_path, file_path, sizeof(file_path));

        FILE *f = fopen(file_path, "rb");
        if (!f) {
            // Check if file_path directly exists
            snprintf(file_path, sizeof(file_path), "%s%s", dir, url_path);
            f = fopen(file_path, "rb");
        }

        if (!f) {
            const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\nConnection: close\r\n\r\nNot Found";
            send(client_fd, not_found, strlen(not_found), 0);
        } else {
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);

            const char *mime = mz_mime_type_for_path(file_path);
            char header[256];
            int hlen = snprintf(header, sizeof(header),
                                "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n",
                                mime, fsize);
            send(client_fd, header, hlen, 0);

            char send_buf[8192];
            size_t n;
            while ((n = fread(send_buf, 1, sizeof(send_buf), f)) > 0) {
                send(client_fd, send_buf, n, 0);
            }
            fclose(f);
        }
        close(client_fd);
    }

    close(server_fd);
    return true;
}
