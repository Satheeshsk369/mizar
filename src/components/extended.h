#ifndef MIZAR_COMPONENTS_EXTENDED_H
#define MIZAR_COMPONENTS_EXTENDED_H

#include "view/html/tags.h"

// -----------------------------------------------------------------------------
// 1. Checkbox: Semantic checkbox with label
// -----------------------------------------------------------------------------
typedef struct {
    const char *label;
    const char *name;
    const char *id;
    bool checked;
    bool disabled;
    const char *hx_post;
    const char *hx_target;
} UiCheckboxProps;

static inline void mz_render_ui_checkbox(UiCheckboxProps p) {
    const char *cid = p.id ? p.id : p.name;
    Label(.style = "display: inline-flex; align-items: center; gap: 8px; font-size: 0.875rem; cursor: pointer; user-select: none; color: #1e293b;") {
        Input(
            .type = "checkbox",
            .id = cid,
            .name = p.name,
            .checked = p.checked,
            .disabled = p.disabled,
            .style = "width: 16px; height: 16px; accent-color: #0284c7; cursor: pointer;",
            .hx_post = p.hx_post,
            .hx_target = p.hx_target
        );
        if (p.label) Text(p.label);
    }
}

#define UiCheckbox(...) mz_render_ui_checkbox((UiCheckboxProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 2. Switch / Toggle: iOS-style slider switch
// -----------------------------------------------------------------------------
typedef struct {
    const char *label;
    const char *name;
    const char *id;
    bool checked;
    const char *hx_post;
    const char *hx_target;
} UiSwitchProps;

static inline void mz_render_ui_switch(UiSwitchProps p) {
    const char *sid = p.id ? p.id : p.name;
    Label(.style = "display: inline-flex; align-items: center; gap: 10px; cursor: pointer; font-size: 0.875rem; color: #1e293b;") {
        Input(
            .type = "checkbox",
            .id = sid,
            .name = p.name,
            .checked = p.checked,
            .style = "appearance: none; width: 36px; height: 20px; background: #cbd5e1; border-radius: 9999px; position: relative; outline: none; cursor: pointer; transition: background 0.2s;",
            .hx_post = p.hx_post,
            .hx_target = p.hx_target
        );
        if (p.label) Text(p.label);
    }
}

#define UiSwitch(...) mz_render_ui_switch((UiSwitchProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 3. Textarea Form Field
// -----------------------------------------------------------------------------
typedef struct {
    const char *label;
    const char *name;
    const char *placeholder;
    const char *value;
    int rows;
    bool required;
    const char *id;
} UiTextareaProps;

static inline void mz_render_ui_textarea(UiTextareaProps p) {
    const char *tid = p.id ? p.id : p.name;
    char rows_str[16];
    snprintf(rows_str, sizeof(rows_str), "%d", p.rows > 0 ? p.rows : 4);

    Div(.style = "display: flex; flex-direction: column; gap: 4px; margin-bottom: 16px;") {
        if (p.label) {
            Label(.for_ = tid, .style = "font-size: 0.875rem; font-weight: 600; color: #334155;") {
                Text(p.label);
                if (p.required) Span(.style = "color: #ef4444; margin-left: 2px;") { Text("*"); }
            }
        }
        Textarea(
            .id = tid,
            .name = p.name,
            .placeholder = p.placeholder,
            .rows = rows_str,
            .required = p.required,
            .style = "padding: 8px 12px; font-size: 0.95rem; border: 1px solid #cbd5e1; border-radius: 6px; outline: none; background: #ffffff; resize: vertical; font-family: inherit;"
        ) {
            if (p.value) Text(p.value);
        }
    }
}

#define UiTextarea(...) mz_render_ui_textarea((UiTextareaProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 4. Dropdown Select Field
// -----------------------------------------------------------------------------
typedef struct {
    const char *label;
    const char *val;
    bool selected;
} UiSelectOption;

typedef struct {
    const char *label;
    const char *name;
    const char *id;
    const UiSelectOption *options;
    size_t option_count;
    const char *hx_post;
    const char *hx_target;
} UiSelectProps;

static inline void mz_render_ui_select(UiSelectProps p) {
    const char *sid = p.id ? p.id : p.name;
    Div(.style = "display: flex; flex-direction: column; gap: 4px; margin-bottom: 16px;") {
        if (p.label) {
            Label(.for_ = sid, .style = "font-size: 0.875rem; font-weight: 600; color: #334155;") {
                Text(p.label);
            }
        }
        Select(
            .id = sid,
            .name = p.name,
            .style = "padding: 8px 12px; font-size: 0.95rem; border: 1px solid #cbd5e1; border-radius: 6px; outline: none; background: #ffffff;",
            .hx_post = p.hx_post,
            .hx_target = p.hx_target
        ) {
            for (size_t i = 0; i < p.option_count; i++) {
                Option(.value = p.options[i].val, .selected = p.options[i].selected) {
                    Text(p.options[i].label);
                }
            }
        }
    }
}

