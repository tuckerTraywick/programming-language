#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "symboltable.h"

#define STACK_SIZE 1024*1024 // 1 MB

// Represents the operation being performed by an instruction.
enum Opcode {
    NOOP,

    HALT,

    PUSH,
    PUSHL,
    PUSHA,
    PUSHD,
    PUSHB,
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

    COPY,
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
    NEGF,
    NEGB,

    ABSI,
    ABSF,
    ABSB,

    FLOORF,

    CEILF,

    ROUNDF,

    SQRTF,

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

    BNOT,
    BNOTB,

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

    LNOT,
    LNOTB,

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
void runCode(uint8_t *code, uint8_t *immutableData, uint8_t *mutableData);

#endif // INTERPRETER_H
