#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stddef.h>

enum SymbolCategory {
    IMMUTABLE_DATA,
    MUTABLE_DATA,
    FUNCTION,
    STRUCT,
    ENUM,
};

struct Symbol {
    enum SymbolCategory category;
    uint64_t hash;
    size_t name;
    size_t size;
    size_t offset;
};

uint64_t getHash(char *name);

#endif // SYMBOLTABLE_H
