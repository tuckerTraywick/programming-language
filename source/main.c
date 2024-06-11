#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH, 0,0,0,0, 0,0,0,0,
    PUSH, 0,0,0,0, 0,0,0,0,
    ADDF,
    PRINTF,
    HALT,
};

int main(void) {
    *(double*)(code + 1) = 1.4;
    *(double*)(code + 10) = -1.7;
    runCode(code, NULL);

    return 0;
}
