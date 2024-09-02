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
	EXPECTED_LINE_END,

	END, // Indicates the end of a subtree.

	TOKEN,
	PROGRAM,
	STATEMENT,
	PACKAGE_STATEMENT,
	EXPRESSION,
	BASIC_EXPRESSION,
	INFIX_EXPRESSION,
	PREFIX_EXPRESSION,

	STYNAX_NODE_TYPE_COUNT,
} SyntaxNodeType;

// An element in an abstract syntax tree.
typedef struct SyntaxNode {
	SyntaxNodeType type;
	Token *token;
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
