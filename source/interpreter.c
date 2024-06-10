#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "interpreter.h"
#include "object.h"

// Redefining `abs` so it works for different types.
#define abs(x) (((x) < 0) ? (-(x)) : (x))

// Pushes an 8 byte value to the stack.
static void push(struct Interpreter *interpreter, uint64_t value) {
    *(uint64_t*)interpreter->sp = value;
    interpreter->sp += 8;
}

// Pushes a single byte to the stack.
static void pushByte(struct Interpreter *interpreter, uint8_t value) {
    *interpreter->sp = (uint64_t)value;
    ++interpreter->sp;
}

// Pops an 8 byte value from the stack.
static uint64_t pop(struct Interpreter *interpreter) {
    interpreter->sp -= 8;
    return *(uint64_t*)interpreter->sp;
}

// Pops a single byte from the stack.
static uint8_t popByte(struct Interpreter *interpreter) {
    --interpreter->sp;
    return *(uint8_t*)interpreter->sp;
}

// Reads an 8 byte operand from the code.
static uint64_t read(struct Interpreter *interpreter) {
    uint64_t value = *(uint64_t*)interpreter->ip;
    interpreter->ip += 8;
    return value;
}

// Reads a single byte from the code.
static uint8_t readByte(struct Interpreter *interpreter) {
    uint8_t value = *interpreter->ip;
    ++interpreter->ip;
    return value;
}

void run(struct Object *object) {
    // TODO: Handle passing a non-executable object.
    assert(object->header.executable && "Must pass an executable object.");
    uint8_t *code = object->bytes + object->header.entryPoint;
    uint8_t *data = object->bytes + object->header.data;
    runCode(code, data);
}

void runCode(uint8_t *code, uint8_t *data) {
    uint8_t *stack = malloc(STACK_SIZE);
    // TODO: Handle failed `malloc()`.
    assert(stack && "`malloc()` failed.");
    struct Interpreter interpreter = {
        .stack = stack,
        .ip = code,
        .fp = stack,
        .sp = stack,
        .keepRunning = true,
    };

    while (interpreter.keepRunning) {
        uint8_t opcode = *interpreter.ip;
        ++interpreter.ip;

        switch (opcode) {
            case NOOP:
                continue;

            case HALT:
                interpreter.keepRunning = false;
                break;

            case PUSH:
                push(&interpreter, read(&interpreter));
                break;

            case PUSHB:
                *interpreter.sp = readByte(&interpreter);
                ++interpreter.sp;
                break;

            case POP:
                pop(&interpreter);
                break;

            case POPB:
                popByte(&interpreter);
                break;

            case POPN: {
                uint64_t n = pop(&interpreter);
                interpreter.sp -= n;
                break;
            }

            case DUP:
                push(&interpreter, *(uint64_t*)(interpreter.sp - 8));
                break;

            case DUPB:
                pushByte(&interpreter, *(interpreter.sp - 1));
                break;

            case DUPN: {
                uint64_t n = pop(&interpreter);
                memcpy(interpreter.sp, interpreter.sp - n, n);
                interpreter.sp += n;
                break;
            }
            
            case ZERO:
                push(&interpreter, 0);
                break;

            case ZEROB:
                pushByte(&interpreter, 0);
                break;

            case ZERON: {
                uint64_t n = pop(&interpreter);
                memset(interpreter.sp, 0, n);
                interpreter.sp += n;
                break;
            }

            case BUMP:
                interpreter.sp += 8;
                break;

            case BUMPB:
                ++interpreter.sp;
                break;

            case BUMPN: {
                uint64_t n = pop(&interpreter);
                interpreter.sp += n;
                break;
            }

            case LOAD:
                push(&interpreter, *(uint64_t*)pop(&interpreter));
                break;

            case LOADB:
                pushByte(&interpreter, *(uint8_t*)pop(&interpreter));
                break;

            case LOADN: {
                uint64_t n = pop(&interpreter);
                uint8_t *source = (uint8_t*)pop(&interpreter);
                memcpy(interpreter.sp, source, n);
                interpreter.sp += n;
                break;
            }

            case STORE:
                *(uint64_t*)pop(&interpreter) = pop(&interpreter);
                break;

            case STOREB:
                *(uint8_t*)pop(&interpreter) = popByte(&interpreter);
                break;

            case STOREN: {
                uint64_t n = pop(&interpreter);
                uint8_t *destination = (uint8_t*)pop(&interpreter);
                memcpy(destination, interpreter.sp - n, n);
                break;
            }

            case COPY:
                *(uint64_t*)pop(&interpreter) = *(uint64_t*)pop(&interpreter);
                break;

            case COPYB:
                *(uint8_t*)pop(&interpreter) = *(uint8_t*)pop(&interpreter);
                break;

            case COPYN: {
                uint64_t n = pop(&interpreter);
                uint8_t *destination = (uint8_t*)pop(&interpreter);
                uint8_t *source = (uint8_t*)pop(&interpreter);
                memcpy(destination, source, n);
                break;
            }

            case JMP:
                interpreter.ip = (uint8_t*)pop(&interpreter);
                break;

            case JMPT: {
                uint64_t offset = pop(&interpreter);
                uint8_t result = popByte(&interpreter);
                if (result) {
                    interpreter.ip = interpreter.code + offset;
                }
                break;
            }

            case JMPF: {
                uint64_t offset = pop(&interpreter);
                uint8_t result = popByte(&interpreter);
                if (!result) {
                    interpreter.ip = interpreter.code + offset;
                }
                break;
            }

            case ADDIB:
                pushByte(&interpreter, popByte(&interpreter) + popByte(&interpreter));
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
