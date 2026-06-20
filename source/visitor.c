#include <stdio.h>

#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include "visitor.h"
#include "lexer.h"
#include "parser.h"
#include "list.h"
#include "map.h"

const char *const compiler_error_messages[] = {
	[COMPILER_ERROR_TYPE_MULTIPLE_NAMESPACE_DEFINITIONS] = "Can't declare multiple namespaces in one file.",
};

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
	static char namespace_name[5*1024];
	struct node *current_node = nodes;
	// Traverse to the program's statements.
	current_node = nodes + current_node->child_index; // current_node = first child of program node
	while (true) {
		if (current_node->type == NODE_TYPE_DEFINITION) {
			current_node = nodes + current_node->child_index; // current_node = first child of definition
			// Skip the `pub` if needed.
			if (current_node->type == NODE_TYPE_TOKEN) {
				current_node = nodes + current_node->next_index; // current node = inner definition
			}
			continue;
		}

		if (current_node->type == NODE_TYPE_NAMESPACE_DEFINITION) {
			// Emit an error if a namespace has already been defined.
			if (namespace_name[0]) {
				struct compiler_error error = {
					.type = COMPILER_ERROR_TYPE_MULTIPLE_NAMESPACE_DEFINITIONS,
					.node_index = current_node - nodes,
				};
				list_push_back(errors, &error);
				return false;
			}
			
			// Traverse to the namespace name and find the index of its first character.
			current_node = nodes + current_node->child_index; // current_node = token namespace
			current_node = nodes + current_node->next_index; // current_node = token identifier
			struct token *first_token = tokens + current_node->child_index;
			size_t start_index = first_token->text_index;
			
			// Copy the characters of the name to a temporary buffer.
			// TODO: Check that the name isn't too big.
			size_t name_index = 0;
			for (size_t i = start_index; i < list_get_count(&text); ++i) {
				if (text[i] == '\n') {
					break;
				}
				if (isspace(text[i])) {
					continue;
				}
				namespace_name[name_index] = text[i];
				++name_index;
			}

			// Traverse back up to the definition node.
			current_node = nodes + current_node->parent_index; // current_node = namespace node
			current_node = nodes + current_node->parent_index; // current_node = definition node
		}

		if (current_node->next_index == NODE_NONE) {
			return true;
		}
		current_node = nodes + current_node->next_index;
	}
}
