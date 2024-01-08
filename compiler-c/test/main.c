#include <stdio.h> // stdout
#define TEST_IMPL
#include "test.h"
#define LOG_IMPL
#include "log.h"
#include "list.h"
#include "lexer.h"
#include "parser.h"

static void printTokens(char *text, struct Token *tokens, size_t tokensCount) {
    printfDebug("%zu tokens:\n", tokensCount);
    for (size_t i = 0; i < tokensCount; ++i) {
        struct Token token = tokens[i];
        char *type = tokenTypeNames[token.type];
        if (token.type == NEWLINE) {
            printfDebug("%zu \"\\n\" length=%zu, index=%zu, row=%zu, column=%zu\n", i, token.length, token.index, token.row + 1, token.column + 1);
        } else if (token.type < PACKAGE) {
            printfDebug("%zu %s \"%.*s\" length=%zu, index=%zu, row=%zu, column=%zu\n", i, type, (int)token.length, text + token.index, token.length, token.index, token.row + 1, token.column + 1);
        } else {
            printfDebug("%zu \"%.*s\" length=%zu, index=%zu, row=%zu, column=%zu\n", i, (int)token.length, text + token.index, token.length, token.index, token.row + 1, token.column + 1);
        }
    }
}

static void printLexingErrors(struct Token *errors, size_t errorsCount) {
    printfDebug("%zu lexing errors:\n", errorsCount);
    for (size_t i = 0; i < errorsCount; ++i) {
        struct Token error = errors[i];
        printfDebug("(%zu:%zu): %s\n", error.row + 1, error.column + 1, tokenTypeNames[error.type]);
    }
}

// static void printParsingErrors(struct Node *errors, size_t errorsCount) {
//     for (size_t i = 0; i < errorsCount; ++i) {
//         struct Node error = errors[i];
//     }
// }

static void printPipes(int depth) {
    putsDebug("");
    for (int i = 0; i < depth; ++i) {
        putsDebug("| ");
    }
}

// static void printNode(char *text, struct Node *node, int depth) {
//     static char *types[] = {
//         [TOKEN] = "token",
//         [PROGRAM] = "program",
//     };

//     assert(node != NULL && "Must pass a node.");
//     assert(depth >= 0 && "`depth` must be >= 0.");
//     printPipes(depth);
//     printfDebug("%s", types[node->type]);
//     // printfDebug("%d", node->type);

//     struct Token token = node->tokens[0];
//     if (node->type == TOKEN) {
//         printfDebug(" \"%.*s\"\n", (int)token.length, text + token.index);
//     } else {
//         putsDebug("\n");
//     }

//     // for (size_t i = 0; i < node->childrenCount; ++i) {
//     //     printNode(node->children + i, depth + 1);
//     // }
// }

// static void printTree(char *text, struct Node *node) {
//     putsDebug("syntax tree:\n");
//     printNode(text, node, 0);
// }

void testList(void) {
    size_t listCapacity = 10;
    size_t listCount = 0;
    int *list = listCreate(sizeof *list, listCapacity);
    int a = 1, b = 2, c = 3;
    listAppend((void**)&list, sizeof *list, &listCapacity, &listCount, &a);
    listAppend((void**)&list, sizeof *list, &listCapacity, &listCount, &b);
    listAppend((void**)&list, sizeof *list, &listCapacity, &listCount, &c);
    test(list[0] == 1);
    test(list[1] == 2);
    test(list[2] == 3);
    test(listCount == 3);

    listDestroy((void**)&list, &listCapacity, &listCount);
    test(list == NULL && listCapacity == 0 && listCount == 0);
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
    // printLexingErrors(result.errors, result.errorsCount);
    printTokens(text, result.tokens, result.tokensCount);
    
    free(text);
    destroyLexingResult(&result);
}

void testParse(void) {
    char *text = openAndReadFile("test/example.txt");
    assert(text != NULL && "Failed to read file.");
    struct LexingResult lexingResult = lexString(text);
    assert(lexingResult.tokens != NULL && "Need tokens to parse.");
    struct ParsingResult parsingResult = parse(lexingResult.tokens, lexingResult.tokensCount);
    // assert(parsingResult.nodes != NULL && "Need nodes to print.");

    logDebug("");
    printTokens(text, lexingResult.tokens, lexingResult.tokensCount);
    putsDebug("\n");
    printLexingErrors(lexingResult.errors, lexingResult.errorsCount);
    putsDebug("\n");
    // printTree(parsingResult.nodes);
    putsDebug("\n");

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
