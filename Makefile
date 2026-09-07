CC = gcc
AR = ar
CFLAGS ?= -std=c23 -Wall -Wextra -O2 -fPIC -Isrc

PREFIX ?= /usr/local
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
PKGCONFIGDIR ?= $(PREFIX)/share/pkgconfig

SRCS = $(shell find src -name '*.c')
OBJS = $(SRCS:src/%.c=build/%.o)

all: build/libmizar.a build/libmizar.so build/mizar.pc

build/libmizar.a: $(OBJS)
	$(AR) rcs $@ $^

build/libmizar.so: $(OBJS)
	$(CC) -shared -o $@ $^

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/mizar.pc: mizar.pc.in
	@mkdir -p build
	@sed -e 's|@PREFIX@|$(PREFIX)|g' \
	     -e 's|@LIBDIR@|$(LIBDIR)|g' \
	     -e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
	     $< > $@

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
	@echo "All tests passed cleanly."

install: all
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)/mizar
	@mkdir -p $(DESTDIR)$(LIBDIR)
	@mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
	@mkdir -p $(DESTDIR)$(PREFIX)/share/pkgconfig
	cp -r src/* $(DESTDIR)$(INCLUDEDIR)/mizar/
	find $(DESTDIR)$(INCLUDEDIR)/mizar -name '*.c' -delete
	cp build/libmizar.a $(DESTDIR)$(LIBDIR)/
	cp build/libmizar.so $(DESTDIR)$(LIBDIR)/
	cp build/mizar.pc $(DESTDIR)$(LIBDIR)/pkgconfig/
	cp build/mizar.pc $(DESTDIR)$(PREFIX)/share/pkgconfig/
	@echo "Mizar successfully installed to $(DESTDIR)$(PREFIX)"

uninstall:
	rm -rf $(DESTDIR)$(INCLUDEDIR)/mizar
	rm -f $(DESTDIR)$(LIBDIR)/libmizar.a
	rm -f $(DESTDIR)$(LIBDIR)/libmizar.so
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/mizar.pc
	rm -f $(DESTDIR)$(PREFIX)/share/pkgconfig/mizar.pc
	@echo "Mizar uninstalled from $(DESTDIR)$(PREFIX)"

clean:
	rm -rf build

.PHONY: all clean test install uninstall
