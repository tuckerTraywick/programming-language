#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define max(a, b) ((a) >= (b)) ? (a) : (b)

// Returns a pointer to the beggining of actual list object pointed to by `elements`.
#define getList(elements) ((struct List*)(elements) - 1)

// The internal representation of a list. The programmer only ever interacts with a pointer to
// `elements`. The capacity, count, and element size are allocated inline with the elements
// themselves.
struct List {
    size_t capacity;
    size_t count;
    size_t elementSize;
	char elements[];
};

// Sets the element at an index in a list.
static void setElement(struct List *list, size_t index, void *element) {
    memcpy(list->elements + index*list->elementSize, element, list->elementSize);
}

void *ListCreate(size_t capacity, size_t elementSize) {
    struct List *list = malloc(sizeof *list + capacity*elementSize);
    *list = (struct List){
        .capacity = capacity,
        .count = 0,
        .elementSize = elementSize,
    };
    return list + 1;
}

void ListDestroy(void *list) {
    free(getList(list));
}

size_t ListGetCapacity(void *list) {
    return getList(list)->capacity;
}

void *ListSetCapacity(void *list, size_t capacity) {
    struct List *l = getList(list);
    if (capacity >= l->capacity*LIST_GROWTH_FACTOR) {
        l->capacity = capacity;
        l = realloc(l, sizeof *l + l->capacity*l->elementSize);
    } else if (l->capacity > l->count && capacity <= l->capacity/LIST_GROWTH_FACTOR) {
        l->capacity = max(l->count, capacity);
        l = realloc(l, sizeof *l + l->capacity*l->elementSize);
    }
    return l + 1;
}

size_t ListGetCount(void *list) {
    return getList(list)->count;
}

void *ListSetCount(void *list, size_t count) {
    struct List *l = getList(list);
    l->count = count;
    list = ListSetCapacity(list, count);
    return list;
}

size_t ListGetElementSize(void *list) {
    return getList(list)->elementSize;
}

bool ListIsEmpty(void *list) {
    return ListGetCount(list) == 0;
}

void *ListInsert(void *list, size_t index, void *element) {
    struct List *l = getList(list);
    assert(index <= l->count && "Invalid index.");
    if (l->count == l->capacity) {
        l->capacity *= LIST_GROWTH_FACTOR;
        l = realloc(l, sizeof *l + l->capacity*l->elementSize);
    }

    if (index < l->count) {
        memmove(
            l->elements + (index + 1)*l->elementSize,
            l->elements + index*l->elementSize,
            (l->count - index)*l->elementSize
        );
    }
    setElement(l, index, element);
    ++l->count;
    return l + 1;
}

void *ListRemove(void *list, size_t index) {
    struct List *l = getList(list);
    assert(index < l->count);
    memmove(
        l->elements + index*l->elementSize,
        l->elements + (index + 1)*l->elementSize,
        (l->count - index - 1)*l->elementSize
    );
    --l->count;
    return l + 1;
}

void *ListPushFront(void *list, void *element) {
    return ListInsert(list, 0, element);
}

void *ListPushBack(void *list, void *element) {
    return ListInsert(list, ListGetCount(list), element);
}

void *ListPopFront(void *list, size_t amount, void *result) {
    assert(ListGetCount(list) >= amount && "Popped too many elements.");
    if (amount == 0) {
        return list;
    }

    if (result) {
        memcpy(
            result,
            (char*)list + (amount - 1)*ListGetElementSize(list),
            ListGetElementSize(list)
        );    
    }
    memmove(
        list,
        (char*)list + amount*ListGetElementSize(list),
        (ListGetCount(list) - amount)*ListGetElementSize(list)
    );
    return ListSetCount(list, ListGetCount(list) - amount);
}

void *ListPopBack(void *list, size_t amount, void *result) {
    assert(ListGetCount(list) >= amount && "Popped too many elements.");
    if (amount == 0) {
        return list;
    }

    if (result) {
        memcpy(
            result,
            (char*)list + (ListGetCount(list) - amount)*ListGetElementSize(list),
            ListGetElementSize(list)
        );    
    }
    return ListSetCount(list, ListGetCount(list) - amount);
}

#undef min
