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
    struct Node *nodes; // An array of nodes (preorder traversal). The first node is the root node of the tree.
    size_t nodesCount;
    struct ParsingError *errors;
    size_t errorsCount;
};

// Represents an entry in the parsing state transition table.
struct ParsingTransition {
    size_t call; // State to call like a function.
    size_t jump; // State to jump directly to, when both `call` and `jump` are > 0, pushed to stack before calling.
    enum NodeType type; // The type of node to begin.
    char *errorMessage; // Error message to produce.
};

// Represents a state transition table for the parser.
typedef struct ParsingTransition (*ParsingTable)[TOKEN_TYPE_COUNT];

// Deallocates a `LexingResult`'s buffers and zeros its memory.
void destroyParsingResult(struct ParsingResult *result);

// Parses an array of tokens into a syntax tree.
struct ParsingResult parse(struct Token *tokens, size_t tokensCount);

#endif // PARSER_H
