#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    COPY8, MODE(IMMEDIATE, STACK_TOP), 255,
    COPY8, MODE(STACK_TOP, STACK_TOP),
    PRINT8,
    HALT,
};

int main(void) {
    run(code);
}
