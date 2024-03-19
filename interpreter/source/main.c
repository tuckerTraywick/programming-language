#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH16, 1, 0,
    PUSH16, 0, 0,

    PUSHFP,
    LOAD16,

    PUSH64, 2,0,0,0, 0,0,0,0,
    PUSHFO,
    STORE16,
    
    HALT,
};

int main(void) {
    run(code);
}
