#include <stdio.h> // stdout
#define TEST_IMPL
#include "test.h"
#define LOG_IMPL
#include "log.h"
#include "parser.h"
#include "list.h"

static void printTokens(TokenList *tokens) {
    static char *types[] = {
        [INVALID] = "invalid",
        [NUMBER] = "number",
        [CHARACTER] = "character",
        [STRING] = "string",
        [IDENTIFIER] = "identifier",
    };

    for (size_t i = 0; i < tokens->count; ++i) {
        struct Token *token = (struct Token*)listGet(tokens, i);
        char *text = token->text;
        size_t textLength = token->textLength;
        if (token->type == NEWLINE) {
            text = "\\n";
            textLength = 2;
        }

        if (token->type < NEWLINE) {
            logfDebug("%zu %s '%.*s' length=%zu, index=%zu", i, types[token->type], (int)textLength, text, textLength, token->index);
        } else {
            logfDebug("%zu '%.*s' length=%zu, index=%zu", i, (int)textLength, text, textLength, token->index);
        }
    }
}

void testList(void) {
    struct List list;
    struct Token token = {
        .type = IDENTIFIER,
        .text = "hi",
        .textLength = 2,
        .index = 0,
        .row = 0,
        .column = 0,
    };
    listInitialize(&list, sizeof(struct Token), 1, 10);
    listAppend(&list, (char*)&token);
    listAppend(&list, (char*)&token);
    struct Token *item = (struct Token*)listGet(&list, 1);
    logfDebug("%zu", item->textLength);
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
    TokenList tokens = {0};
    ErrorList errors = {0};
    lexString(text, &tokens, &errors);
    logfDebug("tokensCount=%zu, errorMessagesCount=%zu", tokens.count, errors.count);
    printTokens(&tokens);
    
    free(text);
    listDestroy(&tokens);
    listDestroy(&errors);
}

int main(void) {
    suiteOut = testOut = assertOut = resultsOut = stdout;
    debugOut = infoOut = warningOut = errorOut = stdout;
    beginTesting();
        // runSuite(testList);
        runSuite(testReadFile);
        runSuite(testOpenAndReadFile);
        runSuite(testLexString);
    endTesting();
    return 0;
}
