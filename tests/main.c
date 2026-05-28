#include <stdio.h>
#include "test.h"
#include "visitor.h"

void test_symbol_table_create_and_destroy(void) {
	struct symbol_table table = symbol_table_create(10, 10);
	assert(table.handles);
	if (!table.handles) {
		return;
	}
	symbol_table_destroy(&table);
	assert(!table.handles);
}

void test_object_create_and_destroy(void) {
	struct object object = object_create(10, 10);
	assert(object.public_symbols.handles);
	if (!object.public_symbols.handles) {
		return;
	}
	object_destroy(&object);
	assert(!object.public_symbols.handles);
}

int main(void) {
	begin_testing();
		run_test(test_symbol_table_create_and_destroy);
		run_test(test_object_create_and_destroy);
	end_testing();
	return 0;
}
