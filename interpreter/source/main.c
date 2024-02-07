#include <stdio.h> // printf()
#include "interpreter.h"

static char code[] = {
    NOOP, NOOP, HALT,
};

int main(void) {
    run(code);
    printf("Done.\n");
}
