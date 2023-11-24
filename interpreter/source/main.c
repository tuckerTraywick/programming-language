#include <stdio.h> // printf()
#include "interpreter.h"

static char code[] = {
    PUSH16I, 0, 0,
    PRINT16,
    HALT,
};

int main(void) {
    run(code);
    printf("Done.\n");
}
