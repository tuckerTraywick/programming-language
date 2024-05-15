#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "interpreter.h"

// Redefining `abs` so it works for different types.
#define abs(x) (((x) < 0) ? (-(x)) : (x))

static uint64_t getWidth(uint64_t opcodeIndex) {
    return 1 << opcodeIndex; // 2^opcodeIndex
}

static void push(struct Interpreter *interpreter,  uint64_t width, uint64_t value) {
    memcpy(interpreter->sp, &value, width);
    interpreter->sp += width;
}

// This function exists to make 32 bit floating point instructions easier to implement.
static void pushFloat(struct Interpreter *interpreter, float value) {
    memcpy(interpreter->sp, &value, sizeof(float));
    interpreter->sp += sizeof(float);
}

// This function exists to make 64 bit floating point instructions easier to implement.
static void pushDouble(struct Interpreter *interpreter, double value) {
    memcpy(interpreter->sp, &value, sizeof(double));
    interpreter->sp += sizeof(double);
}

// Pops a value and pads it to 8 bytes.
static uint64_t pop(struct Interpreter *interpreter, uint64_t width) {
    uint64_t result = 0;
    interpreter->sp -= width;
    memcpy(&result, interpreter->sp, width);
    return result;
}

// This function exists to make 32 bit floating point instructions easier to implement.
static float popFloat(struct Interpreter *interpreter) {
    float result = 0.0;
    interpreter->sp -= sizeof(float);
    memcpy(&result, interpreter->sp, sizeof(float));
    return result;
}

// This function exists to make 64 bit floating point instructions easier to implement.
static double popDouble(struct Interpreter *interpreter) {
    double result = 0.0;
    interpreter->sp -= sizeof(double);
    memcpy(&result, interpreter->sp, sizeof(double));
    return result;
}

void destroyObject(struct Object *object) {
    if (object->isMemoryMapped) {
        // TODO: Unmap the object's memory.
    } else {
        free(object->bytes);
    }
    *object = (struct Object){0};
}

void writeObject(FILE *file, struct Object *object) {
    // TODO: Handle failed `fwrite()`s.
    fwrite(object, sizeof(struct ObjectHeader), 1, file);
    fwrite(object->bytes, 1, object->header.size, file);
}

void readObject(FILE *file, struct Object *object) {
    // TODO: Change this function to use `mmap()` instead of allocating a new buffer for the object's bytes.
    fread(&object->header, sizeof(struct ObjectHeader), 1, file);
    // TODO: Handle failed `fread()`.
    object->bytes = malloc(object->header.size);
    // TODO: Handle failed `malloc()`.
    assert(object->bytes);
    fread(object->bytes, 1, object->header.size, file);
    // TODO: Handle failed `fread()`.
    // TODO: Set `isMemoryMapped` to true when I change this.
    object->isMemoryMapped = false;
}

void printObjectHeader(struct ObjectHeader *header) {
    printf("executable: %s\n", (header->executable) ? "true" : "false");
    printf("size:       %ld\n", header->size);
    printf("code:       %zu\n", header->code);
    printf("data:       %zu\n", header->data);
    printf("entryPoint: %zu\n", header->entryPoint);
}

void run(struct Object *object) {
    // TODO: Handle passing a non-executable object.
    assert(object->header.executable && "Must pass an executable object.");
    uint8_t *code = object->bytes + object->header.entryPoint;
    uint8_t *data = object->bytes + object->header.data;
    runCode(code, data);
}

