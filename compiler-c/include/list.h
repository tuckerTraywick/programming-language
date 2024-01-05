#ifndef LIST_H
#define LIST_H

#include <stddef.h> // size_t

// Creates a new list with the given type of elements and the given capacity.
#define listCreate(type, capacity) (listCreateImpl(sizeof (type), (capacity)))

// Appends the given element to the list. Doubles the capacity of the list if it runs out of space.
#define listAppend(list, element) (listAppendImpl((list), (char*)(element), sizeof *(element)))

// Gets the element at the given index in the list.
#define listGet(type, list, index) ((type*) listGetImpl(sizeof (type), (list), (index)))

// Gets the last element of the list.
#define listLast(type, list) ((type*) listLastImpl(sizeof (type), (list)))

// Represents a dynamic array that can be grown as needed.
struct List {
    char *elements;
    size_t capacity;
    size_t count;
};

// Deallocates a list's elements and zeros its memory.
void listDestroy(struct List *list);

// Implementation of `listCreate()`.
struct List listCreateImpl(size_t elementSize, size_t capacity);

// Implementation of `listAppend()`.
void listAppendImpl(struct List *list, char *element, size_t elementSize);

// Implementation of `listGet()`.
char *listGetImpl(size_t elementSize, struct List *list, size_t index);

// Implementation of `listLast()`.
char *listLastImpl(size_t elementSize, struct List *list);

#endif // LIST_H
