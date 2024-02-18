#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>

#ifndef STARTING_STACK_SIZE
    #define STARTING_STACK_SIZE 1024*1024 // 1MB
#endif // STARTING_STACK_SIZE

#define MODE(source, dest) (((uint8_t)(source) << 4) | (uint8_t)(dest))

// Represents the addressing mode of the source/destination for a copy.
enum AddressingMode {
    IMMEDIATE,
    POINTER,
    STACK,
    LOCAL,
    ARGUMENT,
    STACK_POINTER,
    LOCAL_POINTER,
    ARGUMENT_POINTER,
};

// Represents an opcode.
enum Opcode {
    NOOP,

    HALT,

    COPY8,

    PRINT8,
};

// Runs the given code. Multi-byte values are little-endian.
void run(uint8_t *code);

#endif // INTERPRETER_H
