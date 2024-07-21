#include <stdbool.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"

#define DEFAULT_NODE_CAPACITY 1000
#define INDEX_STACK_SIZE 2000

typedef struct Parser {
	TokenList tokens;
	SyntaxTree tree;
	size_t *tokenIndexStack;
	size_t *tokenIndex;
	size_t *nodeIndexStack;
	size_t *nodeIndex;
} Parser;

static Parser ParserCreate(TokenList tokens) {
	return (Parser){
		.tokens = tokens,
		.tree = SyntaxTreeCreate(DEFAULT_NODE_CAPACITY),
		.tokenIndexStack = malloc(INDEX_STACK_SIZE*sizeof (size_t)),
		.nodeIndexStack = malloc(INDEX_STACK_SIZE*sizeof (size_t)),
	};
}

static void ParserDestroy(Parser *parser) {
	free(parser->tokenIndexStack);
	free(parser->nodeIndexStack);
	*parser = (Parser){0};
}

static void beginNode(Parser *parser, SyntaxNodeType type) {
	// TODO: Make sure the index stacks don't overflow.
	parser->tree.nodes[*parser->nodeIndex] = (SyntaxNode){
		.type = type,
		.children = parser->nodeIndex + 1,
		.childrenCount = 0,
	};
	++parser->nodeIndex;
}

static bool endNode(Parser *parser) {
	--parser->nodeIndex;
	return true;
}

static bool backtrack(Parser *parser) {
	return false;
}

static bool parseProgram(Parser *parser) {
	beginNode(parser, PROGRAM);
	if (!consume(parser, NUMBER)) return backtrack(parser);
	if (!consume(parser, OPERATOR)) return backtrack(parser);
	if (!consume(parser, NUMBER)) return backtrack(parser);
	return endNode(parser);
}

SyntaxTree SyntaxTreeCreate(size_t capacity) {
	SyntaxNode *nodes = malloc(capacity*sizeof (SyntaxNode));
	return (SyntaxTree){
		.capacity = capacity,
		.nodes = nodes,
	};
}

void SyntaxTreeDestroy(SyntaxTree *tree) {
	free(tree->nodes);
	*tree = (SyntaxTree){0};
}

SyntaxTree parse(TokenList tokens) {
	Parser parser = ParserCreate(tokens);
	parseProgram(&parser);
	ParserDestroy(&parser);
}

#undef DEFAULT_NODE_CAPACITY
#undef INDEX_STACK_SIZE
