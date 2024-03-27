#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"

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
    // TODO: Add support for passing a pointer to an existing stack.
    // TODO: Rename local variables.
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
        uint64_t value = 0;
        uint64_t source = 0;
        uint64_t destination = 0;
        uint64_t size = 0;
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

            case PUSHL:
                push(&interpreter, 8, (uint64_t)(interpreter.fp + *(ptrdiff_t*)interpreter.ip));
                interpreter.ip += 8;
                break;

            case PUSHA:
                // TODO: Implement this instruction.
                break;

            case PUSHT:
                // `- 8` to account for the offset popped from the stack.
                push(&interpreter, 8, (uint64_t)interpreter.sp - pop(&interpreter, 8));
                break;

            case PUSHD:
                // TODO: Implement this instruction.
                break;

            case POP8...POP64:
                width = getWidth(opcode - POP8);
                interpreter.sp -= width;
                break;

            case POP:
                interpreter.sp -= pop(&interpreter, 8);
                break;

            case ZERO8...ZERO64:
                width = getWidth(opcode - ZERO8);
                push(&interpreter, width, 0);
                break;

            case ZERO:
                width = pop(&interpreter, 8);
                memset(interpreter.sp, 0, (int)width);
                interpreter.sp += width;
                break;

            case BUMP8...BUMP64:
                width = getWidth(opcode - BUMP8);
                interpreter.sp += width;
                break;

            case BUMP:
                width = pop(&interpreter, 8);
                interpreter.sp += width;
                break;

            case DUP8...DUP64:
                width = getWidth(opcode - DUP8);
                memcpy(interpreter.sp, interpreter.sp - width, width);
                interpreter.sp += width;
                break;

            case LOAD8...LOAD64:
                width = getWidth(opcode - LOAD8);
                value = pop(&interpreter, 8);
                memcpy(interpreter.sp, (void*)value, width);
                interpreter.sp += width;
                break;

            case LOAD:
                width = pop(&interpreter, 8);
                value = pop(&interpreter, 8);
                memcpy(interpreter.sp, (void*)value, width);
                interpreter.sp += width;
                break;

            case STORE8...STORE64:
                width = getWidth(opcode - STORE8);
                destination = pop(&interpreter, 8);
                memcpy((void*)destination, interpreter.sp - width, width);
                interpreter.sp -= width;
                break;

            case STORE:
                size = pop(&interpreter, 8);
                destination = pop(&interpreter, 8);
                memcpy((void*)destination, interpreter.sp - size, size);
                interpreter.sp -= size;
                break;

            case COPY8...COPY64:
                width = getWidth(opcode - COPY8);
                destination = pop(&interpreter, 8);
                source = pop(&interpreter, 8);
                memcpy((void*)destination, (void*)source, width);
                break;

            case COPY:
                size = pop(&interpreter, 8);
                destination = pop(&interpreter, 8);
                source = pop(&interpreter, 8);
                memcpy((void*)destination, (void*)source, size);
                break;

            case JMP:
                destination = pop(&interpreter, 8);
                interpreter.ip = (uint8_t*)(code + destination);
                break;

            case JMPT:
                value = pop(&interpreter, 1);
                destination = pop(&interpreter, 8);
                if (value) {
                    interpreter.ip = (uint8_t*)destination;
                }
                break;

            case JMPF:
                value = pop(&interpreter, 1);
                destination = pop(&interpreter, 8);
                if (!value) {
                    interpreter.ip = (uint8_t*)destination;
                }
                break;

            case ADDI8...ADDI64:
                width = getWidth(opcode - ADDI8);
                push(&interpreter, width, pop(&interpreter, width) + pop(&interpreter, width));
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
