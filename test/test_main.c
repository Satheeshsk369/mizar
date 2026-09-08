#include <assert.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 4096);

    // Document with HTML, SVG and MathML
    Html(&buf, .lang = "en") {
        Head() {
            Title() { Text("Document"); }
            Style() {
                Rule(".box") { Prop("margin", "0"); }
                CssRule(".card", (CssProps){
                    .display = CSS_FLEX,
                    .flex_direction = CSS_COLUMN,
                    .padding = Px(16),
                    .border_radius = Px(8),
                    .background = "#ffffff"
                });
            }
        }
        Body() {
            Div(.id = "app", .cls = "container", HxGet("/items"), HxTarget("#list")) {
                Svg(.width = "100", .height = "100", .viewBox = "0 0 100 100") {
                    Svg_Circle_(.cx = "50", .cy = "50", .r = "40", .fill = "#000");
                }
                Math(.display = "block") {
                    Math_Mrow() {
                        Math_Mi() { Text("x"); }
                        Math_Mo() { Text("="); }
                        Math_Mn() { Text("1"); }
                    }
                }
            }
        }
    }

    assert(strstr(buf.data, "<!DOCTYPE html>\n") != NULL);
    assert(strstr(buf.data, "<title>Document</title>") != NULL);
    assert(strstr(buf.data, ".box {\n  margin: 0;\n}\n") != NULL);
    assert(strstr(buf.data, ".card {\n  display: flex;\n  flex-direction: column;\n  padding: 16px;\n  border-radius: 8px;\n  background: #ffffff;\n}\n") != NULL);
    assert(strstr(buf.data, "hx-get=\"/items\"") != NULL);
    assert(strstr(buf.data, "<circle cx=\"50\" cy=\"50\" r=\"40\" fill=\"#000\" />") != NULL);
    assert(strstr(buf.data, "<math display=\"block\"><mrow><mi>x</mi><mo>=</mo><mn>1</mn></mrow></math>") != NULL);

    mz_buf_free(&buf);

    // 6. Test HTMX 4 Extended Features & Compatibility
    {
        MizarBuffer hbuf;
        mz_buf_init(&hbuf, 1024);
        mz_context_push(&hbuf);

        // Internalized HTMX Script helper
        MzHtmx(.implicit_inheritance = true, .default_swap = "outerHTML");

        // HTMX 4 Partial tag and modern attributes
        Div(HxTargetInherited("#feed"), HxBoostInherited("true")) {
            Form(HxQuery("/search"), HxDisable("button"), HxStatus422("target:#errors"), HxMorphSkip(true)) {
                Button(HxPost("/submit"), HxPending("#loading"), HxPreload("mouseover")) {
                    Text("Submit");
                }
            }
            HxPartial(HxTarget("#notifications"), HxSwap("beforeend")) {
                Div() { Text("New alert!"); }
            }
        }

        mz_context_pop();

        assert(strstr(hbuf.data, "<meta name=\"htmx-config\"") != NULL);
        assert(strstr(hbuf.data, "\"implicitInheritance\": true") != NULL);
        assert(strstr(hbuf.data, "\"defaultSwap\": \"outerHTML\"") != NULL);
        assert(strstr(hbuf.data, "<script src=\"https://unpkg.com/htmx.org@4.0.0\"></script>") != NULL);
        assert(strstr(hbuf.data, "hx-target:inherited=\"#feed\"") != NULL);
        assert(strstr(hbuf.data, "hx-boost:inherited=\"true\"") != NULL);
        assert(strstr(hbuf.data, "hx-query=\"/search\"") != NULL);
        assert(strstr(hbuf.data, "hx-disable=\"button\"") != NULL);
        assert(strstr(hbuf.data, "hx-status:422=\"target:#errors\"") != NULL);
        assert(strstr(hbuf.data, "hx-morph-skip") != NULL);
        assert(strstr(hbuf.data, "hx-pending=\"#loading\"") != NULL);
        assert(strstr(hbuf.data, "hx-preload=\"mouseover\"") != NULL);
        assert(strstr(hbuf.data, "<hx-partial") != NULL);
        assert(strstr(hbuf.data, "hx-target=\"#notifications\"") != NULL);
        assert(strstr(hbuf.data, "hx-swap=\"beforeend\"") != NULL);
        assert(strstr(hbuf.data, "New alert!") != NULL);
        assert(strstr(hbuf.data, "</hx-partial>") != NULL);

        mz_buf_free(&hbuf);
    }

    // 7. Test HTMX 2 alias compatibility (HxDisabledElt -> hx-disable)
    {
        MizarBuffer abuf;
        mz_buf_init(&abuf, 256);
        mz_context_push(&abuf);
        Button(HxDisabledElt("this")) { Text("Click"); }
        mz_context_pop();
        assert(strstr(abuf.data, "hx-disable=\"this\"") != NULL);
        mz_buf_free(&abuf);
    }
    // 8. Test High-Level Hypermedia Patterns (MzPage, MzSearch, MzClickToEdit, MzLoadMore, MzPoll)
    {
        MizarBuffer pbuf;
        mz_buf_init(&pbuf, 2048);

        MzPage(&pbuf, .title = "Test Page", .theme_color = "#0284c7") {
            MzSearch(.url = "/api/search", .target = "#results", .placeholder = "Type here...");
            MzClickToEdit(.url = "/users/1/edit", .label = "Email", .value = "user@example.com");
            MzLoadMore(.url = "/posts?page=2", .target = "#posts", .trigger = MZ_LOAD_REVEALED);
            MzPoll(.url = "/api/status", .interval_sec = 5) {
                Text("System Online");
            }
        }

        assert(strstr(pbuf.data, "<title>Test Page</title>") != NULL);
        assert(strstr(pbuf.data, "theme-color\" content=\"#0284c7\"") != NULL);
        assert(strstr(pbuf.data, "<script src=\"https://unpkg.com/htmx.org@4.0.0\"></script>") != NULL);
        assert(strstr(pbuf.data, "hx-get=\"/api/search\"") != NULL);
        assert(strstr(pbuf.data, "input changed delay:300ms") != NULL);
        assert(strstr(pbuf.data, "user@example.com") != NULL);
        assert(strstr(pbuf.data, "hx-get=\"/users/1/edit\"") != NULL);
        assert(strstr(pbuf.data, "hx-trigger=\"revealed\"") != NULL);
        assert(strstr(pbuf.data, "hx-trigger=\"every 5s\"") != NULL);
        assert(strstr(pbuf.data, "System Online") != NULL);

        mz_buf_free(&pbuf);
    }

    // Standalone SVG & MathML documents
    MizarBuffer sbuf, mbuf;
    mz_buf_init(&sbuf, 256);
    SvgDoc(&sbuf, .width = "50", .height = "50") {
        Svg_Rect_(.width = "50", .height = "50");
    }
    assert(strstr(sbuf.data, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"50\" height=\"50\"><rect width=\"50\" height=\"50\" /></svg>") != NULL);
    mz_buf_free(&sbuf);

    mz_buf_init(&mbuf, 256);
    MathDoc(&mbuf) {
        Math_Mi() { Text("y"); }
    }
    assert(strstr(mbuf.data, "<math xmlns=\"http://www.w3.org/1998/Math/MathML\"><mi>y</mi></math>") != NULL);
    mz_buf_free(&mbuf);

    // 9. Test Type-Safe Result Types and Slices
    {
        MzSlice s = MZ_SLICE_LIT("application/json");
        assert(mz_slice_eq(s, "application/json"));
        assert(!mz_slice_eq(s, "text/html"));
        assert(s.len == 16);

        MzRequest req;
        memset(&req, 0, sizeof(req));
        req.params[0].key = "id";
        req.params[0].value = "42";
        req.param_count = 1;

        req.queries[0].key = "ratio";
        req.queries[0].value = "3.14159";
        req.query_count = 1;

        MzIntResult id_res = mz_req_param_int(&req, "id");
        assert(id_res.ok);
        assert(id_res.val == 42);

        MzIntResult bad_id = mz_req_param_int(&req, "missing");
        assert(!bad_id.ok);

        MzFloatResult fl_res = mz_req_query_float(&req, "ratio");
        assert(fl_res.ok);
        assert(fl_res.val > 3.14 && fl_res.val < 3.15);
    }

    return 0;
}