void runCode(uint8_t *code, uint8_t *data) {
    // TODO: Add support for passing a pointer to an existing stack?
    // TODO: Rename local variables.
    uint8_t *stack = malloc(STACK_SIZE);
    struct Interpreter interpreter = {
        .stack = stack,
        .ip = code,
        .fp = stack,
        .sp = stack,
        .keepRunning = true,
    };
    assert(stack && "`malloc()` failed.");

    while (interpreter.keepRunning) {
        uint64_t width = 0;
        uint64_t value = 0;
        uint64_t source = 0;
        uint64_t destination = 0;
        uint64_t ai = 0, bi = 0;
        float af = 0.0, bf = 0.0;
        double ad = 0.0, bd = 0.0;
        
        uint8_t opcode = *interpreter.ip;
        ++interpreter.ip;

        switch (opcode) {
            case NOOP:
                continue;

            case HALT:
                interpreter.keepRunning = false;
                break;

            case PUSH8...PUSH64:
                width = getWidth(opcode - PUSH8);
                memcpy(interpreter.sp, interpreter.ip, width);
                interpreter.sp += width;
                interpreter.ip += width;
                break;

            case PUSHL:
                push(&interpreter, 8, (uint64_t)(interpreter.fp + *(ptrdiff_t*)interpreter.ip));
                interpreter.ip += 8;
                break;

            case PUSHA:
                push(&interpreter, 8, (uint64_t)(interpreter.fp - *(ptrdiff_t*)interpreter.ip - 25));
                interpreter.ip += 8;
                break;

            case PUSHT:
                push(&interpreter, 8, (uint64_t)interpreter.sp - pop(&interpreter, 8));
                break;

            case PUSHD:
                push(&interpreter, 8, (uint64_t)data + pop(&interpreter, 8));                
                break;

            case POP8...POP64:
                width = getWidth(opcode - POP8);
                interpreter.sp -= width;
                break;

            case POP:
                interpreter.sp -= pop(&interpreter, 8);
                break;

            case DUP8...DUP64:
                width = getWidth(opcode - DUP8);
                memcpy(interpreter.sp, interpreter.sp - width, width);
                interpreter.sp += width;
                break;

            case DUP:
                width = pop(&interpreter, 8);
                memcpy(interpreter.sp, interpreter.sp - width, width);
                interpreter.sp += width;
                break;

            case ZERO8...ZERO64:
                width = getWidth(opcode - ZERO8);
                push(&interpreter, width, 0);
                break;

            case ZERO:
                width = pop(&interpreter, 8);
                memset(interpreter.sp, 0, (int)width);
                interpreter.sp += width;
                break;

            case BUMP8...BUMP64:
                width = getWidth(opcode - BUMP8);
                interpreter.sp += width;
                break;

            case BUMP:
                width = pop(&interpreter, 8);
                interpreter.sp += width;
                break;

            case LOAD8...LOAD64:
                width = getWidth(opcode - LOAD8);
                value = pop(&interpreter, 8);
                memcpy(interpreter.sp, (void*)value, width);
                interpreter.sp += width;
                break;

            case LOAD:
                width = pop(&interpreter, 8);
                value = pop(&interpreter, 8);
                memcpy(interpreter.sp, (void*)value, width);
                interpreter.sp += width;
                break;

            case STORE8...STORE64:
                width = getWidth(opcode - STORE8);
                destination = pop(&interpreter, 8);
                memcpy((void*)destination, interpreter.sp - width, width);
                interpreter.sp -= width;
                break;

            case STORE:
                width = pop(&interpreter, 8);
                destination = pop(&interpreter, 8);
                memcpy((void*)destination, interpreter.sp - width, width);
                interpreter.sp -= width;
                break;

            case COPY8...COPY64:
                width = getWidth(opcode - COPY8);
                destination = pop(&interpreter, 8);
                source = pop(&interpreter, 8);
                memcpy((void*)destination, (void*)source, width);
                break;

            case COPY:
                width = pop(&interpreter, 8);
                destination = pop(&interpreter, 8);
                source = pop(&interpreter, 8);
                memcpy((void*)destination, (void*)source, width);
                break;

            case FIRST16...FIRST64:
                width = getWidth(opcode - FIRST16 + 1);
                push(&interpreter, 1, *(uint8_t*)(interpreter.sp - width));
                break;

            case JMP:
                destination = pop(&interpreter, 8);
                interpreter.ip = (uint8_t*)(code + destination);
                break;

            case JMPT:
                value = pop(&interpreter, 1);
                destination = pop(&interpreter, 8);
                if (value) {
                    interpreter.ip = (uint8_t*)destination;
                }
                break;

            case JMPF:
                value = pop(&interpreter, 1);
                destination = pop(&interpreter, 8);
                if (!value) {
                    interpreter.ip = (uint8_t*)destination;
                }
                break;

            case CALL:
                destination = pop(&interpreter, 8);
                push(&interpreter, 8, (uint64_t)interpreter.sp);
                push(&interpreter, 8, (uint64_t)interpreter.fp);
                push(&interpreter, 8, (uint64_t)interpreter.ip);
                interpreter.ip = code + destination;
                interpreter.fp = interpreter.sp;
                break;

            case RET:
                interpreter.ip = (uint8_t*)pop(&interpreter, 8);
                interpreter.fp = (uint8_t*)pop(&interpreter, 8);
                interpreter.sp = (uint8_t*)pop(&interpreter, 8);
                break;

            case ADDI8...ADDI64:
                width = getWidth(opcode - ADDI8);
                push(&interpreter, width, pop(&interpreter, width) + pop(&interpreter, width));
                break;

            case ADDF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                pushFloat(&interpreter, af + bf);
                break;

            case ADDF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, ad + bd);
                break;

            case SUBI8...SUBI64:
                width = getWidth(opcode - SUBI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, width, ai - bi);
                break;

            case SUBF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                pushFloat(&interpreter, af - bf);
                break;

            case SUBF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, ad - bd);
                break;

            case MULI8...MULI64:
                width = getWidth(opcode - MULI8);
                push(&interpreter, width, (int64_t)pop(&interpreter, width) * (int64_t)pop(&interpreter, width));
                break;

            case MULU8...MULU64:
                width = getWidth(opcode - MULU8);
                push(&interpreter, width, pop(&interpreter, width) * pop(&interpreter, width));
                break;

            case MULF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                pushFloat(&interpreter, af * bf);
                break;

            case MULF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, ad * bd);
                break;

            case DIVI8...DIVI64:
                width = getWidth(opcode - DIVI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, width, (int64_t)ai / (int64_t)bi);
                break;

            case DIVU8...DIVU64:
                width = getWidth(opcode - DIVU8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, width, ai / bi);
                break;

            case DIVF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                pushFloat(&interpreter, af / bf);
                break;

            case DIVF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, ad / bd);
                break;

            case MODI8...MODI64:
                width = getWidth(opcode - MODI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, width, (int64_t)ai % (int64_t)bi);
                break;

            case MODU8...MODU64:
                width = getWidth(opcode - MODU8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, width, ai % bi);
                break;

            case NEGI8...NEGI64:
                width = getWidth(opcode - NEGI8);
                ai = pop(&interpreter, width);
                push(&interpreter, width, -(int64_t)ai);
                break;

            case NEGF32:
                af = popFloat(&interpreter);
                pushFloat(&interpreter, -af);
                break;

            case NEGF64:
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, -ad);
                break;

            case ABSI8:
                ai = pop(&interpreter, 1);
                push(&interpreter, 1, abs((int8_t)ai));
                break;

            case ABSI16:
                ai = pop(&interpreter, 2);
                push(&interpreter, 2, abs((int16_t)ai));
                break;

            case ABSI32:
                ai = pop(&interpreter, 4);
                push(&interpreter, 4, abs((int32_t)ai));
                break;

            case ABSI64:
                ai = pop(&interpreter, 8);
                push(&interpreter, 8, abs((int64_t)ai));
                break;

            case ABSF32:
                af = popFloat(&interpreter);
                pushFloat(&interpreter, abs(af));
                break;

            case ABSF64:
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, abs(ad));
                break;

            case FLOOR32:
                af = popFloat(&interpreter);
                pushFloat(&interpreter, floorf(af));
                break;

            case FLOOR64:
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, floor(ad));
                break;

            case CEIL32:
                af = popFloat(&interpreter);
                pushFloat(&interpreter, ceilf(af));
                break;

            case CEIL64:
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, ceil(ad));
                break;

            case ROUND32:
                af = popFloat(&interpreter);
                pushFloat(&interpreter, roundf(af));
                break;

            case ROUND64:
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, round(ad));
                break;

            case SQRT32:
                af = popFloat(&interpreter);
                pushFloat(&interpreter, sqrtf(af));
                break;

            case SQRT64:
                ad = popDouble(&interpreter);
                pushDouble(&interpreter, sqrt(ad));
                break;

            case BNOT8...BNOT64:
                width = getWidth(opcode - BNOT8);
                push(&interpreter, width, ~pop(&interpreter, width));
                break;

            case BAND8...BAND64:
                width = getWidth(opcode - BAND8);
                push(&interpreter, width, pop(&interpreter, width) & pop(&interpreter, width));
                break;

            case BOR8...BOR64:
                width = getWidth(opcode - BOR8);
                push(&interpreter, width, pop(&interpreter, width) | pop(&interpreter, width));
                break;

            case BXOR8...BXOR64:
                width = getWidth(opcode - BXOR8);
                push(&interpreter, width, pop(&interpreter, width) ^ pop(&interpreter, width));
                break;

            case BNAND8...BNAND64:
                width = getWidth(opcode - BNAND8);
                push(&interpreter, width, ~(pop(&interpreter, width) & pop(&interpreter, width)));
                break;

            case BNOR8...BNOR64:
                width = getWidth(opcode - BNOR8);
                push(&interpreter, width, ~(pop(&interpreter, width) | pop(&interpreter, width)));
                break;

            case BXNOR8...BXNOR64:
                width = getWidth(opcode - BXNOR8);
                push(&interpreter, width, ~(pop(&interpreter, width) ^ pop(&interpreter, width)));
                break;

            case LNOT:
                push(&interpreter, 1, ~pop(&interpreter, 1));
                break;

            case LAND:
                push(&interpreter, 1, pop(&interpreter, 1) && pop(&interpreter, 1));
                break;

            case LOR:
                push(&interpreter, 1, pop(&interpreter, 1) || pop(&interpreter, 1));
                break;

            case LXOR:
                push(&interpreter, 1, !pop(&interpreter, 1) != !pop(&interpreter, 1));
                break;

            case LNAND:
                push(&interpreter, 1, !(pop(&interpreter, 1) && pop(&interpreter, 1)));
                break;

            case LNOR:
                push(&interpreter, 1, !(pop(&interpreter, 1) || pop(&interpreter, 1)));
                break;

            case LXNOR:
                push(&interpreter, 1, !pop(&interpreter, 1) == !pop(&interpreter, 1));
                break;

            case GTI8...GTI64:
                width = getWidth(opcode - GTI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)((int64_t)ai > (int64_t)bi));
                break;

            case GTU8...GTU64:
                width = getWidth(opcode - GTU8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)(ai > bi));
                break;

            case GTF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                push(&interpreter, 1, (uint8_t)(af > bf));
                break;

            case GTF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                push(&interpreter, 1, (uint8_t)(ad > bd));
                break;

            case GTEI8...GTEI64:
                width = getWidth(opcode - GTEI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)((int64_t)ai >= (int64_t)bi));
                break;

            case GTEU8...GTEU64:
                width = getWidth(opcode - GTEU8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)(ai >= bi));
                break;

            case GTEF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                push(&interpreter, 1, (uint8_t)(af >= bf));
                break;

            case GTEF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                push(&interpreter, 1, (uint8_t)(ad >= bd));
                break;

            case LTI8...LTI64:
                width = getWidth(opcode - LTI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)((int64_t)ai < (int64_t)bi));
                break;

            case LTU8...LTU64:
                width = getWidth(opcode - LTU8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)(ai < bi));
                break;

            case LTF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                push(&interpreter, 1, (uint8_t)(af < bf));
                break;

            case LTF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                push(&interpreter, 1, (uint8_t)(ad < bd));
                break;

            case LTEI8...LTEI64:
                width = getWidth(opcode - LTEI8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)((int64_t)ai <= (int64_t)bi));
                break;

            case LTEU8...LTEU64:
                width = getWidth(opcode - LTEU8);
                bi = pop(&interpreter, width);
                ai = pop(&interpreter, width);
                push(&interpreter, 1, (uint8_t)(ai <= bi));
                break;

            case LTEF32:
                bf = popFloat(&interpreter);
                af = popFloat(&interpreter);
                push(&interpreter, 1, (uint8_t)(af <= bf));
                break;

            case LTEF64:
                bd = popDouble(&interpreter);
                ad = popDouble(&interpreter);
                push(&interpreter, 1, (uint8_t)(ad <= bd));
                break;

            case EQ8...EQ64:
                width = getWidth(opcode - EQ8);
                push(&interpreter, 1, (uint8_t)(pop(&interpreter, width) == pop(&interpreter, width)));
                break;

            case PRINTI8:
                printf("%d\n", (int8_t)pop(&interpreter, 1));
                break;

            case PRINTI16:
                printf("%d\n", (int16_t)pop(&interpreter, 2));
                break;

            case PRINTI32:
                printf("%d\n", (int32_t)pop(&interpreter, 4));
                break;

            case PRINTI64:
                printf("%ld\n", (int64_t)pop(&interpreter, 8));
                break;

            case PRINTU8...PRINTU64:
                width = getWidth(opcode - PRINTU8);
                printf("%zu\n", pop(&interpreter, width));
                break;

            case PRINTF32:
                printf("%f\n", popFloat(&interpreter));
                break;

            case PRINTF64:
                printf("%f\n", popDouble(&interpreter));
                break;

            default:
                assert(0 && "Invalid opcode.");
                interpreter.keepRunning = false;
                break;
        }
    }

    printf("\nstack:\n");
    for (uint8_t *byte = interpreter.stack; byte < interpreter.sp; ++byte) {
        printf("%d ", *byte);
    }
    printf("| %d\n", *interpreter.sp);

    free(stack);
}

uint64_t getSymbol(uint8_t *bytes, struct SymbolTable *table, char *name) {
    char *ch = name;
    struct SymbolTableNode *node = table->nodes;
    while (true) {
        printf("ch=%c, node->ch=%c, child=%ld\n", *ch, node->ch, node->child);
        if (*ch == '\0' && node->ch == '\0') {
            return node->child;
        } else if (*ch == '\0' || node->ch == '\0') {
            return 0;
        } else if (*ch == node->ch) {
            printf("match\n");
            node = (struct SymbolTableNode*)(bytes + node->child);
            ++ch;
        } else if (node->next == 0) {
            return 0;
        } else {
            node = (struct SymbolTableNode*)(bytes + node->next);
        }
    }
}

void addSymbol(struct SymbolTable *table, char *name, uint64_t offset) {
    char *ch = name;
    while (ch != '\0') {

    }
}
