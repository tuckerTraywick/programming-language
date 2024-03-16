#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>

#define STACK_SIZE 5*1024*1024 // 5MB

// Represents the operation being performed by an isntruction.
enum Opcode {
    NOOP,

    HALT,

    PUSH8,
    PUSH16,
    PUSH32,
    PUSH64,

    ADDU8,
    ADDU16,
    ADDU32,
    ADDU64,

    ADDI8,
    ADDI16,
    ADDI32,
    ADDI64,

    PRINT8,
    PRINT16,
    PRINT32,
    PRINT64,
};

// Multi-byte values are little-endian.
void run(uint8_t *code);

#endif // INTERPRETER_H
