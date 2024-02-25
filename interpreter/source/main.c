#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    COPY8, MODE(IMMEDIATE, STACK_TOP), 1,
    COPY8, MODE(IMMEDIATE, STACK_TOP), 2,
    COPY8, MODE(STACK_OFFSET, STACK_TOP), 0, 0, 0, 0,  0, 0, 0, 0,
    // PRINT8,
    HALT,
};

int main(void) {
    run(code);
}
