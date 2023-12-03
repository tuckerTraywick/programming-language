#include <stdlib.h>
#include <stdio.h>
#define TEST_IMPL
#include "test.h"
#include "parser.h"

static void printTokens(struct Token *tokens, size_t tokensCount) {
    static char *types[] = {
        [INVALID] = "invalid",
        [NUMBER] = "number",
        [CHARACTER] = "character",
        [STRING] = "string",
        [IDENTIFIER] = "identifier",
        [NEWLINE] = "\\n",
    };

    for (size_t i = 0; i < tokensCount; ++i) {
        char *type = tokens[i].text;
        if (tokens[i].type < FROM) {
            type = types[tokens[i].type];
        }
        char *text = tokens[i].text;
        size_t textLength = tokens[i].textLength;
        if (tokens[i].type == NEWLINE) {
            text = "\\n";
            textLength = 2;
        }
        printf("%zu %s '%.*s' length=%zu, index=%zu\n", i, type, (int)textLength, text, tokens[i].textLength, tokens[i].index);
    }
}

void testReadFile(void) {
    // puts("----");
    FILE *file = fopen("test/example.txt", "r");
    test(file != NULL && "Failed to open file.");
    char *text = readFile(file);
    test(text != NULL && "Failed to read file.");
    // printf("file text:\n%s\n", text);

    fclose(file);
    free(text);
    // puts("");
}

void testOpenAndReadFile(void) {
    // puts("----");
    char *text = openAndReadFile("test/example.txt");
    test(text != NULL && "Failed to read file.");
    // printf("file text:\n%s\n", text);

    free(text);
    // puts("");
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
    printTokens(result.tokens, result.tokensCount);

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
