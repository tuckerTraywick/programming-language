#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static struct SymbolTableNode symbols[] = {
    {.ch='a', .child=24}, // 0
    {.ch='b', .child=48}, // 24
    {.ch='x', .child=72, .next=96}, // 48
    {.ch='\0', .child=1}, // 72
    {.ch='c', .child=120}, // 96
    {.ch='\0', .child=2}, // 120
};

static uint8_t bytes[] = {
    2,1,3,4, 7,8,5,6,
    PUSH32, 1,2,3,4,
    FIRST32,
    HALT,
    [15 ... 100] = 0,
};

int main(void) {
    // struct Object object = {
    //     .isMemoryMapped = true,
    //     .header = (struct ObjectHeader) {
    //         .executable = true,
    //         .size = sizeof(bytes),
    //         .code = 8,
    //         .data = 0,
    //         .entryPoint  = 8,
    //     },
    //     .bytes = bytes,
    // };

    // FILE *file = fopen("./example.bin", "w+");
    // writeObject(file, &object);
    // fclose(file);

    // file = fopen("./example.bin", "r");
    // struct Object result;
    // readObject(file, &result);
    // printObjectHeader(&result.header);
    // destroyObject(&result);

    // run(&object);

    // memcpy(bytes + 20, (uint8_t*)symbols, sizeof(symbols));
    printf("offset = %ld\n", getSymbol(symbols, symbols, ""));

    return 0;
}
