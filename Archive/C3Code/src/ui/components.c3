module mizar::ui::components;
import mizar::view::html;
import mizar::ui::icons;

// 1. Button Variants
struct ButtonProps {
    String class;
    String style;
    String variant; // "primary", "secondary", "danger", "outline"
    String hx_get;
    String hx_post;
    String hx_target;
    String hx_swap;
}

macro void @button(ButtonProps p = {}; @body()) {
    String st = "padding: 8px 16px; border-radius: 6px; font-weight: 500; cursor: pointer; border: 1px solid transparent; display: inline-flex; align-items: center; gap: 8px; background: #0284c7; color: white;";

    if (p.variant == "secondary") {
        st = "padding: 8px 16px; border-radius: 6px; font-weight: 500; cursor: pointer; display: inline-flex; align-items: center; gap: 8px; background: #e2e8f0; color: #1e293b; border: 1px solid #cbd5e1;";
    } else if (p.variant == "danger") {
        st = "padding: 8px 16px; border-radius: 6px; font-weight: 500; cursor: pointer; display: inline-flex; align-items: center; gap: 8px; background: #ef4444; color: white; border: 1px solid transparent;";
    } else if (p.variant == "outline") {
        st = "padding: 8px 16px; border-radius: 6px; font-weight: 500; cursor: pointer; display: inline-flex; align-items: center; gap: 8px; background: transparent; color: #0284c7; border: 1px solid #0284c7;";
    }

    html::@button({
        .class = p.class,
        .style = st,
        .hx_get = p.hx_get,
        .hx_post = p.hx_post,
        .hx_target = p.hx_target,
        .hx_swap = p.hx_swap
    }) {
        @body();
    };
}

// 2. Badge
struct BadgeProps {
    String class;
    String variant; // "info", "success", "danger"
}

macro void @badge(BadgeProps p = {}; @body()) {
    String b_style = "display: inline-flex; align-items: center; gap: 4px; padding: 2px 8px; border-radius: 9999px; font-size: 0.75rem; font-weight: 600; background: #e0f2fe; color: #0369a1;";

    if (p.variant == "success") {
        b_style = "display: inline-flex; align-items: center; gap: 4px; padding: 2px 8px; border-radius: 9999px; font-size: 0.75rem; font-weight: 600; background: #dcfce7; color: #15803d;";
    } else if (p.variant == "danger") {
        b_style = "display: inline-flex; align-items: center; gap: 4px; padding: 2px 8px; border-radius: 9999px; font-size: 0.75rem; font-weight: 600; background: #fee2e2; color: #b91c1c;";
    }

    html::@span({ .class = p.class, .style = b_style }) {
        @body();
    };
}

// 3. Card
struct CardProps {
    String class;
    String style;
}

macro void @card(CardProps p = {}; @body()) {
    String c_style = "background: white; border: 1px solid #e2e8f0; border-radius: 8px; padding: 24px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); max-width: 640px; margin: 0 auto;";
    html::@div({ .class = p.class, .style = c_style }) {
        @body();
    };
}

// 4. Alert Box
struct AlertProps {
    String title;
    String variant; // "info", "success", "error"
}

macro void @alert(AlertProps p = {}; @body()) {
    String al_style = "padding: 12px 16px; border-left: 4px solid #38bdf8; background: #f0f9ff; border-radius: 4px; margin: 12px 0;";

    if (p.variant == "success") {
        al_style = "padding: 12px 16px; border-left: 4px solid #4ade80; background: #f0fdf4; border-radius: 4px; margin: 12px 0;";
    } else if (p.variant == "error") {
        al_style = "padding: 12px 16px; border-left: 4px solid #f87171; background: #fef2f2; border-radius: 4px; margin: 12px 0;";
    }

    html::@div({ .style = al_style }) {
        if (p.title.len > 0) {
            html::@div({ .style = "font-weight: 600; margin-bottom: 4px; color: #0f172a;" }) {
                html::text(p.title);
            };
        }
        @body();
    };
}

// 5. Modal Dialog Overlay
struct ModalProps {
    String id;
    String title;
}

