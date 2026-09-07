#ifndef MIZAR_H
#define MIZAR_H

#include "core/buffer.h"
#include "core/url.h"
#include "core/livereload.h"
#include "view/html/attrs.h"
#include "view/html/tags.h"
#include "view/svg/attrs.h"
#include "view/svg/tags.h"
#include "view/mathml/attrs.h"
#include "view/mathml/tags.h"
#include "view/css/rules.h"
#include "ssg/fs.h"
#include "ssg/site.h"
#include "ssg/components.h"
#include "server/http.h"
#include "server/app.h"

// HTMX Convenience macros for HTML Attrs
#define HxGet(url)        .hx_get = (url)
#define HxPost(url)       .hx_post = (url)
#define HxPut(url)        .hx_put = (url)
#define HxDelete(url)     .hx_delete = (url)
#define HxPatch(url)      .hx_patch = (url)
#define HxTarget(sel)     .hx_target = (sel)
#define HxSwap(method)    .hx_swap = (method)
#define HxTrigger(evt)    .hx_trigger = (evt)
#define HxVals(vals)      .hx_vals = (vals)
#define HxHeaders(hdrs)   .hx_headers = (hdrs)
#define HxInclude(sel)    .hx_include = (sel)
#define HxSelect(sel)     .hx_select = (sel)
#define HxIndicator(sel)  .hx_indicator = (sel)
#define HxConfirm(msg)    .hx_confirm = (msg)
#define HxPushUrl(url)    .hx_push_url = (url)
#define HxBoost(b)        .hx_boost = (b)
#define HxSync(sel)       .hx_sync = (sel)

#endif
