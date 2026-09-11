#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "mizar.h"

static void test_eq_and_Eq_helpers(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 256);

    mz_context_push(&buf);
    eq("E = mc^2");
    Eq("\\frac{a}{b}");
    mz_context_pop();

    assert(strstr(buf.data, "\\(E = mc^2\\)") != NULL);
    assert(strstr(buf.data, "\\[\\frac{a}{b}\\]") != NULL);

    mz_buf_free(&buf);
}

static void test_eqtext_inline_and_block(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 512);

    mz_context_push(&buf);
    EqText("Mass-energy: $E = mc^2$, and the block is $$x = 42$$.");
    mz_context_pop();

    assert(strstr(buf.data, "Mass-energy: \\(E = mc^2\\), and the block is \\[x = 42\\].") != NULL);

    mz_buf_free(&buf);
}

static void test_eqtext_escaped_dollar(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 512);

    mz_context_push(&buf);
    EqText("The item costs \\$5, and the formula is $x + y = 10$.");
    mz_context_pop();

    assert(strstr(buf.data, "The item costs $5, and the formula is \\(x + y = 10\\).") != NULL);

    mz_buf_free(&buf);
}

static void test_eqtext_auto_repair_escapes(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 512);

    mz_context_push(&buf);
    // In C, "\frac" has '\f' (form-feed 0x0C), "\alpha" has '\a' (bell 0x07), "\beta" has '\b' (backspace 0x08)
    // Our auto-repair restores \f -> \f, \a -> \a, \b -> \b!
    EqText("Test: $\frac{\alpha}{\beta}$");
    mz_context_pop();

    assert(strstr(buf.data, "\\(\\frac{\\alpha}{\\beta}\\)") != NULL);

    mz_buf_free(&buf);
}

static void test_eqtext_formatting(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 512);

    mz_context_push(&buf);
    int degree = 2;
    const char *var = "x";
    EqText("Polynomial degree %d: $%s^%d + 1 = 0$", degree, var, degree);
    mz_context_pop();

    assert(strstr(buf.data, "Polynomial degree 2: \\(x^2 + 1 = 0\\)") != NULL);

    mz_buf_free(&buf);
}

static void test_quadratic_formula(void) {
    // x = (-b ± √(b² - 4ac)) / (2a)
    MizarBuffer buf;
    mz_buf_init(&buf, 1024);

    MathDoc(&buf, .display = "block") {
        Math_Mrow() {
            Math_Mi() { Text("x"); }
            Math_Mo() { Text("="); }
            Math_Mfrac() {
                // Numerator: -b ± √(b² - 4ac)
                Math_Mrow() {
                    Math_Mo() { Text("-"); }
                    Math_Mi() { Text("b"); }
                    Math_Mo() { Text("±"); }
                    Math_Msqrt() {
                        Math_Mrow() {
                            Math_Msup() {
                                Math_Mi() { Text("b"); }
                                Math_Mn() { Text("2"); }
                            }
                            Math_Mo() { Text("-"); }
                            Math_Mn() { Text("4"); }
                            Math_Mi() { Text("a"); }
                            Math_Mi() { Text("c"); }
                        }
                    }
                }
                // Denominator: 2a
                Math_Mrow() {
                    Math_Mn() { Text("2"); }
                    Math_Mi() { Text("a"); }
                }
            }
        }
    }

    assert(strstr(buf.data, "display=\"block\"") != NULL);
    assert(strstr(buf.data, "xmlns=\"http://www.w3.org/1998/Math/MathML\"") != NULL);
    assert(strstr(buf.data, "<mfrac>") != NULL);
    assert(strstr(buf.data, "<msqrt>") != NULL);
    assert(strstr(buf.data, "<msup><mi>b</mi><mn>2</mn></msup>") != NULL);
    assert(strstr(buf.data, "<mn>4</mn><mi>a</mi><mi>c</mi>") != NULL);
    assert(strstr(buf.data, "<mrow><mn>2</mn><mi>a</mi></mrow>") != NULL);
    assert(strstr(buf.data, "</mfrac>") != NULL);
    assert(strstr(buf.data, "</math>") != NULL);

    mz_buf_free(&buf);
}

