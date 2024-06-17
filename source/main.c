#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "object.h"

int main(void) {
    static uint8_t data[10] = {0};
    struct Object object = {
        .isMapped = false,
        .header = {
            .size = 40,
            .entryPoint = 0,
            .code = {10, 0},
            .immutableData = {10, 20},
            .mutableData = {10, 30},
            .symbolTable = {10, 40},
        },
        .data = data,
    };

    FILE *file = fopen("example.bin", "wb+");
    ObjectWriteToFile(&object, file);
    object = (struct Object){0};
    object = ObjectReadFromFile(file);
    ObjectPrint(&object);

    ObjectDestroy(&object);
    fclose(file);
    return 0;
}
