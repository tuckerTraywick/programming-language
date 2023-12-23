#include <stdio.h> // stdout
#define TEST_IMPL
#include "test.h"
#define LOG_IMPL
#include "log.h"
#include "list.h"
#include "lexer.h"
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
        struct Token token = tokens[i];
        char *text = token.text;
        size_t length = token.length;
        if (token.type == NEWLINE) {
            text = "\\n";
            length = 2;
        }

        if (token.type < NEWLINE) {
            logfDebug("%zu %s \"%.*s\" length=%zu, index=%zu, row=%zu, column=%zu", i, types[token.type], (int)length, text, length, token.index, token.row, token.column);
        } else {
            logfDebug("%zu \"%.*s\" length=%zu, index=%zu, row=%zu, column=%zu", i, (int)length, text, length, token.index, token.row, token.column);
        }
    }
}

static void printLexingErrors(struct LexingError *errors, size_t errorsCount) {
    if (errors != NULL) {
        for (size_t i = 0; i < errorsCount; ++i) {
            struct LexingError error = errors[i];
            logfDebug("Lexing error (%zu:%zu): %s", error.row + 1, error.column + 1, error.message);
        }
    }
}

void testList(void) {
    struct List list = listCreate(int, 10);
    int a = 1, b = 2, c = 3;
    listAppend(&list, &a);
    listAppend(&list, &b);
    listAppend(&list, &c);
    test(*(int*)listGet(int, &list, 0) == 1);
    test(*(int*)listGet(int, &list, 1) == 2);
    test(*(int*)listGet(int, &list, 2) == 3);

    listDestroy(&list);
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
    struct LexingResult result = lexString(text);
    logfDebug("tokensCount=%zu, errorMessagesCount=%zu", result.tokensCount, result.errorsCount);
    printLexingErrors(result.errors, result.errorsCount);
    printTokens(result.tokens, result.tokensCount);
    
    free(text);
    destroyLexingResult(&result);
}

int main(void) {
    suiteOut = testOut = assertOut = resultsOut = stdout;
    debugOut = infoOut = warningOut = errorOut = stdout;
    beginTesting();
        runSuite(testList);
        runSuite(testReadFile);
        runSuite(testOpenAndReadFile);
        runSuite(testLexString);
    endTesting();
    return 0;
}
