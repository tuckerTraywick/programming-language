#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH32, 1, 2, 3, 4,
    PUSH32, 5, 6, 7, 8,
    PUSHL, 0,0,0,0, 0,0,0,0,
    PUSHL, 4,0,0,0, 0,0,0,0,
    COPY32,
    HALT,
};

int main(void) {
    run(code);
}
