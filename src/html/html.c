#include "stdio.h"
#include "html.h"
  
void format_html(const HTMLDocument *d, int width) {
    printf("<!DOCTYPE html>\n");
    printf("<html>\n");
    format_head(&d->head, width, 1);
    format_body(&d->body, width, 1);
    printf("</html>\n");
}

HTMLDocument parse_html(const char *s[]) {
    HTMLDocument doc;
    return doc;
}

