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

int main(void) {
	begin_testing();
		run_test(test_symbol_table_create_and_destroy);
	end_testing();
	return 0;
}
