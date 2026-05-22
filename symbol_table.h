#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stddef.h>

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
	size_t index;
};

struct module_symbol {

};

struct struct_symbol {
	uint64_t size;
	size_t alignment;
	size_t fields_index;
	size_t fields_count;
	size_t methods_index;
	size_t methods_count;
	size_t embedded_types_index;
	size_t embedded_types_count;
	size_t cases_index;
	size_t cases_count;
	size_t generic_parameters_index;
	size_t generic_parameters_count;
	bool isClosed;
};

struct trait_symbol {
	uint64_t size;
	size_t alignment;
	size_t methods_index;
	size_t methods_count;
	size_t embedded_types_index;
	size_t embedded_types_count;
	size_t cases_index;
	size_t cases_count;
	size_t generic_parameters_index;
	size_t generic_parameters_count;
	bool isClosed;
};

struct variable_symbol {
	size_t type_index;
	bool is_mutable;
	uint64_t value_offset;
};

struct function_symbol {
	size_t return_type_index;
	size_t parameters_index;
	size_t parameters_count;
	size_t generic_parameters_index;
	size_t generic_parameters_count;
	uint64_t code_offset;
};

struct field_symbol {
	enum visibility visibility;
	size_t name_index;
	size_t name_length;
	size_t type_index;
};

struct method_symbol {
	enum visibility visibility;
	size_t name_index;
	size_t name_length;
	size_t return_type_index;
	size_t parameters_index;
	size_t parameters_count;
	size_t generic_parameters_index;
	size_t generic_parameters_count;
};

struct embedded_type_symbol {
	size_t name_index;
	size_t name_length;
	size_t type_index;
};

struct case_symbol {
	size_t type_index;
	uint64_t value_offset;
};

struct function_parameter_symbol {
	size_t name_index;
	size_t name_length;
	size_t type_index;
	uint64_t default_value_offset;
};

struct generic_parameter_symbol {
	size_t name_index;
	size_t name_length;
	size_t type_index;
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

#endif // SYMBOL_TABLE_H
