#include <stddef.h>
#include "visitor.h"
#include "map.h"

struct symbol_table symbol_table_create(size_t buckets_capacity, size_t keys_capacity) {
	struct symbol_table table = {
		.handles = map_create(buckets_capacity, sizeof *table.handles, keys_capacity),
	};
	if (!table.handles) {
		goto error1;
	}
	table.modules = map_create(buckets_capacity, sizeof *table.modules, keys_capacity);
	if (!table.modules) {
		goto error2;
	}
	table.variables = map_create(buckets_capacity, sizeof *table.variables, keys_capacity);
	if (!table.variables) {
		goto error3;
	}
	return table;

error3:
	map_destroy(&table.modules);
error2:
	map_destroy(&table.handles);
error1:
	return (struct symbol_table){0};
}

void symbol_table_destroy(struct symbol_table *table) {
	map_destroy(&table->handles);
	map_destroy(&table->modules);
	map_destroy(&table->variables);
	*table = (struct symbol_table){0};
}
