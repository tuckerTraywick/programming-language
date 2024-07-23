#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "lexer.h"

typedef enum SyntaxNodeType {
	SYNTAX_ERROR,
	PROGRAM,
	STATEMENT,
	EXPRESSION,
} SyntaxNodeType;

typedef struct SyntaxNode {
	SyntaxNodeType type;
	struct SyntaxNode *children;
	size_t childrenCount;
	Token *tokens;
	size_t tokensCount;
} SyntaxNode;

typedef struct SyntaxTree {
	SyntaxNode *nodes;
	size_t nodesCapacity;
	size_t nodesCount;
} SyntaxTree;

SyntaxTree SyntaxTreeCreate(size_t capacity);

void SyntaxTreeDestroy(SyntaxTree *tree);

SyntaxTree parse(TokenList tokens);

#endif // PARSER_H
