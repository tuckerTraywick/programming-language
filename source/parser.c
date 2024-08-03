#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

typedef List SizeList;

// The inital amount of nodes to allocate for the parse tree.
#define INITIAL_NODE_CAPACITY 500

// The inital amount of error nodes to allocate.
#define INITIAL_ERROR_CAPACITY 100

// The initial recursion depth limit.
#define INITIAL_STACK_CAPACITY 200

// The state of the parser.
typedef struct Parser {
	TokenList tokens;
	SyntaxNodeList nodes;
	SyntaxNodeList errors;
	SizeList currentTokenIndexStack;
	SizeList currentNodeIndexStack;
	size_t currentTokenIndex;
	size_t currentNodeIndex;
	bool isChild; // Whether the next node created is a child or a sibling.
} Parser;

// Starts a new parent node in the syntax tree.
static void beginNode(Parser *parser, SyntaxNodeType type) {
	parser->isChild = true;
	if (parser->nodes.count) {
		++parser->currentNodeIndex;
	}
	
	SyntaxNode node = {.type = type};
	ListPushBack(&parser->nodes, &node);
	ListPushBack(&parser->currentTokenIndexStack, &parser->currentTokenIndex);
	ListPushBack(&parser->currentNodeIndexStack, &parser->currentNodeIndex);
}

// Ends a node in the syntax tree. Designates the next node as a sibling to the parent node being
// ended.
static bool endNode(Parser *parser) {
	ListPopBack(&parser->currentTokenIndexStack, 1, NULL);
	ListPopBack(&parser->currentNodeIndexStack, 1, &parser->currentNodeIndex);
	return true;
}

// Returns a pointer to the current token being parsed.
static Token *currentToken(Parser *parser) {
	if (parser->currentTokenIndex < parser->tokens.count) {
		return (Token*)ListGet(&parser->tokens, parser->currentTokenIndex);
	} else {
		return NULL;
	}
}

// Returns a pointer to the last node added to the syntax tree.
static SyntaxNode *currentNode(Parser *parser) {
	if (parser->currentNodeIndex < parser->nodes.count) {
		return (SyntaxNode*)ListGet(&parser->nodes, parser->currentNodeIndex);
	} else {
		return NULL;
	}
}

// Adds a node to the parse tree in the appropriate place.
static void addNode(Parser *parser, SyntaxNode *node) {
	ListPushBack(&parser->nodes, node);

	// Attach the next node to the current node.
	SyntaxNode *current = currentNode(parser);
	assert(current != NULL && "Can't consume a token without a root node.");
	if (parser->isChild) {
		current->child = (SyntaxNode*)ListGet(&parser->nodes, parser->nodes.count - 1);
	} else {
		current->sibling = (SyntaxNode*)ListGet(&parser->nodes, parser->nodes.count - 1);
	}

	parser->currentNodeIndex = parser->nodes.count - 1;
}

// If the next token matches the given type, advances past it and adds it to the syntax tree.
static bool consume(Parser *parser, TokenType type) {
	Token *token = currentToken(parser);
	// Return early if the next token doesn't match.
	if (!token || token->type != type) {
		return false;
	}
	++parser->currentTokenIndex;

	// Append the next node.
	SyntaxNode next = {.type = (SyntaxNodeType)token->type};
	addNode(parser, &next);
	return true;
}

// static bool parseProgram(Parser *parser) {
// 	beginNode(parser, PROGRAM);
// 		consume(parser, PUB);
// 		if (!consume(parser, PACKAGE)) return backtrack(parser);
// 		if (!consume(parser, IDENTIFIER)) return error(parser, MISSING_PACKAGE_NAME);
// 		while (consume(parser, DOT)) {
// 			if (consume(parser, DOT)) break;
// 			if (!consume(parser, IDENTIFIER)) return error(parser, MISSING_SUBPACKAGE_NAME);
// 		}
// 		if (!consume(parser, SEMICOLON)) return recover(parser, EXPECTED_SEMICOLON, SEMICOLON);
// 	return endNode(parser);
// }

void ParsingResultDestroy(ParsingResult *result) {
	ListDestroy(&result->nodes);
	ListDestroy(&result->errors);
	*result = (ParsingResult){0};
}

void ParsingResultPrint(ParsingResult *result) {
	static char *nodeTypeNames[] = {
		[IDENTIFIER] = "identifier",

		[INVALID_SYNTAX] = "Invalid syntax.",

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
			(node->sibling) ? node->sibling - (SyntaxNode*)result->nodes.elements : 0,
			(node->child) ? node->child - (SyntaxNode*)result->nodes.elements : 0
		);
	}
}

ParsingResult parse(TokenList tokens) {
	SyntaxNodeList nodes = ListCreate(INITIAL_NODE_CAPACITY, sizeof (SyntaxNode));
	SyntaxNodeList errors = ListCreate(INITIAL_ERROR_CAPACITY, sizeof (SyntaxNode));
	SizeList currentTokenIndexStack = ListCreate(INITIAL_STACK_CAPACITY, sizeof (size_t));
	SizeList currentNodeIndexStack = ListCreate(INITIAL_STACK_CAPACITY, sizeof (size_t));
	Parser parser = {
		.tokens = tokens,
		.nodes = nodes,
		.errors = errors,
		.currentTokenIndexStack = currentTokenIndexStack,
		.currentNodeIndexStack = currentNodeIndexStack,
	};

	beginNode(&parser, PROGRAM);
		consume(&parser, IDENTIFIER);
		consume(&parser, IDENTIFIER);
	endNode(&parser);

	ListDestroy(&currentTokenIndexStack);
	ListDestroy(&currentNodeIndexStack);
	return (ParsingResult){.nodes = parser.nodes, .errors = parser.errors};
}

#undef INITIAL_NODE_CAPACITY
#undef INITIAL_ERROR_CAPACITY
#undef INITIAL_STACK_CAPACITY
