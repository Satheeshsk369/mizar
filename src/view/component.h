#ifndef MIZAR_VIEW_COMPONENT_H
#define MIZAR_VIEW_COMPONENT_H

#include "core/buffer.h"

// -----------------------------------------------------------------------------
// Declarative UI Component Helper
// -----------------------------------------------------------------------------
// Usage:
//   void Card(CardProps p, void (*slot)(void)) {
//       Div(.cls = "card") {
//           H3() { Text(p.title); }
//           if (slot) slot();
//       }
//   }
//
// In template:
//   Component(Card, (CardProps){ .title = "Live Stats" }) {
//       P() { Text("Real-time metrics."); }
//   }

#define Component(comp_fn, props) \
    for (int _mz_ci = 0; !_mz_ci; ) \
        for (void (^_mz_slot)(void) = nullptr; !_mz_ci; _mz_ci = 1) \
            comp_fn((props), ({ void __fn(void)

#define ComponentSlot() \
    __fn; }))

// Clean declarative macro for components with children:
// Component(MyCard, .title = "Hello") {
//     P() { Text("Card body"); }
// }
#define RenderComponent(comp_fn, props, slot_fn) comp_fn((props), (slot_fn))

#endif
