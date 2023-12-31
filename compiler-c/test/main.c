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

    logDebug("");
    for (size_t i = 0; i < tokensCount; ++i) {
        struct Token token = tokens[i];
        char *text = token.text;
        size_t length = token.length;
        if (token.type == NEWLINE) {
            text = "\\n";
            length = 2;
        }

        if (token.type < NEWLINE) {
            printfDebug("%zu %s \"%.*s\" length=%zu, index=%zu, row=%zu, column=%zu\n", i, types[token.type], (int)length, text, length, token.index, token.row, token.column);
        } else {
            printfDebug("%zu \"%.*s\" length=%zu, index=%zu, row=%zu, column=%zu\n", i, (int)length, text, length, token.index, token.row, token.column);
        }
    }
}

static void printLexingErrors(struct LexingError *errors, size_t errorsCount) {
    for (size_t i = 0; i < errorsCount; ++i) {
        struct LexingError error = errors[i];
        logfDebug("Lexing error (%zu:%zu): %s", error.row + 1, error.column + 1, error.message);
    }
}

static void printParsingErrors(struct ParsingError *errors, size_t errorsCount) {
    for (size_t i = 0; i < errorsCount; ++i) {
        struct ParsingError error = errors[i];
        logfDebug("Parsing error (%zu): %s", error.index, error.message);
    }
}

static void printPipes(int depth) {
    putsDebug("");
    for (int i = 0; i < depth; ++i) {
        putsDebug("| ");
    }
}

static void printNode(struct Node *node, int depth) {
    static char *types[] = {
        [TOKEN] = "token",
        [PROGRAM] = "program",
    };

    assert(depth >= 0 && "`depth` must be positive.");
    printPipes(depth);
    printfDebug("%s", types[node->type]);

    struct Token token = node->tokens[0];
    if (node->type == TOKEN) {
        printfDebug(" \"%.*s\"\n", (int)token.length, token.text);
    } else {
        putsDebug("\n");
    }

    // for (size_t i = 0; i < node->childrenCount; ++i) {
    //     printNode(node->children + i, depth + 1);
    // }
}

static void printTree(struct Node *node) {
    assert(node != NULL && "Must pass a node.");
    logDebug("");
    printNode(node, 0);
}

void testList(void) {
    struct List list = listCreate(int, 10);
    int a = 1, b = 2, c = 3;
    listAppend(&list, &a, 1);
    listAppend(&list, &b, 1);
    listAppend(&list, &c, 1);
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

void testParse(void) {
    char *text = openAndReadFile("test/example.txt");
    assert(text != NULL && "Failed to read file.");
    struct LexingResult lexingResult = lexString(text);
    assert(lexingResult.tokens != NULL && "Need tokens to parse.");
    struct ParsingResult parsingResult = parse(lexingResult.tokens, lexingResult.tokensCount);
    // assert(parsingResult.nodes != NULL && "Need nodes.");

    printTokens(lexingResult.tokens, lexingResult.tokensCount);
    // printTree(parsingResult.nodes);
    logfDebug("tokensCount=%zu, lexingErrorsCount=%zu", lexingResult.tokensCount, lexingResult.errorsCount);
    logfDebug("nodesCount=%zu, parsingErrorsCount=%zu", parsingResult.nodesCount, parsingResult.errorsCount);
    printLexingErrors(lexingResult.errors, lexingResult.errorsCount);
    printParsingErrors(parsingResult.errors, parsingResult.errorsCount);

    free(text);
    destroyLexingResult(&lexingResult);
    destroyParsingResult(&parsingResult);
}

int main(void) {
    suiteOut = testOut = assertOut = resultsOut = stdout;
    debugOut = infoOut = warningOut = errorOut = stdout;
    beginTesting();
        runSuite(testList);
        runSuite(testReadFile);
        runSuite(testOpenAndReadFile);
        // runSuite(testLexString);
        runSuite(testParse);
    endTesting();
    return 0;
}
