#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"

void run(uint8_t *code) {
    uint8_t *stack = malloc(STARTING_STACK_SIZE);
    uint8_t *ip = code;
    uint8_t *fp = stack;
    uint8_t *sp = stack;

    bool keepRunning = true;
    while (keepRunning) {
        uint8_t opcode = *ip;
        ++ip;

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
                uint8_t addressingMode = *ip;
                ++ip;

                uint8_t *source = NULL;
                switch ((addressingMode&0b11110000) >> 4) {
                    case IMMEDIATE:
                        source = ip;
                        ++ip;
                        break;
                    default:
                        assert(0 && "Invalid opcode.");
                        keepRunning = false;
                        break;
                }

                uint8_t *destination = NULL;
                switch (addressingMode&0b00001111) {
                    case STACK:
                        destination = sp;
                        ++sp;
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
                --sp;
                printf("%d\n", *sp);
                break;
                
            default:
                assert(0 && "Invalid opcode.");
                keepRunning = false;
                break;
        }
    }

    free(stack);
}
