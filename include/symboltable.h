#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stddef.h>

// An expandable list of buckets for a hashmap of names to symbols.
typedef struct Symbol *SymbolTable;

// Represents an expandable string pool.
typedef char *ListChar;

// Describes what a symbol points to.
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

// Adds a symbol to a symbol table.
void SymbolTableAdd(SymbolTable table, ListChar strings, char *name, struct Symbol *symbol);

#endif // SYMBOLTABLE_H
