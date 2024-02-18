#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    COPY8, MODE(IMMEDIATE, STACK), 255,
    PRINT8,
    HALT,
};

int main(void) {
    run(code);
    printf("Done.\n");
}
