#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "interpreter.h"

int main(void) {
    // static uint8_t data[] = {
    //     PUSHB, 1,
    //     PRINTUB,
    //     HALT,
    // };

    // struct Object object = {
    //     .isMapped = false,
    //     .header = {
    //         .size = sizeof data,
    //         .entryPoint = 0,
    //         .code = {sizeof data, 0},
    //         .immutableData = {0, 0},
    //         .mutableData = {0, 0},
    //         .symbolTable = {0, 0},
    //     },
    //     .data = data,
    // };

    // FILE *file = fopen("example.bin", "wb+");
    // ObjectWriteToFile(&object, file);
    // object = (struct Object){0};
    // object = ObjectReadFromFile(file);
    // printf("object details\n");
    // ObjectPrint(&object);

    // printf("\nrunning code\n");
    // run(&object);

    // ObjectDestroy(&object);
    // fclose(file);

    struct HotObject object = HotObjectCreate(1024, 100);
    HotObjectDestroy(&object);

    return 0;
}
