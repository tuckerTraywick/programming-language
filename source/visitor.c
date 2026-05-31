#include <stddef.h>
#include <stdbool.h>
#include "visitor.h"
#include "lexer.h"
#include "parser.h"
#include "list.h"
#include "map.h"

struct symbol_table symbol_table_create(size_t buckets_capacity, size_t keys_capacity) {
	struct symbol_table table = {
		.handles = map_create(buckets_capacity, sizeof *table.handles, keys_capacity),
	};
	if (!table.handles) {
		goto error1;
	}
	table.namespaces = map_create(buckets_capacity, sizeof *table.namespaces, keys_capacity);
	if (!table.namespaces) {
		goto error2;
	}
	table.variables = map_create(buckets_capacity, sizeof *table.variables, keys_capacity);
	if (!table.variables) {
		goto error3;
	}
	return table;

error3:
	map_destroy(&table.namespaces);
error2:
	map_destroy(&table.handles);
error1:
	return (struct symbol_table){0};
}

void symbol_table_destroy(struct symbol_table *table) {
	map_destroy(&table->handles);
	map_destroy(&table->namespaces);
	map_destroy(&table->variables);
	*table = (struct symbol_table){0};
}

struct object object_create(size_t buckets_capacity, size_t keys_capacity) {
	struct object object = {
		.public_symbols = symbol_table_create(buckets_capacity, keys_capacity),
	};
	if (!object.public_symbols.handles) {
		goto error1;
	}
	object.private_symbols = symbol_table_create(buckets_capacity, keys_capacity);
	if (!object.private_symbols.handles) {
		goto error2;
	}
	object.scopes = symbol_table_create(buckets_capacity, keys_capacity);
	if (!object.scopes.handles) {
		goto error3;
	}
	return object;

error3:
	symbol_table_destroy(&object.private_symbols);
error2:
	symbol_table_destroy(&object.public_symbols);
error1:
	return (struct object){0};
}

void object_destroy(struct object *object) {
	symbol_table_destroy(&object->public_symbols);
	symbol_table_destroy(&object->private_symbols);
	symbol_table_destroy(&object->scopes);
	*object = (struct object){0};
}

bool initialize_symbols(char *text, struct token *tokens, struct node *nodes, struct object *object, struct compiler_error **errors) {
	struct node *current_node = nodes;
	char *namespace_name = NULL;
	while (true) {
		if (current_node->type == NODE_TYPE_NAMESPACE_DEFINITION) {
			if (namespace_name) {
				struct compiler_error error = {
					.type = COMPILER_ERROR_TYPE_MULTIPLE_NAMESPACE_DEFINITIONS,
					.node_index = current_node - nodes,
				};
				list_push_back(errors, &error);
			}
		}
	}
}
