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
    size_t index;
};

struct SymbolTable {
    size_t capacity;
    size_t count;
    struct Symbol *symbols;
};

uint64_t getHash(char *name);

void SymbolTableDestroy(struct SymbolTable *SymbolTable);

#endif // SYMBOLTABLE_H
