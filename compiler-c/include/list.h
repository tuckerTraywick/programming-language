#ifndef LIST_H
#define LIST_H

#include <stddef.h> // size_t

// Stores a dynamic array of items that can be grown as needed.
struct List {
    char *items;
    size_t itemSize;
    size_t count;
    size_t capacity;
    size_t capacityIncrement;
};

// Indicates a `List` of `LexingError`s or `ParsingErrors`.
typedef struct List ErrorList;

// Sets up a new list. Must be destroyed after use with `listDestroy()`.
void listInitialize(struct List *list, size_t itemSize, size_t capacity, size_t capacityIncrement);

// Deallocates a list's items and zeros its memory.
void listDestroy(struct List *list);

// Appends an item to a list
void listAppend(struct List *list, char *item);

// Gets the address of an element.
char *listGet(struct List *list, size_t index);

#endif // LIST_H
