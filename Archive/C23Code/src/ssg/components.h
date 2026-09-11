#ifndef MIZAR_SSG_COMPONENTS_H
#define MIZAR_SSG_COMPONENTS_H

#include "core/buffer.h"

// Slot callback function signature for passing rendered children to layouts/components
typedef void (*MizarSlotFn)(void *user_data);

// Generic Layout wrapper macro:
// Usage:
//   PageLayout(buf, "My Page Title") {
//       H1() { Text("Hello World"); }
//   }
#define Layout(buf, outer_macro, ...) \
    for (int _mz_l = (outer_macro(buf, __VA_ARGS__), 0); \
         !_mz_l; \
         _mz_l = 1)

#endif