#define UiSelect(...) mz_render_ui_select((UiSelectProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 5. Tooltip (Pure CSS hover hint)
// -----------------------------------------------------------------------------
static inline void mz_ui_tooltip_open(const char *hint_text) {
    mz_tag_open("span", (Attrs){
        .style = "position: relative; display: inline-block; cursor: help; border-bottom: 1px dotted #64748b;",
        .title = hint_text
    });
}

#define UiTooltip(hint) \
    for (int _mz_ui_tt = (mz_ui_tooltip_open(hint), 0); !_mz_ui_tt; _mz_ui_tt = 1, mz_tag_close("span"))

// -----------------------------------------------------------------------------
// 6. Toast: Floating corner notification banner
// -----------------------------------------------------------------------------
typedef struct {
    const char *title;
    const char *message;
    bool success;
} UiToastProps;

static inline void mz_render_ui_toast(UiToastProps p) {
    const char *border_col = p.success ? "#22c55e" : "#0284c7";
    char toast_style[512];
    snprintf(toast_style, sizeof(toast_style),
             "position: fixed; bottom: 24px; right: 24px; z-index: 999; background: #ffffff; border: 1px solid #e2e8f0; border-left: 4px solid %s; padding: 14px 20px; border-radius: 8px; box-shadow: 0 10px 15px -3px rgba(0,0,0,0.1); max-width: 360px;",
             border_col);

    Div(.cls = "mz-toast", .style = toast_style) {
        if (p.title) {
            Div(.style = "font-weight: 700; font-size: 0.95rem; color: #0f172a; margin-bottom: 2px;") {
                Text(p.title);
            }
        }
        if (p.message) {
            Div(.style = "font-size: 0.875rem; color: #64748b;") {
                Text(p.message);
            }
        }
    }
}

#define UiToast(...) mz_render_ui_toast((UiToastProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 7. Hero Section (Landing page hero banner)
// -----------------------------------------------------------------------------
typedef struct {
    const char *badge;
    const char *title;
    const char *tagline;
} UiHeroProps;

static inline void mz_ui_hero_open(UiHeroProps p) {
    mz_tag_open("section", (Attrs){
        .cls = "mz-hero",
        .style = "text-align: center; padding: 64px 20px; max-width: 800px; margin: 0 auto;"
    });

    if (p.badge) {
        mz_tag_open("span", (Attrs){
            .style = "display: inline-block; padding: 4px 12px; font-size: 0.8rem; font-weight: 600; border-radius: 9999px; background: #e0f2fe; color: #0369a1; margin-bottom: 16px;"
        });
        Text(p.badge);
        mz_tag_close("span");
    }

    if (p.title) {
        mz_tag_open("h1", (Attrs){
            .style = "font-size: 2.75rem; font-weight: 800; letter-spacing: -0.03em; color: #0f172a; margin-bottom: 16px; line-height: 1.2;"
        });
        Text(p.title);
        mz_tag_close("h1");
    }

    if (p.tagline) {
        mz_tag_open("p", (Attrs){
            .style = "font-size: 1.25rem; color: #64748b; line-height: 1.6; margin-bottom: 28px;"
        });
        Text(p.tagline);
        mz_tag_close("p");
    }

    mz_tag_open("div", (Attrs){ .style = "display: flex; justify-content: center; gap: 12px;" });
}

static inline void mz_ui_hero_close(void) {
    mz_tag_close("div");
    mz_tag_close("section");
}

#define UiHero(...) \
    for (int _mz_ui_hr = (mz_ui_hero_open((UiHeroProps){ __VA_ARGS__ }), 0); !_mz_ui_hr; _mz_ui_hr = 1, mz_ui_hero_close())

// -----------------------------------------------------------------------------
// 8. Testimonial / Review Quote Card
// -----------------------------------------------------------------------------
typedef struct {
    const char *quote;
    const char *author;
    const char *role;
    const char *avatar_initials;
} UiTestimonialProps;

