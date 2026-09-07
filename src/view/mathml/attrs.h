#ifndef MIZAR_VIEW_MATHML_ATTRS_H
#define MIZAR_VIEW_MATHML_ATTRS_H

#include "core/buffer.h"
#include <stdbool.h>

typedef struct {
    // Global MathML attributes
    const char *id;
    const char *cls;
    const char *style;
    const char *dir;
    const char *mathbackground;
    const char *mathcolor;
    const char *mathsize;
    const char *href;
    const char *tabindex;
    const char *role;

    // math element attributes
    const char *display;
    const char *xmlns;

    // Token elements & specific presentation attributes
    const char *mathvariant;
    const char *lquote;
    const char *rquote;

    // Operator (<mo>) attributes
    const char *lspace;
    const char *rspace;
    const char *maxsize;
    const char *minsize;

    // Fraction (<mfrac>) attributes
    const char *linethickness;
    const char *numalign;
    const char *denomalign;

    // Enclose (<menclose>) attributes
    const char *notation;

    // Padded (<mpadded>) & Space (<mspace>) attributes
    const char *width;
    const char *height;
    const char *depth;
    const char *voffset;

    // Table (<mtable>, <mtd>, <mtr>) attributes
    const char *align;
    const char *rowalign;
    const char *columnalign;
    const char *rowlines;
    const char *columnlines;
    const char *rowspacing;
    const char *columnspacing;
    const char *frame;
    const char *framespacing;
    const char *rowspan;
    const char *columnspan;

    // Scripts (<msub>, <msup>, <msubsup>, <mmultiscripts>)
    const char *subscriptshift;
    const char *superscriptshift;

    // Action (<maction>)
    const char *actiontype;
    const char *selection;

    // Fenced (<mfenced>) legacy
    const char *open;
    const char *close;
    const char *separators;

    // Style (<mstyle>) legacy
    const char *scriptlevel;
    const char *scriptminsize;
    const char *scriptsizemultiplier;

    // Semantics & Annotations
    const char *encoding;
    const char *definitionURL;

    // Boolean attributes
    bool displaystyle;
    bool accent;
    bool accentunder;
    bool fence;
    bool separator;
    bool stretchy;
    bool symmetric;
    bool movablelimits;
    bool largeop;

    // Custom attribute injection
    const char *custom;
} MathAttrs;

void mz_render_math_attrs(MizarBuffer *buf, MathAttrs attrs);

#endif
