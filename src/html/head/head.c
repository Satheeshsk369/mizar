#include "head.h"
#include <stdio.h>

void format_head(const Head *h, int width, int level) {
    if (!h) return;
    int indent = width * level;
    printf("%*s<head>\n", indent, "");
    if (h->title) {
        printf("%*s<title>%s</title>\n", indent + width, "", h->title);
    }
    if (h->meta_tags) {
        printf("%*s%s\n", indent + width, "", h->meta_tags);
    }
    printf("%*s</head>\n", indent, "");
}
