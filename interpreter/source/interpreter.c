#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"

void run(char *code) {
    char *stack = malloc(STARTING_STACK_SIZE);
    char *ip = code;
    char *fp = stack;
    char *sp = stack;

    bool keepRunning = true;
    while (keepRunning) {
        char opcode = *ip;
        ++ip;

        switch (opcode) {
            case NOOP:
                // Does nothing.
                continue;
            case HALT:
                // Exits the program.
                keepRunning = false;
                break;
            case PRINT8:
                // Prints an 8-bit value from the stack.
                printf("%d\n", *sp);
                --sp;
                break;
            default:
                assert(0 && "Invalid opcode.");
                keepRunning = false;
                break;
        }
    }

    free(stack);
}
