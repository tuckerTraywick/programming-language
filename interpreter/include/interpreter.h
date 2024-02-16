#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>

#ifndef STARTING_STACK_SIZE
    #define STARTING_STACK_SIZE 1024*1024 // 1MB
#endif // STARTING_STACK_SIZE

// Represnets the types of arguments instructions can have.
enum AddressingMode {
    IMMEDIATE,

    ARGUMENT,
    ARGUMENT_REF,
    ARGUMENT_DEREF,

    LOCAL,
    LOCAL_REF,
    LOCAL_DEREF,

    STACK,
    STACK_REF,
    STACK_DEREF,
};

// Represents an opcode.
enum Opcode {
    NOOP,

    HALT,

    COPY8,

    PRINT8,
};

// Runs the given code. Multi-byte values are little-endian.
void run(char *code);

#endif // INTERPRETER_H
