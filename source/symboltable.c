#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "symboltable.h"
#include "list.h"

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

struct Symbol *SymbolTableGetSymbol(SymbolTable *table, ListChar *strings, char *name) {
    // TODO: Use a 32 bit hash for 32 bit pointers and a 64 bit hash for 64 bit pointers.
    uint64_t hash = getHash(name);
    size_t startIndex = hash%table->capacity;
    size_t index = startIndex;

    // Probe the buckets linearly for the name.
    while (true) {
        struct Symbol *bucket = (struct Symbol*)table->elements + index;
        // If the bucket is empty, the symbol is not in the table.
        if (bucket->category == EMPTY) {
            return NULL;
        }

        // If the hash and the name are equal, the symbol has been found.
        if (bucket->hash == hash && strcmp((char*)strings->elements + bucket->name, name) == 0) {
            return bucket;
        }

        // Otherwise, advance to the next bucket.
        index = (index + 1)%table->capacity;
        // If we run out of buckets, the symbol is not in the table.
        if (index == startIndex) {
            return NULL;
        }
    }
}

void SymbolTableSetSymbol(SymbolTable *table, ListChar *strings, char *name, struct Symbol *symbol) {
    // TODO: Use a 32 bit hash for 32 bit pointers and a 64 bit hash for 64 bit pointers.
    uint64_t hash = getHash(name);
    size_t startIndex = hash%table->capacity;
    size_t index = startIndex;

    // Probe the buckets linearly for the name or an empty spot.
    while (true) {
        struct Symbol *bucket = (struct Symbol*)table->elements + index;
        // If the bucket is empty, put the symbol in the bucket and add the string to the string
        // pool.
        if (bucket->category == EMPTY) {
            // Fill the bucket.
            *bucket = *symbol;
            bucket->hash = hash;
            bucket->name = strings->count;
            ++table->count;

            // Reserve room for the name and append it to the string pool.
            size_t nameLength = strlen(name);
            ListReserve(strings, strings->count + nameLength + 1);
            strcpy((char*)strings->elements + strings->count, name);
            ListResize(strings, strings->count + nameLength + 1); // `+ 1` for null terminator.
            return;
        }

        // If the bucket is not empty but has the right symbol name, overwrite the bucket.
        if (bucket->hash == hash && strcmp((char*)strings->elements + bucket->name, name) == 0) {
            size_t bucketName = bucket->name;
            *bucket = *symbol;
            bucket->hash = hash;
            bucket->name = bucketName;
            return;
        }

        // Otherwise, advance to the next bucket.
        index = (index + 1)%table->capacity;
        // If we run out of buckets, expand the table and rehash all of the symbols.
        if (index == startIndex) {
            // TODO: Reserve more buckets.
        }
    }
}

void SymbolTableRehash(SymbolTable *table, ListChar *strings, size_t capacity) {
    assert(capacity >= table->count && "Capacity must be larger.");
    SymbolTable newTable = ListCreate(capacity, table->elementSize);
    memset(newTable.elements, 0, newTable.capacity*newTable.elementSize);
    for (size_t i = 0; i < table->capacity; ++i) {
        struct Symbol *oldSymbol = (struct Symbol*)table->elements + i;
        if (oldSymbol->category != EMPTY) {
            SymbolTableSetSymbol(&newTable, strings, (char*)ListGet(strings, oldSymbol->name), oldSymbol);
        }
    }
    
    ListDestroy(table);
    *table = newTable;
}
