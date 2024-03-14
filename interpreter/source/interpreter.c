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
    uint64_t immediate; // Immediate value you can take the address of.
    bool keepRunning;
};

// Returns 2^`power`. `power` is a number from 0-3 representing which size of operand an instruction
// takes.
static uint8_t getWidth(uint8_t power) {
    assert(power <= 3 && "Max power is 3.");
    return 1 << power;
}

// Gets the pointer to the source for a copy instruction.
static uint8_t *getSource(struct Interpreter *interpreter, uint8_t width, uint8_t addressingMode) {
    uint8_t *source = NULL;
    ptrdiff_t offset = 0;
    switch ((addressingMode & 0b11110000) >> 4) {
        case IMMEDIATE:
            source = interpreter->ip;
            interpreter->ip += width;
            break;
        case POINTER:
            source = *(void**)interpreter->ip;
            interpreter->ip += sizeof(void**);
            break;
        case STACK_TOP:
            source = interpreter->sp - width - 1;
            break;
        case STACK_OFFSET:
            offset = *interpreter->ip;
            interpreter->ip += sizeof(ptrdiff_t);
            source = interpreter->sp - offset - 1;
            break;
        case LOCAL_OFFSET:
            offset = *interpreter->ip;
            interpreter->ip += sizeof(ptrdiff_t);
            source = interpreter->fp + offset;
            break;
        case ARGUMENT_OFFSET:
            offset = *interpreter->ip;
            interpreter->ip += sizeof(ptrdiff_t);
            source = interpreter->fp - offset - 1;
            break;
        case STACK_TOP_POINTER:
            source = (uint8_t*)&interpreter->sp;
            break;
        case STACK_OFFSET_POINTER:
            offset = *interpreter->ip;
            interpreter->ip += sizeof(ptrdiff_t);
            interpreter->immediate = (uint64_t)(interpreter->sp - offset - 1);
            source = (uint8_t*)&interpreter->immediate;
            break;
        case LOCAL_OFFSET_POINTER:
            offset = *interpreter->ip;
            interpreter->ip += sizeof(ptrdiff_t);
            interpreter->immediate = (uint64_t)(interpreter->fp + offset);
            source = (uint8_t*)&interpreter->immediate;
            break;
        case ARGUMENT_OFFSET_POINTER:
            offset = *interpreter->ip;
            interpreter->ip += sizeof(ptrdiff_t);
            interpreter->immediate = (uint64_t)(interpreter->fp - offset - 1);
            source = (uint8_t*)&interpreter->immediate;
            break;
        default:
            assert(0 && "Invalid opcode.");
            interpreter->keepRunning = false;
            break;
    }
    return source;
}

// Gets the pointer to the desitination of a copy instruction.
static uint8_t *getDestination(struct Interpreter *interpreter, uint8_t width, uint8_t addressingMode) {
    uint8_t *destination = NULL;
    switch (addressingMode & 0b00001111) {
        case STACK_TOP:
            destination = interpreter->sp;
            interpreter->sp += width;
            break;
        default:
            assert(0 && "Invalid opcode.");
            interpreter->keepRunning = false;
            break;
    }
    return destination;
}

// Returns a 64-bit value with the first `width*8` bits set.
static uint64_t getMask(uint8_t width) {
    // TODO: Make this neater.
    return (width < 3) ? ~(~(uint64_t)0u << width*8) : ~(uint64_t)0u;
}

// Pushes a value of the given width to the stack.
static void push(struct Interpreter *interpreter, uint8_t width, uint64_t value) {
    uint64_t mask = getMask(width);
    *(uint64_t*)interpreter->sp = value & mask;
    interpreter->sp += width;
}

// Pops a value of the given width from the stack.
static uint64_t pop(struct Interpreter *interpreter, uint8_t width) {
    interpreter->sp -= width;
    uint64_t mask = getMask(width);
    return (*(uint64_t*)interpreter->sp) & mask;
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
        uint8_t opcode = *interpreter.ip;
        ++interpreter.ip;

        uint8_t width = 0;
        switch (opcode) {
            case NOOP:
                // Does nothing.
                continue;

            case HALT:
                // Exits the program.
                interpreter.keepRunning = false;
                break;

            case COPY8...COPY64:
                // Copies a source to a destination.
                width = getWidth(opcode - COPY8);
                uint8_t addressingMode = *interpreter.ip;
                ++interpreter.ip;
                uint8_t *source = getSource(&interpreter, width, addressingMode);
                uint8_t *destination = getDestination(&interpreter, width, addressingMode);
                memcpy(destination, source, width);
                break;

            case ADDU8...ADDU64:
                // Adds 2 unsigned integers.
                width = getWidth(opcode - ADDU8);
                uint64_t a = pop(&interpreter, width);
                uint64_t b = pop(&interpreter, width);
                uint64_t maxValue = getMask(width);
                push(&interpreter, width, (a + b)%maxValue);
                break;

            case PRINT8...PRINT64:
                // Prints a value from the stack.
                width = getWidth(opcode - PRINT8);
                uint64_t value = pop(&interpreter, width);
                printf("%ld\n", value);
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
