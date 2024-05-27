#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stddef.h>

// The category of thing a symbol is.
enum SymbolCategory {
    VARIABLE,
    FUNCTION,
};

// A symbol in the symbol table. Maps to code or data in the object.
struct Symbol {
    uint64_t name;
    uint64_t offset;
    enum SymbolCategory category;
};

// A bucket in the symbol hash table.
struct SymbolBucket {
    struct Symbol symbol;
    uint64_t name;
};

// A hash table that maps symbol names to things in the object.
struct SymbolTable {
    uint64_t capacity; // The number of buckets allocated.
    uint64_t size; // The number of buckets occupied.
    struct SymbolBucket *buckets; // The slots available for symbols.
};

// Returns a new symbol table and allocates memory for its buckets. The returned table must be
// destroyed with `destroySymbolTable()`.
struct SymbolTable createSymbolTable(size_t capacity);

// Deallocates a table's buckets and zeroes out its memory.
void destroySymbolTable(struct SymbolTable *table);

// Returns the offset of a name if it is mapped to in a symbol table. Returns 0 if the name is not
// found.
uint64_t getSymbol(struct SymbolTable *table, char *name);

// Sets the offset of a symbol. `name` must be at least 1 character long and `table` must be
// initialized.
void setSymbol(struct SymbolTable *table, char *name, uint64_t offset);

#endif // SYMBOLTABLE_H
