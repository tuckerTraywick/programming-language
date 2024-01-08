#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdbool.h> // bool
#include <stdlib.h> // malloc(), free()
#include "parser.h"
#include "lexer.h"
#include "list.h"

#define NODES_INITIAL_CAPACITY 3000
#define ERRORS_INITIAL_CAPACITY 500

// Represents the state of the parser.
struct Parser {
    struct Token *tokens;
    size_t tokensCount;
    struct Node *nodes;
    size_t nodesCapacity;
    size_t nodesCount;
    struct Node *errors;
    size_t errorsCapacity;
    size_t errorsCount;
    struct Token *currentToken;
    struct Node *currentParent;
    struct Node *currentChild;
};

// Deallocates a `Parser`'s nodes and errors, and zeros its memory.
static void destroyParser(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    listDestroy((void**)&parser->nodes, &parser->nodesCapacity, &parser->nodesCount);
    listDestroy((void**)&parser->errors, &parser->errorsCapacity, &parser->errorsCount);
    *parser = (struct Parser){0};
}

void destroyParsingResult(struct ParsingResult *result) {
    assert(result != NULL && "Must pass a result.");
    free(result->nodes);
    free(result->errors);
    *result = (struct ParsingResult){0};
}

struct ParsingResult parse(struct Token *tokens, size_t tokensCount) {
    assert(tokens != NULL && "Must pass an array of tokens.");
    struct Parser parser = {
        .tokens = tokens,
        .tokensCount = tokensCount,
        .nodes = listCreate(sizeof(struct Node), NODES_INITIAL_CAPACITY),
        .nodesCapacity = NODES_INITIAL_CAPACITY,
        .errors = listCreate(sizeof(struct Node), ERRORS_INITIAL_CAPACITY),
        .errorsCapacity = ERRORS_INITIAL_CAPACITY,
    };



    struct ParsingResult result = {
        .nodes = parser.nodes,
        .nodesCount = parser.nodesCount,
        .errors = parser.errors,
        .errorsCount = parser.errorsCount,
    };
    destroyParser(&parser);
    return result;
}

#undef NODES_INITIAL_CAPACITY
#undef ERRORS_INITIAL_CAPACITY
