#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>
#include <stdbool.h>

#define STACK_SIZE 5*1024*1024 // 5MB

// Represents the state of the virtual machine.
struct Interpreter {
    uint8_t *code;
    uint8_t *stack;
    uint8_t *ip; // Instruction pointer.
    uint8_t *fp; // Frame pointer.
    uint8_t *sp; // Stack pointer. The NEXT available byte of the stack.
    bool keepRunning;
};

// Represents the operation being performed by an isntruction.
enum Opcode {
    NOOP,

    HALT,

    PUSH8,
    PUSH16,
    PUSH32,
    PUSH64,
    PUSHSP,
    PUSHSO,
    PUSHFP,
    PUSHFO,

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
