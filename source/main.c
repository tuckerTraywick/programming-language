#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSHB, 2,
    PUSHB, 1,
    ADDIB,
    HALT,
};

int main(void) {
    runCode(code, NULL);

    return 0;
}
