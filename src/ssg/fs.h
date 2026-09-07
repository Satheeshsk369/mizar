#ifndef MIZAR_SSG_FS_H
#define MIZAR_SSG_FS_H

#include <stddef.h>
#include <stdbool.h>

// Recursively creates directory path (like mkdir -p)
bool mz_fs_mkdir_p(const char *dir_path);

// Writes a string buffer to a file path, ensuring parent directories exist
bool mz_fs_write_file(const char *filepath, const char *data, size_t len);

// Copies a single file from src to dst, creating parent directories
bool mz_fs_copy_file(const char *src_path, const char *dst_path);

// Recursively copies all files and directories from src_dir to dst_dir
bool mz_fs_copy_dir_recursive(const char *src_dir, const char *dst_dir);

// Resolves a route URL (e.g. "/" -> "index.html", "/about" -> "about/index.html", "/feed.xml" -> "feed.xml")
void mz_fs_route_to_filepath(const char *out_dir, const char *route, char *dst, size_t dst_size);

#endif
