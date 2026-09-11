#ifndef MIZAR_CORE_LIVERELOAD_H
#define MIZAR_CORE_LIVERELOAD_H

#include "core/buffer.h"

// Injects a lightweight EventSource script that triggers a page refresh when server sends 'reload'
static inline void MzLiveReloadScript(void) {
    MizarBuffer *buf = mz_context_get();
    if (!buf) return;
    mz_buf_append_str(buf,
        "<script>\n"
        "(function() {\n"
        "  function connect() {\n"
        "    var es = new EventSource('/__mizar_reload');\n"
        "    es.onmessage = function(e) {\n"
        "      if (e.data === 'reload') window.location.reload();\n"
        "    };\n"
        "    es.onerror = function() {\n"
        "      es.close();\n"
        "      setTimeout(connect, 2000);\n"
        "    };\n"
        "  }\n"
        "  connect();\n"
        "})();\n"
        "</script>\n");
}

#endif
