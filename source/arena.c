#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"

typedef struct Arena_Header {
	size_t capacity;
	size_t size;
	char data[];
} Arena_Header;

static Arena_Header *arena_get_header(void *arena) {
	return (Arena_Header*)arena - 1;
}

void *arena_create(size_t capacity) {
	Arena_Header *arena = malloc(sizeof (Arena_Header) + capacity);
	if (!arena) {
		return NULL;
	}
	arena->capacity = capacity;
	return arena + 1;
}

void arena_destroy(void *arena) {
	free((Arena_Header*)arena - 1);
}

void *arena_allocate(void *arena, size_t size) {
	Arena_Header *header = arena_get_header(arena);
	if (header->size + size > header->capacity) {
		return NULL;
	}
	void *allocation = (char*)arena + header->size;
	header->size += size;
	return allocation;
}

void *arena_allocate_zeroed(void *arena, size_t size) {
	void *allocation = arena_allocate(arena, size);
	if (allocation) {
		memset(allocation, 0, size);
	}
	return allocation;
}

size_t arena_get_capacity(void *arena) {
	Arena_Header *header = arena_get_header(arena);
	return header->capacity;
}

void *arena_set_capacity(void *arena, size_t capacity) {
	Arena_Header *header = arena_get_header(arena);
	Arena_Header *new_arena = realloc(header, capacity);
	if (!new_arena) {
		return NULL;
	}
	new_arena->capacity = capacity;
	return new_arena + 1;
}

size_t arena_get_size(void *arena) {
	Arena_Header *header = arena_get_header(arena);
	return header->size;
}

void *arena_set_size(void *arena, size_t size) {
	Arena_Header *header = arena_get_header(arena);
	if (size > header->capacity) {
		return arena_set_capacity(arena, arena_get_capacity(arena)*2);
	}
	header->size = size;
	return arena;
}
