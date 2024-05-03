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

// Represents the operation being performed by an instruction.
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

    FIRST16,
    FIRST32,
    FIRST64,

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

    NEGI8,
    NEGI16,
    NEGI32,
    NEGI64,
    NEGF32,
    NEGF64,

    ABSI8,
    ABSI16,
    ABSI32,
    ABSI64,
    ABSF32,
    ABSF64,

    FLOOR32,
    FLOOR64,

    CEIL32,
    CEIL64,

    ROUND32,
    ROUND64,

    SQRT32,
    SQRT64,

    BNOT8,
    BNOT16,
    BNOT32,
    BNOT64,

    BAND8,
    BAND16,
    BAND32,
    BAND64,

    BOR8,
    BOR16,
    BOR32,
    BOR64,

    BXOR8,
    BXOR16,
    BXOR32,
    BXOR64,

    BNAND8,
    BNAND16,
    BNAND32,
    BNAND64,

    BNOR8,
    BNOR16,
    BNOR32,
    BNOR64,

    BXNOR8,
    BXNOR16,
    BXNOR32,
    BXNOR64,

    LNOT,
    LAND,
    LOR,
    LXOR,
    LNAND,
    LNOR,
    LXNOR,

    GTI8,
    GTI16,
    GTI32,
    GTI64,
    GTU8,
    GTU16,
    GTU32,
    GTU64,
    GTF32,
    GTF64,

    GTEI8,
    GTEI16,
    GTEI32,
    GTEI64,
    GTEU8,
    GTEU16,
    GTEU32,
    GTEU64,
    GTEF32,
    GTEF64,

    LTI8,
    LTI16,
    LTI32,
    LTI64,
    LTU8,
    LTU16,
    LTU32,
    LTU64,
    LTF32,
    LTF64,

    LTEI8,
    LTEI16,
    LTEI32,
    LTEI64,
    LTEU8,
    LTEU16,
    LTEU32,
    LTEU64,
    LTEF32,
    LTEF64,

    EQ8,
    EQ16,
    EQ32,
    EQ64,

    NEQ8,
    NEQ16,
    NEQ32,
    NEQ64,

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

// All offsets in this struct are relative to the beginning of `bytes`.
struct Object {
    bool executable;
    uint64_t code; // char*
    uint64_t entryPoint; // char*
    uint64_t size;
    char *bytes;
};

// Returns a pointer to an empty object.
struct Object *createObject(size_t size);

void destroyObject(struct Object *object);

struct Object *loadObject(FILE *file);

void runObject(struct Object *object);

void runCode(uint8_t *code, uint8_t *data);

#endif // INTERPRETER_H
