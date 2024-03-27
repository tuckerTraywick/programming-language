#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH8, 1,
    PUSH8, 2,
    PUSHL, 0,0,0,0, 0,0,0,0,
    LOAD8,
    PUSHL, 1,0,0,0, 0,0,0,0,
    LOAD8,
    ADDI8,
    HALT,
};

int main(void) {
    run(code);
}
