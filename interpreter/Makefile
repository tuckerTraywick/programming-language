CC := gcc
CFLAGS := -std=c17 -O -g -Wall -Wextra -Wpedantic
INCLUDE := -Isource

SOURCE_FILES := main.c interpreter.c
OBJECT_FILES := main.o interpreter.o


.PHONY: build
build: binary/run

.PRECIOUS: build/%.d

.PHONY: run
run: build
	@binary/run

.PHONY: clean
clean:
	@rm -rf build binary

binary/run: $(addprefix build/, $(OBJECT_FILES))
	@mkdir -p binary
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: source/%.c build/%.d
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

build/%.d:
	@mkdir -p build
	$(CC) -MM -MP -MT build/$*.o -MF $@ source/$*.c

-include $(SOURCE_FILES:%.c=build/%.d)
