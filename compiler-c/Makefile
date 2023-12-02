CC := gcc
CFLAGS := -std=c17 -O -g -Wall -Wextra -Wpedantic
INCLUDE := -Isource

SOURCE_FILES := $(shell find source -name '*.c')
OBJECT_FILES := $(SOURCE_FILES:source/%.c=build/%.o)

.PRECIOUS: build/%.d

.PHONY: build
build: buildmessage binary/run
	@echo

buildmessage:
	@echo "---- BUILDING ----"

.PHONY: run
run: build
	@echo "---- RUNNING ----"
	@binary/run

.PHONY: clean
clean:
	@rm -rf build binary

binary/run: $(OBJECT_FILES)
	@mkdir -p binary
	@$(CC) $(CFLAGS) $^ -o $@

build/%.o: source/%.c
	@mkdir -p build
	@$(CC) $(CFLAGS) $(INCLUDE) -c -MMD -MP -MT $@ -MF build/$*.d $< -o $@

-include $(OBJECT_FILES:.o=.d)
