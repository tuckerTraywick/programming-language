// Definitions
#ifndef TEST_H
#define TEST_H

#define test(expression) testImpl((expression), #expression, __FILE__, __LINE__, __func__)

void beginTesting(void);
void endTesting(void);

#endif

// Implementation
#ifdef TEST_IMPL
#undef TEST_IMPL

#include <stddef.h> // size_t
#include <stdio.h> // printf

static size_t testsRun;
static size_t testsPassed;

__attribute__((unused))
static void testImpl(int result, const char *test, const char *file, int line, const char *func) {
    ++testsRun;
    if (result) {
        ++testsPassed;
        return;
    }
    printf("%s:%i: %s:\nfailed '%s'\n\n", file, line, func, test);
}

__attribute__((unused))
void beginTesting(void) {
    testsRun = 0;
    testsPassed = 0;
}

__attribute__((unused))
void endTesting(void) {
    if (testsRun) {
        printf("%zu tests run, ", testsRun);
        if (testsRun == testsPassed) {
            printf("all tests passed\n");
        } else if (testsPassed == 0) {
            printf("all tests failed\n");
        } else {
            printf("%zu tests passed, ", testsPassed);
            printf("%zu tests failed\n", testsRun - testsPassed);    
        }
    } else {
        printf("No tests run\n");
    }
    beginTesting(); // Zero out the global variables for the next test suite.
}

#endif
