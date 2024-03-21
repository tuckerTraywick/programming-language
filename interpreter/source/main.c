#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH16, 1, 2,
    PUSH16, 0, 0,

    PUSHFP,

    PUSH64, 2,0,0,0, 0,0,0,0,
    PUSHFO,

    PUSH64, 2,0,0,0, 0,0,0,0,
    COPY,
    
    HALT,
};

int main(void) {
    run(code);
}
