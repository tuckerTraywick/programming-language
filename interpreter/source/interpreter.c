#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

// Represents the state of the virtual machine.
struct Interpreter {
    uint8_t *code;
    uint8_t *stack;
    uint8_t *ip; // Instruction pointer.
    uint8_t *fp; // Frame pointer.
    uint8_t *sp; // Stack pointer. The NEXT available byte of the stack.
    bool keepRunning;
};

// Gets the width of the operand of an instruction.
static uint8_t getWidth(uint8_t opcodeIndex) {
    static uint8_t widths[] = {1, 2, 4, 8};
    return widths[opcodeIndex];
}

// Pushes a value.
static void push(struct Interpreter *interpreter,  uint8_t width, uint64_t value) {
    memcpy(interpreter->sp, &value, width);
    interpreter->sp += width;
}

// Pops a value and pads it to 8 bytes.
static uint64_t pop(struct Interpreter *interpreter, uint8_t width) {
    uint64_t result = 0;
    memcpy(&result, interpreter->sp - width, width);
    interpreter->sp -= width;
    return result;
}

void run(uint8_t *code) {
    uint8_t *stack = malloc(STACK_SIZE);
    struct Interpreter interpreter = {
        .stack = stack,
        .ip = code,
        .fp = stack,
        .sp = stack,
        .keepRunning = true,
    };
    assert(stack && "`malloc()` failed.");

    while (interpreter.keepRunning) {
        uint8_t width = 0;
        uint8_t opcode = *interpreter.ip;
        ++interpreter.ip;

        switch (opcode) {
            case NOOP:
                continue;

            case HALT:
                interpreter.keepRunning = false;
                break;

            case PUSH8...PUSH64:
                width = getWidth(opcode - PUSH8);
                memcpy(interpreter.sp, interpreter.ip, width);
                interpreter.sp += width;
                interpreter.ip += width;
                break;

            case ADDU8...ADDU64:
                width = getWidth(opcode - ADDU8);
                push(&interpreter, width, pop(&interpreter, width) + pop(&interpreter, width));
                break;

            case ADDI8...ADDI64:
                width = getWidth(opcode - ADDI8);
                push(&interpreter, width, (int64_t)pop(&interpreter, width) + (int64_t)pop(&interpreter, width));
                break;

            case PRINT8...PRINT64:
                width = getWidth(opcode - PRINT8);
                printf("%zu\n", pop(&interpreter, width));
                break;
                
            default:
                assert(0 && "Invalid opcode.");
                interpreter.keepRunning = false;
                break;
        }
    }

    printf("\nstack:\n");
    for (uint8_t *byte = interpreter.stack; byte < interpreter.sp; ++byte) {
        printf("%d ", *byte);
    }
    printf("| %d\n", *interpreter.sp);

    free(stack);
}
