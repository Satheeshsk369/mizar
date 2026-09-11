#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 4096);
    mz_context_push(&buf);

    // 1. Box with EdgeInsets & Elevation physics calculations
    Box(
        .max_width = 800,
        .padding = EdgeInsets_Symmetric(.h = 24, .v = 16),
        .margin = EdgeInsets_All(12),
        .elevation = 4,
        .cls = "user-defined-theme-class"
    ) {
        // 2. Row with Flex Alignment Geometry & Gap calculation
        Row(.gap = 16, .main_align = ALIGN_SPACE_BETWEEN, .cross_align = ALIGN_CENTER) {
            // 3. Proportional layout with Expanded
            Expanded(.flex = 2) {
                H2() { Text("Left Column"); }
            }
            Expanded(.flex = 1) {
                H2() { Text("Right Column"); }
            }
        }

        // 4. Column layout
        Column(.gap = 8, .cross_align = ALIGN_STRETCH) {
            P() { Text("Vertical stack item 1"); }
            P() { Text("Vertical stack item 2"); }
        }

        // 5. AspectRatio geometry calculator
        AspectRatio(16.0 / 9.0) {
            Img(.src = "/hero.jpg", .alt = "Video placeholder");
        }

        // 6. Responsive Grid calculation
        Grid(.min_col_width = 250, .gap = 20) {
            Div() { Text("Item 1"); }
            Div() { Text("Item 2"); }
            Div() { Text("Item 3"); }
        }
    }

    mz_context_pop();

    // Verify Output & Calculations
    assert(strstr(buf.data, "max-width: 800px;") != NULL);
    assert(strstr(buf.data, "padding: 16px 24px;") != NULL);
    assert(strstr(buf.data, "margin: 12px;") != NULL);
    assert(strstr(buf.data, "box-shadow: 0 7px 12px") != NULL); // elevation 4 calculation!
    assert(strstr(buf.data, "class=\"user-defined-theme-class\"") != NULL);

    // Row & Flex assertions
    assert(strstr(buf.data, "flex-direction: row;") != NULL);
    assert(strstr(buf.data, "justify-content: space-between;") != NULL);
    assert(strstr(buf.data, "align-items: center;") != NULL);
    assert(strstr(buf.data, "flex: 2 1 0%;") != NULL);
    assert(strstr(buf.data, "flex: 1 1 0%;") != NULL);

    // Column assertions
    assert(strstr(buf.data, "flex-direction: column;") != NULL);
    assert(strstr(buf.data, "align-items: stretch;") != NULL);

    // Aspect Ratio & Grid assertions
    assert(strstr(buf.data, "aspect-ratio: 1.778;") != NULL); // 16/9 calculated!
    assert(strstr(buf.data, "grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));") != NULL);

    mz_buf_free(&buf);
    printf("Headless Geometry & Material Calculation test passed cleanly!\n");
    return 0;
}
