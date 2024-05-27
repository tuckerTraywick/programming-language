#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "symboltable.h"

// The first section of an object file. Gives general info about the object and its size.
struct ObjectHeader {
    uint64_t size; // Size of the object (excluding the header).
    uint64_t code; // The offset from the end of the header.
    uint64_t data; // The offset from the end of the header.
    uint64_t symbols; // The offset of the symbol table from the end of the header.
    uint64_t entryPoint; // The offset the end of the header. Where the interpreter starts executing.
    bool executable; // Whether the object can be executed. This is the last field for alignment purposes.
};

// Represents an object currently loaded into memory + some metadata. All offsets in the header are
// relative to the beginning of `bytes`.
struct Object {
    struct ObjectHeader header;
    uint8_t *bytes; // The bytes of the object. Contains the code and data.
    uint8_t *code; // The beginning of the code segment.
    uint8_t *data; // The beginning of the data segment.
    struct SymbolTable symbolTable; // A mapping of symbols to offsets.
    bool isMemoryMapped; // If true, the object's bytes were mapped from a file. Else, they were `malloc()`ed.
};

// Destroys an object and frees or unmaps its memory.
void destroyObject(struct Object *object);

// Writes an object to a file. The object and file still need to be destroyed after use.
void writeObject(FILE *file, struct Object *object);

// Loads an object from a file. The returned object must be destroyed with `destroyObject()`.
void readObject(FILE *file, struct Object *object);

// Prints an object header nicely.
void printObjectHeader(struct ObjectHeader *header);

#endif // OBJECT_H