macro void @modal(ModalProps p; @body()) {
    html::@div({
        .id = p.id,
        .style = "display: flex; position: fixed; inset: 0; background: rgba(15, 23, 42, 0.5); align-items: center; justify-content: center; z-index: 50; backdrop-filter: blur(2px);"
    }) {
        html::@div({
            .style = "background: white; border-radius: 8px; max-width: 480px; width: 100%; box-shadow: 0 20px 25px -5px rgba(0, 0, 0, 0.1); overflow: hidden; margin: 16px;"
        }) {
            html::@div({
                .style = "display: flex; align-items: center; justify-content: space-between; padding: 16px 20px; border-bottom: 1px solid #e2e8f0;"
            }) {
                html::@h3({ .style = "margin: 0; font-size: 1.1rem; font-weight: 600;" }) {
                    html::text(p.title);
                };
                html::@button({
                    .style = "background: none; border: none; cursor: pointer; color: #64748b; padding: 4px;",
                    .hx_target = "#" + p.id,
                    .hx_swap = "delete"
                }) {
                    icons::@icon_close({ .size = 18 });
                };
            };
            html::@div({ .style = "padding: 20px;" }) {
                @body();
            };
        };
    };
}

// 6. Form Input Field
struct InputProps {
    String label;
    String name;
    String type;
    String placeholder;
    String value;
    String hx_post;
    String hx_target;
}

macro void @form_field(InputProps p) {
    html::@div({ .style = "margin-bottom: 16px;" }) {
        if (p.label.len > 0) {
            html::@label({ .style = "display: block; font-size: 0.875rem; font-weight: 500; margin-bottom: 6px; color: #334155;" }) {
                html::text(p.label);
            };
        }
        html::@input({
            .type = p.type.len > 0 ? p.type : "text",
            .name = p.name,
            .placeholder = p.placeholder,
            .value = p.value,
            .hx_post = p.hx_post,
            .hx_target = p.hx_target,
            .style = "width: 100%; box-sizing: border-box; padding: 8px 12px; border: 1px solid #cbd5e1; border-radius: 6px; outline: none; font-size: 0.95rem;"
        });
    };
}

// 7. Navbar
struct NavbarProps {
    String title;
    String brand_url;
}

macro void @navbar(NavbarProps p = {}; @body()) {
    html::@nav({ .style = "background: white; border-bottom: 1px solid #e2e8f0; padding: 12px 24px; display: flex; align-items: center; justify-content: space-between;" }) {
        html::@div({ .style = "display: flex; align-items: center; gap: 12px;" }) {
            html::@a({ .href = p.brand_url.len > 0 ? p.brand_url : "/", .style = "font-weight: 700; font-size: 1.2rem; color: #0284c7; text-decoration: none;" }) {
                html::text(p.title.len > 0 ? p.title : "Mizar App");
            };
        };
        html::@div({ .style = "display: flex; align-items: center; gap: 16px;" }) {
            @body();
        };
    };
}

// 8. Interactive Tabs Switcher
struct TabItem {
    String label;
    String target_url;
    bool active;
}

macro void @tabs(TabItem[] items, String content_target = "#tab-content") {
    html::@div({ .style = "display: flex; border-bottom: 2px solid #e2e8f0; gap: 8px; margin-bottom: 20px;" }) {
        foreach (TabItem item : items) {
            String tab_style = item.active 
                ? "padding: 10px 16px; background: none; border: none; cursor: pointer; text-decoration: none; display: inline-block; border-bottom: 2px solid #0284c7; color: #0284c7; font-weight: 600;" 
                : "padding: 10px 16px; background: none; border: none; cursor: pointer; text-decoration: none; display: inline-block; border-bottom: 2px solid transparent; color: #64748b;";
            html::@button({
                .style = tab_style,
                .hx_get = item.target_url,
                .hx_target = content_target,
                .hx_swap = "innerHTML"
            }) {
                html::text(item.label);
            };
        }
    };
}

// 9. Data Table Container
macro void @data_table(; @body()) {
    html::@div({ .style = "width: 100%; overflow-x: auto; border: 1px solid #e2e8f0; border-radius: 8px;" }) {
        html::@table({ .style = "width: 100%; border-collapse: collapse; text-align: left; font-size: 0.9rem;" }) {
            @body();
        };
    };
}
