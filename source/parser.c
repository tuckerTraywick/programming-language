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

// Returns the next empty node in the parse tree.
static struct Node *nextNode(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    return parser->nodes + parser->nextNodeIndex;
}

// Appends a new empty node the list of nodes.
static void appendNode(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    struct Node node = parser->nodes[parser->nextNodeIndex];
    listAppend((void**)&parser->nodes, sizeof *parser->nodes, &parser->nodesCapacity, &parser->nodesCount, &node);
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

// Looks at the current token and returns true if it has the given type.
static bool peek(struct Parser *parser, enum TokenType type) {
    assert(parser != NULL && "Must pass a parser.");
    return hasTokens(parser) && currentToken(parser)->type == type;
}

// Consumes the current token if it has the given type. Adds the token to the syntax tree.
static bool consume(struct Parser *parser, enum TokenType type) {
    if (peek(parser, type)) {
        struct Node *next = nextNode(parser);
        next->type = TOKEN;
        next->firstToken = parser->currentTokenIndex;
        next->tokensCount = 1;
        next->next = parser->nextNodeIndex + 1;
        appendNode(parser);
        ++parser->currentTokenIndex;
        return true;
    }
    return false;
}

char *nodeTypeNames[] = {
    [INVALID_NODE] = "Invalid syntax.",
    [PACKAGE_STATEMENT_EXPECTED_PACKAGE_NAME] = "Expected a package name.",
    [TOKEN] = "token",
    [PROGRAM] = "program",
    [PACKAGE_STATEMENT] = "package statement",
    [IMPORT_STATEMENT] = "import statement",
};

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

    consume(&parser, PACKAGE);
    consume(&parser, IMPORT);

    return (struct ParsingResult){
        .nodes = parser.nodes,
        .nodesCount = parser.nodesCount,
        .errors = parser.errors,
        .errorsCount = parser.errorsCount,
    };
}

#undef NODES_INITIAL_CAPACITY
#undef ERRORS_INITIAL_CAPACITY
