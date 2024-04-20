#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH16, 255, 255,
    DUP16,
    PRINTI16,
    ABSI16,
    PRINTI16,
    HALT,
};

static uint8_t data[] = {
    2,1,3,4, 7,8,5,6,
};

int main(void) {
    run(code, data);
}
