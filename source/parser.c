#include <assert.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"

#define INITIAL_NODE_CAPACITY 500

SyntaxNode nullNode = {0};

typedef struct Parser {
	TokenList tokens;
	size_t tokenIndex;
	SyntaxNodeList nodes;
	ParsingErrorList errors;
} Parser;

static SyntaxNode *currentNode(Parser *parser) {
	assert(parser->nodes.count > 1 && "Can't get current node if the tree is empty.");
	return ListGet(&parser->nodes, parser->nodes.count - 1);
}

static void beginNode(Parser *parser, SyntaxNodeType type) {
	SyntaxNode node = {.type = type, .child = &nullNode};
	ListPushBack(&parser->nodes, &node);
}

static bool consume(Parser *parser, TokenType type) {
	SyntaxNode *current = currentNode(parser);
	// Return early if the next token doesn't match.
	if (parser->tokenIndex >= parser->tokens.count || ((Token*)ListGet(&parser->tokens, parser->tokenIndex))->type != type) {
		return false;
	}

	SyntaxNode next = {.type = (SyntaxNodeType)type, .sibling = &nullNode};
	ListPushBack(&parser->nodes, &next);
	// If the current node is a parent, add the next node as a child.
	if (current->child == &nullNode) {
		current->child = ListGet(&parser->nodes, parser->nodes.count - 1);
	} else if (current->sibling == &nullNode) {
		current->sibling = ListGet(&parser->nodes, parser->nodes.count - 1);
	} else {
		assert(false && "The previous node must have either `child` or `sibling` set to the null node.");
	}

	return true;
}

ParsingResult parse(TokenList tokens) {
	SyntaxNodeList nodes = ListCreate(INITIAL_NODE_CAPACITY, sizeof (SyntaxNode));
	ParsingErrorList errors = ListCreate(INITIAL_NODE_CAPACITY, sizeof (ParsingError));
	Parser parser = {
		.tokens = tokens,
		.nodes = nodes,
		.errors = errors,
	};
}

#undef INITIAL_NODE_CAPACITY
