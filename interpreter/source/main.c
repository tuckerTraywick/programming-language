#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    COPY16, MODE(IMMEDIATE, STACK_TOP), 1, 1,
    COPY16, MODE(IMMEDIATE, STACK_TOP), 1, 2,
    ADDU16,
    PRINT16,
    HALT,
};

int main(void) {
    run(code);
}
