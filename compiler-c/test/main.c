#include <stdio.h> // stdout
#define TEST_IMPL
#include "test.h"
#define LOG_IMPL
#include "log.h"
#include "parser.h"

static void printTokens(struct Token *tokens, size_t tokensCount) {
    static char *types[] = {
        [INVALID] = "invalid",
        [NUMBER] = "number",
        [CHARACTER] = "character",
        [STRING] = "string",
        [IDENTIFIER] = "identifier",
    };

    for (size_t i = 0; i < tokensCount; ++i) {
        char *text = tokens[i].text;
        size_t textLength = tokens[i].textLength;
        if (tokens[i].type == NEWLINE) {
            text = "\\n";
            textLength = 2;
        }

        if (tokens[i].type < NEWLINE) {
            logfDebug("%zu %s '%.*s' length=%zu, index=%zu", i, types[tokens[i].type], (int)textLength, text, textLength, tokens[i].index);
        } else {
            logfDebug("%zu '%.*s' length=%zu, index=%zu", i, (int)textLength, text, textLength, tokens[i].index);
        }
    }
}

void testReadFile(void) {
    FILE *file = fopen("test/example.txt", "r");
    assert(file != NULL && "Failed to open file.");
    char *text = readFile(file);
    assert(text != NULL && "Failed to read file.");
    fclose(file);
    free(text);
}

void testOpenAndReadFile(void) {
    debugOut = NULL;
    char *text = openAndReadFile("test/example.txt");
    assert(text != NULL && "Failed to read file.");
    logfDebug("\nfile text:\n%s", text);
    free(text);
    debugOut = stdout;
}

void testLexString(void) {
    char *text = openAndReadFile("test/example.txt");
    assert(text != NULL && "Failed to read file.");
    struct LexingResult result = lexString(text, false);
    logfDebug("tokens=%p, tokensCount=%zu, errorMessages=%p, errorMessagesCount=%zu",
        (void*)result.tokens,
        result.tokensCount,
        (void*)result.errorMessages,
        result.errorMessagesCount
    );
    printTokens(result.tokens, result.tokensCount);
    free(text);
    destroyLexingResult(&result);
}

int main(void) {
    suiteOut = testOut = assertOut = resultsOut = stdout;
    debugOut = stdout;
    beginTesting();
        runSuite(testReadFile);
        runSuite(testOpenAndReadFile);
        runSuite(testLexString);
    endTesting();
    return 0;
}
