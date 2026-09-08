#include "docs_shared.h"

static inline void render_sidebar(const char *current_slug) {
    Aside(.cls = "sidebar") {
        A(.cls = "brand", .href = "/") {
            Text("Mizar");
            Span() { Text("C23"); }
        }

        Div(.cls = "nav-section") {
            Div(.cls = "nav-title") { Text("1. Overview"); }
            A(.cls = strcmp(current_slug, "index") == 0 ? "nav-link active" : "nav-link", .href = "/") {
                Text("Mizar Overview");
            }
            A(.cls = strcmp(current_slug, "intro") == 0 ? "nav-link active" : "nav-link", .href = "/intro/") {
                Text("Introduction");
            }
            A(.cls = strcmp(current_slug, "quickstart") == 0 ? "nav-link active" : "nav-link", .href = "/quickstart/") {
                Text("Quickstart");
            }
        }

        Div(.cls = "nav-section") {
            Div(.cls = "nav-title") { Text("2. Progressive Examples"); }
            A(.cls = strcmp(current_slug, "ex-minimal") == 0 ? "nav-link active" : "nav-link", .href = "/examples/minimal/") {
                Text("01. Minimal Server");
            }
            A(.cls = strcmp(current_slug, "ex-routing") == 0 ? "nav-link active" : "nav-link", .href = "/examples/routing/") {
                Text("02. Routing & Params");
            }
            A(.cls = strcmp(current_slug, "ex-components") == 0 ? "nav-link active" : "nav-link", .href = "/examples/components/") {
                Text("03. HTML, SVG & Math");
            }
            A(.cls = strcmp(current_slug, "ex-htmx") == 0 ? "nav-link active" : "nav-link", .href = "/examples/htmx/") {
                Text("04. HTMX 4 Hypermedia");
            }
            A(.cls = strcmp(current_slug, "ex-middleware") == 0 ? "nav-link active" : "nav-link", .href = "/examples/middleware/") {
                Text("05. Middleware & Auth");
            }
            A(.cls = strcmp(current_slug, "ex-ssg") == 0 ? "nav-link active" : "nav-link", .href = "/examples/ssg/") {
                Text("06. Static Site Pipeline");
            }
        }

        Div(.cls = "nav-section") {
            Div(.cls = "nav-title") { Text("3. Source Architecture"); }
            A(.cls = strcmp(current_slug, "src-overview") == 0 ? "nav-link active" : "nav-link", .href = "/source/overview/") {
                Text("Source & Memory Model");
            }
            A(.cls = strcmp(current_slug, "src-algo") == 0 ? "nav-link active" : "nav-link", .href = "/source/algo/") {
                Text("algo/ Data Structures");
            }
            A(.cls = strcmp(current_slug, "src-server") == 0 ? "nav-link active" : "nav-link", .href = "/source/server/") {
                Text("server/ Network Engine");
            }
            A(.cls = strcmp(current_slug, "src-view") == 0 ? "nav-link active" : "nav-link", .href = "/source/view/") {
                Text("view/ Declarative DSL");
            }
            A(.cls = strcmp(current_slug, "src-ssg") == 0 ? "nav-link active" : "nav-link", .href = "/source/ssg/") {
                Text("ssg/ Generator Engine");
            }
        }
    }
}

