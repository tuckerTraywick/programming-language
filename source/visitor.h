#ifndef VISITOR_H
#define VISITOR_H

#include <stddef.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"

enum symbol_type {
	SYMBOL_TYPE_NAMESPACE,
	SYMBOL_TYPE_VARIABLE,
	SYMBOL_TYPE_COUNT,
};

struct symbol_handle {
	size_t index;
	enum symbol_type type;
};

struct namespace_symbol {

};

struct variable_symbol {
	size_t type_index;
	size_t value_offset;
	bool is_immutable;
};

struct symbol_table {
	struct symbol_handle *handles; // Points to a map.
	struct namespace_symbol *namespaces; // Points to a list.
	struct variable_symbol *variables; // Points to a list.
};

struct object {
	struct symbol_table public_symbols; // Points to a map.
	struct symbol_table private_symbols; // Points to a map.
	// char *symbol_stubs; // Points to a list. Symbols that are to be linked later.
	struct symbol_table scopes; // Points to a map. Symbols defined in functions.
};

enum compiler_error_type {
	COMPILER_ERROR_TYPE_MULTIPLE_NAMESPACE_DEFINITIONS,
	COMPILER_ERROR_TYPE_COUNT,
};

struct compiler_error {
	size_t node_index;
	enum compiler_error_type type;
};

extern const char *const compiler_error_messages[];

// Returns a completely zeroed struct if a memory error occurred.
struct symbol_table symbol_table_create(size_t buckets_capacity, size_t keys_capacity);

void symbol_table_destroy(struct symbol_table *table);

// Returns null if no symbol is found.
struct symbol_handle *symbol_table_get_symbol_handle(struct symbol_table *table, char *name);

// Returns true if no memory errors occurred.
// bool symbol_table_add_symbol_handle(struct symbol_table *table, char *name, struct symbol_handle *handle);

// Returns true if no memory errors occurred.
bool symbol_table_add_namespace_symbol(struct symbol_table *table, char *name, struct namespace_symbol *symbol);

// Returns true if no memory errors occurred.
bool symbol_table_add_variable_symbol(struct symbol_table *table, char *name, struct variable_symbol *symbol);

// Returns a completely zeroed struct if a memory error occurred.
struct object object_create(size_t buckets_capacity, size_t keys_capacity);

void object_destroy(struct object *object);

// Makes a symbol for each definition and makes sure there are no duplicate definitions.
bool initialize_symbols(char *text, struct token *tokens, struct node *nodes, struct object *object, struct compiler_error **errors);

#endif // VISITOR_H
