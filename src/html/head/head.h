#ifndef HEAD_H
#define HEAD_H

typedef struct {
    const char *title;
    const char *meta_tags;
} Head;

void format_head(const Head *h, int width, int level);

#endif

