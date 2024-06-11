#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH, 14,0,0,0, 0,0,0,0,
    CALL,
    PUSHB, 2,
    PRINTUB,
    HALT,

    PUSHB, 1,
    PRINTUB,
    RET,
};

int main(void) {
    runCode(code, NULL);

    return 0;
}
