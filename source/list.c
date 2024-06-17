#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define max(a, b) ((a) >= (b)) ? (a) : (b)

struct List ListCreate(size_t capacity, size_t elementSize) {
    return (struct List){
        .capacity = capacity,
        .count = 0,
        .elementSize = elementSize,
        .elements = malloc(capacity*elementSize),
    };
}

void ListDestroy(struct List *list) {
    free(list->elements);
    *list = (struct List){0};
}

void *ListGet(struct List *list, size_t index) {
    assert(index < list->count && "Index out of bounds.");
    return (char*)list->elements + index*list->elementSize;
}

void ListSet(struct List *list, size_t index, void *element) {
    memcpy(ListGet(list, index), element, list->elementSize);
}

void ListReserve(struct List *list, size_t capacity) {
    if (capacity > list->capacity) {
        list->capacity = capacity;
        list->elements = realloc(list->elements, list->capacity*list->elementSize);
    }
}

void ListSetCount(struct List *list, size_t count) {
    if (count > list->capacity) {
        while (list->capacity < count) {
            list->capacity *= LIST_GROWTH_FACTOR;
        }
        list->elements = realloc(list->elements, list->capacity*list->elementSize);
    }
    list->count = count;
}

bool ListIsEmpty(struct List *list) {
    return list->count == 0;
}

void ListInsert(struct List *list, size_t index, void *element) {
    assert(index <= list->count && "Invalid index.");
    ListSetCount(list, list->count + 1);
    if (index < list->count - 1) {
        memmove(
            ListGet(list, index + 1),
            ListGet(list, index),
            (list->count - index - 2)*list->elementSize
        );
    }
    ListSet(list, index, element);
}

// void ListRemove(struct List *list, size_t index) {
//     struct List *l = getList(list);
//     assert(index < l->count);
//     memmove(
//         l->elements + index*l->elementSize,
//         l->elements + (index + 1)*l->elementSize,
//         (l->count - index - 1)*l->elementSize
//     );
//     --l->count;
//     return l + 1;
// }

// void ListPushFront(struct List *list, void *element) {
//     return ListInsert(list, 0, element);
// }

// void ListPushBack(struct List *list, void *element) {
//     return ListInsert(list, ListGetCount(list), element);
// }

// void *ListPopFront(struct List *list, size_t amount, void *result) {
//     assert(ListGetCount(list) >= amount && "Popped too many elements.");
//     if (amount == 0) {
//         return list;
//     }

//     if (result) {
//         memcpy(
//             result,
//             (char*)list + (amount - 1)*ListGetElementSize(list),
//             ListGetElementSize(list)
//         );    
//     }
//     memmove(
//         list,
//         (char*)list + amount*ListGetElementSize(list),
//         (ListGetCount(list) - amount)*ListGetElementSize(list)
//     );
//     return ListSetCount(list, ListGetCount(list) - amount);
// }

// void *ListPopBack(struct List *list, size_t amount, void *result) {
//     assert(ListGetCount(list) >= amount && "Popped too many elements.");
//     if (amount == 0) {
//         return list;
//     }

//     if (result) {
//         memcpy(
//             result,
//             (char*)list + (ListGetCount(list) - amount)*ListGetElementSize(list),
//             ListGetElementSize(list)
//         );    
//     }
//     return ListSetCount(list, ListGetCount(list) - amount);
// }

// #undef min
