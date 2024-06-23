#define _POSIX_C_SOURCE 200809L // for `fileno()`
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "object.h"
#include "symboltable.h"
#include "list.h"

// Metadata about a segment of an object.
struct SegmentHeader {
    size_t size;
    size_t offset;
};

// Metadata about an object file. Occurs at the beginning of the file.
struct ObjectFileHeader {
    size_t size;
    size_t entryPoint;
    struct SegmentHeader code;
    struct SegmentHeader immutableData;
    struct SegmentHeader mutableData;
    struct SegmentHeader strings;
    struct SegmentHeader symbolTable;
};

// Finds the length of the file, then maps it into memory using `mmap()`.
static uint8_t *mapFile(FILE *file) {
    // Find the length of the file.
    rewind(file);
    fseek(file, 0L, SEEK_END);
    size_t length = (unsigned long)ftell(file);
    rewind(file);
    // Map the file.
    uint8_t *bytes = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fileno(file), 0);
    return bytes;
}

struct Object ObjectCreate(size_t segmentCapacity, size_t symbolTableCapacity) {
    struct Object object = {
        .size = 0,
        .data = NULL,
        .entryPoint = 0,
        .code = ListCreate(segmentCapacity, 1),
        .immutableData = ListCreate(segmentCapacity, 1),
        .mutableData = ListCreate(segmentCapacity, 1),
        .strings = ListCreate(segmentCapacity, 1),
        .symbolTable = ListCreate(symbolTableCapacity, sizeof (struct Symbol)),
    };
    object.entryPoint = object.code.elements;
    return object;
}

void ObjectDestroy(struct Object *object) {
    if (object->data) {
        munmap(object->data, object->size);
        // We don't need to destroy the object's segments because they weren't actually allocated.
    } else {
        ListDestroy(&object->code);
        ListDestroy(&object->immutableData);
        ListDestroy(&object->mutableData);
        ListDestroy(&object->strings);
        ListDestroy(&object->symbolTable);
    }
    *object = (struct Object){0};
}

bool ObjectIsMapped(struct Object *object) {
    return object->data != NULL;
}

struct Object ObjectReadFromFile(FILE *file) {
    uint8_t *bytes = mapFile(file);
    // Read the header.
    struct ObjectFileHeader header = *(struct ObjectFileHeader*)bytes;
    bytes += sizeof (struct ObjectFileHeader);
    struct Object object = {
        .size = header.size,
        .data = bytes,
        .entryPoint = bytes + header.code.offset + header.entryPoint,
        // Setup each segment. These won't need to be destroyed with `ListDestroy()` because they
        // weren't actually created with `ListCreate()`.
        .code = (struct List){.elements = bytes + header.code.offset, .count = header.code.size},
        .immutableData = (struct List){.elements = bytes + header.immutableData.offset, .count = header.immutableData.size},
        .mutableData = (struct List){.elements = bytes + header.mutableData.offset, .count = header.mutableData.size},
        .strings = (struct List){.elements = bytes + header.strings.offset, .count = header.strings.size},
        .symbolTable = (struct List){.elements = bytes + header.symbolTable.offset, .capacity = header.symbolTable.size, .count = header.symbolTable.size, .elementSize = sizeof (struct Symbol)},
    };
    return object;
}

void ObjectWriteToFile(struct Object *object, FILE *file) {
    if (ObjectIsMapped(object)) {
        // Just copy the object byte for byte.
        uint8_t *bytes = object->data - sizeof (struct ObjectFileHeader);
        fwrite(bytes, 1, object->size + sizeof (struct ObjectFileHeader), file);
    } else {
        // Write the header.
        struct ObjectFileHeader header = {
            .size = object->size,
            .entryPoint = object->entryPoint - (uint8_t*)object->code.elements,
            .code = {.size = object->code.count, .offset = 0},
            .immutableData = {.size = object->immutableData.count, .offset = object->code.count},
            .mutableData = {.size = object->mutableData.count, .offset = object->code.count + object->immutableData.count},
            .strings = {.size = object->strings.count, .offset = object->code.count + object->immutableData.count + object->mutableData.count},
            .symbolTable = {.size = object->symbolTable.count, .offset = object->code.count + object->immutableData.count + object->mutableData.count + object->strings.count},
        };
        fwrite(&header, sizeof (struct ObjectFileHeader), 1, file);
        // Write the segments.
        fwrite(object->code.elements, 1, object->code.count, file);
        fwrite(object->immutableData.elements, 1, object->immutableData.count, file);
        fwrite(object->mutableData.elements, 1, object->mutableData.count, file);
        fwrite(object->strings.elements, 1, object->strings.count, file);
        fwrite(object->symbolTable.elements, sizeof (struct Symbol), object->symbolTable.count, file);
    }
}

void ObjectPrint(struct Object *object) {
    if (ObjectIsMapped(object)) {
        printf("memory mapped:         true\n");
        printf("size:                  %zu\n", object->size);
        printf("entry point:           %zu\n", object->entryPoint - (uint8_t*)object->code.elements);
        printf("code size:             %zu\n", object->code.count);
        printf("code offset:           %zu\n", (uint8_t*)object->code.elements - object->data);
        printf("immutable data size:   %zu\n", object->immutableData.count);
        printf("immutable data offset: %zu\n", (uint8_t*)object->immutableData.elements - object->data);
        printf("mutable data size:     %zu\n", object->mutableData.count);
        printf("mutable data offset:   %zu\n", (uint8_t*)object->mutableData.elements - object->data);
        printf("string pool size:      %zu\n", object->strings.count);
        printf("string pool offset:    %zu\n", (uint8_t*)object->strings.elements - object->data);
        printf("symbol table size:     %zu\n", object->symbolTable.count*object->symbolTable.elementSize);
        printf("symbol table offset:   %zu\n", (uint8_t*)object->symbolTable.elements - object->data);
    } else {
        printf("memory mapped:           false\n");
        printf("size:                    %zu\n", object->size);
        printf("entry point:             %zu\n", object->entryPoint - (uint8_t*)object->code.elements);
        printf("code size:               %zu\n", object->code.count);
        printf("code capacity:           %zu\n", object->code.capacity);
        printf("immutable data size:     %zu\n", object->immutableData.count);
        printf("immutable data capacity: %zu\n", object->immutableData.capacity);
        printf("mutable data size:       %zu\n", object->mutableData.count);
        printf("mutable data capacity:   %zu\n", object->mutableData.capacity);
        printf("string pool size:        %zu\n", object->strings.count);
        printf("string pool capacity:    %zu\n", object->strings.capacity);
        printf("symbol table size:       %zu\n", object->symbolTable.count*object->symbolTable.elementSize);
        printf("symbol table capacity:   %zu\n", object->symbolTable.capacity*object->symbolTable.elementSize);
    }
}