static void test_definite_integral(void) {
    // ∫_{0}^{∞} e^{-x²} dx = √π / 2
    MizarBuffer buf;
    mz_buf_init(&buf, 1024);

    MathDoc(&buf) {
        Math_Mrow() {
            Math_Msubsup() {
                Math_Mo() { Text("∫"); }
                Math_Mn() { Text("0"); }
                Math_Mo() { Text("∞"); }
            }
            Math_Msup() {
                Math_Mi() { Text("e"); }
                Math_Mrow() {
                    Math_Mo() { Text("-"); }
                    Math_Msup() {
                        Math_Mi() { Text("x"); }
                        Math_Mn() { Text("2"); }
                    }
                }
            }
            Math_Mi() { Text("d"); }
            Math_Mi() { Text("x"); }
            Math_Mo() { Text("="); }
            Math_Mfrac() {
                Math_Msqrt() {
                    Math_Mi() { Text("π"); }
                }
                Math_Mn() { Text("2"); }
            }
        }
    }

    assert(strstr(buf.data, "<msubsup><mo>∫</mo><mn>0</mn><mo>∞</mo></msubsup>") != NULL);
    assert(strstr(buf.data, "<mfrac><msqrt><mi>π</mi></msqrt><mn>2</mn></mfrac>") != NULL);

    mz_buf_free(&buf);
}

static void test_matrix_equation(void) {
    // 2x2 Matrix equation: A = [[a, b], [c, d]]
    MizarBuffer buf;
    mz_buf_init(&buf, 1024);

    MathDoc(&buf) {
        Math_Mrow() {
            Math_Mi() { Text("A"); }
            Math_Mo() { Text("="); }
            Math_Mfenced(.open = "[", .close = "]") {
                Math_Mtable() {
                    Math_Mtr() {
                        Math_Mtd() { Math_Mi() { Text("a"); } }
                        Math_Mtd() { Math_Mi() { Text("b"); } }
                    }
                    Math_Mtr() {
                        Math_Mtd() { Math_Mi() { Text("c"); } }
                        Math_Mtd() { Math_Mi() { Text("d"); } }
                    }
                }
            }
        }
    }

    assert(strstr(buf.data, "<mfenced open=\"[\" close=\"]\">") != NULL);
    assert(strstr(buf.data, "<mtable>") != NULL);
    assert(strstr(buf.data, "<mtr><mtd><mi>a</mi></mtd><mtd><mi>b</mi></mtd></mtr>") != NULL);
    assert(strstr(buf.data, "<mtr><mtd><mi>c</mi></mtd><mtd><mi>d</mi></mtd></mtr>") != NULL);
    assert(strstr(buf.data, "</mtable>") != NULL);
    assert(strstr(buf.data, "</mfenced>") != NULL);

    mz_buf_free(&buf);
}

static void test_latex_annotation_semantics(void) {
    // Semantic MathML containing both MathML presentation and LaTeX/KaTeX source annotation
    MizarBuffer buf;
    mz_buf_init(&buf, 1024);

    MathDoc(&buf) {
        Math_Semantics() {
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
            Math_Annotation(.encoding = "application/x-tex") {
                Text("E = mc^2");
            }
        }
    }

    assert(strstr(buf.data, "<semantics>") != NULL);
    assert(strstr(buf.data, "<annotation encoding=\"application/x-tex\">E = mc^2</annotation>") != NULL);
    assert(strstr(buf.data, "</semantics>") != NULL);

    mz_buf_free(&buf);
}

int main(void) {
    test_eq_and_Eq_helpers();
    test_eqtext_inline_and_block();
    test_eqtext_escaped_dollar();
    test_eqtext_auto_repair_escapes();
    test_eqtext_formatting();
    test_quadratic_formula();
    test_definite_integral();
    test_matrix_equation();
    test_latex_annotation_semantics();

    printf("All equation tests passed cleanly.\n");
    return 0;
}
