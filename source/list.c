#include <assert.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define min(a, b) (((a) <= (b)) ? (a) : (b))

#define max(a, b) (((a) >= (b)) ? (a) : (b))

typedef struct List_Header {
	size_t capacity;
	size_t size;
	char data[];
} List_Header;

static List_Header *list_get_header(void *list) {
	return (List_Header*)list - 1;
}

void *list_create(size_t capacity, size_t element_size) {
	List_Header *list = malloc(sizeof (List_Header) + capacity*element_size);
	if (!list) {
		return NULL;
	}
	list->capacity = capacity;
	return list + 1;
}

void list_destroy(void *list) {
	free((List_Header*)list - 1);
}

size_t list_get_capacity(void *list) {
	List_Header *header = list_get_header(list);
	return header->capacity;
}

bool list_set_capacity(void **list, size_t capacity) {
	List_Header *header = list_get_header(*list);
	List_Header *new_list = realloc(header, capacity);
	if (!new_list) {
		return false;
	}
	new_list->capacity = capacity;
	*list = new_list + 1;
	return true;
}

size_t list_get_size(void *list) {
	List_Header *header = list_get_header(list);
	return header->size;
}

bool list_set_size(void **list, size_t size) {
	List_Header *header = list_get_header(*list);
	if (size > header->capacity) {
		return list_set_capacity(*list, max(list_get_capacity(list)*2, size));
	}
	header->size = size;
	return true;
}

bool list_push_impl(void **list, void *element, size_t element_size) {
	if (!list_set_size(list, list_get_size(*list) + 1)) {
		return false;
	}
	memcpy((char*)*list + (list_get_size(*list) - 1)*element_size, element, element_size);
	return true;
}

bool list_pop_impl(void **list, void *destination, size_t element_size) {
	assert(list_get_size(*list) > 0);
	if (!list_set_size(list, list_get_size(*list) - 1)) {
		return false;
	}
	memcpy(destination, (char*)*list + (list_get_size(*list) + 1)*element_size, element_size);
	return true;
}

#undef min
#undef max
