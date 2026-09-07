#ifndef HTML_H
#define HTML_H

#include "head/head.h"
#include "body/body.h"

typedef struct {
    Head head;
    Body body;
} HTMLDocument;

void format_html(const HTMLDocument *d, int width);

#endif 
