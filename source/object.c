#include <stdlib.h>
#include "object.h"
#include "map.h"
#include "list.h"

static const size_t initial_symbols_buckets_capacity = 1000;

static const size_t initial_symbols_keys_capacity = 10*1024;

static const size_t initial_segment_capacity = 10*1024;

struct object *object_create(void) {
	struct object *object = malloc(sizeof *object);
	if (!object) {
		return NULL;
	}
	object->symbols = map_create(initial_symbols_buckets_capacity, sizeof *object->symbols, initial_symbols_keys_capacity);
	if (!object->symbols) {
		free(object);
		return NULL;
	}
	object->code = list_create(initial_segment_capacity, sizeof *object->code);
	if (!object->code) {
		free(object->symbols);
		free(object);
		return NULL;
	}
	object->immutable_data = list_create(initial_segment_capacity, sizeof *object->immutable_data);
	if (!object->immutable_data) {
		free(object->code);
		free(object->symbols);
		free(object);
		return NULL;
	}
	object->mutable_data = list_create(initial_segment_capacity, sizeof *object->mutable_data);
	if (!object->mutable_data) {
		free(object->immutable_data);
		free(object->code);
		free(object->symbols);
		free(object);
		return NULL;
	}
	return object;
}

void object_destroy(struct object *object) {
	map_destroy(&object->symbols);
	list_destroy(&object->code);
	list_destroy(&object->immutable_data);
	list_destroy(&object->mutable_data);
	free(object);
}
