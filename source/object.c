#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "object.h"
#include "symboltable.h"

#define DEFAULT_SEGMENT_SIZE 16*1024 // 16 kilobytes

// Finds the length of the file, then maps it into memory using `mmap()`.
static uint8_t *mapFile(FILE *file) {
    rewind(file);
    fseek(file, 0L, SEEK_END);
    size_t length = (unsigned long)ftell(file);
    rewind(file);

    uint8_t *bytes = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fileno(file), 0);
    return bytes;
}

void ObjectDestroy(struct Object *object) {
    if (object->isMapped) {
        munmap(object->data, object->header.size);
    } else {
        free(object->data);
    }
    *object = (struct Object){0};
}

struct Object ObjectReadFromFile(FILE *file) {
    uint8_t *bytes = mapFile(file);
    struct Object object = {
        .isMapped = true,
        .header = *(struct ObjectHeader*)bytes,
        .data = bytes + sizeof (struct ObjectHeader),
    };
    return object;
}

void ObjectWriteToFile(struct Object *object, FILE *file) {
    fwrite(&object->header, 1, sizeof (struct ObjectHeader), file);
    fwrite(object->data, 1, object->header.size, file);
}

void ObjectPrint(struct Object *object) {
    struct ObjectHeader header = object->header;
    printf("size:                  %zu\n", header.size);
    printf("entry point:           %zu\n", header.entryPoint);
    printf("code size:             %zu\n", header.code.size);
    printf("code offset:           %zu\n", header.code.offset);
    printf("mutable data size:     %zu\n", header.immutableData.size);
    printf("immutable data offset: %zu\n", header.immutableData.offset);
    printf("symbol table size:     %zu\n", header.symbolTable.size);
    printf("symbol table offset:   %zu\n", header.symbolTable.offset);
}

#undef DEFAULT_SEGMENT_SIZE