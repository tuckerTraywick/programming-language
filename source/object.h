#ifndef OBJECT_H
#define OBJECT_H
#include <stdio.h>

enum symbol_type {
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
	void *symbol;
};

struct type {};

struct variable {};

struct function {};

struct object {
	struct symbol *symbols; // Points to a map.
	char *code; // Points to a list.
	char *immutable_data; // Points to a list.
	char *mutable_data; // Points to a list.
};

// Returns NULL if memory error occurs.
struct object *object_create(void);

void object_destroy(struct object *object);

#endif // OBJECT_H
