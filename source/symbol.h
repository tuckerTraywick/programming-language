#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include <stdint.h>
#include "node.h"

enum visibility {
	VISIBILITY_PRIVATE,
	VISIBILITY_PUBLIC,
	VISIBILITY_COUNT,
};

enum symbol_type {
	SYMBOL_TYPE_MODULE,
	SYMBOL_TYPE_STRUCT,
	SYMBOL_TYPE_TRAIT,
	SYMBOL_TYPE_VARIABLE,
	SYMBOL_TYPE_FUNCTION,
	SYMBOL_TYPE_COUNT,
};

struct symbol_handle {
	enum symbol_type type;
	enum visibility visibility;
	uint32_t index;
};

struct module_symbol {

};

struct struct_symbol {
	uint64_t size;
	uint32_t alignment;
	bool isClosed;
	uint32_t fields_index;
	uint32_t fields_count;
	uint32_t methods_index;
	uint32_t methods_count;
	uint32_t embedded_types_index;
	uint32_t embedded_types_count;
	uint32_t cases_index;
	uint32_t cases_count;
	uint32_t generic_parameters_index;
	uint32_t generic_parameters_count;
};

struct trait_symbol {
	uint64_t size;
	uint32_t alignment;
	bool isClosed;
	uint32_t methods_index;
	uint32_t methods_count;
	uint32_t embedded_types_index;
	uint32_t embedded_types_count;
	uint32_t cases_index;
	uint32_t cases_count;
	uint32_t generic_parameters_index;
	uint32_t generic_parameters_count;
};

struct variable_symbol {
	uint32_t type_index;
	bool is_mutable;
	uint64_t value_offset;
};

struct function_symbol {
	uint32_t return_type_index;
	uint32_t parameters_index;
	uint32_t parameters_count;
	uint32_t generic_parameters_index;
	uint32_t generic_parameters_count;
	uint64_t code_offset;
};

struct field_symbol {
	enum visibility visibility;
	uint32_t name_index;
	uint32_t name_length;
	uint32_t type_index;
};

struct method_symbol {
	enum visibility visibility;
	uint32_t name_index;
	uint32_t name_length;
	uint32_t return_type_index;
	uint32_t parameters_index;
	uint32_t parameters_count;
	uint32_t generic_parameters_index;
	uint32_t generic_parameters_count;
};

struct embedded_type_symbol {
	uint32_t name_index;
	uint32_t name_length;
	uint32_t type_index;
};

struct case_symbol {
	uint32_t type_index;
	uint64_t value_offset;
};

struct function_parameter_symbol {
	uint32_t name_index;
	uint32_t name_length;
	uint32_t type_index;
	uint64_t default_value_offset;
};

struct generic_parameter_symbol {
	uint32_t name_index;
	uint32_t name_length;
	uint32_t type_index;
	uint64_t default_value_offset;
};

struct symbol_table {
	struct symbol_handle *symbol_handles; // Points to a map from symbol names to buckets.
	// All point to lists.
	struct module_symbol *module_symbols;
	struct struct_symbol *struct_symbols;
	struct trait_symbol *trait_symbols;
	struct variable_symbol *variable_symbols;
	struct function_symbol *function_symbols;
	struct field_symbol *field_symbols;
	struct method_symbol *method_symbols;
	struct embedded_type_symbol *embedded_type_symbols;
	struct case_symbol *case_symbols;
	struct function_parameter_symbol *function_parameter_symbols;
	struct generic_parameter_symbol *generic_parameter_symbols;
};

struct symbol_table symbol_table_create(void);

void symbol_table_destroy(struct symbol_table *symbol_table);

#endif // SYMBOL_H
