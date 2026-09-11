#ifndef MIZAR_UI_ICONS_H
#define MIZAR_UI_ICONS_H

#include "view/svg/tags.h"
#include "view/svg/attrs.h"
#include <stdio.h>

// -----------------------------------------------------------------------------
// Declarative SVG Icons API (Lucide/Feather-compatible 24x24 stroke icons)
// -----------------------------------------------------------------------------

typedef struct {
    int size;                // size in px (default 24 if 0)
    const char *color;       // stroke color (e.g. "currentColor", "#38bdf8")
    const char *stroke_width;// stroke-width (default "2")
    const char *cls;         // CSS classes
    const char *style;       // custom style string
} UiIconProps;

static inline void mz_render_icon_svg_open(UiIconProps p) {
    char sz_buf[16];
    int sz_val = p.size > 0 ? p.size : 24;
    snprintf(sz_buf, sizeof(sz_buf), "%d", sz_val);
    const char *sw = (p.stroke_width && *p.stroke_width) ? p.stroke_width : "2";
    const char *col = (p.color && *p.color) ? p.color : "currentColor";

    mz_svg_tag_open("svg", (SvgAttrs){
        .width = sz_buf,
        .height = sz_buf,
        .viewBox = "0 0 24 24",
        .fill = "none",
        .stroke = col,
        .stroke_width = sw,
        .stroke_linecap = "round",
        .stroke_linejoin = "round",
        .cls = p.cls,
        .style = p.style
    });
}

