#include "symboltable.h"
#include "list.h"

#define SYMBOL_TABLE_INITIAL_CAPACITY 100

bool Symbol_Table_initialize(Symbol_Table *symbol_table) {
	*symbol_table = (Symbol_Table){
		.buckets = list_create(SYMBOL_TABLE_INITIAL_CAPACITY, sizeof *symbol_table->buckets),
	};
	return symbol_table->buckets != NULL;
}

void Symbol_Table_destroy(Symbol_Table *symbol_table) {
	list_destroy(symbol_table);
}
