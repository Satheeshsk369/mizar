#ifndef MIZAR_COMPONENTS_FORM_H
#define MIZAR_COMPONENTS_FORM_H

#include "view/html/tags.h"

typedef struct {
    const char *label;
    const char *name;
    const char *type; // "text", "password", "email", "number", etc.
    const char *placeholder;
    const char *value;
    const char *helper;
    const char *error;
    const char *id;
    const char *cls;
    bool required;
    bool disabled;

    // HTMX triggers on input
    const char *hx_post;
    const char *hx_target;
    const char *hx_trigger; // e.g. "keyup changed delay:300ms"
} UiFieldProps;

static inline void mz_render_ui_field(UiFieldProps p) {
    const char *field_id = p.id ? p.id : p.name;

    Div(.cls = p.cls ? p.cls : "mz-form-field", .style = "display: flex; flex-direction: column; gap: 4px; margin-bottom: 16px;") {
        if (p.label) {
            Label(.for_ = field_id, .style = "font-size: 0.875rem; font-weight: 600; color: #334155;") {
                Text(p.label);
                if (p.required) {
                    Span(.style = "color: #ef4444; margin-left: 2px;") { Text("*"); }
                }
            }
        }

        const char *border_color = p.error ? "#ef4444" : "#cbd5e1";
        char input_style[256];
        snprintf(input_style, sizeof(input_style),
                 "padding: 8px 12px; font-size: 0.95rem; border: 1px solid %s; border-radius: 6px; outline: none; background: #ffffff;",
                 border_color);

        Input(
            .type = p.type ? p.type : "text",
            .id = field_id,
            .name = p.name,
            .placeholder = p.placeholder,
            .value = p.value,
            .required = p.required,
            .disabled = p.disabled,
            .style = input_style,
            .hx_post = p.hx_post,
            .hx_target = p.hx_target,
            .hx_trigger = p.hx_trigger
        );

        if (p.error) {
            Span(.style = "font-size: 0.75rem; color: #ef4444; margin-top: 2px;") {
                Text(p.error);
            }
        } else if (p.helper) {
            Span(.style = "font-size: 0.75rem; color: #64748b; margin-top: 2px;") {
                Text(p.helper);
            }
        }
    }
}

#define UiField(...) mz_render_ui_field((UiFieldProps){ __VA_ARGS__ })

#endif
