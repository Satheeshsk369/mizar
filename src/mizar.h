#ifndef MIZAR_H
#define MIZAR_H

#include "core/buffer.h"
#include "core/url.h"
#include "core/json.h"
#include "core/livereload.h"
#include "core/htmx.h"
#include "view/html/attrs.h"
#include "view/html/tags.h"
#include "view/svg/attrs.h"
#include "view/svg/tags.h"
#include "view/mathml/attrs.h"
#include "view/mathml/tags.h"
#include "view/equation.h"
#include "view/component.h"
#include "view/css/rules.h"
#include "ui/geometry.h"
#include "ui/icons.h"
#include "ui/components.h"
#include "ssg/fs.h"
#include "ssg/site.h"
#include "ssg/components.h"
#include "server/http.h"
#include "server/app.h"

// HTMX Convenience macros for HTML Attrs (HTMX 4 & HTMX 2 Compatible)
#define HxGet(url)              .hx_get = (url)
#define HxPost(url)             .hx_post = (url)
#define HxPut(url)              .hx_put = (url)
#define HxDelete(url)           .hx_delete = (url)
#define HxPatch(url)            .hx_patch = (url)
#define HxQuery(url)            .hx_query = (url)
#define HxAction(url)           .hx_action = (url)
#define HxMethod(meth)          .hx_method = (meth)

#define HxTarget(sel)           .hx_target = (sel)
#define HxSwap(method)          .hx_swap = (method)
#define HxTrigger(evt)          .hx_trigger = (evt)
#define HxVals(vals)            .hx_vals = (vals)
#define HxHeaders(hdrs)         .hx_headers = (hdrs)
#define HxInclude(sel)          .hx_include = (sel)
#define HxSelect(sel)           .hx_select = (sel)
#define HxSelectOob(sel)        .hx_select_oob = (sel)
#define HxIndicator(sel)        .hx_indicator = (sel)
#define HxConfirm(msg)          .hx_confirm = (msg)
#define HxPushUrl(url)          .hx_push_url = (url)
#define HxReplaceUrl(url)       .hx_replace_url = (url)
#define HxBoost(b)              .hx_boost = (b)
#define HxSync(sel)             .hx_sync = (sel)
#define HxConfig(cfg)           .hx_config = (cfg)

// HTMX 4 Element Disabling & Compatibility Aliases
#define HxDisable(sel)          .hx_disable = (sel)
#define HxDisabledElt(sel)      .hx_disabled_elt = (sel) // HTMX 2 alias -> renders hx-disable

#define HxPreload(evt)          .hx_preload = (evt)
#define HxPending(sel)          .hx_pending = (sel)
#define HxEncoding(enc)         .hx_encoding = (enc)
#define HxValidate(v)           .hx_validate = (v)
#define HxHistoryElt(sel)       .hx_history_elt = (sel)
#define HxOn(evt_expr)          .hx_on = (evt_expr)

// HTMX 4 Explicit Inheritance Modifiers (:inherited)
#define HxTargetInherited(sel)    .hx_target_inherited = (sel)
#define HxIncludeInherited(sel)   .hx_include_inherited = (sel)
#define HxSwapInherited(m)        .hx_swap_inherited = (m)
#define HxHeadersInherited(hdrs)  .hx_headers_inherited = (hdrs)
#define HxValsInherited(vals)     .hx_vals_inherited = (vals)
#define HxConfirmInherited(msg)   .hx_confirm_inherited = (msg)
#define HxIndicatorInherited(sel) .hx_indicator_inherited = (sel)
#define HxSyncInherited(sel)      .hx_sync_inherited = (sel)
#define HxConfigInherited(cfg)    .hx_config_inherited = (cfg)
#define HxBoostInherited(b)       .hx_boost_inherited = (b)
#define HxDisableInherited(sel)   .hx_disable_inherited = (sel)
#define HxEncodingInherited(enc)  .hx_encoding_inherited = (enc)
#define HxValidateInherited(v)    .hx_validate_inherited = (v)

// HTMX 4 Append Modifiers (:append and :inherited:append)
#define HxIncludeAppend(sel)               .hx_include_append = (sel)
#define HxIncludeInheritedAppend(sel)      .hx_include_inherited_append = (sel)
#define HxHeadersAppend(hdrs)              .hx_headers_append = (hdrs)
#define HxHeadersInheritedAppend(hdrs)     .hx_headers_inherited_append = (hdrs)
#define HxValsAppend(vals)                 .hx_vals_append = (vals)
#define HxValsInheritedAppend(vals)        .hx_vals_inherited_append = (vals)

// HTMX 4 Status Response Handlers (hx-status:XXX)
#define HxStatus200(spec)       .hx_status_200 = (spec)
#define HxStatus204(spec)       .hx_status_204 = (spec)
#define HxStatus304(spec)       .hx_status_304 = (spec)
#define HxStatus400(spec)       .hx_status_400 = (spec)
#define HxStatus401(spec)       .hx_status_401 = (spec)
#define HxStatus403(spec)       .hx_status_403 = (spec)
#define HxStatus404(spec)       .hx_status_404 = (spec)
#define HxStatus422(spec)       .hx_status_422 = (spec)
#define HxStatus4xx(spec)       .hx_status_4xx = (spec)
#define HxStatus500(spec)       .hx_status_500 = (spec)
#define HxStatus502(spec)       .hx_status_502 = (spec)
#define HxStatus503(spec)       .hx_status_503 = (spec)
#define HxStatus5xx(spec)       .hx_status_5xx = (spec)

// HTMX Flags
#define HxPreserve(b)           .hx_preserve = (b)
#define HxIgnore(b)             .hx_ignore = (b)
#define HxMorphSkip(b)          .hx_morph_skip = (b)
#define HxMorphSkipChildren(b)  .hx_morph_skip_children = (b)

#endif
