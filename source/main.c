#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH, 20,0,0,0, 0,0,0,0,
    BUMPN,
    HALT,
};

int main(void) {
    runCode(code, NULL);

    return 0;
}
