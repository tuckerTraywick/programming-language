#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stdbool.h>
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

// Returns the hash for a symbol name.
uint64_t getHash(char *name);

// Probes the buckets of a symbol table for a name and returns a pointer to it. If the name is not
// already in the table, returns a pointer to an empty bucket. Returns NULL if the name wasn't found
// and there isn't an open bucket in the table.
struct Symbol *probe(SymbolTable *table, ListChar *strings, char *name);

// Returns a pointer to the symbol with the given name. Returns NULL if there is no symbol with the
// same name.
struct Symbol *SymbolTableGetSymbol(SymbolTable *table, ListChar *strings, char *name);

// Associates a symbol with a name. If `nameIndex` != -1, copies the name to the string pool. Else,
// sets the name field of the symbol to `nameIndex`.
void SymbolTableSetSymbol(SymbolTable *table, ListChar *strings, enum SymbolCategory category, char *name, size_t nameIndex, size_t size, size_t index);

// Reallocates a symbol table's buckets and rehsashes them. Destroys the old table.
void SymbolTableReserve(SymbolTable *table, ListChar *strings, size_t capacity);

// Combines two symbol tables in place. Puts the result in `first`. Shifts indexes of the symbols
// copied from `second` by `offset.
void SymbolTableCombine(SymbolTable *first, SymbolTable *second, ListChar *strings, size_t indexOffset, size_t nameOffset);

#endif // SYMBOLTABLE_H
