#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH64, 0,0,0,0, 0,0,0,0,
    PUSHD,
    LOAD16,
    PRINT16,
    HALT,
};

static uint8_t data[] = {
    2, 1,
};

int main(void) {
    run(code, data);
}
