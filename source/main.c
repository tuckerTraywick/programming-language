#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSHB, 2,
    PUSH, 14,0,0,0, 0,0,0,0,
    CALL,
    POPB,
    HALT,

    PUSH, 0,0,0,0, 0,0,0,0,
    PUSHAB,
    PRINTUB,
    RET,
};

int main(void) {
    runCode(code, NULL);

    return 0;
}
