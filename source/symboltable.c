#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "symboltable.h"

uint64_t getHash(char *name) {
    // Uses the MurmurOAT algorithm: https://stackoverflow.com/questions/7666509/hash-function-for-string.
    // I do not understand how this works.
    uint64_t hash = 525201411107845655ull;
    char *ch = name;
    while (*ch != '\0') {
        hash ^= *ch;
        hash *= 0x5bd1e9955bd1e995;
        hash ^= hash >> 47;
        ++ch;
    }
    return hash;
}

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

struct Symbol *getSymbol(struct SymbolTable *table, char *name) {
    // Hash the name.
    uint64_t hash = getHash(name);
    uint64_t index = hash%table->capacity;

    // Return the symbol if it is found.
    if (table->buckets[index].hash == hash) {
        return &table->buckets[index].symbol;
    }

    // Probe the buckets if there is a collision.
    for (uint64_t i = 0; i < table->capacity; ++i) {

    }
}

void setSymbol(struct SymbolTable *table, char *name, struct Symbol *symbol) {

}
