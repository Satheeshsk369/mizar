#include "view/mathml/attrs.h"
#include <string.h>

static inline void mz_render_math_attr_str(MizarBuffer *buf, const char *name, const char *val) {
    if (!val) return;
    mz_buf_append_char(buf, ' ');
    mz_buf_append_str(buf, name);
    mz_buf_append_str(buf, "=\"");
    mz_buf_append_escaped(buf, val, strlen(val));
    mz_buf_append_char(buf, '"');
}

static inline void mz_render_math_attr_bool(MizarBuffer *buf, const char *name, bool val) {
    if (!val) return;
    mz_buf_append_char(buf, ' ');
    mz_buf_append_str(buf, name);
    mz_buf_append_str(buf, "=\"true\"");
}

void mz_render_math_attrs(MizarBuffer *buf, MathAttrs attrs) {
    if (!buf) return;

    // Global
    mz_render_math_attr_str(buf, "id", attrs.id);
    mz_render_math_attr_str(buf, "class", attrs.cls);
    mz_render_math_attr_str(buf, "style", attrs.style);
    mz_render_math_attr_str(buf, "dir", attrs.dir);
    mz_render_math_attr_str(buf, "mathbackground", attrs.mathbackground);
    mz_render_math_attr_str(buf, "mathcolor", attrs.mathcolor);
    mz_render_math_attr_str(buf, "mathsize", attrs.mathsize);
    mz_render_math_attr_str(buf, "href", attrs.href);
    mz_render_math_attr_str(buf, "tabindex", attrs.tabindex);
    mz_render_math_attr_str(buf, "role", attrs.role);

    // Root math
    mz_render_math_attr_str(buf, "display", attrs.display);
    mz_render_math_attr_str(buf, "xmlns", attrs.xmlns);

    // Tokens
    mz_render_math_attr_str(buf, "mathvariant", attrs.mathvariant);
    mz_render_math_attr_str(buf, "lquote", attrs.lquote);
    mz_render_math_attr_str(buf, "rquote", attrs.rquote);

    // Operator
    mz_render_math_attr_str(buf, "lspace", attrs.lspace);
    mz_render_math_attr_str(buf, "rspace", attrs.rspace);
    mz_render_math_attr_str(buf, "maxsize", attrs.maxsize);
    mz_render_math_attr_str(buf, "minsize", attrs.minsize);

    // Fraction
    mz_render_math_attr_str(buf, "linethickness", attrs.linethickness);
    mz_render_math_attr_str(buf, "numalign", attrs.numalign);
    mz_render_math_attr_str(buf, "denomalign", attrs.denomalign);

    // Enclose
    mz_render_math_attr_str(buf, "notation", attrs.notation);

    // Padded & Space
    mz_render_math_attr_str(buf, "width", attrs.width);
    mz_render_math_attr_str(buf, "height", attrs.height);
    mz_render_math_attr_str(buf, "depth", attrs.depth);
    mz_render_math_attr_str(buf, "voffset", attrs.voffset);

    // Table
    mz_render_math_attr_str(buf, "align", attrs.align);
    mz_render_math_attr_str(buf, "rowalign", attrs.rowalign);
    mz_render_math_attr_str(buf, "columnalign", attrs.columnalign);
    mz_render_math_attr_str(buf, "rowlines", attrs.rowlines);
    mz_render_math_attr_str(buf, "columnlines", attrs.columnlines);
    mz_render_math_attr_str(buf, "rowspacing", attrs.rowspacing);
    mz_render_math_attr_str(buf, "columnspacing", attrs.columnspacing);
    mz_render_math_attr_str(buf, "frame", attrs.frame);
    mz_render_math_attr_str(buf, "framespacing", attrs.framespacing);
    mz_render_math_attr_str(buf, "rowspan", attrs.rowspan);
    mz_render_math_attr_str(buf, "columnspan", attrs.columnspan);

    // Scripts
    mz_render_math_attr_str(buf, "subscriptshift", attrs.subscriptshift);
    mz_render_math_attr_str(buf, "superscriptshift", attrs.superscriptshift);

    // Action
    mz_render_math_attr_str(buf, "actiontype", attrs.actiontype);
    mz_render_math_attr_str(buf, "selection", attrs.selection);

    // Fenced
    mz_render_math_attr_str(buf, "open", attrs.open);
    mz_render_math_attr_str(buf, "close", attrs.close);
    mz_render_math_attr_str(buf, "separators", attrs.separators);

    // Style
    mz_render_math_attr_str(buf, "scriptlevel", attrs.scriptlevel);
    mz_render_math_attr_str(buf, "scriptminsize", attrs.scriptminsize);
    mz_render_math_attr_str(buf, "scriptsizemultiplier", attrs.scriptsizemultiplier);

    // Semantics & Annotations
    mz_render_math_attr_str(buf, "encoding", attrs.encoding);
    mz_render_math_attr_str(buf, "definitionURL", attrs.definitionURL);

    // Booleans
    mz_render_math_attr_bool(buf, "displaystyle", attrs.displaystyle);
    mz_render_math_attr_bool(buf, "accent", attrs.accent);
    mz_render_math_attr_bool(buf, "accentunder", attrs.accentunder);
    mz_render_math_attr_bool(buf, "fence", attrs.fence);
    mz_render_math_attr_bool(buf, "separator", attrs.separator);
    mz_render_math_attr_bool(buf, "stretchy", attrs.stretchy);
    mz_render_math_attr_bool(buf, "symmetric", attrs.symmetric);
    mz_render_math_attr_bool(buf, "movablelimits", attrs.movablelimits);
    mz_render_math_attr_bool(buf, "largeop", attrs.largeop);

    if (attrs.custom && *attrs.custom) {
        mz_buf_append_char(buf, ' ');
        mz_buf_append_str(buf, attrs.custom);
    }
}
