#ifndef PARSER_H
#define PARSER_H

#include <stddef.h> // size_t
#include "lexer.h"

// Represents the type of a node.
enum NodeType {
    INVALID_NODE,
    PACKAGE_STATEMENT_EXPECTED_PACKAGE_NAME,
    
    TOKEN,
    PROGRAM,
    PACKAGE_STATEMENT,
    IMPORT_STATEMENT,
};

// Represents a node in a syntax tree.
struct Node {
    enum NodeType type;
    struct Node *child;
    struct Node *sibling;
    struct Token *tokens; // Points to the first token parsed to create the node. Unowned, don't free.
    size_t tokensCount;
};

// Represents the result of parsing an array of tokens.
struct ParsingResult {
    struct Node *nodes; // The preorder traversal of the parse tree. The first node is the root.
    size_t nodesCount;
    struct Node *errors;
    size_t errorsCount;
};

// Deallocates a `LexingResult`'s buffers and zeros its memory.
void destroyParsingResult(struct ParsingResult *result);

// Parses an array of tokens into a syntax tree.
struct ParsingResult parse(struct Token *tokens, size_t tokensCount);

#endif // PARSER_H
