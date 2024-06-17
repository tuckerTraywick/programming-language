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
#define abs(x) (((x) < 0) ? -(x) : (x))

// Represents the state of the virtual machine.
struct Interpreter {
    uint8_t *code; // The entrypoint of the code to be executed.
    uint8_t *mutableData; // The data section of the program.
    uint8_t *stack; // The stack. Allocated and freed in `runCode()`.
    uint8_t *ip; // Instruction pointer.
    uint8_t *fp; // Frame pointer.
    uint8_t *sp; // Stack pointer. The NEXT available byte of the stack.
    bool keepRunning; // Set to false by `HALT` instruction.
};

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

// Pushes a double to the stack.
static void pushDouble(struct Interpreter *Interpreter, double value) {
    push(Interpreter, *(uint64_t*)(&value));
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

// Pops a double from the stack.
static double popDouble(struct Interpreter *Interpreter) {
    uint64_t value = pop(Interpreter);
    return *(double*)(&value);
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
    uint8_t *code = object->data + object->header.entryPoint;
    uint8_t *immutableData = getSegmentPointer(object, object->header.immutableData);
    uint8_t *mutableData = getSegmentPointer(object, object->header.mutableData);
    runCode(code, immutableData, mutableData);
}

void runCode(uint8_t *code, uint8_t *immutableData, uint8_t *mutableData) {
    uint8_t *stack = malloc(STACK_SIZE);
    // TODO: Handle failed `malloc()`.
    assert(stack && "`malloc()` failed.");
    struct Interpreter interpreter = {
        .code = code,
        .mutableData = mutableData,
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

            case PUSHL:
                push(&interpreter, *(uint64_t*)(interpreter.fp + pop(&interpreter)));
                break;

            case PUSHA:
                push(&interpreter, *(uint64_t*)(interpreter.fp - pop(&interpreter) - 25));
                break;

            case PUSHD:
                push(&interpreter, *(uint64_t*)(interpreter.mutableData + pop(&interpreter)));
                break;

            case PUSHB:
                *interpreter.sp = readByte(&interpreter);
                ++interpreter.sp;
                break;

            case PUSHLB:
                pushByte(&interpreter, *(interpreter.fp + pop(&interpreter)));
                break;

            case PUSHAB:
                // `- 25` to account for the 24 bytes of data pushed by `CALL` - 1 to get to the
                // last byte of the last argument.
                pushByte(&interpreter, *(interpreter.fp - pop(&interpreter) - 25));
                break;

            case PUSHDB:
                pushByte(&interpreter, *(interpreter.mutableData + pop(&interpreter)));
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
                interpreter.ip = interpreter.code + pop(&interpreter);
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

            case CALL: {
                uint64_t offset = pop(&interpreter);
                push(&interpreter, (uint64_t)interpreter.sp);
                push(&interpreter, (uint64_t)interpreter.fp);
                push(&interpreter, (uint64_t)interpreter.ip);
                interpreter.fp = interpreter.sp;
                interpreter.ip = interpreter.code + offset;
                break;
            }

            case RET:
                interpreter.sp = interpreter.fp;
                interpreter.ip = (uint8_t*)pop(&interpreter);
                interpreter.fp = (uint8_t*)pop(&interpreter);
                interpreter.sp = (uint8_t*)pop(&interpreter);
                break;

            case ADDI:
                push(&interpreter, pop(&interpreter) + pop(&interpreter));
                break;

            case ADDF:
                pushDouble(&interpreter, popDouble(&interpreter) + popDouble(&interpreter));
                break;

            case ADDIB:
                pushByte(&interpreter, popByte(&interpreter) + popByte(&interpreter));
                break;

            case SUBI:
                push(&interpreter, pop(&interpreter) - pop(&interpreter));
                break;

            case SUBF:
                pushDouble(&interpreter, popDouble(&interpreter) - popDouble(&interpreter));
                break;

            case SUBIB:
                pushByte(&interpreter, popByte(&interpreter) - popByte(&interpreter));
                break;

            case MULI:
                push(&interpreter, (int64_t)pop(&interpreter) * (int64_t)pop(&interpreter));
                break;

            case MULU:
                push(&interpreter, pop(&interpreter) * pop(&interpreter));
                break;

            case MULF:
                pushDouble(&interpreter, popDouble(&interpreter) * popDouble(&interpreter));
                break;

            case MULIB:
                pushByte(&interpreter, popByte(&interpreter) * popByte(&interpreter));
                break;

            case MULUB:
                pushByte(&interpreter, (int8_t)popByte(&interpreter) * (int8_t)popByte(&interpreter));
                break;

            case DIVI:
                push(&interpreter, (int64_t)pop(&interpreter) / (int64_t)pop(&interpreter));
                break;

            case DIVU:
                push(&interpreter, pop(&interpreter) / pop(&interpreter));
                break;

            case DIVF:
                pushDouble(&interpreter, popDouble(&interpreter) / popDouble(&interpreter));
                break;

            case DIVIB:
                pushByte(&interpreter, popByte(&interpreter) / popByte(&interpreter));
                break;

            case DIVUB:
                pushByte(&interpreter, (int8_t)popByte(&interpreter) / (int8_t)popByte(&interpreter));
                break;

            case MODI:
                push(&interpreter, (int64_t)pop(&interpreter) % (int64_t)pop(&interpreter));
                break;

            case MODU:
                push(&interpreter, pop(&interpreter) % pop(&interpreter));
                break;

            case MODIB:
                pushByte(&interpreter, popByte(&interpreter) % popByte(&interpreter));
                break;

            case MODUB:
                pushByte(&interpreter, (int8_t)popByte(&interpreter) % (int8_t)popByte(&interpreter));
                break;

            case NEGI:
                push(&interpreter, -(int64_t)pop(&interpreter));
                break;

            case NEGF:
                pushDouble(&interpreter, -popDouble(&interpreter));
                break;

            case NEGB:
                pushByte(&interpreter, -(int8_t)popByte(&interpreter));
                break;

            case ABSI:
                push(&interpreter, abs((int64_t)pop(&interpreter)));
                break;

            case ABSF:
                pushDouble(&interpreter, abs(popDouble(&interpreter)));
                break;

            case ABSB:
                pushByte(&interpreter, abs((int8_t)popByte(&interpreter)));
                break;

            case FLOORF:
                pushDouble(&interpreter, floor(popDouble(&interpreter)));
                break;

            case CEILF:
                pushDouble(&interpreter, ceil(popDouble(&interpreter)));
                break;

            case ROUNDF:
                pushDouble(&interpreter, round(popDouble(&interpreter)));
                break;

            case SQRTF:
                pushDouble(&interpreter, sqrt(popDouble(&interpreter)));
                break;

            case PRINTI:
                printf("%li\n", pop(&interpreter));
                break;

            case PRINTU:
                printf("%lu\n", pop(&interpreter));
                break;

            case PRINTF:
                printf("%f\n", popDouble(&interpreter));
                break;

            case PRINTIB:
                printf("%i\n", popByte(&interpreter));
                break;

            case PRINTUB:
                printf("%u\n", popByte(&interpreter));
                break;

            case PRINTC:
                printf("%c\n", (char)popByte(&interpreter));
                break;

            default:
                assert(0 && "Invalid opcode.");
                interpreter.keepRunning = false;
                break;
        }
    }

    printf("\nsp: %lu\n", interpreter.sp - interpreter.stack);
    printf("stack:\n");
    for (uint8_t *byte = interpreter.stack; byte < interpreter.sp; ++byte) {
        printf("%d ", *byte);
    }
    printf("| %d\n", *interpreter.sp);

    free(stack);
}
