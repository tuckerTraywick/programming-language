#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>

#define STARTING_STACK_SIZE 1024*1024 // 1MB
#define MODE(source, dest) (((uint8_t)(source) << 4) | (uint8_t)(dest))

// Represents the addressing mode of the source/destination for a copy.
enum AddressingMode {
    IMMEDIATE,
    POINTER,
    STACK_TOP,
    STACK_OFFSET,
    LOCAL_OFFSET,
    ARGUMENT_OFFSET,
    STACK_TOP_POINTER,
    STACK_OFFSET_POINTER,
    LOCAL_OFFSET_POINTER,
    ARGUMENT_OFFSET_POINTER,
};

// Represents an opcode.
enum Opcode {
    NOOP,

    HALT,

    COPY8,
    COPY16,

    ADDU8,
    ADDU16,

    PRINT8,
    PRINT16,
};

// Runs the given code. Multi-byte values are little-endian.
void run(uint8_t *code);

#endif // INTERPRETER_H
