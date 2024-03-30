#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH8, 1,
    PUSH64, 13,0,0,0, 0,0,0,0,
    CALL,
    HALT,

    PUSHL, 0,0,0,0, 0,0,0,0,
    LOAD8,
    PRINT8,
    RET,
};

int main(void) {
    run(code);
}
