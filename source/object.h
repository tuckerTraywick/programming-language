#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "symboltable.h"

typedef struct Object_File_Header {
	size_t size;
	size_t symbol_buckets_count;
	size_t symbol_buckets_offset;
	size_t symbol_names_size;
	size_t symbol_names_offset;
	size_t data_size;
	size_t data_offset;
	size_t code_size;
	size_t code_offset;
} Object_File_Header;

typedef struct Object {
	void *file_contents; // NULL if object was created from `Object_initialize()`. Points to the beginning of an mmap()'d object file otherwise.
	size_t size;
	Symbol_Table symbol_table;
	char *symbol_names;
	char *data;
	char *code;
} Object;

// Returns true if intialization succeeded. Returns the object in `object`. Assumes `object` is
// empty.
bool Object_initialize(Object *object);

void Object_destroy(Object *object);

// Returns true if deserialization succeeded. Returns the object in `object`.
bool Object_read_from_file(Object *object, FILE *file);

// Returns true if writing succeeded.
bool Object_write_to_file(Object *object, FILE *file);

#endif // OBJECT_H
