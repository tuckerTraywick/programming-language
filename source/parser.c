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
	// TODO: make this more readable. Get the previous node if there is one else get the first node.
	return (SyntaxNode*)parser->nodes.elements + parser->nodes.count - ((parser->nodes.count) ? 1 : 0);
}

static void beginNode(Parser *parser, SyntaxNodeType type) {
	SyntaxNode node = {.type = type, .child = &nullNode};
	ListPushBack(&parser->nodes, &node);
}

static bool consume(Parser *parser, TokenType type) {
	// Return early if the next token doesn't match.
	if (parser->tokenIndex >= parser->tokens.count || ((Token*)ListGet(&parser->tokens, parser->tokenIndex))->type != type) {
		return false;
	}

	SyntaxNode *current = currentNode(parser);
	// If the current node is a parent, add the next node as a child.
	if (current->child == &nullNode) {
		current->child = current + 1;
	// If the current node is not a parent, add the next node as a sibling.
	} else if (current->sibling == &nullNode) {
		current->sibling = current + 1;
	}

	SyntaxNode next = {.type = TOKEN, .sibling = &nullNode};
	ListPushBack(&parser->nodes, &next);
	return true;
}

void ParsingResultDestroy(ParsingResult *result) {
	ListDestroy(&result->nodes);
	ListDestroy(&result->errors);
	*result = (ParsingResult){0};
}

void ParsingResultPrint(ParsingResult *result) {
	static char *nodeTypeNames[] = {
		[INVALID_SYNTAX] = "Invalid syntax.",

		[TOKEN] = "token",
		[PROGRAM] = "program",
		[STATEMENT] = "statement",
		[EXPRESSION] = "expression",
	};
	printf("%zu NODES:\n", result->nodes.count);
	for (size_t i = 0; i < result->nodes.count; ++i) {
		SyntaxNode *node = (SyntaxNode*)ListGet(&result->nodes, i);
		printf(
			"%-5zu %s sibling=%zu, child=%zu\n",
			i,
			nodeTypeNames[node->type],
			node->sibling,
			node->child
		);
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

	consume(&parser, IDENTIFIER);
	consume(&parser, IDENTIFIER);

	return (ParsingResult){.nodes = parser.nodes, .errors = parser.errors};
}

#undef INITIAL_NODE_CAPACITY
#undef INITIAL_ERROR_CAPACITY
