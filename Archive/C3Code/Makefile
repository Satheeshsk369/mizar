C3C ?= c3c
PREFIX ?= /usr/local

all: build-cli lib

build-cli:
	$(C3C) build mizar-cli

lib:
	$(C3C) build mizar

test:
	$(C3C) build test_suite

run: test
	./out/test_suite

install: build-cli
	install -d $(PREFIX)/bin
	install -m 755 out/mizar-cli $(PREFIX)/bin/mizar

uninstall:
	rm -f $(PREFIX)/bin/mizar

clean:
	rm -rf out obj build

.PHONY: all build-cli lib test run install uninstall clean
