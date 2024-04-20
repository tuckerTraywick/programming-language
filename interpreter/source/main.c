#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH32, 1, 2, 3, 4,
    FIRST32,
    HALT,
};

static uint8_t data[] = {
    2,1,3,4, 7,8,5,6,
};

int main(void) {
    run(code, data);
}
