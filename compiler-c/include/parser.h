#ifndef PARSER_H
#define PARSER_H

#include <stddef.h> // size_t
#include "lexer.h"

// Represents an error encountered during parsing.
struct ParsingError {
    char *message;
    size_t index; // Index of the first token the error corresponds to.
};

// Represents the type of a node.
enum NodeType {
    TOKEN,
    PROGRAM,
    PACKAGE_STATEMENT,
    IMPORT_STATEMENT,

    NODE_TYPE_COUNT,
};

// Represents a node in a syntax tree.
struct Node {
    enum NodeType type;
    struct Node *parent;
    struct Node *child;
    struct Node *next;
    struct Token *tokens; // Points to the first token parsed to create the node. Unowned, don't free.
    size_t tokensCount;
};

// Represents the result of parsing an array of tokens.
struct ParsingResult {
    struct Node *nodes; // An array of nodes (preorder traversal).
    size_t nodesCount;
    struct ParsingError *errors;
    size_t errorsCount;
};

enum ParsingAction {
    ERROR, // message, ?next
    GOTO, // next
    BEGIN, // type, next
    END,
};

struct ParsingTransition {
    size_t next;
    enum ParsingAction action;
    char *message;
    enum NodeType type;
};

// typedef struct ParsingTransition (*ParsingTable)[TOKEN_TYPE_COUNT];

// Deallocates a `LexingResult`'s buffers and zeros its memory.
void destroyParsingResult(struct ParsingResult *result);

// Parses an array of tokens into a syntax tree.
struct ParsingResult parse(struct Token *tokens, size_t tokensCount);

#endif // PARSER_H
