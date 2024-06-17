#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "symboltable.h"
#include "list.h"

// Represents a possibly expandable segment in an object file.
typedef struct List ListUint8_t;

// Represents an object currently loaded into memory + some metadata.
struct Object {
    size_t size; // The size of the object's data (excluding this header).
    uint8_t *data; // NULL if the object was not mapped from a file.
    uint8_t *entryPoint;
    ListUint8_t code;
    ListUint8_t immutableData;
    ListUint8_t mutableData;
    SymbolTable symbolTable;
    ListChar strings;
};

// Creates a new object in RAM. Must be destroyed with `ObjectDestroy()`.
struct Object ObjectCreate(size_t segmentCapacity, size_t symbolTableCapacity);

// Destroys an object and frees or unmaps its memory.
void ObjectDestroy(struct Object *object);

// Returns true if an object was memory mapped from a file.
bool ObjectIsMapped(struct Object *object);

// Deserializes object from a file. The returned object must be destroyed with `ObjectDestroy()`.
struct Object ObjectReadFromFile(FILE *file);

// Serializes an object to a file. The object and file still need to be destroyed after use.
void ObjectWriteToFile(struct Object *object, FILE *file);

// Prints an object's metadata.
void ObjectPrint(struct Object *object);

#endif // OBJECT_H
