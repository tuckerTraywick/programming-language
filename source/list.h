#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

#define list_push(list, element) (list_push_impl((void**)(list), (element), sizeof *(element)))

#define list_pop(list, destination) (list_pop_impl((void**)(list), (destination), sizeof *(destination)))

void *list_create(size_t capacity, size_t element_size);

void list_destroy(void *list);

// Returns the maximum capacity of a list.
size_t list_get_capacity(void *list);

// Sets the maximum capacity of a list and returns a pointer to the list it. May reallocate the list.
bool list_set_capacity(void **list, size_t capacity);

// Returns the number of elements in the list.
size_t list_get_size(void *list);

// Sets the number of elements used in a list. DOES NOT ZERO ELEMENTS.
bool list_set_size(void **list, size_t size);

// Pushes an element on the end of a list. Returns true if successful and false if the list
// doesn't have enough capacity.
bool list_push_impl(void **list, void *element, size_t element_size);

// Pops an element off the end of a list. Returns true if successful and false if an error occurred.
bool list_pop_impl(void **list, void *destination, size_t element_size);

#endif // LIST_H
