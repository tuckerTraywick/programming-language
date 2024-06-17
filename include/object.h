#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "symboltable.h"

// Represents an object currently loaded into memory + some metadata.
struct Object {
    size_t size; // The size of `bytes`.
    uint8_t *bytes; // Points to the beginning of the data of the object. NULL unless the object was mapped from a file.
    uint8_t *code;
    uint8_t *entryPoint;
    uint8_t *immutableData;
    uint8_t *mutableData;
    struct SymbolTable symbolTable;
    bool executable;
};

// Returns true if an object was mapped from a file and false if it's sections were allocated using
// `malloc()`.
bool ObjectIsMapped(struct Object *object);

// Destroys an object and frees or unmaps its memory.
void ObjectDestroy(struct Object *object);

// Loads an object from a file. The returned object must be destroyed with `destroyObject()`.
struct Object ObjectReadFromFile(FILE *file);

// Writes an object to a file. The object and file still need to be destroyed after use.
void ObjectWriteToFile(FILE *file, struct Object *object);

// Prints information about an object.
void ObjectPrintInfo(struct Object *object);

#endif // OBJECT_H
