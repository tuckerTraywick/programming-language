#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdbool.h> // bool
#include <stdlib.h> // free()
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
    size_t currentTokenIndex;
    size_t nextNodeIndex;
};

// Deallocates a parser's nodes and errors, and zeros its memory.
static void destroyParser(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    listDestroy((void**)&parser->nodes, &parser->nodesCapacity, &parser->nodesCount);
    listDestroy((void**)&parser->errors, &parser->errorsCapacity, &parser->errorsCount);
    *parser = (struct Parser){0};
}

// Returns true if the parser still has tokens to consume.
static bool hasTokens(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    return parser->tokens != NULL && parser->currentTokenIndex < parser->tokensCount;
}

// Returns the current token the parser is looking at.
static struct Token *currentToken(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    assert(hasTokens(parser));
    return parser->tokens + parser->currentTokenIndex;
}

// Returns the next new node in the parse tree.
static struct Node *nextNode(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    return parser->nodes + parser->nextNodeIndex;
}

// Looks at the current token and returns true if it has the given type.
static bool peek(struct Parser *parser, enum TokenType type) {
    assert(parser != NULL && "Must pass a parser.");
    return hasTokens(parser) && currentToken(parser)->type == type;
}

// Consumes the current token if it has the given type. Adds the token to the syntax tree.
static bool consume(struct Parser *parser, enum TokenType type) {
    if (peek(parser, type)) {
        struct Node *newNode = nextNode(parser);
        nextNode(parser)->type = TOKEN;
        nextNode(parser)->firstToken = parser->currentTokenIndex;
        nextNode(parser)->tokensCount = 1;
        nextNode(parser);
        ++parser->currentTokenIndex;
        return true;
    }
    return false;
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



    return (struct ParsingResult){
        .nodes = parser.nodes,
        .nodesCount = parser.nodesCount,
        .errors = parser.errors,
        .errorsCount = parser.errorsCount,
    };
}

#undef NODES_INITIAL_CAPACITY
#undef ERRORS_INITIAL_CAPACITY
