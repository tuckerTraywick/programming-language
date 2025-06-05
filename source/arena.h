#ifndef ARENA_H

#include <stddef.h>

void *arena_create(size_t capacity);

void arena_destroy(void *arena);

void *arena_allocate(void *arena, size_t size);

void *arena_allocate_zeroed(void *arena, size_t size);

size_t arena_get_capacity(void *arena);

void *arena_set_capacity(void *arena, size_t capacity);

size_t arena_get_size(void *arena);

void *arena_set_size(void *arena, size_t size);

void *arena_push(void *arena, void *value, size_t size);

void *arena_push_zeroed(void *arena, size_t size);

void arena_pop(void *arena, size_t size);

#endif // ARENA_H
