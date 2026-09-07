#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "mizar.h"

static void render_index(MizarBuffer *buf, void *user_data) {
    (void)user_data;
    Html(buf, .lang = "en") {
        Head() {
            Title() { Text("Home"); }
            Link(.rel = "stylesheet", .href = "/style.css");
        }
        Body() {
            H1() { Text("Index"); }
        }
    }
}

static void render_about(MizarBuffer *buf, void *user_data) {
    (void)user_data;
    Html(buf, .lang = "en") {
        Body() {
            P() { Text("About"); }
        }
    }
}

int main(void) {
    // Setup static asset
    mz_fs_mkdir_p("build/test_static");
    const char *css = "body { margin: 0; }";
    mz_fs_write_file("build/test_static/style.css", css, strlen(css));

    MizarSite site;
    mz_site_init(&site, "build/test_dist");
    mz_site_set_static_dir(&site, "build/test_static");

    mz_site_add_page(&site, "/", render_index, nullptr);
    mz_site_add_page(&site, "/about", render_about, nullptr);

    assert(mz_site_build(&site) == true);

    // Verify generated files
    FILE *f_index = fopen("build/test_dist/index.html", "r");
    assert(f_index != NULL);
    char buf[1024];
    size_t n = fread(buf, 1, sizeof(buf) - 1, f_index);
    buf[n] = '\0';
    fclose(f_index);
    assert(strstr(buf, "<title>Home</title>") != NULL);

    FILE *f_about = fopen("build/test_dist/about/index.html", "r");
    assert(f_about != NULL);
    n = fread(buf, 1, sizeof(buf) - 1, f_about);
    buf[n] = '\0';
    fclose(f_about);
    assert(strstr(buf, "<p>About</p>") != NULL);

    FILE *f_css = fopen("build/test_dist/style.css", "r");
    assert(f_css != NULL);
    fclose(f_css);

    mz_site_free(&site);
    return 0;
}
