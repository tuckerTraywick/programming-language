#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH16, 2, 1,
    PUSH64, 15,0,0,0, 0,0,0,0,
    CALL,
    POP16,
    HALT,

    PUSHA, 0,0,0,0, 0,0,0,0,
    LOAD16,
    PRINT16,
    RET,
};

int main(void) {
    run(code);
}
