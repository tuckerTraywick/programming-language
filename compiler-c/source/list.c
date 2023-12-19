#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdlib.h> // malloc(), realloc(), free()
#include <string.h> // memcpy()
#include "list.h"

void listInitialize(struct List *list, size_t itemSize, size_t capacity, size_t capacityIncrement) {
    char *items = malloc(capacity*itemSize);
    assert(items != NULL && "`malloc()` failed.");
    *list = (struct List) {
        .capacity = capacity,
        .capacityIncrement = capacityIncrement,
        .count = 0,
        .items = items,
        .itemSize = itemSize,
    };
}

void listDestroy(struct List *list) {
    assert(list != NULL && "Must pass a list.");
    free(list->items);
    *list = (struct List) {0};
}

void listAppend(struct List *list, char *item) {
    assert(list != NULL && "Must pass a list.");
    assert(item != NULL && "Must pass an item.");

    // Extend the list if needed.
    if (list->count >= list->capacity) {
        list->capacity += list->capacityIncrement;
        char *newItems = realloc(list->items, list->capacity*list->itemSize);
        assert(newItems != NULL && "`realloc()` failed.");
        list->items = newItems;
    }

    memcpy(list->items + list->count*list->itemSize, item, list->itemSize);
    ++list->count;
}

char *listGet(struct List *list, size_t index) {
    assert(index < list->count && "Index out of bounds.");
    return list->items + index*list->itemSize;
}
