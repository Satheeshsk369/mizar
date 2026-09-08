CC ?= gcc
AR ?= ar
CFLAGS ?= -std=c23 -Wall -Wextra -O2 -fPIC -Isrc

# Optional BearSSL TLS Support (make TLS=1)
ifeq ($(TLS),1)
    CFLAGS += -DMIZAR_ENABLE_TLS=1
    LIBS_EXTRA += -lbearssl
endif

# Platform detection
UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(UNAME_S),Darwin)
    SO_EXT = dylib
    SHLIB_FLAGS = -dynamiclib
else ifeq ($(OS),Windows_NT)
    SO_EXT = dll
    SHLIB_FLAGS = -shared
else
    # Linux, FreeBSD, OpenBSD, NetBSD, Solaris
    SO_EXT = so
    SHLIB_FLAGS = -shared
endif

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
PKGCONFIGDIR ?= $(PREFIX)/share/pkgconfig

VERSION ?= $(shell grep '#define MIZAR_VERSION_STRING' src/core/version.h | cut -d '"' -f 2)

SRCS = $(shell find src -name '*.c' ! -name 'cli.c')
OBJS = $(SRCS:src/%.c=build/%.o)

LIB_STATIC = build/libmizar.a
LIB_SHARED = build/libmizar.$(SO_EXT)
CLI_BIN = build/mizar

DOCS_SRCS = $(wildcard docs/*.c)

all: $(LIB_STATIC) $(LIB_SHARED) $(CLI_BIN) build/mizar.pc build/compile_commands.json

showcase: $(LIB_STATIC)
	@mkdir -p build
	@$(CC) $(CFLAGS) demo/showcase.c $(LIB_STATIC) -o build/showcase
	@./build/showcase 4000

doc-build: $(LIB_STATIC)
	@mkdir -p build/docs
	@$(CC) $(CFLAGS) -Idocs docs/main.c $(LIB_STATIC) -o build/docs_builder
	@./build/docs_builder build

doc-serve: $(LIB_STATIC)
	@mkdir -p build/docs
	@$(CC) $(CFLAGS) -Idocs docs/main.c $(LIB_STATIC) -o build/docs_builder
	@./build/docs_builder serve

$(LIB_STATIC): $(OBJS)
	$(AR) rcs $@ $^

$(LIB_SHARED): $(OBJS)
	$(CC) $(SHLIB_FLAGS) -o $@ $^ $(LIBS_EXTRA)

$(CLI_BIN): src/cli.c
	@mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Automatically re-trigger object rebuild when TLS mode changes
build/.tls_mode: FORCE
	@mkdir -p build
	@echo "$(TLS)" > $@.tmp
	@if [ ! -f $@ ] || ! cmp -s $@.tmp $@; then mv $@.tmp $@; rm -f $(OBJS) $(LIB_STATIC) $(LIB_SHARED); else rm -f $@.tmp; fi

FORCE:

$(OBJS): build/.tls_mode

build/mizar.pc: mizar.pc.in src/core/version.h
	@mkdir -p build
	@sed -e 's|@PREFIX@|$(PREFIX)|g' \
	     -e 's|@LIBDIR@|$(LIBDIR)|g' \
	     -e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
	     -e 's|@VERSION@|$(VERSION)|g' \
	     $< > $@

build/compile_commands.json: $(SRCS)
	@mkdir -p build
	@printf '[\n' > $@
	@for src in $(SRCS); do \
		printf '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -c '$$src'", "file": "'$$src'"},\n' >> $@; \
	done
	@for test_src in $(wildcard test/*.c); do \
		printf '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) '$$test_src' build/libmizar.a", "file": "'$$test_src'"},\n' >> $@; \
	done
	@for doc_src in $(wildcard docs/*.c); do \
		printf '  {"directory": "$(CURDIR)", "command": "$(CC) $(CFLAGS) -Idocs '$$doc_src' build/libmizar.a", "file": "'$$doc_src'"},\n' >> $@; \
	done
	@sed -i '$$ s/,$$//' $@
	@printf ']\n' >> $@

test: $(LIB_STATIC)
	@mkdir -p build
	@$(CC) $(CFLAGS) test/test_main.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_main
	@./build/test_main
	@$(CC) $(CFLAGS) test/test_ssg.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_ssg
	@./build/test_ssg
	@$(CC) $(CFLAGS) test/test_server.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_server
	@./build/test_server
	@$(CC) $(CFLAGS) test/test_concurrency.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_concurrency
	@./build/test_concurrency
	@$(CC) $(CFLAGS) test/test_equation.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_equation
	@./build/test_equation
	@$(CC) $(CFLAGS) test/test_components.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_components
	@./build/test_components
	@$(CC) $(CFLAGS) test/test_geometry.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_geometry
	@./build/test_geometry
	@$(CC) $(CFLAGS) test/test_icons.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_icons
	@./build/test_icons
	@$(CC) $(CFLAGS) test/test_tls.c $(LIB_STATIC) $(LIBS_EXTRA) -o build/test_tls
	@./build/test_tls
	@echo "All tests passed cleanly."

install: all
	@mkdir -p $(DESTDIR)$(BINDIR)
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)/mizar
	@mkdir -p $(DESTDIR)$(LIBDIR)
	@mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
	@mkdir -p $(DESTDIR)$(PREFIX)/share/pkgconfig
	cp $(CLI_BIN) $(DESTDIR)$(BINDIR)/mizar
	chmod 755 $(DESTDIR)$(BINDIR)/mizar
	cp -r src/* $(DESTDIR)$(INCLUDEDIR)/mizar/
	find $(DESTDIR)$(INCLUDEDIR)/mizar -name '*.c' -delete
	cp $(LIB_STATIC) $(DESTDIR)$(LIBDIR)/
	cp $(LIB_SHARED) $(DESTDIR)$(LIBDIR)/
	cp build/mizar.pc $(DESTDIR)$(LIBDIR)/pkgconfig/
	cp build/mizar.pc $(DESTDIR)$(PREFIX)/share/pkgconfig/
	@echo "Mizar successfully installed to $(DESTDIR)$(PREFIX)"

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/mizar
	rm -rf $(DESTDIR)$(INCLUDEDIR)/mizar
	rm -f $(DESTDIR)$(LIBDIR)/libmizar.a
	rm -f $(DESTDIR)$(LIBDIR)/libmizar.$(SO_EXT)
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/mizar.pc
	rm -f $(DESTDIR)$(PREFIX)/share/pkgconfig/mizar.pc
	@echo "Mizar uninstalled from $(DESTDIR)$(PREFIX)"

clean:
	rm -rf build

.PHONY: all clean test install uninstall doc-build doc-serve showcase
