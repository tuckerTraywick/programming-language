#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "symboltable.h"

// Represents the metadata for a segment of an object.
struct SegmentHeader {
    size_t size;
    size_t offset;
};

// Represents the metadata for an object.
struct ObjectHeader {
    size_t size; // The size of the object's data (excluding this header).
    size_t entryPoint;
    struct SegmentHeader code;
    struct SegmentHeader immutableData;
    struct SegmentHeader mutableData;
    struct SegmentHeader symbolTable;
};

// Represents an object currently loaded into memory + some metadata.
struct Object {
    bool isMapped; // True if the object was mapped into memory from a file, false if it was allocated with `malloc()`.
    struct ObjectHeader header;
    uint8_t *data;
};

// Destroys an object and frees or unmaps its memory.
void ObjectDestroy(struct Object *object);

// Deserializes object from a file. The returned object must be destroyed with `destroyObject()`.
struct Object ObjectReadFromFile(FILE *file);

// Serializes an object to a file. The object and file still need to be destroyed after use.
void ObjectWriteToFile(struct Object *object, FILE *file);

// Returns a pointer to a section in an object.
uint8_t *getSegmentPointer(struct Object *object, struct SegmentHeader segment);

// Prints an object's metadata.
void ObjectPrint(struct Object *object);

#endif // OBJECT_H
