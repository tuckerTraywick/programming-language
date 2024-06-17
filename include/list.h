#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

// The value the list's capacity is multiplied/divided by when the list needs to grow or shrink.
#ifndef LIST_GROWTH_FACTOR
	#define LIST_GROWTH_FACTOR 2
#endif

// Returns a pointer the elements of a new list. Cast this to a pointer to whatever type of element
// you're storing in the list. Use `[]` and `[]=` to get and set elements of the list. The list
// structure is internal and not exposed to the programmer. Must be destroyed by `ListDestroy()`.
void *ListCreate(size_t capacity, size_t elementSize);

// Deallocates a list.
void ListDestroy(void *list);

// Returns the maximum number of elements in a list.
size_t ListGetCapacity(void *list);

// Suggests a new capacity for a list. If the new capacity differs from the old capacity by more
// than the growth factor, the list is reallocated to fit the new capacity. Will not shrink the
// capacity beyond the count of the list. Returns a pointer to the list or a new list if the
// capacity changed.
void *ListSetCapacity(void *list, size_t capacity);

// Returns the current number of elements in a list.
size_t ListGetCount(void *list);

// Grows or shrinks the count of a list. Returns a pointer to the list or a new list if the
// capacity changes.
void *ListSetCount(void *list, size_t count);

// Returns the size in bytes of one element of a list.
size_t ListGetElementSize(void *list);

// Returns true if a list has 0 elements.
bool ListIsEmpty(void *list);

// Inserts an element at an index in a list.
void *ListInsert(void *list, size_t index, void *element);

// Removes the element at an index in a list.
void *ListRemove(void *list, size_t index);

// Prepends an element to a list.
void *ListPushFront(void *list, void *element);

// Appends an element to a list.
void *ListPushBack(void *list, void *element);

// Pops a number of elements from the front of a list and stores the last element popped in
// `result`.
void *ListPopFront(void *list, size_t amount, void *result);

// Pops a number of elements from the back of a list and stores the last element popped in `result`.
void *ListPopBack(void *list, size_t amount, void *result);

#endif // LIST_H
