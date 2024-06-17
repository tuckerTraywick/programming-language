#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "object.h"
#include "symboltable.h"
#include "list.h"

// Represents metadata about a segment of an object.
struct SegmentHeader {
    size_t size;
    size_t offset;
};

// Represents metadata about an object file. Occurs at the beginning of the file.
struct ObjectFileHeader {
    size_t size;
    size_t entryPoint;
    struct SegmentHeader code;
    struct SegmentHeader immutableData;
    struct SegmentHeader mutableData;
    struct SegmentHeader symbolTable;
    struct SegmentHeader strings;
};

// Finds the length of the file, then maps it into memory using `mmap()`.
static uint8_t *mapFile(FILE *file) {
    rewind(file);
    fseek(file, 0L, SEEK_END);
    size_t length = (unsigned long)ftell(file);
    rewind(file);

    uint8_t *bytes = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fileno(file), 0);
    return bytes;
}

struct Object ObjectCreate(size_t segmentCapacity, size_t symbolTableCapacity) {
    return (struct Object){
        .size = 4*segmentCapacity + symbolTableCapacity,
        .data = NULL,
        .entryPoint = 0,
        .code = ListCreate(segmentCapacity, 1),
        .immutableData = ListCreate(segmentCapacity, 1),
        .mutableData = ListCreate(segmentCapacity, 1),
        .strings = ListCreate(segmentCapacity, 1),
    };
}

void ObjectDestroy(struct Object *object) {
    if (object->data) {
        munmap(object->data, object->size);
    } else {
        free(object->data);
    }
    *object = (struct Object){0};
}

bool ObjectIsMapped(struct Object *object) {
    return object->data == NULL;
}

// struct Object ObjectReadFromFile(FILE *file) {
//     uint8_t *bytes = mapFile(file);
//     struct ObjectFileHeader header = *(struct ObjectFileHeader*)bytes;
//     bytes += sizeof (struct ObjectFileHeader);
//     struct Object object = {
//         .size = header.size,
//         .data = bytes,
//         .entryPoint = header.entryPoint,
//         .code = bytes + header.code.offset,
//         .immutableData = bytes + header.immutableData.offset,
//         .mutableData = bytes + header.mutableData.offset,
//         .symbolTable = (SymbolTable)(bytes + header.symbolTable.offset),
//         .strings = (ListChar)(bytes + header.strings.offset),
//     };
//     return object;
// }

// void ObjectWriteToFile(struct Object *object, FILE *file) {
//     if (ObjectIsMapped(object)) {
//         fwrite(file, sizeof (struct ObjectFileHeader))
//     } else {

//     }
// }

// void ObjectPrint(struct Object *object) {
//     printf("size:                  %zu\n", header.size);
//     printf("entry point:           %zu\n", header.entryPoint);
//     printf("code size:             %zu\n", header.code.size);
//     printf("code offset:           %zu\n", header.code.offset);
//     printf("immutable data size:   %zu\n", header.immutableData.size);
//     printf("immutable data offset: %zu\n", header.immutableData.offset);
//     printf("mutable data size:     %zu\n", header.mutableData.size);
//     printf("mutable data offset:   %zu\n", header.mutableData.offset);
//     printf("symbol table size:     %zu\n", header.symbolTable.size);
//     printf("symbol table offset:   %zu\n", header.symbolTable.offset);
//     printf("string pool size:      %zu\n", header.symbolTable.size);
//     printf("string pool offset:    %zu\n", header.symbolTable.size);
// }