void docs_layout(MizarBuffer *buf, const char *title, const char *slug, void (*content_fn)(void)) {
    Html(buf, .lang = "en") {
        Head() {
            Meta(.charset = "UTF-8");
            Meta(.name = "viewport", .content = "width=device-width, initial-scale=1.0");
            Title() { Text("%s — Mizar C23 Manual", title); }
            Script() {
                Js("try { const t = localStorage.getItem('mizar-theme') || 'dark'; document.documentElement.setAttribute('data-theme', t); } catch(e) {}");
            }
            Link(.rel = "stylesheet", .href = "/style.css");
            Link(.rel = "stylesheet", .id = "prism-light", .href = "https://cdn.jsdelivr.net/npm/prismjs@1.29.0/themes/prism.min.css", .disabled = true);
            Link(.rel = "stylesheet", .id = "prism-dark", .href = "https://cdn.jsdelivr.net/npm/prismjs@1.29.0/themes/prism-tomorrow.min.css");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/prismjs@1.29.0/prism.min.js") {}
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/prismjs@1.29.0/components/prism-c.min.js") {}
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/prismjs@1.29.0/components/prism-makefile.min.js") {}
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/prismjs@1.29.0/components/prism-bash.min.js") {}
            Link(.rel = "stylesheet", .href = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js");
            Script(.defer = true, .src = "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/contrib/auto-render.min.js") {}
            Script() {
                JsOnReady() {
                    JsCall("renderMathInElement(document.body)");
                    JsForEach("document.querySelectorAll('pre code')", "el") {
                        JsIf("!el.className") {
                            JsAssign("el.className", "'language-c'");
                        }
                    }
                    JsIf("window.Prism") {
                        JsCall("Prism.highlightAll()");
                    }
                }
            }
        }
        Body() {
            render_sidebar(slug);
            Main(.cls = "main-wrapper") {
                Div(.cls = "top-bar") {
                    Div(.cls = "search-box") {
                        Input(.type = "search", .id = "doc-search", .cls = "search-input", .placeholder = "Search documentation (Ctrl+K)...", .autocomplete = "off");
                        Div(.id = "search-dropdown", .cls = "search-dropdown") {}
                    }
                    Button(.id = "theme-btn", .cls = "theme-toggle", .title = "Toggle Dark Mode") {
                        Span(.id = "theme-icon-sun", .style = "display:none;line-height:0;") {
                            Icon_Sun(.size = 18);
                        }
                        Span(.id = "theme-icon-moon", .style = "display:inline-block;line-height:0;") {
                            Icon_Moon(.size = 18);
                        }
                    }
                }
                content_fn();
            }
            Script() {
                JsFn("updateThemeUI", "theme") {
                    JsGetEl("pLight", "prism-light");
                    JsGetEl("pDark", "prism-dark");
                    JsIf("pLight && pDark") {
                        JsAssign("pLight.disabled", "theme === 'dark'");
                        JsAssign("pDark.disabled", "theme !== 'dark'");
                    }
                    JsGetEl("sunIcon", "theme-icon-sun");
                    JsGetEl("moonIcon", "theme-icon-moon");
                    JsIf("sunIcon && moonIcon") {
                        JsAssign("sunIcon.style.display", "theme === 'dark' ? 'inline-block' : 'none'");
                        JsAssign("moonIcon.style.display", "theme === 'dark' ? 'none' : 'inline-block'");
                    }
                }

                JsOnReady() {
                    JsConst("savedTheme", "(typeof localStorage !== 'undefined' && localStorage.getItem('mizar-theme')) || 'dark'");
                    JsSetTheme("savedTheme");
                    JsCall("updateThemeUI(savedTheme)");

                    JsOn("document.getElementById('theme-btn')", "click") {
                        JsConst("cur", "document.documentElement.getAttribute('data-theme') || 'dark'");
                        JsConst("next", "cur === 'dark' ? 'light' : 'dark'");
                        JsSetTheme("next");
                        JsIf("typeof localStorage !== 'undefined'") {
                            JsCall("localStorage.setItem('mizar-theme', next)");
                        }
                        JsCall("updateThemeUI(next)");
                    }

                    JsLet("searchIndex", "null");
                    JsAsyncFn("loadSearchIndex", "") {
                        JsIf("!searchIndex") {
                            JsTry() {
                                JsConst("res", "await fetch('/search-index.json')");
                                JsAssign("searchIndex", "await res.json()");
                            }
                            JsCatch("e") {
                                JsAssign("searchIndex", "[]");
                            }
                        }
                        JsReturn("searchIndex");
                    }

                    JsGetEl("search", "doc-search");
                    JsGetEl("dropdown", "search-dropdown");

                    JsOn("window", "keydown") {
                        JsIf("(e.ctrlKey || e.metaKey) && e.key === 'k'") {
                            JsCall("e.preventDefault()");
                            JsCall("search?.focus()");
                        }
                        JsIf("e.key === 'Escape'") {
                            JsAssign("dropdown.style.display", "'none'");
                        }
                    }

                    JsOn("search", "focus") {
                        JsCall("loadSearchIndex()");
                    }

                    JsOnAsync("search", "input") {
                        JsConst("q", "e.target.value.toLowerCase().trim()");
                        JsIf("!q") {
                            JsAssign("dropdown.style.display", "'none'");
                            JsAssign("dropdown.innerHTML", "''");
                            JsReturn("");
                        }
                        JsConst("idx", "await loadSearchIndex()");
                        JsConst("matches", "[]");
                        JsForOf("doc", "idx") {
                            JsConst("titleIdx", "doc.title.toLowerCase().indexOf(q)");
                            JsConst("catIdx", "(doc.category || '').toLowerCase().indexOf(q)");
                            JsConst("textIdx", "doc.body.toLowerCase().indexOf(q)");
                            JsIf("titleIdx !== -1 || catIdx !== -1 || textIdx !== -1") {
                                JsLet("snippet", "''");
                                JsIf("textIdx !== -1") {
                                    JsConst("start", "Math.max(0, textIdx - 40)");
                                    JsConst("end", "Math.min(doc.body.length, textIdx + 80)");
                                    JsAssign("snippet", "(start > 0 ? '...' : '') + doc.body.slice(start, end) + (end < doc.body.length ? '...' : '')");
                                }
                                JsElse() {
                                    JsAssign("snippet", "doc.body.slice(0, 100) + '...'");
                                }
                                JsCall("matches.push({ title: doc.title, category: doc.category || 'Docs', url: doc.url, snippet: snippet })");
                                JsIf("matches.length >= 8") {
                                    Js("break;");
                                }
                            }
                        }
                        JsIf("matches.length === 0") {
                            JsAssign("dropdown.innerHTML", "'<div style=\"padding:12px;color:var(--text-muted);font-size:0.85rem;\">No matching documentation found.</div>'");
                        }
                        JsElse() {
                            JsAssign("dropdown.innerHTML", "matches.map(m => `\n"
                                     "  <a href=\"${m.url}\" class=\"search-result-item\">\n"
                                     "    <div style=\"display:flex; justify-content:space-between; align-items:center;\">\n"
                                     "      <span class=\"search-result-title\">${m.title}</span>\n"
                                     "      <span style=\"font-size:0.7rem; text-transform:uppercase; background:var(--bg-card); padding:2px 6px; border-radius:4px; border:1px solid var(--border-color); color:var(--text-muted);\">${m.category}</span>\n"
                                     "    </div>\n"
                                     "    <span class=\"search-result-snippet\">${m.snippet.replace(/</g, '&lt;')}</span>\n"
                                     "  </a>`).join('')");
                        }
                        JsAssign("dropdown.style.display", "'flex'");
                    }

                    JsOn("document", "click") {
                        JsIf("!search?.contains(e.target) && !dropdown?.contains(e.target)") {
                            JsIf("dropdown") {
                                JsAssign("dropdown.style.display", "'none'");
                            }
                        }
                    }
                }
            }
        }
    }
}
