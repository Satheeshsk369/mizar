#ifndef MIZAR_VIEW_COMPONENT_H
#define MIZAR_VIEW_COMPONENT_H

#include "core/buffer.h"

// -----------------------------------------------------------------------------
// Declarative React-like UI Components in C23
// -----------------------------------------------------------------------------
//
// Pattern 1: Standalone Self-Closing Component (like <Avatar src="..." />)
// -----------------------------------------------------------------------------
//   typedef struct { const char *src; const char *alt; int size; } AvatarProps;
//   void Avatar(AvatarProps p) {
//       Img_(.src = p.src, .alt = p.alt, .width = "40", .height = "40", .cls = "rounded-full");
//   }
//   // Usage:
//   Avatar((AvatarProps){ .src = "/user.png", .alt = "Alice" });
//
// Pattern 2: Component with Child Slot / Children (like <Card title="...">...</Card>)
// -----------------------------------------------------------------------------
//   typedef struct { const char *title; const char *badge; } CardProps;
//   void Card(CardProps p, void (*slot)(void)) {
//       Div(.cls = "card") {
//           Div(.cls = "card-header") {
//               H3() { Text(p.title); }
//               if (p.badge) Span(.cls = "badge") { Text(p.badge); }
//           }
//           Div(.cls = "card-body") {
//               if (slot) slot();
//           }
//       }
//   }
//
//   // Usage in template with RAII-style children block:
//   UseComponent(Card, (CardProps){ .title = "Analytics", .badge = "Live" }) {
//       P() { Text("Active users: 1,420"); }
//   }
// -----------------------------------------------------------------------------

typedef void (*MzSlotFn)(void);

static inline void mz_component_call_with_slot(void (*comp_fn)(void *props, MzSlotFn slot), void *props, MzSlotFn slot) {
    comp_fn(props, slot);
}

// Seamless macro for wrapping components that take (props, slot)
#define UseComponent(comp_fn, props_val) \
    for (int _mz_c_step = 0; _mz_c_step == 0; ) \
        for (auto _mz_c_p = (props_val); _mz_c_step == 0; _mz_c_step = 1) \
            for (void _mz_c_runner(void) { \
                    void (*_c_fn)(decltype(_mz_c_p), MzSlotFn) = (comp_fn); \
                    void _mz_c_slot(void)

#define EndComponent \
                    ; \
                    _c_fn(_mz_c_p, _mz_c_slot); \
                 }; _mz_c_step == 0; _mz_c_step = 1) _mz_c_runner();

// Ultra-clean Component wrapper:
// Component(MyCard, (MyCardProps){ .title = "Hello" }, {
//     P() { Text("Child content"); }
// });
#define Component(comp_fn, props, child_code) \
    do { \
        auto _mz_props = (props); \
        void _mz_child_slot(void) { child_code; } \
        (comp_fn)(_mz_props, _mz_child_slot); \
    } while (0)

#endif
