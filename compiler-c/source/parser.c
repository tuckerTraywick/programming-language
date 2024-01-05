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
    size_t currentToken;
    struct List nodes;
    struct Node *currentNode;
    struct List errors;
};

// Deallocates a `Parser`'s nodes and errors, and zeros its memory.
static void destroyParser(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");

}

// Returns true if the parser has consumed all of its tokens.
static bool hasTokens(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
    return parser->currentToken < parser->tokensCount;
}

// Returns true if the current token of the parser is of the given type.
static bool peek(struct Parser *parser, enum TokenType type) {
    assert(parser != NULL && "Must pass a parser.");
    return hasTokens(parser) && parser->tokens[parser->currentToken].type == type;
}

// Returns true and advances to the next token if the current token of the parser is of the given type.
static bool consume(struct Parser *parser, enum TokenType type) {
    assert(parser != NULL && "Must pass a parser.");
    if (peek(parser, type)) {
        ++parser->currentToken;
        return false;
    }
    return true;
}

// Begins a new node in the parse tree with the given type. The node is appended to `parser->nodes`.
static void beginNode(struct Parser *parser, enum NodeType type) {
    assert(parser != NULL && "Must pass a parser.");
    struct Node node = {
        .type = type,
        .tokens = parser->tokens + parser->currentToken,
        .parent = parser->currentNode,
    };
    listAppend(&parser->nodes, &node);
    parser->currentNode->next = listLast(struct Node, &parser->nodes);
}

// Ends the current node and moves the current node pointer to its parent if it has one. Always
// returns `true` so the return value can be used in parsing functions.
static bool endNode(struct Parser *parser) {
    assert(parser != NULL && "Must pass a parser.");
}

// static bool parsePackageStatement(struct Parser *parser) {
//     assert(parser != NULL && "Must pass a parser.");
//     beginNode(parser, PACKAGE_STATEMENT);
//     if (!consume(parser, PACKAGE)) return fail(parser);
//     if (!consume(parser, IDENTIFIER)) recover(parser, "Expected a packge name.");
//     while (consume(parser, DOT)) {
//         if (consume(parser, TIMES)) {
//             if (hasTokens(parser) || consume(parser, NEWLINE)) break;
//             recover(parser, "Expected end of statement.");
//         }
//         if (!consume(parser, IDENTIFIER)) recover(parser, "Expected an identifier.");
//     }
//     return endNode(parser);
// }

void destroyParsingResult(struct ParsingResult *result) {
    assert(result != NULL && "Must pass a result.");
    free(result->nodes);
    free(result->errors);
    *result = (struct ParsingResult){0};
}

struct ParsingResult parse(struct Token *tokens, size_t tokensCount) {
    assert(tokens != NULL && "Must pass an array of tokens.");
    struct List nodes = listCreate(struct Node, NODES_INITIAL_CAPACITY);
    struct List errors = listCreate(struct ParsingError, ERRORS_INITIAL_CAPACITY);
    struct Parser parser = {
        .nodes = nodes,
        .currentNode = (struct Node*)nodes.elements,
        .errors = errors,
    };
    

    return (struct ParsingResult){
        .nodes = (struct Node*)parser.nodes.elements,
        .nodesCount = parser.nodes.count,
        .errors = (struct ParsingError*)parser.errors.elements,
        .errorsCount = parser.errors.count,
    };
}

#undef NODES_INITIAL_CAPACITY
#undef ERRORS_INITIAL_CAPACITY
