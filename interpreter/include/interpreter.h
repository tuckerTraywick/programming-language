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
    PUSHL,
    PUSHA,
    PUSHT,
    PUSHD,

    POP8,
    POP16,
    POP32,
    POP64,
    POP,

    DUP8,
    DUP16,
    DUP32,
    DUP64,
    DUP,
    
    ZERO8,
    ZERO16,
    ZERO32,
    ZERO64,
    ZERO,

    BUMP8,
    BUMP16,
    BUMP32,
    BUMP64,
    BUMP,

    LOAD8,
    LOAD16,
    LOAD32,
    LOAD64,
    LOAD,

    STORE8,
    STORE16,
    STORE32,
    STORE64,
    STORE,

    COPY8,
    COPY16,
    COPY32,
    COPY64,
    COPY,

    JMP,
    JMPT,
    JMPF,

    CALL,

    RET,

    ADDI8,
    ADDI16,
    ADDI32,
    ADDI64,
    ADDF32,
    ADDF64,

    SUBI8,
    SUBI16,
    SUBI32,
    SUBI64,
    SUBF32,
    SUBF64,

    MULI8,
    MULI16,
    MULI32,
    MULI64,
    MULU8,
    MULU16,
    MULU32,
    MULU64,
    MULF32,
    MULF64,

    DIVI8,
    DIVI16,
    DIVI32,
    DIVI64,
    DIVU8,
    DIVU16,
    DIVU32,
    DIVU64,
    DIVF32,
    DIVF64,

    MODI8,
    MODI16,
    MODI32,
    MODI64,
    MODU8,
    MODU16,
    MODU32,
    MODU64,
    MODF32,
    MODF64,
    
    PRINTI8,
    PRINTI16,
    PRINTI32,
    PRINTI64,
    PRINTU8,
    PRINTU16,
    PRINTU32,
    PRINTU64,
    PRINTF32,
    PRINTF64,
};

// Multi-byte values are little endian. `data` points to the data section.
void run(uint8_t *code, uint8_t *data);

#endif // INTERPRETER_H
