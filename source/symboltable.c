#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "symboltable.h"
#include "list.h"

struct Symbol *probe(SymbolTable *table, ListChar *strings, char *name) {
    uint64_t hash = getHash(name);
    size_t startIndex = hash%table->capacity;
    size_t bucketIndex = startIndex;

    // Probe the buckets linearly for the name or an empty spot.
    while (true) {
        struct Symbol *bucket = (struct Symbol*)table->elements + bucketIndex;
        // If the bucket is empty or the name matches, return it.
        if (bucket->category == EMPTY
        || (bucket->hash == hash && strcmp((char*)strings->elements + bucket->name, name) == 0)) {
            return bucket;
        }

        bucketIndex = (bucketIndex + 1)%table->capacity;
        if (bucketIndex == startIndex) {
            return NULL;
        }
    }
}

uint64_t getHash(char *name) {
    // Uses the MurmurOAT algorithm:
    // https://stackoverflow.com/questions/7666509/hash-function-for-string.
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

struct Symbol *SymbolTableGetSymbol(SymbolTable *table, ListChar *strings, char *name) {
    // TODO: Use a 32 bit hash for 32 bit pointers and a 64 bit hash for 64 bit pointers.
    struct Symbol *symbol = probe(table, strings, name);
    if (symbol != NULL && symbol->category != EMPTY) {
        return symbol;
    }
    return NULL;
}

void SymbolTableSetSymbol(SymbolTable *table, ListChar *strings, enum SymbolCategory category, char *name, size_t nameIndex, size_t size, size_t index) {
    // TODO: Use a 32 bit hash for 32 bit pointers and a 64 bit hash for 64 bit pointers.
    struct Symbol *bucket = probe(table, strings, name);
    
    // If the probe failed, we must allocate more space and probe for the bucket again.
    if (bucket == NULL) {
        SymbolTableReserve(table, strings, table->count + 1);
        bucket = probe(table, strings, name);
    }

    uint64_t hash = getHash(name); // TODO: Don't compute the hash twice here.
    // If the bucket is empty, fill it and add the name to the string pool.
    if (bucket->category == EMPTY) {
        *bucket = (struct Symbol){
            .category = category,
            .hash = hash,
            .name = (nameIndex) ? nameIndex - 1 : strings->count,
            .size = size,
            .index = index
        };
        ++table->count;

        if (nameIndex == 0) {
            size_t nameLength = strlen(name);
            ListReserve(strings, strings->count + nameLength + 1); // `+ 1` for null terminator.
            strcpy((char*)strings->elements + strings->count, name);
            // Reserve room for the name and append it to the string pool.
            strings->count += nameLength + 1;
        }
    // Else if the bucket has the right name, overwrite it.
    } else if (bucket->hash == hash && !strcmp((char*)strings->elements + bucket->name, name)) {
        *bucket = (struct Symbol){
            .hash = hash,
            .name = bucket->name,
            .size = size,
            .index = index
        };
    }
}

void SymbolTableReserve(SymbolTable *table, ListChar *strings, size_t capacity) {
    assert(capacity >= table->count && "Capacity must be larger.");
    // Create a new table and zero all of its elements.
    // TODO: Maybe just make `ListCreate()` use `calloc()`?
    SymbolTable newTable = ListCreate(capacity, table->elementSize);
    memset(newTable.elements, 0, newTable.capacity*newTable.elementSize);

    // Add each non-empty bucket from the old table to the new table.
    for (size_t i = 0; i < table->capacity; ++i) {
        struct Symbol *symbol = (struct Symbol*)table->elements + i;
        if (symbol->category != EMPTY) {
            SymbolTableSetSymbol(
                &newTable,
                strings,
                symbol->category,
                (char*)strings->elements + symbol->name,
                symbol->name + 1,
                symbol->size,
                symbol->index
            );
        }
    }
    
    ListDestroy(table);
    *table = newTable;
}

void SymbolTableCombine(SymbolTable *first, SymbolTable *second, ListChar *strings, size_t indexOffset, size_t nameOffset) {
    SymbolTableReserve(first, strings, first->count + second->count);
    // Loop over each symbol from the second table and add it to the first.
    for (size_t i = 0; i < second->capacity; ++i) {
        struct Symbol *symbol = (struct Symbol*)second->elements + i;
        // If the bucket is not empty, add it to the first table.
        if (symbol->category != EMPTY) {
            SymbolTableSetSymbol(
                first,
                strings,
                symbol->category,
                (char*)strings->elements + symbol->name + nameOffset,
                symbol->name + 1,
                symbol->size,
                symbol->index + indexOffset
            );
        }
    }
}