static inline void mz_render_ui_testimonial(UiTestimonialProps p) {
    Div(.cls = "mz-testimonial", .style = "background: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; padding: 24px; box-shadow: 0 1px 3px rgba(0,0,0,0.05); margin-bottom: 16px;") {
        P(.style = "font-size: 1.05rem; color: #334155; font-style: italic; line-height: 1.6; margin-bottom: 16px;") {
            Text("“%s”", p.quote ? p.quote : "");
        }
        Div(.style = "display: flex; align-items: center; gap: 12px;") {
            if (p.avatar_initials) {
                Div(.style = "width: 40px; height: 40px; border-radius: 50%; background: #0284c7; color: #fff; display: flex; align-items: center; justify-content: center; font-weight: 700;") {
                    Text(p.avatar_initials);
                }
            }
            Div() {
                Div(.style = "font-weight: 700; font-size: 0.95rem; color: #0f172a;") {
                    Text(p.author ? p.author : "");
                }
                if (p.role) {
                    Div(.style = "font-size: 0.8rem; color: #64748b;") {
                        Text(p.role);
                    }
                }
            }
        }
    }
}

#define UiTestimonial(...) mz_render_ui_testimonial((UiTestimonialProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 9. Pricing Card Column
// -----------------------------------------------------------------------------
typedef struct {
    const char *plan;
    const char *price;
    const char *period; // e.g. "/month"
    const char *desc;
    bool popular;
} UiPricingProps;

static inline void mz_ui_pricing_open(UiPricingProps p) {
    const char *border = p.popular ? "2px solid #0284c7" : "1px solid #e2e8f0";
    char style[512];
    snprintf(style, sizeof(style),
             "background: #ffffff; border: %s; border-radius: 12px; padding: 32px 24px; position: relative; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.05);",
             border);

    mz_tag_open("div", (Attrs){ .cls = "mz-pricing-card", .style = style });

    if (p.popular) {
        mz_tag_open("span", (Attrs){
            .style = "position: absolute; top: -12px; left: 50%; transform: translateX(-50%); background: #0284c7; color: #ffffff; padding: 2px 12px; font-size: 0.75rem; font-weight: 700; border-radius: 9999px; text-transform: uppercase;"
        });
        Text("Most Popular");
        mz_tag_close("span");
    }

    if (p.plan) {
        mz_tag_open("h3", (Attrs){ .style = "font-size: 1.25rem; font-weight: 700; color: #0f172a; margin-bottom: 8px;" });
        Text(p.plan);
        mz_tag_close("h3");
    }

    if (p.desc) {
        mz_tag_open("p", (Attrs){ .style = "font-size: 0.875rem; color: #64748b; margin-bottom: 20px;" });
        Text(p.desc);
        mz_tag_close("p");
    }

    if (p.price) {
        mz_tag_open("div", (Attrs){ .style = "margin-bottom: 24px; display: flex; align-items: baseline; gap: 4px;" });
        mz_tag_open("span", (Attrs){ .style = "font-size: 2.5rem; font-weight: 800; color: #0f172a;" });
        Text(p.price);
        mz_tag_close("span");
        if (p.period) {
            mz_tag_open("span", (Attrs){ .style = "font-size: 0.875rem; color: #64748b;" });
            Text(p.period);
            mz_tag_close("span");
        }
        mz_tag_close("div");
    }

    mz_tag_open("div", (Attrs){ .cls = "mz-pricing-features", .style = "margin-bottom: 28px;" });
}

static inline void mz_ui_pricing_close(void) {
    mz_tag_close("div");
    mz_tag_close("div");
}

#define UiPricing(...) \
    for (int _mz_ui_pr = (mz_ui_pricing_open((UiPricingProps){ __VA_ARGS__ }), 0); !_mz_ui_pr; _mz_ui_pr = 1, mz_ui_pricing_close())

// -----------------------------------------------------------------------------
// 10. Live Search Input (HTMX Instant Typeahead)
// -----------------------------------------------------------------------------
typedef struct {
    const char *placeholder;
    const char *name;
    const char *hx_post_url;
    const char *hx_target;
    int delay_ms; // default 300
} UiLiveSearchProps;

static inline void mz_render_ui_live_search(UiLiveSearchProps p) {
    int del = p.delay_ms > 0 ? p.delay_ms : 300;
    char trigger[128];
    snprintf(trigger, sizeof(trigger), "keyup changed delay:%dms, search", del);

    Div(.style = "position: relative; width: 100%; max-width: 400px;") {
        Input(
            .type = "search",
            .name = p.name ? p.name : "q",
            .placeholder = p.placeholder ? p.placeholder : "Search...",
            .style = "width: 100%; padding: 10px 16px; font-size: 0.95rem; border: 1px solid #cbd5e1; border-radius: 8px; outline: none;",
            .hx_post = p.hx_post_url,
            .hx_target = p.hx_target,
            .hx_trigger = trigger
        );
    }
}

#define UiLiveSearch(...) mz_render_ui_live_search((UiLiveSearchProps){ __VA_ARGS__ })

#endif
