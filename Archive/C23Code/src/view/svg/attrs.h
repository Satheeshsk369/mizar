#ifndef MIZAR_VIEW_SVG_ATTRS_H
#define MIZAR_VIEW_SVG_ATTRS_H

#include "core/buffer.h"
#include <stdbool.h>

typedef struct {
    // Core attributes
    const char *id;
    const char *cls;
    const char *style;
    const char *tabindex;
    const char *role;
    const char *lang;
    const char *xmlns;
    const char *xmlns_xlink;
    const char *version;
    const char *baseProfile;

    // Geometry / Dimensions / Viewport
    const char *x;
    const char *y;
    const char *x1;
    const char *y1;
    const char *x2;
    const char *y2;
    const char *dx;
    const char *dy;
    const char *cx;
    const char *cy;
    const char *r;
    const char *rx;
    const char *ry;
    const char *width;
    const char *height;
    const char *viewBox;
    const char *preserveAspectRatio;
    const char *transform;
    const char *transform_origin;

    // Paths & Shapes
    const char *d;
    const char *points;
    const char *pathLength;

    // Presentation: Fill & Stroke
    const char *fill;
    const char *fill_opacity;
    const char *fill_rule;
    const char *stroke;
    const char *stroke_width;
    const char *stroke_linecap;
    const char *stroke_linejoin;
    const char *stroke_miterlimit;
    const char *stroke_dasharray;
    const char *stroke_dashoffset;
    const char *stroke_opacity;
    const char *opacity;

    // Presentation: Color & Graphics
    const char *color;
    const char *color_interpolation;
    const char *color_interpolation_filters;
    const char *visibility;
    const char *display;
    const char *overflow;
    const char *clip;
    const char *clip_path;
    const char *clip_rule;
    const char *mask;
    const char *filter;
    const char *cursor;
    const char *pointer_events;
    const char *image_rendering;
    const char *shape_rendering;
    const char *text_rendering;

    // Text Presentation
    const char *font_family;
    const char *font_size;
    const char *font_size_adjust;
    const char *font_stretch;
    const char *font_style;
    const char *font_variant;
    const char *font_weight;
    const char *text_anchor;
    const char *text_decoration;
    const char *dominant_baseline;
    const char *alignment_baseline;
    const char *baseline_shift;
    const char *letter_spacing;
    const char *word_spacing;
    const char *writing_mode;
    const char *lengthAdjust;
    const char *startOffset;
    const char *method;
    const char *spacing;
    const char *side;

    // Referencing & Hyperlinks
    const char *href;
    const char *xlink_href;
    const char *target;
    const char *rel;
    const char *download;
    const char *type;
    const char *media;

    // Gradients & Patterns
    const char *gradientUnits;
    const char *gradientTransform;
    const char *spreadMethod;
    const char *patternUnits;
    const char *patternContentUnits;
    const char *patternTransform;
    const char *offset;
    const char *stop_color;
    const char *stop_opacity;
    const char *fx;
    const char *fy;
    const char *fr;

    // Markers & ClipPath & Mask
    const char *markerUnits;
    const char *markerWidth;
    const char *markerHeight;
    const char *refX;
    const char *refY;
    const char *orient;
    const char *marker_start;
    const char *marker_mid;
    const char *marker_end;
    const char *clipPathUnits;
    const char *maskUnits;
    const char *maskContentUnits;

    // Filter Primitive Attributes
    const char *in;
    const char *in2;
    const char *result;
    const char *mode;
    const char *values;
    const char *type_attr; // for feColorMatrix / feTurbulence type
    const char *slope;
    const char *intercept;
    const char *amplitude;
    const char *exponent;
    const char *tableValues;
    const char *operator_;
    const char *k1;
    const char *k2;
    const char *k3;
    const char *k4;
    const char *order;
    const char *kernelMatrix;
    const char *divisor;
    const char *bias;
    const char *targetX;
    const char *targetY;
    const char *edgeMode;
    const char *preserveAlpha;
    const char *surfaceScale;
    const char *diffuseConstant;
    const char *specularConstant;
    const char *specularExponent;
    const char *lighting_color;
    const char *scale;
    const char *xChannelSelector;
    const char *yChannelSelector;
    const char *stdDeviation;
    const char *radius;
    const char *baseFrequency;
    const char *numOctaves;
    const char *seed;
    const char *stitchTiles;
    const char *azimuth;
    const char *elevation;
    const char *pointsAtX;
    const char *pointsAtY;
    const char *pointsAtZ;
    const char *limitingConeAngle;
    const char *flood_color;
    const char *flood_opacity;

    // Animation Attributes
    const char *attributeName;
    const char *attributeType;
    const char *begin;
    const char *dur;
    const char *end;
    const char *min;
    const char *max;
    const char *restart;
    const char *repeatCount;
    const char *repeatDur;
    const char *fill_anim; // fill="freeze" or "remove"
    const char *calcMode;
    const char *keyTimes;
    const char *keySplines;
    const char *from;
    const char *to;
    const char *by;
    const char *additive;
    const char *accumulate;
    const char *keyPoints;
    const char *path;
    const char *rotate;

    // HTMX integration
    const char *hx_get;
    const char *hx_post;
    const char *hx_put;
    const char *hx_delete;
    const char *hx_patch;
    const char *hx_target;
    const char *hx_swap;
    const char *hx_trigger;

    // Custom attribute injection
    const char *custom;
} SvgAttrs;

void mz_render_svg_attrs(MizarBuffer *buf, SvgAttrs attrs);

#endif
