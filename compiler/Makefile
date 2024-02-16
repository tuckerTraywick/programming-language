CC := gcc
CFLAGS := -std=c99 -O -g -Wall -Wextra -Wpedantic
VFLAGS := --leak-check=yes
INCLUDE := -Isource -Iinclude
TEST_INCLUDE := -Itest

SOURCE_FILES := $(shell find source -name '*.c' ! -name 'main.c')
OBJECT_FILES := $(SOURCE_FILES:source/%.c=build/source/%.o)
TEST_SOURCE_FILES := $(shell find test -name '*.c' ! -name 'main.c')
TEST_OBJECT_FILES := $(TEST_SOURCE_FILES:test/%.c=build/test/%.o)

.PRECIOUS: bild/source/%.d build/test/%.d

.PHONY: buildsourceandtests
buildsourceandtests: buildsource buildtests

.PHONY: buildsource
buildsource: buildsourcemessage binary/run
	@echo

.PHONY: buildtests
buildtests: buildtestsmessage binary/test
	@echo

buildsourcemessage:
	@echo "---- BUILDING SOURCE ----"

buildtestsmessage:
	@echo "---- BUILDING TESTS ----"

.PHONY: testandrun
testandrun: buildsourceandtests
	@echo "---- TESTING ----"
	@binary/test
	@echo
	@echo "---- RUNNING ----"
	@binary/run

.PHONY: testandvalgrind
testandvalgrind: buildsourceandtests
	@echo "---- TESTING IN VALGRIND ----"
	@valgrind $(VFLAGS) binary/test
	@echo
	@echo "---- RUNNING IN VALGRIND ----"
	@valgrind $(VFLAGS) binary/run

.PHONY: run
run: buildsource
	@echo "---- RUNNING ----"
	@binary/run

.PHONY: valgrind
valgrind: buildsource
	@echo "---- RUNNING IN VALGRIND ----"
	@valgrind $(VFLAGS) binary/run

.PHONY: test
test: buildtests
	@echo "---- TESTING ----"
	@binary/test

.PHONY: testinvalgrind
testinvalgrind: buildtests
	@echo "---- TESTING IN VALGRIND ----"
	@valgrind $(VFLAGS) binary/test

.PHONY: clean
clean:
	@rm -rf build binary

binary/run: $(OBJECT_FILES) build/source/main.o
	@mkdir -p binary
	@$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

binary/test: $(OBJECT_FILES) $(TEST_OBJECT_FILES) build/test/main.o
	@mkdir -p binary
	@$(CC) $(CFLAGS) $(INCLUDE) $(TEST_INCLUDE) $^ -o $@

build/source/%.o: source/%.c
	@mkdir -p build/source
	@$(CC) $(CFLAGS) $(INCLUDE) -c -MMD -MP -MT $@ -MF build/source/$*.d $< -o $@

build/test/%.o: test/%.c
	@mkdir -p build/test
	@$(CC) $(CFLAGS) $(INCLUDE) $(TEST_INCLUDE) -c -MMD -MP -MT $@ -MF build/test/$*.d $< -o $@

-include $(OBJECT_FILES:.o=.d)
