#include <stddef.h> // size_t
#include <stdint.h> // int types
#include <stdbool.h> // bool
#include <assert.h> // assert()
#include <stdlib.h> // malloc(), free()
#include <stdio.h> // printf()
#include "interpreter.h"

void run(char *code) {
    // TODO: Figure out a smarter way to reserve the stack to avoid wasting memory.
    char *stack = malloc(STARTING_STACK_SIZE);
    char *ip = code;
    char *fp = stack;
    char *sp = stack;
    char *rp = NULL;

    bool keepRunning = true;
    while (keepRunning) {
        ptrdiff_t offset = 0;
        char *address = NULL;
        uint64_t value = 0;
        char opcode = *ip;
        ++ip;
        switch (opcode) {
            case HALT:
                keepRunning = false;
                break;
            case PUSH8I:
                // push8i char value
                // Push an 8-bit immediate value.
                ++sp;
                *sp = *ip;
                ++ip;
                break;
            case PUSH8:
                // push8
                // Move the stack pointer up 1 byte.
                ++sp;
                break;
            case PUSH16I:
                // push16i uint16_t value
                // Push a 16-bit immediate value.
                ++sp;
                *(uint16_t*)sp = *(uint16_t*)ip;
                sp += sizeof(uint16_t) - 1;
                ip += sizeof(uint16_t);
                break;
            case PUSH16:
                // push8
                // Move the stack pointer up 2 bytes.
                sp += sizeof(uint16_t);
                break;
            case LOAD8L:
                // load8l ptrdiff_t frameOffset
                // Push an 8-bit local variable or argument.
                offset = *ip;
                ip += sizeof(size_t);
                ++sp;
                *sp = fp[offset];
                break;
            case LOAD8:
                // load8
                // Push the 8-bit value at the memory address on the stack.
                sp -= sizeof(char*) + 1;
                address = *sp;
                *sp = *address;
                break;
            case LOADA:
                // loada ptrdiff_t frameOffset
                // Push the address of a local variable or argument.
                offset = *ip;
                ip += sizeof(char*);
                ++sp;
                *(char**)sp = fp + offset;
                sp += sizeof(char*);
                break;

            case PRINT8:
                // print8
                // Prints an 8-bit value from the stack.
                value = *(char*)sp;
                --sp;
                printf("%ld\n", value);
                break;
            case PRINT16:
                // print16
                // Prints a 16-bit value from the stack.
                sp -= sizeof(uint16_t);
                value = *(uint16_t*)(sp + 1);
                printf("%d\n", *(uint16_t*)(sp + 1));
                break;
            default:
                assert(0 && "Invalid opcode.");
                keepRunning = false;
                break;
        }
    }

    free(stack);
}
