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
    PROGRAM,
    PACKAGE_STATEMENT,
    IMPORT_STATEMENT,
};

// Represents a node in a syntax tree.
struct Node {
    enum NodeType type;
    struct Node *children; // The children are allocated in one chunk. Unowned, don't free.
    size_t childrenCount;
    struct Token *tokens; // Points to the first token parsed to create the node. Unowned, don't free.
    size_t tokensCount;
};

// Represents the result of parsing an array of tokens.
struct ParsingResult {
    struct Node *nodes; // An array of all of the nodes in breadth-first order, also a pointer to the topmost node in the tree.
    size_t nodesCount;
    struct ParsingError *errors;
    size_t errorsCount;
};

// Parses an array of tokens into a syntax tree.
struct ParsingResult parse(struct Token *tokens, size_t tokensCount);

#endif // PARSER_H
