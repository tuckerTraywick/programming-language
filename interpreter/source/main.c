#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

static uint8_t code[] = {
    PUSH32, 0,0,0,0,
    PUSH32, 0,0,0,0,
    ADDF32,
    PRINTF32,
    HALT,
};

static uint8_t data[] = {
    2,1,3,4, 7,8,5,6,
};

int main(void) {
    *(float*)(code + 1) = 3.4;
    *(float*)(code + 6) = 4.4;
    run(code, data);
}
