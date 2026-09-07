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

clean:
	rm -rf build

.PHONY: all clean
