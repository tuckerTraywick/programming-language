#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "symbol_table.h"

struct object {
	struct symbol_table public_symbols;
	struct symbol_table private_symbols;
	struct symbol_table unresolved_symbols; // Symbols from header files.
	// All point to lists.
	char *imports;
	struct node *nodes;
	struct token *tokens;
	char *text;
	char *code;
	char *immutable_data;
	char *mutable_data;
};

enum compiling_error_type {
	COMPILING_ERROR_TYPE_COUNT,
};

struct compiling_error {
	enum compiling_error_type type;
};

struct object object_create(void);

struct object object_read_from_file(FILE *file);

void object_destroy(struct object *object);

bool object_is_valid(struct object *object);

bool object_write_to_file(struct object *object, FILE *file);

bool object_combine(struct object *dest, struct object *source, struct compiling_error **errors);

#endif // OBJECT_H
