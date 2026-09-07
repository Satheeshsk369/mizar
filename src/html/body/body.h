#ifndef BODY_H
#define BODY_H

typedef struct {
    const char *content;
} Body;

void format_body(const Body *b, int width, int level);

#endif
