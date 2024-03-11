#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    COPY64, MODE(STACK_TOP_POINTER, STACK_TOP),
    PRINT64,
    HALT,
};

int main(void) {
    run(code);
}
