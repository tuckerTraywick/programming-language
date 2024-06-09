#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t bytes[] = {
    2,1,3,4, 7,8,5,6,
    HALT,
};

int main(void) {
    struct Object object = {
        .isMemoryMapped = true,
        .header = (struct ObjectHeader) {
            .executable = true,
            .size = sizeof(bytes),
            .code = 8,
            .data = 0,
            .entryPoint  = 8,
        },
        .bytes = bytes,
    };

    FILE *file = fopen("./example.bin", "w+");
    writeObject(file, &object);
    fclose(file);

    file = fopen("./example.bin", "r");
    struct Object result;
    readObject(file, &result);
    printObjectHeader(&result.header);
    destroyObject(&result);

    run(&object);

    fclose(file);

    return 0;
}
