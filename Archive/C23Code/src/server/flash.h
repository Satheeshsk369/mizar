#ifndef MIZAR_SERVER_FLASH_H
#define MIZAR_SERVER_FLASH_H

#include "server/http.h"
#include "view/html/tags.h"
#include "view/js/dsl.h"
#include <stdio.h>

typedef enum {
    MZ_FLASH_INFO = 0,
    MZ_FLASH_SUCCESS,
    MZ_FLASH_WARNING,
    MZ_FLASH_ERROR
} MzFlashType;

// Sets a flash message via HTMX HX-Trigger header with JSON payload
static inline void mz_res_flash(MzResponse *res, const char *message, MzFlashType type) {
    if (!res || !message) return;
    const char *type_str = "info";
    switch (type) {
        case MZ_FLASH_SUCCESS: type_str = "success"; break;
        case MZ_FLASH_WARNING: type_str = "warning"; break;
        case MZ_FLASH_ERROR:   type_str = "error"; break;
        default:               type_str = "info"; break;
    }

    char trigger_json[512];
    snprintf(trigger_json, sizeof(trigger_json),
             "{\"mzFlash\": {\"message\": \"%s\", \"type\": \"%s\"}}",
             message, type_str);

    mz_res_trigger(res, trigger_json);
}

// Client-side Toast notification container that listens to mzFlash events
static inline void MzFlashContainer(void) {
    Div(
        .id = "mz-flash-container",
        .style = "position: fixed; top: 20px; right: 20px; z-index: 9999; display: flex; flex-direction: column; gap: 8px; pointer-events: none;"
    ) {}

    // Embedded vanilla micro-script to display and auto-dismiss the toast
    Script() {
        JsOn("document", "mzFlash") {
            JsConst("data", "e.detail");
            JsIf("!data") { Js("return;"); }
            JsConst("c", "document.getElementById('mz-flash-container')");
            JsIf("!c") { Js("return;"); }
            JsConst("toast", "document.createElement('div')");
            JsLet("bg", "'#0284c7'");
            JsIf("data.type === 'success'") { JsAssign("bg", "'#16a34a'"); }
            JsIf("data.type === 'warning'") { JsAssign("bg", "'#eab308'"); }
            JsIf("data.type === 'error'") { JsAssign("bg", "'#dc2626'"); }
            JsAssign("toast.style.cssText",
                     "'background:' + bg + '; color:white; padding:12px 18px; border-radius:8px; "
                     "box-shadow:0 4px 6px -1px rgba(0,0,0,0.1); font-size:0.875rem; font-weight:500; "
                     "opacity:0; transition:opacity 0.25s, transform 0.25s; transform:translateY(-10px); "
                     "pointer-events:auto; cursor:pointer;'");
            JsAssign("toast.textContent", "data.message");
            JsAssign("toast.onclick", "() => toast.remove()");
            JsCall("c.appendChild(toast)");
            JsCall("requestAnimationFrame(() => { toast.style.opacity = '1'; toast.style.transform = 'translateY(0)'; })");
            JsCall("setTimeout(() => { toast.style.opacity = '0'; setTimeout(() => toast.remove(), 300); }, 4000)");
        }
    }
}

#endif
