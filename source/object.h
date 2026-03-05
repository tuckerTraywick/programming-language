#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "token.h"
#include "node.h"

#define MAX_SYMBOL_NAME_LENGTH (5*1024 - 1) // 5kb including terminator.

enum compiler_error_type {
	COMPILER_ERROR_TYPE_MODULE_ALREADY_DECLEARED,
	COMPILER_ERROR_TYPE_ALREADY_DEFINED,
	COMPILER_ERROR_TYPE_COUNT,
};

struct compiler_error {
	size_t node_index;
	enum compiler_error_type type;
};

enum symbol_type {
	SYMBOL_TYPE_MODULE,
	SYMBOL_TYPE_TYPE,
	SYMBOL_TYPE_VARIABLE,
	SYMBOL_TYPE_FUNCTION,
};

enum symbol_visibility {
	SYMBOL_VISIBILITY_PUBLIC,
	SYMBOL_VISIBILITY_PRIVATE,
};

struct symbol_handle {
	enum symbol_type type;
	enum symbol_visibility visibility;
	size_t offset;
};

struct module_symbol {

};

struct type_symbol {
	size_t alignment;
	size_t size;
	size_t type_description_offset;
};

struct variable_symbol {
	size_t value_alignment;
	size_t value_size;
	size_t value_offset;
	size_t type_description_offset;
};

struct function_symbol {
	size_t return_value_alignment;
	size_t return_value_size;
	size_t code_offset;
	size_t type_description_offset;
};

struct object {
	struct object_file_header *header;
	struct symbol_handle *symbols;
	struct module_symbol *modules;
	struct type_symbol *types;
	struct variable_symbol *variables;
	struct function_symbol *functions;
	struct node *type_descriptions; // Trees representing the types of nodes and symbols.
	struct node *nodes;
	struct token *tokens;
	char *text;
	char *immutable_values;
	char *mutable_values;
	char *code;
	struct lexer_error *lexer_errors;
	struct parser_error *parser_errors;
	struct compiler_error *compiler_errors;
};

struct object_file_header {

};

// A map of error types to error messages.
extern const char *const compiler_error_messages[];

struct object *object_create(void);

// Assumes `file` is open for reading text. Returns true if no errors occurred.
bool object_read_text_from_file(struct object *object, FILE *file);

void object_destroy(struct object *object);

#endif // OBJECT_H
