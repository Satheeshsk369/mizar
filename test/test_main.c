#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 4096);

    Html(&buf, .lang = "en") {
        Head() {
            Comment("Metadata section");
            Title() { Text("Full Spec Demo: HTML, SVG & MathML"); }
            Style() {
                Css_Comment("Global Theme");
                Rule(":root") {
                    Prop("--theme", "dark");
                }
            }
        }
        Body() {
            Comment("Main Content");
            Div(.cls = "card", .aria_label = "main card", HxGet("/api/data"), HxTarget("#result")) {
                H1() { Text("Vector Graphics (SVG)"); }
                Svg(.width = "200", .height = "200", .viewBox = "0 0 100 100", .xmlns = "http://www.w3.org/2000/svg") {
                    Svg_Defs() {
                        Svg_LinearGradient(.id = "grad1", .x1 = "0%", .y1 = "0%", .x2 = "100%", .y2 = "0%") {
                            Svg_Stop_(.offset = "0%", .stop_color = "#ff0000");
                            Svg_Stop_(.offset = "100%", .stop_color = "#0000ff");
                        }
                        Svg_Filter(.id = "blur") {
                            Svg_FeGaussianBlur_(.stdDeviation = "2");
                        }
                    }
                    Svg_Circle_(.cx = "50", .cy = "50", .r = "40", .fill = "url(#grad1)", .filter = "url(#blur)");
                    Svg_Text(.x = "50", .y = "55", .text_anchor = "middle", .fill = "#ffffff") {
                        Text("SVG text");
                    }
                }

                H1() { Text("Mathematics (MathML)"); }
                Math(.display = "block", .xmlns = "http://www.w3.org/1998/Math/MathML") {
                    // Quadratic formula: x = (-b +- sqrt(b^2 - 4ac)) / 2a
                    Math_Mrow() {
                        Math_Mi() { Text("x"); }
                        Math_Mo() { Text("="); }
                        Math_Mfrac() {
                            Math_Mrow() {
                                Math_Mrow() {
                                    Math_Mo() { Text("-"); }
                                    Math_Mi() { Text("b"); }
                                }
                                Math_Mo() { Text("±"); }
                                Math_Msqrt() {
                                    Math_Mrow() {
                                        Math_Msup() {
                                            Math_Mi() { Text("b"); }
                                            Math_Mn() { Text("2"); }
                                        }
                                        Math_Mo() { Text("-"); }
                                        Math_Mrow() {
                                            Math_Mn() { Text("4"); }
                                            Math_Mi() { Text("a"); }
                                            Math_Mi() { Text("c"); }
                                        }
                                    }
                                }
                            }
                            Math_Mrow() {
                                Math_Mn() { Text("2"); }
                                Math_Mi() { Text("a"); }
                            }
                        }
                    }
                }
            }
        }
    }

    printf("%s\n", buf.data);

    // HTML assertions
    assert(strstr(buf.data, "<!DOCTYPE html>\n") != NULL);
    assert(strstr(buf.data, "aria-label=\"main card\"") != NULL);
    assert(strstr(buf.data, "hx-get=\"/api/data\"") != NULL);
    assert(strstr(buf.data, "hx-target=\"#result\"") != NULL);

    // SVG assertions
    assert(strstr(buf.data, "<svg width=\"200\" height=\"200\" viewBox=\"0 0 100 100\" xmlns=\"http://www.w3.org/2000/svg\">") != NULL);
    assert(strstr(buf.data, "<defs>") != NULL);
    assert(strstr(buf.data, "<linearGradient id=\"grad1\"") != NULL);
    assert(strstr(buf.data, "<stop offset=\"0%\" stop-color=\"#ff0000\" />") != NULL);
    assert(strstr(buf.data, "<feGaussianBlur stdDeviation=\"2\" />") != NULL);
    assert(strstr(buf.data, "<circle cx=\"50\" cy=\"50\" r=\"40\" fill=\"url(#grad1)\" filter=\"url(#blur)\" />") != NULL);
    assert(strstr(buf.data, "<text x=\"50\" y=\"55\" text-anchor=\"middle\" fill=\"#ffffff\">SVG text</text>") != NULL);

    // MathML assertions
    assert(strstr(buf.data, "<math display=\"block\" xmlns=\"http://www.w3.org/1998/Math/MathML\">") != NULL);
    assert(strstr(buf.data, "<mfrac>") != NULL);
    assert(strstr(buf.data, "<msqrt>") != NULL);
    assert(strstr(buf.data, "<msup>") != NULL);
    assert(strstr(buf.data, "<mi>x</mi>") != NULL);

    mz_buf_free(&buf);
    printf("\n>>> ALL HTML, SVG & MATHML TESTS PASSED! <<<\n");
    return 0;
}
