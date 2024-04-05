#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH16, 1, 0,
    PUSH64, 13,0,0,0, 0,0,0,0,
    CALL,

    PUSHA, 2,0,0,0, 0,0,0,0,
    LOAD16,
    PUSH16, 1, 0,
    ADDI16,
    HALT,
};

static uint8_t data[] = {
    1,
};

int main(void) {
    run(code, data);
}
