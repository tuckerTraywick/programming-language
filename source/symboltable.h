typedef struct Variable_Symbol {
	size_t data_offset;
} Variable_Symbol;

typedef struct Function_Symbol {
	size_t code_offset;
} Function_Symbol;

typedef enum Symbol_Type {
	EMPTY, // Used to identify when symbol buckets are empty.
	SYMBOL_TYPE_VARIABLE,
	SYMBOL_TYPE_FUNCTION,
	// SYMBOL_TYPE_STRUCT,
	// SYMBOL_TYPE_TRAIT,
} Symbol_Type;

typedef enum Symbol_Visibility {
	PUBLIC,
	PRIVATE,
} Symbol_Visibility;

typedef struct Symbol {
	size_t name_offset;
	union {
		Variable_Symbol variable;
		Function_Symbol function;
	};
	Symbol_Type type;
	Symbol_Visibility visibility;
} Symbol;

typedef struct Symbol_Table {
	char *names;
	Symbol *buckets;
} Symbol_Table;
