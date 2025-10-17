#include "symboltable.h"
#include "list.h"

#define SYMBOL_BUCKETS_INITIAL_CAPACITY 100

#define SYMBOL_STRINGS_INITIAL_CAPACITY (1024*5)

bool Symbol_Table_initialize(Symbol_Table *symbol_table) {
	*symbol_table = (Symbol_Table){
		.buckets = list_create(SYMBOL_BUCKETS_INITIAL_CAPACITY, sizeof *symbol_table->buckets),
		.names = list_create(SYMBOL_STRINGS_INITIAL_CAPACITY, sizeof *symbol_table->names),
	};
	return symbol_table->buckets != NULL && symbol_table->names != NULL;
}

void Symbol_Table_destroy(Symbol_Table *symbol_table) {
	list_destroy(symbol_table->buckets);
}
