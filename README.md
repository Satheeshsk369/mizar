# Mizar: Web Framework

Mizar is a web framework written in C23. It provides a native DSL for HTML, SVG, MathML, and CSS, as well as a static site generator and an HTTP server.

## Features

- **Declarative DSL:** Write HTML, SVG, MathML, and CSS directly in C.
- **HTMX Protocol:** First-class support for `hx-*` attributes and server response headers.
- **Static Site Generator:** Pre-render pages to clean URLs and sync static assets.
- **HTTP Server:** Multi-worker server supporting `epoll` on Linux, `kqueue` on BSD/macOS, and `poll` on POSIX.

## Building and Testing

```bash
make
make test
```

## System-Wide Installation

```bash
sudo make install
```

To remove:
```bash
sudo make uninstall
```
