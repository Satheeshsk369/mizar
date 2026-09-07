#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mizar.h"

// Custom Component / Layout
static void render_base_layout(MizarBuffer *buf, const char *title, void (*body_fn)(void)) {
    Html(buf, .lang = "en") {
        Head() {
            Title() { Text("%s", title); }
            Link(.rel = "stylesheet", .href = "/style.css");
        }
        Body() {
            Header() {
                Nav() {
                    A(.href = "/") { Text("Home"); }
                    Text(" | ");
                    A(.href = "/about") { Text("About"); }
                }
            }
            Main() {
                body_fn();
            }
            Footer() {
                P() { Text("Built with Mizar SSG"); }
            }
        }
    }
}

// Page 1: Home Page
static void home_body(void) {
    H1() { Text("Welcome to Mizar Static Site"); }
    P() { Text("This site was pre-rendered at compile time!"); }
    Svg(.width = "100", .height = "100", .viewBox = "0 0 100 100") {
        Svg_Circle_(.cx = "50", .cy = "50", .r = "45", .fill = "teal");
    }
}

static void render_home_page(MizarBuffer *buf, void *user_data) {
    (void)user_data;
    render_base_layout(buf, "Home Page", home_body);
}

// Page 2: About Page
static void about_body(void) {
    H1() { Text("About Us"); }
    P() { Text("Mizar is a modern C23 declarative web framework and SSG."); }
    Math(.display = "block") {
        Math_Mrow() {
            Math_Mi() { Text("E"); }
            Math_Mo() { Text("="); }
            Math_Mrow() {
                Math_Mi() { Text("m"); }
                Math_Msup() {
                    Math_Mi() { Text("c"); }
                    Math_Mn() { Text("2"); }
                }
            }
        }
    }
}

static void render_about_page(MizarBuffer *buf, void *user_data) {
    (void)user_data;
    render_base_layout(buf, "About Page", about_body);
}

int main(void) {
    printf("Testing Mizar SSG site build...\n");

    // 1. Create a dummy static asset directory
    mz_fs_mkdir_p("build/test_static");
    const char *css = "body { font-family: sans-serif; margin: 2rem; }";
    mz_fs_write_file("build/test_static/style.css", css, strlen(css));

    // 2. Initialize Site
    MizarSite site;
    mz_site_init(&site, "build/test_dist");
    mz_site_set_static_dir(&site, "build/test_static");

    // 3. Register Routes
    mz_site_add_page(&site, "/", render_home_page, nullptr);
    mz_site_add_page(&site, "/about", render_about_page, nullptr);

    // 4. Build the site
    bool ok = mz_site_build(&site);
    assert(ok == true);

    // 5. Verify output files
    FILE *f_index = fopen("build/test_dist/index.html", "r");
    assert(f_index != NULL);
    char content[4096];
    size_t n = fread(content, 1, sizeof(content) - 1, f_index);
    content[n] = '\0';
    fclose(f_index);

    assert(strstr(content, "<title>Home Page</title>") != NULL);
    assert(strstr(content, "circle cx=\"50\" cy=\"50\" r=\"45\" fill=\"teal\"") != NULL);

    FILE *f_about = fopen("build/test_dist/about/index.html", "r");
    assert(f_about != NULL);
    n = fread(content, 1, sizeof(content) - 1, f_about);
    content[n] = '\0';
    fclose(f_about);

    assert(strstr(content, "<title>About Page</title>") != NULL);
    assert(strstr(content, "<math display=\"block\"><mrow><mi>E</mi><mo>=</mo><mrow><mi>m</mi><msup><mi>c</mi><mn>2</mn></msup></mrow></mrow></math>") != NULL);

    // Verify static asset copy
    FILE *f_css = fopen("build/test_dist/style.css", "r");
    assert(f_css != NULL);
    fclose(f_css);

    mz_site_free(&site);
    printf(">>> ALL SSG & SITE BUILD TESTS PASSED! <<<\n");
    return 0;
}
