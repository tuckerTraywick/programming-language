#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSHB, 1,
    PUSHB, 2,
    POPN,
    HALT,
};

int main(void) {
    runCode(code, NULL);

    return 0;
}
