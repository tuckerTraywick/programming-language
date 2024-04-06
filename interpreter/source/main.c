#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH8, 98,
    PUSH8, 2,
    MODI8,
    HALT,
};

static uint8_t data[] = {
    2, 1,
};

int main(void) {
    run(code, data);
}
