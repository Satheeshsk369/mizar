#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "core/version.h"

#define MIZAR_VERSION MIZAR_VERSION_STRING

static const char MAIN_C_TEMPLATE[] =
"#include <mizar.h>\n"
"\n"
"static void handle_home(const MzRequest *req, MzResponse *res, void *user_data) {\n"
"    (void)req; (void)user_data;\n"
"    mz_res_html(res);\n"
"\n"
"    MzPage(&res->body, .title = \"My Mizar App\", .theme_color = \"#0284c7\") {\n"
"        MzFlashContainer();\n"
"\n"
"        Div(.style = \"max-width: 720px; margin: 60px auto; font-family: system-ui, sans-serif; padding: 0 20px;\") {\n"
"            H1(.style = \"color: #0f172a; font-size: 2.25rem; font-weight: 800; margin-bottom: 8px;\") {\n"
"                Text(\"Welcome to Mizar + HTMX 4\");\n"
"            }\n"
"            P(.style = \"color: #475569; font-size: 1.1rem; line-height: 1.6; margin-bottom: 32px;\") {\n"
"                Text(\"A high-performance reactive web application built with modern C23.\");\n"
"            }\n"
"\n"
"            // Interactive Hypermedia Pattern\n"
"            Div(.style = \"margin-bottom: 24px;\") {\n"
"                MzSearch(.url = \"/api/search\", .target = \"#results\", .placeholder = \"Live search items...\");\n"
"                Div(.id = \"results\", .style = \"margin-top: 12px;\") {}\n"
"            }\n"
"\n"
"            // Click-to-Edit Pattern\n"
"            MzClickToEdit(.url = \"/api/profile/edit\", .label = \"User Status\", .value = \"Active Contributor\");\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"static void handle_search(const MzRequest *req, MzResponse *res, void *user_data) {\n"
"    (void)user_data;\n"
"    const char *q = mz_req_query(req, \"q\");\n"
"    mz_res_html(res);\n"
"    mz_context_push(&res->body);\n"
"    Div(.style = \"padding: 12px; background: #f1f5f9; border-radius: 6px; font-size: 0.9rem; color: #334155;\") {\n"
"        Text(\"Found results for: %s\", q && *q ? q : \"(all)\");\n"
"    }\n"
"    mz_context_pop();\n"
"}\n"
"\n"
"int main(void) {\n"
"    MzApp app;\n"
"    mz_app_init(&app);\n"
"    mz_app_set_workers(&app, 4);\n"
"\n"
"    // Standard middlewares\n"
"    mz_app_use(&app, mz_middleware_security_headers);\n"
"\n"
"    // Routes\n"
"    mz_app_get(&app, \"/\", handle_home);\n"
"    mz_app_get(&app, \"/api/search\", handle_search);\n"
"\n"
"    printf(\"Mizar application running at http://localhost:3000\\n\");\n"
"    mz_app_listen(&app, 3000);\n"
"\n"
"    mz_app_free(&app);\n"
"    return 0;\n"
"}\n";

static const char MAKEFILE_TEMPLATE[] =
"CC ?= gcc\n"
"CFLAGS ?= -std=c23 -Wall -Wextra -O2 $(shell pkg-config --cflags mizar 2>/dev/null || echo \"-I/usr/local/include/mizar -I/usr/include/mizar\")\n"
"LIBS ?= $(shell pkg-config --libs mizar 2>/dev/null || echo \"-L/usr/local/lib -lmizar -lpthread\") -Wl,-rpath,/usr/local/lib\n"
"\n"
"app: main.c\n"
"\t$(CC) $(CFLAGS) main.c $(LIBS) -o app\n"
"\n"
"run: app\n"
"\t./app\n"
"\n"
"clean:\n"
"\trm -f app\n";

static const char GITIGNORE_TEMPLATE[] =
"app\n"
"*.o\n"
"*.so\n"
"*.a\n"
".DS_Store\n";

static void print_usage(const char *prog) {
    printf("Mizar %s\n\n", MIZAR_VERSION);
    printf("Usage:\n");
    printf("  %s init <project-name>    Create a new Mizar project\n", prog);
    printf("  %s --version, -v          Show version information\n", prog);
    printf("  %s --help, -h             Show this help message\n", prog);
}

static int write_file(const char *filepath, const char *content) {
    FILE *f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Error creating '%s': %s\n", filepath, strerror(errno));
        return -1;
    }
    fputs(content, f);
    fclose(f);
    return 0;
}

static int cmd_new(const char *dir) {
    struct stat st;
    if (stat(dir, &st) == 0) {
        fprintf(stderr, "Error: Directory '%s' already exists.\n", dir);
        return 1;
    }

    if (mkdir(dir, 0755) != 0) {
        fprintf(stderr, "Error creating directory '%s': %s\n", dir, strerror(errno));
        return 1;
    }

    char path[1024];

    snprintf(path, sizeof(path), "%s/main.c", dir);
    if (write_file(path, MAIN_C_TEMPLATE) != 0) return 1;

    snprintf(path, sizeof(path), "%s/Makefile", dir);
    if (write_file(path, MAKEFILE_TEMPLATE) != 0) return 1;

    snprintf(path, sizeof(path), "%s/.gitignore", dir);
    if (write_file(path, GITIGNORE_TEMPLATE) != 0) return 1;

    printf("Created new Mizar project in '%s'.\n\n", dir);
    printf("To get started:\n");
    printf("  cd %s\n", dir);
    printf("  make run\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("Mizar %s\n", MIZAR_VERSION);
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: missing project name.\n");
            fprintf(stderr, "Usage: %s init <project-name>\n", argv[0]);
            return 1;
        }
        return cmd_new(argv[2]);
    }

    fprintf(stderr, "Unknown command: %s\n\n", argv[1]);
    print_usage(argv[0]);
    return 1;
}
