#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "lexer.h"
#include "list.h"

typedef List SyntaxNodeList;

// The type of thing in the grammar a node represents.
typedef enum SyntaxNodeType {
	INVALID_SYNTAX,
	MISSING_PACKAGE_NAME,
	MISSING_SUBPACKAGE_NAME,
	EXPECTED_SEMICOLON,

	TOKEN,
	PROGRAM,
	STATEMENT,
	EXPRESSION,

	STYNAX_NODE_TYPE_COUNT,
} SyntaxNodeType;

// A node in the abstract syntax tree.
typedef struct SyntaxNode {
	SyntaxNodeType type;
	struct SyntaxNode *sibling;
	struct SyntaxNode *child;
	Token *token; // NULL if the node doesn't represent a token.
} SyntaxNode;

// The output of the parser. Must be destroyed by `ParsingResultDestroy()` after use.
typedef struct ParsingResult {
	SyntaxNodeList nodes;
	SyntaxNodeList errors;
} ParsingResult;

// Frees a `ParsingResult`'s lists and zeros its memory.
void ParsingResultDestroy(ParsingResult *result);

// Pretty prints a `ParsingResult` to the terminal.
void ParsingResultPrint(ParsingResult *result);

// Turns a list of tokens into an abstract syntax tree.
ParsingResult parse(TokenList tokens);

#endif // PARSER_H
