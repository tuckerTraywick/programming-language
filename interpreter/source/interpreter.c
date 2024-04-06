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
    return 1 << opcodeIndex; // 2^opcodeIndex
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

void run(uint8_t *code, uint8_t *data) {
    // TODO: Add support for passing a pointer to an existing stack??
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
        uint64_t a = 0;
        uint64_t b = 0;
        uint8_t opcode = *interpreter.ip;
        // printf("ip=%d, opcode=%d\n", interpreter.ip - code, opcode);
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
                push(&interpreter, 8, (uint64_t)(interpreter.fp - *(ptrdiff_t*)interpreter.ip - 25));
                interpreter.ip += 8;
                break;

            case PUSHT:
                push(&interpreter, 8, (uint64_t)interpreter.sp - pop(&interpreter, 8));
                break;

            case PUSHD:
                push(&interpreter, 8, (uint64_t)data + pop(&interpreter, 8));                
                break;

            case POP8...POP64:
                width = getWidth(opcode - POP8);
                interpreter.sp -= width;
                break;

            case POP:
                interpreter.sp -= pop(&interpreter, 8);
                break;

            case DUP8...DUP64:
                width = getWidth(opcode - DUP8);
                memcpy(interpreter.sp, interpreter.sp - width, width);
                interpreter.sp += width;
                break;

            case DUP:
                size = pop(&interpreter, 8);
                memcpy(interpreter.sp, interpreter.sp - size, size);
                interpreter.sp += size;
                break;

            case ZERO8...ZERO64:
                width = getWidth(opcode - ZERO8);
                push(&interpreter, width, 0);
                break;

            case ZERO:
                size = pop(&interpreter, 8);
                memset(interpreter.sp, 0, (int)size);
                interpreter.sp += size;
                break;

            case BUMP8...BUMP64:
                width = getWidth(opcode - BUMP8);
                interpreter.sp += width;
                break;

            case BUMP:
                size = pop(&interpreter, 8);
                interpreter.sp += size;
                break;

            case LOAD8...LOAD64:
                width = getWidth(opcode - LOAD8);
                value = pop(&interpreter, 8);
                memcpy(interpreter.sp, (void*)value, width);
                interpreter.sp += width;
                break;

            case LOAD:
                size = pop(&interpreter, 8);
                value = pop(&interpreter, 8);
                memcpy(interpreter.sp, (void*)value, size);
                interpreter.sp += size;
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

            case CALL:
                destination = pop(&interpreter, 8);
                push(&interpreter, 8, (uint64_t)interpreter.sp);
                push(&interpreter, 8, (uint64_t)interpreter.fp);
                push(&interpreter, 8, (uint64_t)interpreter.ip);
                // printf("ip push=%d\n", interpreter.ip);
                interpreter.ip = code + destination;
                interpreter.fp = interpreter.sp;
                break;

            case RET:
                interpreter.ip = (uint8_t*)pop(&interpreter, 8);
                interpreter.fp = (uint8_t*)pop(&interpreter, 8);
                interpreter.sp = (uint8_t*)pop(&interpreter, 8);
                // printf("ip pop=%d\n", interpreter.ip);
                break;

            case ADDI8...ADDI64:
                width = getWidth(opcode - ADDI8);
                push(&interpreter, width, pop(&interpreter, width) + pop(&interpreter, width));
                break;

            case SUBI8...SUBI64:
                width = getWidth(opcode - SUBI8);
                b = pop(&interpreter, width);
                a = pop(&interpreter, width);
                push(&interpreter, width, a - b);
                break;

            case MULI8...MULI64:
                width = getWidth(opcode - MULI8);
                push(&interpreter, width, pop(&interpreter, width) * pop(&interpreter, width));
                break;

            case DIVI8...DIVI64:
                width = getWidth(opcode - DIVI8);
                b = pop(&interpreter, width);
                a = pop(&interpreter, width);
                push(&interpreter, width, a / b);
                break;

            case MODI8...MODI64:
                width = getWidth(opcode - MODI8);
                b = pop(&interpreter, width);
                a = pop(&interpreter, width);
                push(&interpreter, width, a % b);
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
