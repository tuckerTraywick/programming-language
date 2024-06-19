#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stddef.h>

// A mapping of names to symbols.
typedef struct List SymbolTable;

// A pool of strings.
typedef struct List ListChar;

// Describes what kind of thing a symbol points to.
enum SymbolCategory {
    IMMUTABLE_DATA = 1,
    MUTABLE_DATA,
    FUNCTION,
    STRUCT,
    ENUM,
};

// An entry in the symbol table mapping a name to an offset from a segment.
struct Symbol {
    enum SymbolCategory category;
    uint64_t hash;
    size_t name;
    size_t size;
    size_t offset;
};

// Returns the hash code for a symbol name.
uint64_t getHash(char *name);

#endif // SYMBOLTABLE_H
