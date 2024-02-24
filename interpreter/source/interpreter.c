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
    bool keepRunning;
};

// Gets the pointer to the source for a copy instruction.
static uint8_t *getSource(struct Interpreter *interpreter, uint8_t addressingMode) {
    uint8_t *source = NULL;
    switch ((addressingMode&0b11110000) >> 4) {
        case IMMEDIATE:
            source = interpreter->ip;
            ++interpreter->ip;
            break;
        case STACK_TOP:
            source = interpreter->sp - 1;
            break;
        default:
            assert(0 && "Invalid opcode.");
            interpreter->keepRunning = false;
            break;
    }
    return source;
}

// Gets the pointer to the desitination of a copy instruction.
static uint8_t *getDestination(struct Interpreter *interpreter, uint8_t addressingMode) {
    uint8_t *destination = NULL;
    switch (addressingMode&0b00001111) {
        case STACK_TOP:
            destination = interpreter->sp;
            ++interpreter->sp;
            break;
        default:
            assert(0 && "Invalid opcode.");
            interpreter->keepRunning = false;
            break;
    }
    return destination;
}

void run(uint8_t *code) {
    uint8_t *stack = malloc(STARTING_STACK_SIZE);
    struct Interpreter interpreter = {
        .stack = stack,
        .ip = code,
        .fp = stack,
        .sp = stack,
        .keepRunning = true,
    };
    assert(stack && "`malloc()` failed.");

    while (interpreter.keepRunning) {
        uint8_t opcode = *interpreter.ip;
        ++interpreter.ip;

        switch (opcode) {
            case NOOP:
                // Does nothing.
                continue;

            case HALT:
                // Exits the program.
                interpreter.keepRunning = false;
                break;

            case COPY8:
                // Copies a source to a destination.
                uint8_t addressingMode = *interpreter.ip;
                ++interpreter.ip;
                uint8_t *source = getSource(&interpreter, addressingMode);
                uint8_t *destination = getDestination(&interpreter, addressingMode);
                *destination = *source;
                break;

            case PRINT8:
                // Prints an 8-bit value from the stack.
                --interpreter.sp;
                printf("%d\n", *interpreter.sp);
                break;
                
            default:
                assert(0 && "Invalid opcode.");
                interpreter.keepRunning = false;
                break;
        }
    }

    printf("\nstack:\n");
    for (uint8_t *byte = interpreter.stack; byte <= interpreter.sp; ++byte) {
        printf("%d ", *byte);
    }
    printf("\n");

    free(stack);
}
