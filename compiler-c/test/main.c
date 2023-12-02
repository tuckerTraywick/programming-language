#include <stdlib.h>
#include <stdio.h>
#define TEST_IMPL
#include "test.h"
#include "parser.h"

void testReadFile(void) {
    puts("----");
    FILE *file = fopen("test/example.txt", "r");
    test(file != NULL && "Failed to open file.");
    char *text = readFile(file);
    test(text != NULL && "Failed to read file.");
    printf("file text:\n%s\n", text);
    fclose(file);
    free(text);
    puts("");
}

void testOpenAndReadFile(void) {
    puts("----");
    char *text = openAndReadFile("test/example.txt");
    test(text != NULL && "Failed to read file.");
    printf("file text:\n%s\n", text);
    free(text);
    puts("");
}

void testLexString(void) {
    puts("----");
    char *text = openAndReadFile("test/example.txt");
    test(text != NULL && "Failed to read file.");
    struct LexingResult result = lexString(text, false);
    printf("tokens = %p, tokensCount = %zu, errorMessages = %p, errorMessagesCount = %zu\n",
        (void*)result.tokens,
        result.tokensCount,
        (void*)result.errorMessages,
        result.errorMessagesCount
    );
    free(text);
    destroyLexingResult(&result);
    puts("");
}

int main(void) {
    beginTesting();
        testReadFile();
        testOpenAndReadFile();
        testLexString();
    endTesting();
    return 0;
}
