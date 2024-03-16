#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH16, 1, 0,
    PUSH16, 2, 0,
    ADDU16,
    PRINT16,
    HALT,
};

int main(void) {
    run(code);
}
