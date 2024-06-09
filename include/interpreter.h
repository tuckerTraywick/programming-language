#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "symboltable.h"

#define STACK_SIZE 1024*1024 // 1 MB

// Represents the state of the virtual machine.
struct Interpreter {
    uint8_t *code; // The entrypoint of the code to be executed.
    uint8_t *stack; // The stack. Allocated and freed in `runCode()`.
    uint8_t *ip; // Instruction pointer.
    uint8_t *fp; // Frame pointer.
    uint8_t *sp; // Stack pointer. The NEXT available byte of the stack.
    bool keepRunning; // Set to false by `HALT` instruction.
};

// Represents the operation being performed by an instruction.
enum Opcode {
    NOOP,

    HALT,

    PUSH,
    PUSHB,
    PUSHL,
    PUSHA,
    PUSHD,
    PUSHLB,
    PUSHAB,
    PUSHDB,

    POP,
    POPB,
    POPN,

    DUP,
    DUPB,
    DUPN,
    
    ZERO,
    ZEROB,
    ZERON,

    BUMP,
    BUMPB,
    BUMPN,

    LOAD,
    LOADB,
    LOADN,

    STORE,
    STOREB,
    STOREN,

    COPY8,
    COPYB,
    COPYN,

    JMP,
    JMPT,
    JMPF,

    CALL,

    RET,

    ADDI,
    ADDIB,
    ADDF,

    SUBI,
    SUBIB,
    SUBF,

    MULI,
    MULU,
    MULIB,
    MULUB,
    MULF,

    DIVI,
    DIVU,
    DIVIB,
    DIVUB,
    DIVF,

    MODI,
    MODU,
    MODIB,
    MODUB,

    NEGI,
    NEGB,
    NEGF,

    ABSI,
    ABSB,
    ABSF,

    FLOORF,

    CEILF,

    ROUNDF,

    SQRTF,

    BNOT,
    BNOTB,

    BAND,
    BANDB,

    BOR,
    BORB,

    BXOR,
    BXORB,

    BNAND,
    BNANDB,

    BNOR,
    BNORB,

    BXNOR,
    BXNORB,


    LNOT,
    LNOTB,

    LAND,
    LANDB,

    LOR,
    LORB,

    LXOR,
    LXORB,

    LNAND,
    LNANDB,

    LNOR,
    LNORB,

    LXNOR,
    LXNORB,

    GTI,
    GTU,
    GTF,
    GTIB,
    GTUB,

    GTEI,
    GTEU,
    GTEF,
    GTEIB,
    GTEUB,

    LTI,
    LTU,
    LTF,
    LTIB,
    LTUB,

    LTEI,
    LTEU,
    LTEF,
    LTEIB,
    LTEUB,

    EQ,
    EQB,

    NEQ,
    NEB,

    PRINTI,
    PRINTU,
    PRINTF,
    PRINTIB,
    PRINTUB,
    PRINTC,
};

// Runs the code starting at the entrypoint in an object.
void run(struct Object *object);

// Runs the code.
void runCode(uint8_t *code, uint8_t *data);

#endif // INTERPRETER_H
