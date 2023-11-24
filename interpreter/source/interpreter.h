#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h> // size_t

#ifndef STARTING_STACK_SIZE
    #define STARTING_STACK_SIZE 10*1024*1024 // 10MB
#endif // STARTING_STACK_SIZE

// Represents an opcode.
enum Opcode {
    HALT,
    PUSH8I,
    PUSH8,
    PUSH16I,
    PUSH16,
    PUSH32I,
    PUSH32,
    PUSH64I,
    PUSH64,
    LOAD8L,
    LOAD8,
    LOADA,
    LOAD,
    STORE8L,
    STORE8,
    STORE,

    PRINT8,
    PRINT16,
};

void run(char *code);

#endif // INTERPRETER_H
