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

struct Symbol *getSymbol(SymbolTable *table, ListChar *strings, char *name) {
    // TODO: Use a 32 bit hash for 32 bit pointers and a 64 bit hash for 64 bit pointers.
    uint64_t hash = getHash(name);
    size_t startIndex = hash%table->capacity;
    size_t index = startIndex;

    // Probe the buckets linearly for the name.
    while (true) {
        struct Symbol *symbol = (struct Symbol*)ListGet(table, index);
        // If the bucket is empty, the name is not in the table.
        if (symbol->category == EMPTY) {
            return NULL;
        }

        // If the hash and the name are equal, the symbol has been found.
        if (symbol->hash == hash && strcmp((char*)strings->elements + symbol->name, name) == 0) {
            return symbol;
        }

        // Otherwise, advance to the next bucket.
        index = (index + 1)%table->capacity;
        // If we run out of buckets, the name is not in the table.
        if (index == startIndex) {
            return NULL;
        }
    }
}
