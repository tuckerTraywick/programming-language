#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define max(a, b) ((a) >= (b)) ? (a) : (b)

List ListCreate(size_t capacity, size_t elementSize) {
    return (List){
        .capacity = capacity,
        .count = 0,
        .elementSize = elementSize,
        .elements = calloc(capacity*elementSize, 1),
    };
}

void ListDestroy(List *list) {
    free(list->elements);
    *list = (List){0};
}

void *ListGet(List *list, size_t index) {
    assert(index < list->count && "Index out of bounds.");
    return (char*)list->elements + index*list->elementSize;
}

void ListSet(List *list, size_t index, void *element) {
    memcpy(ListGet(list, index), element, list->elementSize);
}

void ListReserve(List *list, size_t capacity) {
    if (list->capacity < capacity) {
        while (list->capacity < capacity) {
            list->capacity *= LIST_GROWTH_FACTOR;
        }
        list->elements = realloc(list->elements, list->capacity*list->elementSize);
    }
}

void ListResize(List *list, size_t count) {
    if (count > list->capacity) {
        while (list->capacity < count) {
            list->capacity *= LIST_GROWTH_FACTOR;
        }
        list->elements = realloc(list->elements, list->capacity*list->elementSize);
    } else if (count < list->count/LIST_GROWTH_FACTOR) {
        list->capacity = count*LIST_GROWTH_FACTOR;
        list->elements = realloc(list->elements, list->capacity*list->elementSize);
    }
    list->count = count;
}

bool ListIsEmpty(List *list) {
    return list->count == 0;
}

void ListInsert(List *list, size_t index, void *element) {
    assert(index <= list->count && "Invalid index.");
    ListResize(list, list->count + 1);
    if (index < list->count - 1) {
        memmove(
            (char*)list->elements + index + 1,//ListGet(list, index + 1),
            (char*)list->elements + index,//ListGet(list, index),
            (list->count - index - 2)*list->elementSize
        );
    }
    ListSet(list, index, element);
}

void ListRemove(List *list, size_t index) {
    assert(index <= list->count && "Invalid index.");
    ListResize(list, list->count - 1);
    if (index < list->count - 1) {
        memmove(
            ListGet(list, index),
            ListGet(list, index + 1),
            (list->count - index)*list->elementSize
        );
    }
}

void ListPushFront(List *list, void *element) {
    ListInsert(list, 0, element);
}

void ListPushBack(List *list, void *element) {
     ListInsert(list, list->count, element);
}

void ListPopFront(List *list, size_t amount, void *result) {
    assert(list->count >= amount && "Popped too many elements.");
    if (amount == 0) {
        // Nothing to pop.
        return;
    }

    if (result) {
        memcpy(
            result,
            ListGet(list, amount - 1),
            list->elementSize
        );
    }
    memmove(
        list->elements,
        ListGet(list, amount),
        (list->count - amount)*list->elementSize
    );
    ListResize(list, list->count - amount);
}

void ListPopBack(List *list, size_t amount, void *result) {
    assert(list->count >= amount && "Popped too many elements.");
    if (amount == 0) {
        // Nothing to pop.
        return;
    }

    if (result) {
        memcpy(
            result,
            ListGet(list, list->count - amount),
            list->elementSize
        );    
    }
    ListResize(list, list->count - amount);
}

void ListCombine(List *first, List *second) {
    assert(first->elementSize == second->elementSize && "Can't combine two lists containing different data types.");
    if (second->count > first->capacity - first->count) {
        // Reserve enough for the second list's elements.
        ListReserve(first, first->count + second->count);
    }
    // Copy the second list's elements to the end of the first list.
    memcpy(
        (char*)first->elements + first->count*first->elementSize,
        second->elements,
        second->count*second->elementSize
    );
    first->count += second->count;
}

#undef max
