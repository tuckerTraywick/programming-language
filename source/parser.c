#include <assert.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"

// The inital amount of nodes to allocate for the parse tree.
#define INITIAL_NODE_CAPACITY 500

// The inital amount of error nodes to allocate.
#define INITIAL_ERROR_CAPACITY 100

// The parser sets the current node's sibling or child pointer to this node depending on whether the
// next node is to be a sibling or child of the current node.
static SyntaxNode nullNode = {0};

// The state of the parser.
typedef struct Parser {
	TokenList tokens;
	size_t tokenIndex;
	SyntaxNodeList nodes;
	SyntaxNodeList errors;
} Parser;

static SyntaxNode *currentNode(Parser *parser) {
	return (SyntaxNode*)((char*)parser->nodes.elements + parser->nodes.count);
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
	// If the current node is not a parent, add the next node as a sibling.
	} else if (current->sibling == &nullNode) {
		current->sibling = ListGet(&parser->nodes, parser->nodes.count - 1);
	} else {
		assert(false && "The previous node must have either `child` or `sibling` set to the null node.");
	}

	return true;
}

void ParsingResultDestroy(ParsingResult *result) {
	ListDestroy(&result->nodes);
	ListDestroy(&result->errors);
	*result = (ParsingResult){0};
}

void ParsingResultPrint(ParsingResult *result) {
	printf("%zu NODES:\n", result->nodes.count);
	for (size_t i = 0; i < result->nodes.count; ++i) {
		printf("");
	}
}

ParsingResult parse(TokenList tokens) {
	SyntaxNodeList nodes = ListCreate(INITIAL_NODE_CAPACITY, sizeof (SyntaxNode));
	SyntaxNodeList errors = ListCreate(INITIAL_ERROR_CAPACITY, sizeof (SyntaxNode));
	Parser parser = {
		.tokens = tokens,
		.nodes = nodes,
		.errors = errors,
	};
	return (ParsingResult){.nodes = nodes, .errors = errors};
}

#undef INITIAL_NODE_CAPACITY
#undef INITIAL_ERROR_CAPACITY
