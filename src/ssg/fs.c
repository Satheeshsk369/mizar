#include "ssg/fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

bool mz_fs_mkdir_p(const char *dir_path) {
    if (!dir_path || !*dir_path) return false;

    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", dir_path);
    size_t len = strlen(tmp);

    if (tmp[len - 1] == '/') tmp[len - 1] = '\0';

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                return false;
            }
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
        return false;
    }
    return true;
}

static bool mz_fs_ensure_parent_dir(const char *filepath) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", filepath);
    char *last_slash = strrchr(tmp, '/');
    if (last_slash) {
        *last_slash = '\0';
        return mz_fs_mkdir_p(tmp);
    }
    return true;
}

bool mz_fs_write_file(const char *filepath, const char *data, size_t len) {
    if (!filepath || !data) return false;
    if (!mz_fs_ensure_parent_dir(filepath)) return false;

    FILE *f = fopen(filepath, "wb");
    if (!f) return false;

    size_t written = fwrite(data, 1, len, f);
    fclose(f);
    return written == len;
}

bool mz_fs_copy_file(const char *src_path, const char *dst_path) {
    if (!src_path || !dst_path) return false;
    if (!mz_fs_ensure_parent_dir(dst_path)) return false;

    FILE *src = fopen(src_path, "rb");
    if (!src) return false;

    FILE *dst = fopen(dst_path, "wb");
    if (!dst) {
        fclose(src);
        return false;
    }

    char buf[8192];
    size_t n;
    bool ok = true;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        if (fwrite(buf, 1, n, dst) != n) {
            ok = false;
            break;
        }
    }

    fclose(src);
    fclose(dst);
    return ok;
}

bool mz_fs_copy_dir_recursive(const char *src_dir, const char *dst_dir) {
    if (!src_dir || !dst_dir) return false;

    DIR *dir = opendir(src_dir);
    if (!dir) return false;

    if (!mz_fs_mkdir_p(dst_dir)) {
        closedir(dir);
        return false;
    }

    struct dirent *entry;
    bool ok = true;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_sub[1024];
        char dst_sub[1024];
        snprintf(src_sub, sizeof(src_sub), "%s/%s", src_dir, entry->d_name);
        snprintf(dst_sub, sizeof(dst_sub), "%s/%s", dst_dir, entry->d_name);

        struct stat st;
        if (stat(src_sub, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                if (!mz_fs_copy_dir_recursive(src_sub, dst_sub)) {
                    ok = false;
                    break;
                }
            } else if (S_ISREG(st.st_mode)) {
                if (!mz_fs_copy_file(src_sub, dst_sub)) {
                    ok = false;
                    break;
                }
            }
        }
    }

    closedir(dir);
    return ok;
}

void mz_fs_route_to_filepath(const char *out_dir, const char *route, char *dst, size_t dst_size) {
    if (!out_dir || !route || !dst || dst_size == 0) return;

    // Strip leading slashes from route
    const char *r = route;
    while (*r == '/') r++;

    if (*r == '\0') {
        // Root route "/" -> "out_dir/index.html"
        snprintf(dst, dst_size, "%s/index.html", out_dir);
        return;
    }

    // Check if route already has a file extension (.html, .xml, .txt, etc.)
    const char *dot = strrchr(r, '.');
    const char *slash = strrchr(r, '/');
    if (dot && (!slash || dot > slash)) {
        snprintf(dst, dst_size, "%s/%s", out_dir, r);
    } else {
        // Clean route like "about" or "blog/post-1" -> "out_dir/about/index.html"
        size_t len = strlen(r);
        if (r[len - 1] == '/') {
            snprintf(dst, dst_size, "%s/%sindex.html", out_dir, r);
        } else {
            snprintf(dst, dst_size, "%s/%s/index.html", out_dir, r);
        }
    }
}
