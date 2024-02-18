CC := gcc
CFLAGS := -O -g3 -std=c99 -Wall -Wextra -Wpedantic
INCLUDE := -Iinclude

SOURCE_FILES := $(shell find source -name '*.c')
OBJECT_FILES := $(SOURCE_FILES:source/%.c=build/source/%.o)
TEST_SOURCE_FILES := $(shell find test -name '*.c')
TEST_OBJECT_FILES := $(TEST_SOURCE_FILES:test/%.c=build/test/%.o)

.PHONY: build
build: binary/run binary/test

binary/run: $(OBJECT_FILES)
	@mkdir -p binary
	@$(CC) $(CFLAGS) $^ -o $@

binary/test: $(TEST_OBJECT_FILES)
	@mkdir -p binary
	@$(CC) $(CFLAGS) $^ -o $@

build/source/%.o: source/%.c
	@mkdir -p build/source
	@$(CC) $(CFLAGS) $(INCLUDE) -c -MMD -MP -MT $@ -MF build/source/$*.d $< -o $@

build/test/%.o: test/%.c
	@mkdir -p build/test
	@$(CC) $(CFLAGS) $(INCLUDE) -c -MMD -MP -MT $@ -MF build/test/$*.d $< -o $@

.PRECIOUS: build/%.d

.PHONY: clean
clean:
	@rm -rf build binary

-include $(OBJECT_FILES:.o=.d)
