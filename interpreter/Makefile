CFLAGS := -std=c17 -O -g -Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined
INCLUDE := -Isource
# VFLAGS := --leak-check=yes
CC := gcc

SOURCE_FILES := main.c interpreter.c
OBJECT_FILES := main.o interpreter.o

.PHONY: build
build: binary/run

.PHONY: run
run: bulid
	@binary/run

.PHONY: clean
clean:
	@rm -rf build binary

source/main.c: source/interpreter.h

binary/run: build/$(OBJECT_FILES)
	@mkdir -p binary
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: source/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@
