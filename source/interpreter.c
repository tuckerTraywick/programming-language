#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "interpreter.h"
#include "object.h"

// Redefining `abs` so it works for different types.
#define abs(x) (((x) < 0) ? (-(x)) : (x))

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
    // TODO: Handle failed `malloc()`.
    assert(stack && "`malloc()` failed.");
    struct Interpreter interpreter = {
        .stack = stack,
        .ip = code,
        .fp = stack,
        .sp = stack,
        .keepRunning = true,
    };

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
