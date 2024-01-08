#ifndef LIST_H
#define LIST_H

#include <stddef.h> // size_t

// Creates a new list with the given element size and capacity.
void *listCreate(size_t elementSize, size_t capacity);

// Deallocates a list.
void listDestroy(void **list, size_t *capacity, size_t *count);

// Expands a list to fit the given capacity.

// Appends an element to a list. Reallocates the list if needed.
void listAppend(void **list, size_t elementSize, size_t *capacity, size_t *count, void *element);

#endif // LIST_H
