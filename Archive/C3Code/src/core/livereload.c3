module mizar::core::livereload;
import mizar::core::buffer;

fn void inject_livereload_script() {
    buffer::Buffer* buf = buffer::context_get();
    if (buf == null) return;
    buf.append_str(
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
        "</script>\n"
    );
}
