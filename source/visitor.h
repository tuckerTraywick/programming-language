#ifndef VISITOR_H
#define VISITOR_H

#include <stddef.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"

enum symbol_type {
	SYMBOL_TYPE_MODULE,
	SYMBOL_TYPE_VARIABLE,
	SYMBOL_TYPE_COUNT,
};

struct symbol_handle {
	size_t index;
	enum symbol_type type;
};

struct module_symbol {

};

struct variable_symbol {
	size_t type_index;
	size_t value_offset;
	bool is_immutable;
};

struct symbol_table {
	struct symbol_handle *handles; // Points to a map.
	struct module_symbol *modules; // Points to a list.
	struct variable_symbol *variables; // Points to a list.
};

struct object {
	struct symbol_table public_symbols; // Points to a map.
	struct symbol_table private_symbols; // Points to a map.
	struct symbol_table symbol_stubs; // Points to a map. Symbols to be linked later.
	struct symbol_table scopes; // Points to a map. Symbols defined in functions.
};

enum compiler_error_type {
	COMPILER_ERROR_TYPE_COUNT,
};

struct compiler_error {
	size_t node_index;
	enum compiler_error_type type;
};

// Returns a completely zeroed struct if a memory error occurred.
struct symbol_table symbol_table_create(size_t buckets_capacity, size_t keys_capacity);

void symbol_table_destroy(struct symbol_table *table);

struct object *object_create(size_t buckets_capacity, size_t keys_capacity);

void object_destroy(struct object *object);

// Makes a symbol for each definition and makes sure there are no duplicate definitions.
bool initialize_symbols(char *text, struct token *tokens, struct node *nodes, struct object *object, struct compiler_error **errors);

#endif // VISITOR_H
