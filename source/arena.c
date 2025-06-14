#include <stddef.h>
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

void *list_create(size_t capacity) {
	List_Header *list = malloc(sizeof (List_Header) + capacity);
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

void *list_set_capacity(void *list, size_t capacity) {
	List_Header *header = list_get_header(list);
	List_Header *new_list = realloc(header, capacity);
	if (!new_list) {
		return NULL;
	}
	new_list->capacity = capacity;
	return new_list + 1;
}

size_t list_get_size(void *list) {
	List_Header *header = list_get_header(list);
	return header->size;
}

void *list_set_size(void *list, size_t size) {
	List_Header *header = list_get_header(list);
	if (size > header->capacity) {
		return list_set_capacity(list, max(list_get_capacity(list)*2, size));
	}
	header->size = size;
	return list;
}

void *list_allocate(void *list, size_t size) {
	List_Header *header = list_get_header(list);
	if (header->size + size > header->capacity) {
		return NULL;
	}
	void *allocation = (char*)list + header->size;
	header->size += size;
	return allocation;
}

void *list_allocate_zeroed(void *list, size_t size) {
	void *allocation = list_allocate(list, size);
	if (allocation) {
		memset(allocation, 0, size);
	}
	return allocation;
}

void *list_push(void *list, void *value, size_t size) {
	void *allocation = list_allocate(list, size);
	if (allocation) {
		memcpy(allocation, value, size);
	}
	return allocation;
}

size_t list_pop(void *list, size_t size) {
	List_Header *header = list_get_header(list);
	size_t amount_popped = min(header->size, size);
	header->size -= amount_popped;
	return amount_popped;
}

#undef min
#undef max
