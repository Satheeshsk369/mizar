#ifndef MIZAR_H
#define MIZAR_H

#include "core/buffer.h"
#include "view/html/attrs.h"
#include "view/html/tags.h"
#include "view/css/rules.h"

#define HxGet(url)      .hx_get = (url)
#define HxPost(url)     .hx_post = (url)
#define HxPut(url)      .hx_put = (url)
#define HxDelete(url)   .hx_delete = (url)
#define HxPatch(url)    .hx_patch = (url)
#define HxTarget(sel)   .hx_target = (sel)
#define HxSwap(method)  .hx_swap = (method)
#define HxTrigger(evt)  .hx_trigger = (evt)

#endif
