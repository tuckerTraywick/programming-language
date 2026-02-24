#ifndef SYMBOL_H
#define SYMBOL_H

#include <stddef.h>

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

struct symbol {
	enum symbol_type type;
	enum symbol_visibility visibility;
	size_t offset;
};

struct module_symbol {

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

#endif // SYMBOL_H
