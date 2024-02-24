#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"

// Represents the state of the virtual machine.
struct Interpreter {
    uint8_t *code;
    uint8_t *stack;
    uint8_t *ip;
    uint8_t *fp;
    uint8_t *sp;
};

void run(uint8_t *code) {
    uint8_t *stack = malloc(STARTING_STACK_SIZE);

    struct Interpreter interpreter = {
        .stack = malloc(STARTING_STACK_SIZE),
        .ip = code,
        .fp = stack,
        .sp = stack,
    };

    bool keepRunning = true;
    while (keepRunning) {
        uint8_t opcode = *interpreter.ip;
        ++interpreter.ip;

        switch (opcode) {
            case NOOP:
                // Does nothing.
                continue;

            case HALT:
                // Exits the program.
                keepRunning = false;
                break;

            case COPY8:
                // Copies a source to a destination.
                uint8_t addressingMode = *interpreter.ip;
                ++interpreter.ip;

                uint8_t *source = NULL;
                switch ((addressingMode&0b11110000) >> 4) {
                    case IMMEDIATE:
                        source = interpreter.ip;
                        ++interpreter.ip;
                        break;
                    default:
                        assert(0 && "Invalid opcode.");
                        keepRunning = false;
                        break;
                }

                uint8_t *destination = NULL;
                switch (addressingMode&0b00001111) {
                    case STACK_TOP:
                        destination = interpreter.sp;
                        ++interpreter.sp;
                        break;
                    default:
                        assert(0 && "Invalid opcode.");
                        keepRunning = false;
                        break;
                }

                *destination = *source;
                break;

            case PRINT8:
                // Prints an 8-bit value from the stack.
                --interpreter.sp;
                printf("%d\n", *interpreter.sp);
                break;
                
            default:
                assert(0 && "Invalid opcode.");
                keepRunning = false;
                break;
        }
    }

    free(stack);
}
