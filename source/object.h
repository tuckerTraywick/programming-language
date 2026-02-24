#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include "token.h"
#include "node.h"

enum symbol_class {
	SYMBOL_CLASS_MODULE,
	SYMBOL_CLASS_TYPE,
	SYMBOL_CLASS_VARIABLE,
	SYMBOL_CLASS_FUNCTION,
};

enum symbol_visibility {
	SYMBOL_VISIBILITY_PUBLIC,
	SYMBOL_VISIBILITY_PRIVATE,
};

struct symbol_handle {
	enum symbol_class class;
	enum symbol_visibility visibility;
	size_t offset;
};

struct module_symbol {
	size_t parent_offset;
};

struct type_symbol {
	size_t alignment;
	size_t size;
	size_t type_tree_offset;
};

struct variable_symbol {
	size_t value_alignment;
	size_t value_size;
	size_t value_offset;
	size_t type_tree_offset;
};

struct function_symbol {
	size_t return_value_alignment;
	size_t return_value_size;
	size_t code_offset;
	size_t signature_tree_offset;
};

struct object {
	struct object_file_header *header;
	struct symbol_handle *symbols;
	struct module_symbol *modules;
	struct type_symbol *types;
	struct variable_symbol *variables;
	struct function_symbol *functions;
	struct node *type_descriptions; // Trees representing the types of nodes.
	struct node *nodes;
	struct token *tokens;
	char *token_text;
	char *symbol_names;
	char *immutable_values;
	char *mutable_values;
	char *code;
};

struct object_file_header {

};

#endif // OBJECT_H
