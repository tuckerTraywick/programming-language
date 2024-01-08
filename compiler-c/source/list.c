#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdlib.h> // malloc(), realloc(), free()
#include <string.h> // memcpy()
#include "list.h"

void *listCreate(size_t elementSize, size_t capacity) {
    void *elements = malloc(capacity*elementSize);
    assert(elements != NULL && "`malloc()` failed.");
    return elements;
}

void listDestroy(void **list, size_t *capacity, size_t *count) {
    assert(list != NULL && "Must pass a list.");
    free(*list);
    *list = NULL;
    *capacity = 0;
    *count = 0;
}

void listAppend(void **list, size_t elementSize, size_t *capacity, size_t *count, void *element) {
    assert(list != NULL && "Must pass a list.");
    assert(capacity != NULL && "Must pass a capacity.");
    assert(count != NULL && "Must pass a count.");
    assert(element != NULL && "Must pass an element.");
    if (*count >= *capacity) {
        *capacity *= 2;
        void *newList = realloc(*list, *capacity*elementSize);
        assert(newList != NULL && "`realloc()` failed.");
        *list = list;
    }
    memcpy((char*)*list + *count*elementSize, element, elementSize);
    ++*count;
}
