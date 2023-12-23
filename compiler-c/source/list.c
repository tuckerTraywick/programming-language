#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdlib.h> // malloc(), realloc(), free()
#include <string.h> // memcpy()
#include "list.h"

void listDestroy(struct List *list) {
    assert(list != NULL && "Must pass a list.");
    free(list->elements);
    *list = (struct List) {0};
}

struct List listCreateImpl(size_t elementSize, size_t capacity) {
    char *elements = malloc(capacity*elementSize);
    assert(elements != NULL && "`malloc()` failed.");
    return (struct List) {
        .elements = elements,
        .capacity = capacity,
        .count = 0,
    };
}

void listAppendImpl(struct List *list, char *element, size_t elementSize, size_t capacityIncrement) {
    assert(list != NULL && "Must pass a list.");
    assert(element != NULL && "Must pass an element.");
    assert(capacityIncrement && "Must pass a capacity increment > 0.");

    // Extend the list if needed.
    if (list->count >= list->capacity) {
        list->capacity += elementSize;
        char *newElements = realloc(list->elements, (list->capacity + capacityIncrement)*elementSize);
        assert(newElements != NULL && "`realloc()` failed.");
        list->elements = newElements;
    }

    memcpy(list->elements + list->count*elementSize, element, elementSize);
    ++list->count;
}

char *listGetImpl(size_t elementSize, struct List *list, size_t index) {
    assert(list != NULL && "Must pass a list.");
    assert(index < list->count && "Index out of bounds.");
    return list->elements + index*elementSize;
}

char *listLastImpl(size_t elementSize, struct List *list) {
    assert(list != NULL && "Must pass a list.");
    return list->elements + list->count*elementSize;
}
