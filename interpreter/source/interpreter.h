#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h> // size_t

#ifndef STARTING_STACK_SIZE
    #define STARTING_STACK_SIZE 1024*1024 // 1MB
#endif // STARTING_STACK_SIZE

// Represents an opcode.
enum Opcode {
    NOOP,
    HALT,

    PUSH8,
    PUSH16,
    PUSH32,
    PUSH64,
    
    
    
    PRINT8,
    PRINT16,
};

// Runs the given code. Multi-byte values are little-endian.
void run(char *code);

#endif // INTERPRETER_H
