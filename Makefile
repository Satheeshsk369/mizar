CC = gcc
AR = ar
CFLAGS ?= -std=c23 -Wall -Wextra -O2 -fPIC -Isrc

SRCS = $(shell find src -name '*.c')
OBJS = $(SRCS:src/%.c=build/%.o)

all: build/libmizar.a build/libmizar.so

build/libmizar.a: $(OBJS)
	$(AR) rcs $@ $^

build/libmizar.so: $(OBJS)
	$(CC) -shared -o $@ $^

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: build/libmizar.a
	@mkdir -p build
	$(CC) $(CFLAGS) test/test_main.c build/libmizar.a -o build/test_main
	@./build/test_main
	$(CC) $(CFLAGS) test/test_ssg.c build/libmizar.a -o build/test_ssg
	@./build/test_ssg
	$(CC) $(CFLAGS) test/test_server.c build/libmizar.a -o build/test_server
	@./build/test_server

clean:
	rm -rf build

.PHONY: all clean test
