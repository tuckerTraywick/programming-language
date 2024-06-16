#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "object.h"

void destroyObject(struct Object *object) {
    if (object->isMemoryMapped) {
        // TODO: Unmap the object's memory.
    } else {
        free(object->bytes);
    }
    *object = (struct Object){0};
}

void writeObject(FILE *file, struct Object *object) {
    // TODO: Handle failed `fwrite()`s.
    fwrite(object, sizeof(struct ObjectHeader), 1, file);
    fwrite(object->bytes, 1, object->header.size, file);
}

void readObject(FILE *file, struct Object *object) {
    // TODO: Change this function to use `mmap()` instead of allocating a new buffer for the object's bytes.
    fread(&object->header, sizeof(struct ObjectHeader), 1, file);
    // TODO: Handle failed `fread()`.
    object->bytes = malloc(object->header.size);
    // TODO: Handle failed `malloc()`.
    assert(object->bytes);
    fread(object->bytes, 1, object->header.size, file);
    // TODO: Handle failed `fread()`.
    // TODO: Set `isMemoryMapped` to true when I change this.
    object->isMemoryMapped = false;
}

void printObjectHeader(struct ObjectHeader *header) {
    printf("size:         %ld\n", header->size);
    printf("code:         %zu\n", header->code);
    printf("data:         %zu\n", header->data);
    printf("symbol table: %zu\n", header->symbolTable);
    printf("entry point:  %zu\n", header->entryPoint);
    printf("executable:   %s\n", (header->executable) ? "true" : "false");
}
