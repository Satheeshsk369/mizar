CC ?= gcc
AR ?= ar
CFLAGS ?= -std=c23 -Wall -Wextra -O2 -fPIC -Isrc

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
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
PKGCONFIGDIR ?= $(PREFIX)/share/pkgconfig

SRCS = $(shell find src -name '*.c')
OBJS = $(SRCS:src/%.c=build/%.o)

LIB_STATIC = build/libmizar.a
LIB_SHARED = build/libmizar.$(SO_EXT)

DOCS_SRCS = $(wildcard docs/*.c)

all: $(LIB_STATIC) $(LIB_SHARED) build/mizar.pc build/compile_commands.json

showcase: $(LIB_STATIC)
	@mkdir -p build
	@$(CC) $(CFLAGS) examples/showcase.c $(LIB_STATIC) -o build/showcase
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
	$(CC) $(SHLIB_FLAGS) -o $@ $^

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/mizar.pc: mizar.pc.in
	@mkdir -p build
	@sed -e 's|@PREFIX@|$(PREFIX)|g' \
	     -e 's|@LIBDIR@|$(LIBDIR)|g' \
	     -e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
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

test: build/libmizar.a
	@mkdir -p build
	@$(CC) $(CFLAGS) test/test_main.c build/libmizar.a -o build/test_main
	@./build/test_main
	@$(CC) $(CFLAGS) test/test_ssg.c build/libmizar.a -o build/test_ssg
	@./build/test_ssg
	@$(CC) $(CFLAGS) test/test_server.c build/libmizar.a -o build/test_server
	@./build/test_server
	@$(CC) $(CFLAGS) test/test_concurrency.c build/libmizar.a -o build/test_concurrency
	@./build/test_concurrency
	@$(CC) $(CFLAGS) test/test_equation.c build/libmizar.a -o build/test_equation
	@./build/test_equation
	@$(CC) $(CFLAGS) test/test_components.c build/libmizar.a -o build/test_components
	@./build/test_components
	@$(CC) $(CFLAGS) test/test_geometry.c build/libmizar.a -o build/test_geometry
	@./build/test_geometry
	@$(CC) $(CFLAGS) test/test_icons.c build/libmizar.a -o build/test_icons
	@./build/test_icons
	@echo "All tests passed cleanly."

install: all
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)/mizar
	@mkdir -p $(DESTDIR)$(LIBDIR)
	@mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
	@mkdir -p $(DESTDIR)$(PREFIX)/share/pkgconfig
	cp -r src/* $(DESTDIR)$(INCLUDEDIR)/mizar/
	find $(DESTDIR)$(INCLUDEDIR)/mizar -name '*.c' -delete
	cp $(LIB_STATIC) $(DESTDIR)$(LIBDIR)/
	cp $(LIB_SHARED) $(DESTDIR)$(LIBDIR)/
	cp build/mizar.pc $(DESTDIR)$(LIBDIR)/pkgconfig/
	cp build/mizar.pc $(DESTDIR)$(PREFIX)/share/pkgconfig/
	@echo "Mizar successfully installed to $(DESTDIR)$(PREFIX)"

uninstall:
	rm -rf $(DESTDIR)$(INCLUDEDIR)/mizar
	rm -f $(DESTDIR)$(LIBDIR)/libmizar.a
	rm -f $(DESTDIR)$(LIBDIR)/libmizar.$(SO_EXT)
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/mizar.pc
	rm -f $(DESTDIR)$(PREFIX)/share/pkgconfig/mizar.pc
	@echo "Mizar uninstalled from $(DESTDIR)$(PREFIX)"

clean:
	rm -rf build

.PHONY: all clean test install uninstall doc-build doc-serve showcase
