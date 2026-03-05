#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "object.h"
#include "list.h"
#include "map.h"

static const size_t initial_bucket_capacity = 1000;

static const size_t initial_section_capacity = 10*1024;

static long get_file_length(FILE *file) {
	if (fseek(file, 0, SEEK_END) != 0) {
		return -1;
	}
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

const char *const compiler_error_messages[] = {
	[COMPILER_ERROR_TYPE_ALREADY_DEFINED] = "Symbol has already been defined.",
};

struct object *object_create(void) {
	// TODO: There's got to be a better way to do this...
	struct object *object = malloc(sizeof *object);
	if (!object) goto error1;
	object->header = NULL;
	object->symbols = map_create(initial_bucket_capacity, sizeof *object->symbols, initial_section_capacity);
	if (!object->symbols) goto error2;
	object->modules = list_create(initial_bucket_capacity, sizeof *object->modules);
	if (!object->modules) goto error3;
	object->types = list_create(initial_bucket_capacity, sizeof *object->types);
	if (!object->types) goto error4;
	object->variables = list_create(initial_bucket_capacity, sizeof *object->variables);
	if (!object->variables) goto error5;
	object->functions = list_create(initial_bucket_capacity, sizeof *object->functions);
	if (!object->functions) goto error6;
	object->type_descriptions = list_create(initial_bucket_capacity, sizeof *object->type_descriptions);
	if (!object->type_descriptions) goto error7;
	object->nodes = list_create(initial_bucket_capacity, sizeof *object->nodes);
	if (!object->nodes) goto error8;
	object->tokens = list_create(initial_bucket_capacity, sizeof *object->tokens);
	if (!object->tokens) goto error9;
	object->text = list_create(initial_section_capacity, sizeof *object->text);
	if (!object->text) goto error10;
	// object->symbol_names = list_create(initial_section_capacity, sizeof *object->symbol_names);
	// if (!object->symbol_names) goto error11;
	object->immutable_values = list_create(initial_section_capacity, sizeof *object->immutable_values);
	if (!object->immutable_values) goto error12;
	object->mutable_values = list_create(initial_section_capacity, sizeof *object->mutable_values);
	if (!object->mutable_values) goto error13;
	object->code = list_create(initial_section_capacity, sizeof *object->code);
	if (!object->code) goto error14;
	object->lexer_errors = list_create(initial_bucket_capacity, sizeof *object->lexer_errors);
	if (!object->lexer_errors) goto error15;
	object->parser_errors = list_create(initial_bucket_capacity, sizeof *object->parser_errors);
	if (!object->parser_errors) goto error16;
	object->compiler_errors = list_create(initial_bucket_capacity, sizeof *object->compiler_errors);
	if (!object->compiler_errors) goto error17;
	return object;

	error17:
		list_destroy(&object->parser_errors);
	error16:
		list_destroy(&object->lexer_errors);
	error15:
		list_destroy(&object->code);
	error14:
		list_destroy(&object->mutable_values);
	error13:
		list_destroy(&object->immutable_values);
	error12:
	// 	list_destroy(&object->symbol_names);
	// error11:
		list_destroy(&object->text);
	error10:
		list_destroy(&object->tokens);
	error9:
		list_destroy(&object->nodes);
	error8:
		list_destroy(&object->type_descriptions);
	error7:
		list_destroy(object->functions);
	error6:
		list_destroy(&object->variables);
	error5:
		list_destroy(&object->types);
	error4:
		list_destroy(&object->modules);
	error3:
		map_destroy(&object->symbols);
	error2:
		free(object);
	error1:
		return NULL;
}

bool object_read_text_from_file(struct object *object, FILE *file) {
	long length = get_file_length(file);
	if (length == -1) {
		return false;
	}
	// `+ 1` because `get_file_length()` does not account for the file terminator.
	if (length + 1 > list_get_capacity(&object->text) && !list_set_capacity(&object->text, length + 1)) {
		return false;
	}
	
	size_t bytes_read = fread(object->text, 1, length, file);
	if (ferror(file) || bytes_read != length) {
		clearerr(file);
		return false;
	}
	object->text[length] = '\0';
	return true;
}

void object_destroy(struct object *object) {
	map_destroy(&object->symbols);
	list_destroy(&object->modules);
	list_destroy(&object->types);
	list_destroy(&object->variables);
	list_destroy(&object->functions);
	list_destroy(&object->type_descriptions);
	list_destroy(&object->nodes);
	list_destroy(&object->tokens);
	list_destroy(&object->text);
	// list_destroy(&object->symbol_names);
	list_destroy(&object->immutable_values);
	list_destroy(&object->mutable_values);
	list_destroy(&object->code);
	list_destroy(&object->lexer_errors);
	list_destroy(&object->parser_errors);
	list_destroy(&object->compiler_errors);
	free(object);
}
