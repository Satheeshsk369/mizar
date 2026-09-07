#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 1024);

    Html(&buf, .lang = "en") {
        Head() {
            Comment("Metadata section");
            Title() { Text("Comment Demo"); }
            Style() {
                Css_Comment("Global Theme");
                Rule(":root") {
                    Prop("--theme", "dark");
                }
            }
        }
        Body() {
            Comment("Main Content");
            Div(.cls = "card") {
                Text("Hello World");
            }
        }
    }

    printf("%s\n", buf.data);

    assert(strstr(buf.data, "<!-- Metadata section -->\n") != NULL);
    assert(strstr(buf.data, "<!-- Main Content -->\n") != NULL);
    assert(strstr(buf.data, "/* Global Theme */\n") != NULL);

    mz_buf_free(&buf);
    printf("\n>>> ALL TESTS PASSED! <<<\n");
    return 0;
}
