#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "lexer.h"
#include "list.h"

typedef List NodeList;

// The type of thing in the grammar a node represents.
typedef enum NodeType {
	INVALID_SYNTAX,
	EXPECTED_LINE_END,
	MISSING_PACKAGE_NAME,
	MISSING_SUBPACKAGE_NAME,
	EXPECTED_VARIABLE_NAME,
	EXPECTED_TYPE,

	TOKEN,
	PROGRAM,
	STATEMENT,
	PACKAGE_STATEMENT,
	IMPORT_STATEMENT,
	VARIABLE_DEFINITION,
	STRUCT_DEFINITION,
	TYPE,

	ATOM,
	EXPRESSION,

	STYNAX_NODE_TYPE_COUNT,
} NodeType;

// An element in an abstract syntax tree.
typedef struct Node {
	NodeType type;
	struct Node *parent;
	struct Node *child;
	struct Node *next;
	struct Node *previous;
	Token *tokens;
	size_t tokenCount;
} Node;

// The output of the parser. Must be destroyed by `ParsingResultDestroy()` after use.
typedef struct ParsingResult {
	NodeList nodes;
	NodeList errors;
} ParsingResult;

// Frees a `ParsingResult`'s lists and zeros its memory.
void ParsingResultDestroy(ParsingResult *result);

// Pretty prints a `ParsingResult` to the terminal.
void ParsingResultPrint(ParsingResult *result);

// Turns a list of tokens into an abstract syntax tree.
ParsingResult parse(TokenList tokens);

#endif // PARSER_H
