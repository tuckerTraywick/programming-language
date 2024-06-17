#include <stdint.h>
#include <stdlib.h>
#include "symboltable.h"

uint64_t getHash(char *name) {
    // Uses the MurmurOAT algorithm: https://stackoverflow.com/questions/7666509/hash-function-for-string.
    // I do not understand how this works.
    uint64_t hash = 525201411107845655ull;
    char *ch = name;
    while (*ch != '\0') {
        hash ^= *ch;
        hash *= 0x5bd1e9955bd1e995;
        hash ^= hash >> 47;
        ++ch;
    }
    return hash;
}
