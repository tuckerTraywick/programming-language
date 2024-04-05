#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH8, 0,
    PUSH8, 3,
    SUBI8,
    HALT,
};

static uint8_t data[] = {
    2, 1,
};

int main(void) {
    run(code, data);
}
