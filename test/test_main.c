#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mizar.h"

// A component: standard C function with zero extra arguments!
void user_card(const char *name, const char *role, bool admin) {
    Div(.cls = "card", .id = name) {
        H3() { Text("User: %s", name); }
        P(.cls = "role") { Text("Role: %s", role); }
        if (admin) {
            Span(.cls = "badge admin") { Text("Administrator"); }
        }
        A(.href = "/profile", .cls = "btn", HxGet("/api/profile"), HxTarget("#main")) {
            Text("View Profile");
        }
    }
}

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 1024);

    Html(&buf, .lang = "en") {
        Head() {
            Title() { Text("Mizar Web Framework"); }
            Meta(.charset = "utf-8");
            Meta(.name = "viewport", .content = "width=device-width, initial-scale=1.0");
            Link(.rel = "stylesheet", .href = "/style.css");
        }
        Body(.cls = "dark-theme") {
            Header() {
                Nav(.cls = "navbar") {
                    A(.href = "/", .cls = "brand") { Text("Mizar SSR"); }
                }
            }
            
            // Layout boundary flush (sends navbar & layout immediately)
            Flush();

            Main(.cls = "container", .id = "main") {
                H1() { Text("Welcome to Mizar!"); }
                P() {
                    Text("Auto-escaped unsafe input: %s", "<script>alert('xss')</script> & 'quotes'");
                }
                Br();
                Hr();
                
                Section(.cls = "user-grid") {
                    user_card("Alice", "Engineering Lead", true);
                    user_card("Bob", "Designer", false);
                }

                Form(.action = "/submit", .method = "POST") {
                    Input(.type = "text", .name = "username", .placeholder = "Enter username", .required = true);
                    Button(.type = "submit", .cls = "btn-primary") { Text("Save"); }
                }
            }
            Footer() {
                Small() { Text("Rendered with pure C23 Mizar"); }
            }
        }
    }

    printf("%s\n", buf.data);
    
    // Verify escaping
    assert(strstr(buf.data, "&lt;script&gt;alert(&#39;xss&#39;)&lt;/script&gt; &amp; &#39;quotes&#39;") != NULL);
    // Verify void tags have no closing tags
    assert(strstr(buf.data, "<meta charset=\"utf-8\" />") != NULL);
    assert(strstr(buf.data, "<br />") != NULL);
    assert(strstr(buf.data, "<hr />") != NULL);
    assert(strstr(buf.data, "</hr>") == NULL);
    // Verify container tags
    assert(strstr(buf.data, "<!DOCTYPE html>\n<html lang=\"en\">") != NULL);
    assert(strstr(buf.data, "</html>") != NULL);

    mz_buf_free(&buf);
    printf("\n>>> ALL TESTS PASSED SUCCESSFULLY! <<<\n");
    return 0;
}
