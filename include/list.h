#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

// The value the list's capacity is multiplied/divided by when the list needs to grow or shrink.
#ifndef LIST_GROWTH_FACTOR
	#define LIST_GROWTH_FACTOR 2
#endif

// Represents a dynamic array that can shrink or grow at runtime.
typedef struct List {
	size_t capacity;
	size_t count;
	size_t elementSize;
	void *elements;
} List;

// Returns a new list. Must be destroyed by `ListDestroy()`.
List ListCreate(size_t capacity, size_t elementSize);

// Deallocates a list and zeroes it.
void ListDestroy(List *list);

// Gets a pointer to the element at an index in a list.
void *ListGet(List *list, size_t index);

// Gets a pointer to the element at an index in a list.
void ListSet(List *list, size_t index, void *element);

// Makes sure a list has a large enough capacity to fit `capacity` elements. Will reallocate if the
// capacity is larger than the current capacity.
void ListReserve(List *list, size_t capacity);

// Changes the count of a list. Will reallocate if the count is greater than the capacity.
void ListResize(List *list, size_t count);

// Returns true if a list has 0 elements.
bool ListIsEmpty(List *list);

// Inserts an element at an index in a list.
void ListInsert(List *list, size_t index, void *element);

// Removes the element at an index in a list.
void ListRemove(List *list, size_t index);

// Prepends an element to a list.
void ListPushFront(List *list, void *element);

// Appends an element to a list.
void ListPushBack(List *list, void *element);

// Pops a number of elements from the front of a list and copies the last element popped to `result`
// if `result` is not NULL.
void ListPopFront(List *list, size_t amount, void *result);

// Pops a number of elements from the back of a list and copies the last element popped to `result`
// if result is not NULL.
void ListPopBack(List *list, size_t amount, void *result);

// Combines two lists in place. Puts the result in `first`.
void ListCombine(List *first, List *second);

// Returns a pointer to the first element in the list. Returns null if the list is empty.
void *ListFront(List *list);

// Returns a pointer to the last element in the list. Returns null if the list is empty.
void *ListBack(List *list);

#endif // LIST_H
