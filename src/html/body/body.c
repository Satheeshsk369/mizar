#include "body.h"
#include <stdio.h>

void format_body(const Body *b, int width, int level) {
    if (!b) return;
    int indent = width * level;
    printf("%*s<body>\n", indent, "");
    if (b->content) {
        printf("%*s%s\n", indent + width, "", b->content);
    }
    printf("%*s</body>\n", indent, "");
}
