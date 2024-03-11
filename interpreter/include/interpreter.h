#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>

#define STACK_SIZE 1024*1024 // 1MB

// Combines `source` and `dest` into one byte representing the addressing mode for a copy
// instruction. The source is in the first 4 bits, the destination is in the last 4 bits.
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

// Represents the first byte of an opcode.
enum Opcode {
    NOOP,

    HALT,

    COPY8,
    COPY16,
    COPY32,
    COPY64,

    ADDU8,
    ADDU16,

    PRINT8,
    PRINT16,
    PRINT32,
    PRINT64,
};

// Multi-byte values are little-endian.
void run(uint8_t *code);

#endif // INTERPRETER_H
