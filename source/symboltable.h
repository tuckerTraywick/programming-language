#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct Variable_Symbol {
	size_t data_offset;
} Variable_Symbol;

typedef struct Function_Symbol {
	size_t code_offset;
} Function_Symbol;

typedef enum Symbol_Type {
	SYMBOL_TYPE_VARIABLE,
	SYMBOL_TYPE_FUNCTION,
	// SYMBOL_TYPE_STRUCT,
	// SYMBOL_TYPE_TRAIT,
} Symbol_Type;

typedef enum Symbol_Visibility {
	PUBLIC,
	PRIVATE,
} Symbol_Visibility;

typedef struct Symbol_Header {
	size_t name_offset;
	union {
		Variable_Symbol variable;
		Function_Symbol function;
	};
	Symbol_Type type;
	Symbol_Visibility visibility;
} Symbol_Header;

typedef struct Symbol_Table {
	Symbol_Header *buckets;
} Symbol_Table;

// Returns true if initialization succeeded. Assumes `symbol_table` is empty.
bool Symbol_Table_initialize(Symbol_Table *symbol_table);

void Symbol_Table_destroy(Symbol_Table *Symbol_Table);

#endif // SYMBOL_TABLE_H
