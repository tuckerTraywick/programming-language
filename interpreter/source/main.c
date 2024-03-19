#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSHSP,
    PRINT64,
    PUSH64, 0,0,0,0, 0,0,0,0,
    PUSHSO,
    PRINT64,
    PUSH64, 0,0,0,0, 0,0,0,0,
    PUSHSO,
    PRINT64,
    HALT,
};

int main(void) {
    run(code);
}
