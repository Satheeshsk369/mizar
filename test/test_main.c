#include <assert.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 4096);

    // Document with HTML, SVG and MathML
    Html(&buf, .lang = "en") {
        Head() {
            Title() { Text("Document"); }
            Style() {
                Rule(".box") { Prop("margin", "0"); }
                CssRule(".card", (CssProps){
                    .display = CSS_FLEX,
                    .flex_direction = CSS_COLUMN,
                    .padding = Px(16),
                    .border_radius = Px(8),
                    .background = "#ffffff"
                });
            }
        }
        Body() {
            Div(.id = "app", .cls = "container", HxGet("/items"), HxTarget("#list")) {
                Svg(.width = "100", .height = "100", .viewBox = "0 0 100 100") {
                    Svg_Circle_(.cx = "50", .cy = "50", .r = "40", .fill = "#000");
                }
                Math(.display = "block") {
                    Math_Mrow() {
                        Math_Mi() { Text("x"); }
                        Math_Mo() { Text("="); }
                        Math_Mn() { Text("1"); }
                    }
                }
            }
        }
    }

    assert(strstr(buf.data, "<!DOCTYPE html>\n") != NULL);
    assert(strstr(buf.data, "<title>Document</title>") != NULL);
    assert(strstr(buf.data, ".box {\n  margin: 0;\n}\n") != NULL);
    assert(strstr(buf.data, ".card {\n  display: flex;\n  flex-direction: column;\n  padding: 16px;\n  border-radius: 8px;\n  background: #ffffff;\n}\n") != NULL);
    assert(strstr(buf.data, "hx-get=\"/items\"") != NULL);
    assert(strstr(buf.data, "<circle cx=\"50\" cy=\"50\" r=\"40\" fill=\"#000\" />") != NULL);
    assert(strstr(buf.data, "<math display=\"block\"><mrow><mi>x</mi><mo>=</mo><mn>1</mn></mrow></math>") != NULL);

    mz_buf_free(&buf);

    // Standalone SVG & MathML documents
    MizarBuffer sbuf, mbuf;
    mz_buf_init(&sbuf, 256);
    SvgDoc(&sbuf, .width = "50", .height = "50") {
        Svg_Rect_(.width = "50", .height = "50");
    }
    assert(strstr(sbuf.data, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"50\" height=\"50\"><rect width=\"50\" height=\"50\" /></svg>") != NULL);
    mz_buf_free(&sbuf);

    mz_buf_init(&mbuf, 256);
    MathDoc(&mbuf) {
        Math_Mi() { Text("y"); }
    }
    assert(strstr(mbuf.data, "<math xmlns=\"http://www.w3.org/1998/Math/MathML\"><mi>y</mi></math>") != NULL);
    mz_buf_free(&mbuf);

    return 0;
}
