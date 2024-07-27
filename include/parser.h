#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "lexer.h"
#include "list.h"

typedef List SyntaxNodeList;

typedef List ParsingErrorList;

typedef enum SyntaxNodeType {
	PROGRAM,
	STATEMENT,
	EXPRESSION,
} SyntaxNodeType;

typedef struct SyntaxNode {
	SyntaxNodeType type;
	struct SyntaxNode *sibling;
	struct SyntaxNode *child;
} SyntaxNode;

typedef struct ParsingError {
	char *message;
	Token *tokens;
	size_t tokensCount;
} ParsingError;

typedef struct ParsingResult {
	SyntaxNodeList nodes;
	ParsingErrorList errors;
} ParsingResult;

// The parser sets the current node's child or sibling to the address of this variable depending on
// whether the node is a parent or a child.
extern SyntaxNode nullNode;

ParsingResult parse(TokenList tokens);

#endif // PARSER_H
