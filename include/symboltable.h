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
    EMPTY, // Indicates an unfilled symbol bucket.
    IMMUTABLE_DATA,
    MUTABLE_DATA,
    FUNCTION,
    STRUCT,
    ENUM,
};

// An entry in the symbol table mapping a name to an index from a segment.
struct Symbol {
    enum SymbolCategory category;
    uint64_t hash;
    size_t name; // Index of the name relative to the first byte of the string pool.
    size_t size;
    size_t index; // Index of the value the symbol refers to relative to the first byte after the object file header.
};

// Returns the hash code for a symbol name.
uint64_t getHash(char *name);

// Returns a pointer to the symbol with the given name. Returns NULL if there is no symbol with the
// same name.
struct Symbol *SymbolTableGetSymbol(SymbolTable *table, ListChar *strings, char *name);

// Associates a symbol with a name.
void SymbolTableSetSymbol(SymbolTable *table, ListChar *strings, enum SymbolCategory category, char *name, size_t size, size_t index);

// Reallocates a symbol table's buckets and rehsashes them. Destroys the old table.
void SymbolTableReserve(SymbolTable *table, ListChar *strings, size_t capacity);

// Combines two symbol tables in place. Puts the result in `first`. Shifts indexes of the symbols
// copied from `second` by `offset.
void SymbolTableCombine(SymbolTable *first, SymbolTable *second, ListChar *strings, size_t indexOffset, size_t nameOffset);

#endif // SYMBOLTABLE_H
