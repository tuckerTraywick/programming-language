#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include "symboltable.h"

struct SymbolTable createSymbolTable(size_t capacity) {
    struct SymbolTable table = {
        .capacity = capacity,
        .size = 0,
        .buckets = malloc(sizeof(struct SymbolBucket)*capacity),
    };
    // TODO: Handle failed `malloc()`.
    assert(table.buckets && "`malloc()` failed.");
}

void destroySymbolTable(struct SymbolTable *table) {
    free(table->buckets);
    *table = (struct SymbolTable){0};
}
