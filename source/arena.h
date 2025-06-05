#ifndef ARENA_H

#include <stddef.h>

// Creates a new arena and returns a pointer to its data.
void *arena_create(size_t capacity);

// Destroys an arena and frees all of its allocations at once.
void arena_destroy(void *arena);

// Returns the maximum capacity of an arena.
size_t arena_get_capacity(void *arena);

// Sets the maximum capacity of an arena and returns a pointer to it. May reallocate the arena.
void *arena_set_capacity(void *arena, size_t capacity);

// Returns the number of bytes used in an arena.
size_t arena_get_size(void *arena);

// Sets the number of bytes used in an arena.
void *arena_set_size(void *arena, size_t size);

// Creates a new allocation in an arena and returns a pointer to it. Returns NULL if the arena
// doesn't have enough free space for the allocation.
void *arena_allocate(void *arena, size_t size);

// Creates a new allocation in an arena then zeroes it and returns a pointer to it. Returns NULL
// if the arena doesn't have enough free space for the allocation.
void *arena_allocate_zeroed(void *arena, size_t size);

// Creates a new allocation in the arena then copies a value to it and returns a pointer to the
// allocation. Returns NULL if the arena doesn't have enough free space for the allocation.
void *arena_push(void *arena, void *value, size_t size);

// Reduces the size of an arena by a given amount. Returns the number of bytes popped. If the size
// of the arena is already lower than `size`, this function will return the size of the arena before
// popping.
size_t arena_pop(void *arena, size_t size);

#endif // ARENA_H
