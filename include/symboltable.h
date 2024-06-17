#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

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
    size_t name;
    size_t type;
    size_t offset;
};

uint64_t getHash(char *name);

#endif // SYMBOLTABLE_H
