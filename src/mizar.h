#ifndef MIZAR_H
#define MIZAR_H

#include "buffer.h"
#include "attrs.h"
#include "tags.h"

// HTMX & Data attribute helper macro conveniences
#define HxGet(url)      .hx_get = (url)
#define HxPost(url)     .hx_post = (url)
#define HxPut(url)      .hx_put = (url)
#define HxDelete(url)   .hx_delete = (url)
#define HxPatch(url)    .hx_patch = (url)
#define HxTarget(sel)   .hx_target = (sel)
#define HxSwap(method)  .hx_swap = (method)
#define HxTrigger(evt)  .hx_trigger = (evt)

#endif // MIZAR_H
