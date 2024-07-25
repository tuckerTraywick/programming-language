#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "lexer.h"
#include "list.h"

typedef List SyntaxNodeList;

typedef List ParsingErrorList;

typedef enum SyntaxNodeType {
	PARSING_ERROR = OPERATOR + 1,
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

// A sentinal value representing a pointer to be filled by the next node parsed.
extern SyntaxNode nullNode = {.type = PARSING_ERROR, .child = NULL, .sibling = NULL};

ParsingResult parse(TokenList tokens);

#endif // PARSER_H
