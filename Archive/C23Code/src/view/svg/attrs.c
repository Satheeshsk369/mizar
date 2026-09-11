#include "view/svg/attrs.h"
#include <string.h>

static inline void mz_render_svg_attr_str(MizarBuffer *buf, const char *name, const char *val) {
    if (!val) return;
    mz_buf_append_char(buf, ' ');
    mz_buf_append_str(buf, name);
    mz_buf_append_str(buf, "=\"");
    mz_buf_append_escaped(buf, val, strlen(val));
    mz_buf_append_char(buf, '"');
}

void mz_render_svg_attrs(MizarBuffer *buf, SvgAttrs attrs) {
    if (!buf) return;

    // Core
    mz_render_svg_attr_str(buf, "id", attrs.id);
    mz_render_svg_attr_str(buf, "class", attrs.cls);
    mz_render_svg_attr_str(buf, "style", attrs.style);
    mz_render_svg_attr_str(buf, "tabindex", attrs.tabindex);
    mz_render_svg_attr_str(buf, "role", attrs.role);
    mz_render_svg_attr_str(buf, "lang", attrs.lang);
    mz_render_svg_attr_str(buf, "xmlns", attrs.xmlns);
    mz_render_svg_attr_str(buf, "xmlns:xlink", attrs.xmlns_xlink);
    mz_render_svg_attr_str(buf, "version", attrs.version);
    mz_render_svg_attr_str(buf, "baseProfile", attrs.baseProfile);

    // Geometry / Dimensions / Viewport
    mz_render_svg_attr_str(buf, "x", attrs.x);
    mz_render_svg_attr_str(buf, "y", attrs.y);
    mz_render_svg_attr_str(buf, "x1", attrs.x1);
    mz_render_svg_attr_str(buf, "y1", attrs.y1);
    mz_render_svg_attr_str(buf, "x2", attrs.x2);
    mz_render_svg_attr_str(buf, "y2", attrs.y2);
    mz_render_svg_attr_str(buf, "dx", attrs.dx);
    mz_render_svg_attr_str(buf, "dy", attrs.dy);
    mz_render_svg_attr_str(buf, "cx", attrs.cx);
    mz_render_svg_attr_str(buf, "cy", attrs.cy);
    mz_render_svg_attr_str(buf, "r", attrs.r);
    mz_render_svg_attr_str(buf, "rx", attrs.rx);
    mz_render_svg_attr_str(buf, "ry", attrs.ry);
    mz_render_svg_attr_str(buf, "width", attrs.width);
    mz_render_svg_attr_str(buf, "height", attrs.height);
    mz_render_svg_attr_str(buf, "viewBox", attrs.viewBox);
    mz_render_svg_attr_str(buf, "preserveAspectRatio", attrs.preserveAspectRatio);
    mz_render_svg_attr_str(buf, "transform", attrs.transform);
    mz_render_svg_attr_str(buf, "transform-origin", attrs.transform_origin);

    // Paths & Shapes
    mz_render_svg_attr_str(buf, "d", attrs.d);
    mz_render_svg_attr_str(buf, "points", attrs.points);
    mz_render_svg_attr_str(buf, "pathLength", attrs.pathLength);

    // Fill & Stroke
    mz_render_svg_attr_str(buf, "fill", attrs.fill);
    mz_render_svg_attr_str(buf, "fill-opacity", attrs.fill_opacity);
    mz_render_svg_attr_str(buf, "fill-rule", attrs.fill_rule);
    mz_render_svg_attr_str(buf, "stroke", attrs.stroke);
    mz_render_svg_attr_str(buf, "stroke-width", attrs.stroke_width);
    mz_render_svg_attr_str(buf, "stroke-linecap", attrs.stroke_linecap);
    mz_render_svg_attr_str(buf, "stroke-linejoin", attrs.stroke_linejoin);
    mz_render_svg_attr_str(buf, "stroke-miterlimit", attrs.stroke_miterlimit);
    mz_render_svg_attr_str(buf, "stroke-dasharray", attrs.stroke_dasharray);
    mz_render_svg_attr_str(buf, "stroke-dashoffset", attrs.stroke_dashoffset);
    mz_render_svg_attr_str(buf, "stroke-opacity", attrs.stroke_opacity);
    mz_render_svg_attr_str(buf, "opacity", attrs.opacity);

    // Color & Graphics
    mz_render_svg_attr_str(buf, "color", attrs.color);
    mz_render_svg_attr_str(buf, "color-interpolation", attrs.color_interpolation);
    mz_render_svg_attr_str(buf, "color-interpolation-filters", attrs.color_interpolation_filters);
    mz_render_svg_attr_str(buf, "visibility", attrs.visibility);
    mz_render_svg_attr_str(buf, "display", attrs.display);
    mz_render_svg_attr_str(buf, "overflow", attrs.overflow);
    mz_render_svg_attr_str(buf, "clip", attrs.clip);
    mz_render_svg_attr_str(buf, "clip-path", attrs.clip_path);
    mz_render_svg_attr_str(buf, "clip-rule", attrs.clip_rule);
    mz_render_svg_attr_str(buf, "mask", attrs.mask);
    mz_render_svg_attr_str(buf, "filter", attrs.filter);
    mz_render_svg_attr_str(buf, "cursor", attrs.cursor);
    mz_render_svg_attr_str(buf, "pointer-events", attrs.pointer_events);
    mz_render_svg_attr_str(buf, "image-rendering", attrs.image_rendering);
    mz_render_svg_attr_str(buf, "shape-rendering", attrs.shape_rendering);
    mz_render_svg_attr_str(buf, "text-rendering", attrs.text_rendering);

    // Text Presentation
    mz_render_svg_attr_str(buf, "font-family", attrs.font_family);
    mz_render_svg_attr_str(buf, "font-size", attrs.font_size);
    mz_render_svg_attr_str(buf, "font-size-adjust", attrs.font_size_adjust);
    mz_render_svg_attr_str(buf, "font-stretch", attrs.font_stretch);
    mz_render_svg_attr_str(buf, "font-style", attrs.font_style);
    mz_render_svg_attr_str(buf, "font-variant", attrs.font_variant);
    mz_render_svg_attr_str(buf, "font-weight", attrs.font_weight);
    mz_render_svg_attr_str(buf, "text-anchor", attrs.text_anchor);
    mz_render_svg_attr_str(buf, "text-decoration", attrs.text_decoration);
    mz_render_svg_attr_str(buf, "dominant-baseline", attrs.dominant_baseline);
    mz_render_svg_attr_str(buf, "alignment-baseline", attrs.alignment_baseline);
    mz_render_svg_attr_str(buf, "baseline-shift", attrs.baseline_shift);
    mz_render_svg_attr_str(buf, "letter-spacing", attrs.letter_spacing);
    mz_render_svg_attr_str(buf, "word-spacing", attrs.word_spacing);
    mz_render_svg_attr_str(buf, "writing-mode", attrs.writing_mode);
    mz_render_svg_attr_str(buf, "lengthAdjust", attrs.lengthAdjust);
    mz_render_svg_attr_str(buf, "startOffset", attrs.startOffset);
    mz_render_svg_attr_str(buf, "method", attrs.method);
    mz_render_svg_attr_str(buf, "spacing", attrs.spacing);
    mz_render_svg_attr_str(buf, "side", attrs.side);

    // Referencing & Hyperlinks
    mz_render_svg_attr_str(buf, "href", attrs.href);
    mz_render_svg_attr_str(buf, "xlink:href", attrs.xlink_href);
    mz_render_svg_attr_str(buf, "target", attrs.target);
    mz_render_svg_attr_str(buf, "rel", attrs.rel);
    mz_render_svg_attr_str(buf, "download", attrs.download);
    mz_render_svg_attr_str(buf, "type", attrs.type);
    mz_render_svg_attr_str(buf, "media", attrs.media);

    // Gradients & Patterns
    mz_render_svg_attr_str(buf, "gradientUnits", attrs.gradientUnits);
    mz_render_svg_attr_str(buf, "gradientTransform", attrs.gradientTransform);
    mz_render_svg_attr_str(buf, "spreadMethod", attrs.spreadMethod);
    mz_render_svg_attr_str(buf, "patternUnits", attrs.patternUnits);
    mz_render_svg_attr_str(buf, "patternContentUnits", attrs.patternContentUnits);
    mz_render_svg_attr_str(buf, "patternTransform", attrs.patternTransform);
    mz_render_svg_attr_str(buf, "offset", attrs.offset);
    mz_render_svg_attr_str(buf, "stop-color", attrs.stop_color);
    mz_render_svg_attr_str(buf, "stop-opacity", attrs.stop_opacity);
    mz_render_svg_attr_str(buf, "fx", attrs.fx);
    mz_render_svg_attr_str(buf, "fy", attrs.fy);
    mz_render_svg_attr_str(buf, "fr", attrs.fr);

    // Markers & ClipPath & Mask
    mz_render_svg_attr_str(buf, "markerUnits", attrs.markerUnits);
    mz_render_svg_attr_str(buf, "markerWidth", attrs.markerWidth);
    mz_render_svg_attr_str(buf, "markerHeight", attrs.markerHeight);
    mz_render_svg_attr_str(buf, "refX", attrs.refX);
    mz_render_svg_attr_str(buf, "refY", attrs.refY);
    mz_render_svg_attr_str(buf, "orient", attrs.orient);
    mz_render_svg_attr_str(buf, "marker-start", attrs.marker_start);
    mz_render_svg_attr_str(buf, "marker-mid", attrs.marker_mid);
    mz_render_svg_attr_str(buf, "marker-end", attrs.marker_end);
    mz_render_svg_attr_str(buf, "clipPathUnits", attrs.clipPathUnits);
    mz_render_svg_attr_str(buf, "maskUnits", attrs.maskUnits);
    mz_render_svg_attr_str(buf, "maskContentUnits", attrs.maskContentUnits);

    // Filter Primitive Attributes
    mz_render_svg_attr_str(buf, "in", attrs.in);
    mz_render_svg_attr_str(buf, "in2", attrs.in2);
    mz_render_svg_attr_str(buf, "result", attrs.result);
    mz_render_svg_attr_str(buf, "mode", attrs.mode);
    mz_render_svg_attr_str(buf, "values", attrs.values);
    mz_render_svg_attr_str(buf, "type", attrs.type_attr);
    mz_render_svg_attr_str(buf, "slope", attrs.slope);
    mz_render_svg_attr_str(buf, "intercept", attrs.intercept);
    mz_render_svg_attr_str(buf, "amplitude", attrs.amplitude);
    mz_render_svg_attr_str(buf, "exponent", attrs.exponent);
    mz_render_svg_attr_str(buf, "tableValues", attrs.tableValues);
    mz_render_svg_attr_str(buf, "operator", attrs.operator_);
    mz_render_svg_attr_str(buf, "k1", attrs.k1);
    mz_render_svg_attr_str(buf, "k2", attrs.k2);
    mz_render_svg_attr_str(buf, "k3", attrs.k3);
    mz_render_svg_attr_str(buf, "k4", attrs.k4);
    mz_render_svg_attr_str(buf, "order", attrs.order);
    mz_render_svg_attr_str(buf, "kernelMatrix", attrs.kernelMatrix);
    mz_render_svg_attr_str(buf, "divisor", attrs.divisor);
    mz_render_svg_attr_str(buf, "bias", attrs.bias);
    mz_render_svg_attr_str(buf, "targetX", attrs.targetX);
    mz_render_svg_attr_str(buf, "targetY", attrs.targetY);
    mz_render_svg_attr_str(buf, "edgeMode", attrs.edgeMode);
    mz_render_svg_attr_str(buf, "preserveAlpha", attrs.preserveAlpha);
    mz_render_svg_attr_str(buf, "surfaceScale", attrs.surfaceScale);
    mz_render_svg_attr_str(buf, "diffuseConstant", attrs.diffuseConstant);
    mz_render_svg_attr_str(buf, "specularConstant", attrs.specularConstant);
    mz_render_svg_attr_str(buf, "specularExponent", attrs.specularExponent);
    mz_render_svg_attr_str(buf, "lighting-color", attrs.lighting_color);
    mz_render_svg_attr_str(buf, "scale", attrs.scale);
    mz_render_svg_attr_str(buf, "xChannelSelector", attrs.xChannelSelector);
    mz_render_svg_attr_str(buf, "yChannelSelector", attrs.yChannelSelector);
    mz_render_svg_attr_str(buf, "stdDeviation", attrs.stdDeviation);
    mz_render_svg_attr_str(buf, "radius", attrs.radius);
    mz_render_svg_attr_str(buf, "baseFrequency", attrs.baseFrequency);
    mz_render_svg_attr_str(buf, "numOctaves", attrs.numOctaves);
    mz_render_svg_attr_str(buf, "seed", attrs.seed);
    mz_render_svg_attr_str(buf, "stitchTiles", attrs.stitchTiles);
    mz_render_svg_attr_str(buf, "azimuth", attrs.azimuth);
    mz_render_svg_attr_str(buf, "elevation", attrs.elevation);
    mz_render_svg_attr_str(buf, "pointsAtX", attrs.pointsAtX);
    mz_render_svg_attr_str(buf, "pointsAtY", attrs.pointsAtY);
    mz_render_svg_attr_str(buf, "pointsAtZ", attrs.pointsAtZ);
    mz_render_svg_attr_str(buf, "limitingConeAngle", attrs.limitingConeAngle);
    mz_render_svg_attr_str(buf, "flood-color", attrs.flood_color);
    mz_render_svg_attr_str(buf, "flood-opacity", attrs.flood_opacity);

    // Animation Attributes
    mz_render_svg_attr_str(buf, "attributeName", attrs.attributeName);
    mz_render_svg_attr_str(buf, "attributeType", attrs.attributeType);
    mz_render_svg_attr_str(buf, "begin", attrs.begin);
    mz_render_svg_attr_str(buf, "dur", attrs.dur);
    mz_render_svg_attr_str(buf, "end", attrs.end);
    mz_render_svg_attr_str(buf, "min", attrs.min);
    mz_render_svg_attr_str(buf, "max", attrs.max);
    mz_render_svg_attr_str(buf, "restart", attrs.restart);
    mz_render_svg_attr_str(buf, "repeatCount", attrs.repeatCount);
    mz_render_svg_attr_str(buf, "repeatDur", attrs.repeatDur);
    mz_render_svg_attr_str(buf, "fill", attrs.fill_anim);
    mz_render_svg_attr_str(buf, "calcMode", attrs.calcMode);
    mz_render_svg_attr_str(buf, "keyTimes", attrs.keyTimes);
    mz_render_svg_attr_str(buf, "keySplines", attrs.keySplines);
    mz_render_svg_attr_str(buf, "from", attrs.from);
    mz_render_svg_attr_str(buf, "to", attrs.to);
    mz_render_svg_attr_str(buf, "by", attrs.by);
    mz_render_svg_attr_str(buf, "additive", attrs.additive);
    mz_render_svg_attr_str(buf, "accumulate", attrs.accumulate);
    mz_render_svg_attr_str(buf, "keyPoints", attrs.keyPoints);
    mz_render_svg_attr_str(buf, "path", attrs.path);
    mz_render_svg_attr_str(buf, "rotate", attrs.rotate);

    // HTMX
    mz_render_svg_attr_str(buf, "hx-get", attrs.hx_get);
    mz_render_svg_attr_str(buf, "hx-post", attrs.hx_post);
    mz_render_svg_attr_str(buf, "hx-put", attrs.hx_put);
    mz_render_svg_attr_str(buf, "hx-delete", attrs.hx_delete);
    mz_render_svg_attr_str(buf, "hx-patch", attrs.hx_patch);
    mz_render_svg_attr_str(buf, "hx-target", attrs.hx_target);
    mz_render_svg_attr_str(buf, "hx-swap", attrs.hx_swap);
    mz_render_svg_attr_str(buf, "hx-trigger", attrs.hx_trigger);

    if (attrs.custom && *attrs.custom) {
        mz_buf_append_char(buf, ' ');
        mz_buf_append_str(buf, attrs.custom);
    }
}
