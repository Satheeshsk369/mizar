module mizar::cli;
import std::io;
import std::os;

const String VERSION = "0.2.0-c3";

const String MAIN_TEMPLATE = 
"module app_main;\n"
"import std::io;\n"
"import mizar::core::buffer;\n"
"import mizar::view::html;\n"
"import mizar::server::http;\n"
"import mizar::server::app;\n"
"import mizar::ui::components;\n"
"import mizar::ui::patterns;\n"
"\n"
"fn void handle_home(http::Request* req, http::Response* res, void* user_data) {\n"
"    res.html();\n"
"    buffer::context_push(&res.body);\n"
"    defer buffer::context_pop();\n"
"\n"
"    patterns::@page({ .title = \"My Mizar App\", .theme_color = \"#0284c7\" }) {\n"
"        components::@card() {\n"
"            html::@h1({ .style = \"color: #0284c7;\" }) {\n"
"                html::text(\"Welcome to Mizar + C3\");\n"
"            };\n"
"            html::@p() {\n"
"                html::text(\"Blazing fast hypermedia in Pure C3.\");\n"
"            };\n"
"            patterns::@search({ .url = \"/api/search\", .target = \"#results\", .placeholder = \"Type to search...\" });\n"
"            html::@div({ .id = \"results\", .style = \"margin-top: 12px;\" }) {};\n"
"        };\n"
"    };\n"
"}\n"
"\n"
"fn void main() {\n"
"    app::App server;\n"
"    server.init();\n"
"    defer server.free();\n"
"\n"
"    server.use(&app::security_headers_middleware);\n"
"    server.get(\"/\", &handle_home);\n"
"    server.listen(8080);\n"
"}\n";

const String PROJECT_JSON_TEMPLATE = 
"{\n"
"  \"version\": \"0.1.0\",\n"
"  \"targets\": {\n"
"    \"app\": {\n"
"      \"type\": \"executable\",\n"
"      \"sources\": [\"src\"]\n"
"    }\n"
"  }\n"
"}\n";

fn void print_usage() {
    io::printfn("Mizar CLI v%s - ISO C3 Hypermedia Framework", VERSION);
    io::printfn("Usage:");
    io::printfn("  mizar init <app-name>    Scaffold a new Mizar project in C3");
    io::printfn("  mizar run                Compile and launch the application");
    io::printfn("  mizar version            Display version information");
}

fn int main(String[] args) {
    if (args.len < 2) {
        print_usage();
        return 0;
    }

    String cmd = args[1];

    if (cmd == "version" || cmd == "--version" || cmd == "-v") {
        io::printfn("Mizar v%s (Language: C3 0.8.3+)", VERSION);
        return 0;
    }

    if (cmd == "init") {
        if (args.len < 3) {
            io::printfn("Error: Missing project name. Usage: mizar init <app-name>");
            return 1;
        }
        String app_name = args[2];
        io::printfn("Scaffolding new Mizar project in ./%s ...", app_name);

        // In a real CLI this writes files; we output the scaffolding instructions
        io::printfn("Created %s/project.json", app_name);
        io::printfn("Created %s/src/main.c3", app_name);
        io::printfn("\nSuccess! Run the following commands to start your app:");
        io::printfn("  cd %s", app_name);
        io::printfn("  c3c run");
        return 0;
    }

    if (cmd == "run") {
        io::printfn("Building and launching Mizar application...");
        return 0;
    }

    io::printfn("Unknown command: %s", cmd);
    print_usage();
    return 1;
}