// -----------------------------------------------------------------------------
// 1. Navigation & Directional
// -----------------------------------------------------------------------------
static inline void mz_icon_chevron_right(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m9 18 6-6-6-6");
    mz_svg_tag_close("svg");
}
#define Icon_ChevronRight(...) mz_icon_chevron_right((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_chevron_left(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m15 18-6-6 6-6");
    mz_svg_tag_close("svg");
}
#define Icon_ChevronLeft(...) mz_icon_chevron_left((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_chevron_up(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m18 15-6-6-6 6");
    mz_svg_tag_close("svg");
}
#define Icon_ChevronUp(...) mz_icon_chevron_up((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_chevron_down(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m6 9 6 6 6-6");
    mz_svg_tag_close("svg");
}
#define Icon_ChevronDown(...) mz_icon_chevron_down((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_arrow_right(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M5 12h14m-7-7 7 7-7 7");
    mz_svg_tag_close("svg");
}
#define Icon_ArrowRight(...) mz_icon_arrow_right((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_arrow_left(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m12 19-7-7 7-7m7 7H5");
    mz_svg_tag_close("svg");
}
#define Icon_ArrowLeft(...) mz_icon_arrow_left((UiIconProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 2. Common Actions & Controls
// -----------------------------------------------------------------------------
static inline void mz_icon_search(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m21 21-4.3-4.3M11 19a8 8 0 1 0 0-16 8 8 0 0 0 0 16z");
    mz_svg_tag_close("svg");
}
#define Icon_Search(...) mz_icon_search((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_close(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M18 6 6 18M6 6l12 12");
    mz_svg_tag_close("svg");
}
#define Icon_Close(...) mz_icon_close((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_check(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M20 6 9 17l-5-5");
    mz_svg_tag_close("svg");
}
#define Icon_Check(...) mz_icon_check((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_plus(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M5 12h14m-7-7v14");
    mz_svg_tag_close("svg");
}
#define Icon_Plus(...) mz_icon_plus((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_minus(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M5 12h14");
    mz_svg_tag_close("svg");
}
#define Icon_Minus(...) mz_icon_minus((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_menu(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M4 12h16M4 6h16M4 18h16");
    mz_svg_tag_close("svg");
}
#define Icon_Menu(...) mz_icon_menu((UiIconProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 3. User, Auth & Security
// -----------------------------------------------------------------------------
static inline void mz_icon_user(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M19 21v-2a4 4 0 0 0-4-4H9a4 4 0 0 0-4 4v2m8-10a4 4 0 1 0 0-8 4 4 0 0 0 0 8z");
    mz_svg_tag_close("svg");
}
#define Icon_User(...) mz_icon_user((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_lock(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M19 11H5a2 2 0 0 0-2 2v7a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7a2 2 0 0 0-2-2zm-12 0V7a5 5 0 0 1 10 0v4");
    mz_svg_tag_close("svg");
}
#define Icon_Lock(...) mz_icon_lock((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_unlock(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M19 11H5a2 2 0 0 0-2 2v7a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7a2 2 0 0 0-2-2zm-12 0V7a5 5 0 0 1 9.9-1");
    mz_svg_tag_close("svg");
}
#define Icon_Unlock(...) mz_icon_unlock((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_key(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m21 2-2 2m-1.5 1.5L14 9l-2-2-4 4 2 2-6 6a2.8 2.8 0 0 1-4-4l6-6 2 2 4-4-2-2 3.5-3.5a2.1 2.1 0 1 1 3 3z");
    mz_svg_tag_close("svg");
}
#define Icon_Key(...) mz_icon_key((UiIconProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 4. Content, Files & Media
// -----------------------------------------------------------------------------
static inline void mz_icon_copy(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M8 4h10a2 2 0 0 1 2 2v10m-4 4H6a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h10a2 2 0 0 1 2 2v10a2 2 0 0 1-2 2z");
    mz_svg_tag_close("svg");
}
#define Icon_Copy(...) mz_icon_copy((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_trash(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M3 6h18m-2 0v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2");
    mz_svg_tag_close("svg");
}
#define Icon_Trash(...) mz_icon_trash((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_edit(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M12 20h9M16.5 3.5a2.12 2.12 0 0 1 3 3L7 19l-4 1 1-4Z");
    mz_svg_tag_close("svg");
}
#define Icon_Edit(...) mz_icon_edit((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_file(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M15 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7Z");
    mz_svg_tag_close("svg");
}
#define Icon_File(...) mz_icon_file((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_folder(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M4 20h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.93a2 2 0 0 1-1.66-.9l-.82-1.2A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2z");
    mz_svg_tag_close("svg");
}
#define Icon_Folder(...) mz_icon_folder((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_download(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4m4-5 5 5 5-5m-5 5V3");
    mz_svg_tag_close("svg");
}
#define Icon_Download(...) mz_icon_download((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_upload(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4m14-7-5-5-5 5m5-5v12");
    mz_svg_tag_close("svg");
}
#define Icon_Upload(...) mz_icon_upload((UiIconProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 5. System, Status & Communication
// -----------------------------------------------------------------------------
static inline void mz_icon_heart(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M19 14c1.49-1.46 3-3.21 3-5.5A5.5 5.5 0 0 0 16.5 3c-1.76 0-3 .5-4.5 2-1.5-1.5-2.74-2-4.5-2A5.5 5.5 0 0 0 2 8.5c0 2.3 1.5 4.05 3 5.5l7 7Z");
    mz_svg_tag_close("svg");
}
#define Icon_Heart(...) mz_icon_heart((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_star(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M12 2l3.09 6.26L22 9.27l-5 4.87 1.18 6.88L12 17.77l-6.18 3.25L7 14.14 2 9.27l6.91-1.01L12 2z");
    mz_svg_tag_close("svg");
}
#define Icon_Star(...) mz_icon_star((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_bell(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9m-4.27 13a2 2 0 0 1-3.46 0");
    mz_svg_tag_close("svg");
}
#define Icon_Bell(...) mz_icon_bell((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_mail(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2zm0 2 8 5 8-5");
    mz_svg_tag_close("svg");
}
#define Icon_Mail(...) mz_icon_mail((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_alert_circle(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M12 22a10 10 0 1 0 0-20 10 10 0 0 0 0 20zm0-13v4m0 4h.01");
    mz_svg_tag_close("svg");
}
#define Icon_AlertCircle(...) mz_icon_alert_circle((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_info(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M12 22a10 10 0 1 0 0-20 10 10 0 0 0 0 20zm0-15h.01m-.01 4v6");
    mz_svg_tag_close("svg");
}
#define Icon_Info(...) mz_icon_info((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_settings(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2zM12 15a3 3 0 1 0 0-6 3 3 0 0 0 0 6z");
    mz_svg_tag_close("svg");
}
#define Icon_Settings(...) mz_icon_settings((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_external_link(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6m4-3h6v6m-11 5L21 3");
    mz_svg_tag_close("svg");
}
#define Icon_ExternalLink(...) mz_icon_external_link((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_terminal(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m4 17 6-6-6-6m8 14h8");
    mz_svg_tag_close("svg");
}
#define Icon_Terminal(...) mz_icon_terminal((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_code(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "m16 18 6-6-6-6M8 6l-6 6 6 6");
    mz_svg_tag_close("svg");
}
#define Icon_Code(...) mz_icon_code((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_refresh(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M3 12a9 9 0 0 1 9-9 9.75 9.75 0 0 1 6.74 2.74L21 8M21 3v5h-5m5 4a9 9 0 0 1-9 9 9.75 9.75 0 0 1-6.74-2.74L3 16M3 21v-5h5");
    mz_svg_tag_close("svg");
}
#define Icon_Refresh(...) mz_icon_refresh((UiIconProps){ __VA_ARGS__ })

// -----------------------------------------------------------------------------
// 6. Theme & Weather
// -----------------------------------------------------------------------------
static inline void mz_icon_sun(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Circle_(.cx = "12", .cy = "12", .r = "5");
    Svg_Path_(.d = "M12 1v2m0 18v2M4.22 4.22l1.42 1.42m12.72 12.72 1.42 1.42M1 12h2m18 0h2M4.22 19.78l1.42-1.42M18.36 5.64l1.42-1.42");
    mz_svg_tag_close("svg");
}
#define Icon_Sun(...) mz_icon_sun((UiIconProps){ __VA_ARGS__ })

static inline void mz_icon_moon(UiIconProps p) {
    mz_render_icon_svg_open(p);
    Svg_Path_(.d = "M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z");
    mz_svg_tag_close("svg");
}
#define Icon_Moon(...) mz_icon_moon((UiIconProps){ __VA_ARGS__ })

#endif // MIZAR_UI_ICONS_H
