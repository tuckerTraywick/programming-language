#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "object.h"
#include "symboltable.h"

#define DEFAULT_SEGMENT_SIZE 16*1024 // 16 kilobytes

// Represents the beginning of an object file. Contains offsets of sections.
struct ObjectFileHeader {
    size_t size;
    size_t codeOffset;
    size_t entryPointOffset;
    size_t immutableDataOffset;
    size_t mutableDataOffset;
    size_t symbolTableOffset;
    bool executable;
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

bool ObjectIsMapped(struct Object *object) {
    return object->bytes != NULL;
}

void ObjectDestroy(struct Object *object) {
    if (ObjectIsMapped(object)) {
        // TODO: unmap the object's bytes.
    } else {
        free(object->code);
        free(object->immutableData);
        free(object->mutableData);
        SymbolTableDestroy(&object->symbolTable);
    }
    *object = (struct Object){0};
}

struct Object ObjectReadFromFile(FILE *file) {
    // Map the file and read the header
    uint8_t *bytes = mapFile(file);
    struct ObjectFileHeader header = *(struct ObjectFileHeader*)bytes;
    // Increment `bytes` to the beginning of the actual data.
    bytes += sizeof (struct ObjectFileHeader);
    struct Object object = {
        .bytes = bytes,
        .code = bytes + header.codeOffset,
        .entryPoint = bytes + header.entryPointOffset,
        .immutableData = bytes + header.immutableDataOffset,
        .mutableData = bytes + header.mutableDataOffset,
        .symbolTable = {0},
    };
    return object;
}

#undef DEFAULT_SEGMENT_SIZE
