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
#define INITIAL_STACK_CAPACITY 100

// The state of the parser.
typedef struct Parser {
	TokenList tokens;
	SyntaxNodeList nodes;
	SyntaxNodeList errors;
	SizeList currentTokenIndexStack;
	SizeList nextNodeIndexStack;
	size_t currentTokenIndex;
	size_t nextNodeIndex;
	size_t previousNodeIndex;
	bool isChild; // Whether the next node created is a child or a sibling.
} Parser;

// Returns a pointer to the next node to be added to the syntax tree.
static SyntaxNode *nextNode(Parser *parser) {
	return (SyntaxNode*)parser->nodes.elements + parser->nextNodeIndex;
}

// Returns a pointer to the previous node added to the syntax tree.
static SyntaxNode *previousNode(Parser *parser) {
	return (parser->nextNodeIndex) ? nextNode(parser) - 1 : NULL;
}

// Adds a node to the parse tree in the appropriate place.
static void addNode(Parser *parser, SyntaxNode *node) {
	// Attach the next previous node to the current node.
	SyntaxNode *previous = previousNode(parser);
	if (previous) {
		if (parser->isChild) {
			previous->child = nextNode(parser);
			parser->isChild = false;
		} else {
			previous->sibling = nextNode(parser);
		}
	}
	ListPushBack(&parser->nodes, node);
	++parser->nextNodeIndex;
}

// Starts a new parent node in the syntax tree.
static void beginNode(Parser *parser, SyntaxNodeType type) {
	ListPushBack(&parser->currentTokenIndexStack, &parser->currentTokenIndex);
	SyntaxNode node = {.type = type};
	addNode(parser, &node);
	ListPushBack(&parser->nextNodeIndexStack, &parser->nextNodeIndex);
	parser->isChild = true;
}

// Ends a node in the syntax tree. Designates the next node as a sibling to the parent node being
// ended.
static bool endNode(Parser *parser) {
	ListPopBack(&parser->currentTokenIndexStack, 1, NULL);
	ListPopBack(&parser->nextNodeIndexStack, 1, NULL);
	parser->isChild = false;
	return true;
}

// Returns a pointer to the current token being parsed.
static Token *currentToken(Parser *parser) {
	if (parser->currentTokenIndex < parser->tokens.count) {
		return (Token*)parser->tokens.elements + parser->currentTokenIndex;
	} else {
		return NULL;
	}
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
	SyntaxNode next = {.type = TOKEN, .token = token};
	addNode(parser, &next);
	return true;
}

// Adds an error of the given type to the syntax tree and ends the current parent node.
static bool error(Parser *parser, SyntaxNodeType type) {
	SyntaxNode next = {.type = type};
	addNode(parser, &next);
	return true;
}

// Keeps consuming tokens until it consumes one of the given type.
static bool recover(Parser *parser, SyntaxNodeType type) {
	return false;
}

// Cancels parsing the current parent node and removes it and all of its children from the parse
// tree.
static bool backtrack(Parser *parser) {
	ListPopBack(&parser->currentTokenIndexStack, 1, &parser->currentTokenIndex);
	size_t previousIndex = parser->nextNodeIndex;
	ListPopBack(&parser->nextNodeIndexStack, 1, &parser->nextNodeIndex);
	ListPopBack(&parser->nodes, previousIndex - parser->nextNodeIndex, NULL);
	return false;
}

static bool parseProgram(Parser *parser) {
	beginNode(parser, PACKAGE_STATEMENT);
		consume(parser, PUB);
		if (!consume(parser, PACKAGE)) return backtrack(parser);
		if (!consume(parser, IDENTIFIER)) return error(parser, MISSING_PACKAGE_NAME) && endNode(parser);
		while (consume(parser, DOT)) {
			if (consume(parser, TIMES)) break;
			if (!consume(parser, IDENTIFIER)) return error(parser, MISSING_SUBPACKAGE_NAME) && endNode(parser);
		}
		if (!consume(parser, SEMICOLON)) error(parser, EXPECTED_SEMICOLON);
	return endNode(parser);
}

void ParsingResultDestroy(ParsingResult *result) {
	ListDestroy(&result->nodes);
	ListDestroy(&result->errors);
	*result = (ParsingResult){0};
}

void ParsingResultPrint(ParsingResult *result) {
	static char *nodeTypeNames[] = {
		[INVALID_SYNTAX] = "Invalid syntax.",

		[PROGRAM] = "program",
		[STATEMENT] = "statement",
		[PACKAGE_STATEMENT] = "package statement",
		[EXPRESSION] = "expression",

		[MISSING_PACKAGE_NAME] = "Missing package name.",
		[MISSING_SUBPACKAGE_NAME] = "Missing subpackage name.",
		[EXPECTED_SEMICOLON] = "Expected a semicolon.",
	};
	printf("%zu NODES:\n", result->nodes.count);
	for (size_t i = 0; i < result->nodes.count; ++i) {
		SyntaxNode *node = (SyntaxNode*)ListGet(&result->nodes, i);
		if (node->type == TOKEN) {
			printf(
				"%-5zu token `%.*s` sibling=%zu\n",
				i,
				(int)node->token->length,
				node->token->text,
				(node->sibling) ? node->sibling - (SyntaxNode*)result->nodes.elements : 0
			);
		} else {
			printf(
				"%-5zu %s sibling=%zu, child=%zu\n",
				i,
				nodeTypeNames[node->type],
				(node->sibling) ? node->sibling - (SyntaxNode*)result->nodes.elements : 0,
				(node->child) ? node->child - (SyntaxNode*)result->nodes.elements : 0
			);
		}
	}
}

ParsingResult parse(TokenList tokens) {
	SyntaxNodeList nodes = ListCreate(INITIAL_NODE_CAPACITY, sizeof (SyntaxNode));
	SyntaxNodeList errors = ListCreate(INITIAL_ERROR_CAPACITY, sizeof (SyntaxNode));
	SizeList currentTokenIndexStack = ListCreate(INITIAL_STACK_CAPACITY, sizeof (size_t));
	SizeList nextNodeIndexStack = ListCreate(INITIAL_STACK_CAPACITY, sizeof (size_t));
	Parser parser = {
		.tokens = tokens,
		.nodes = nodes,
		.errors = errors,
		.currentTokenIndexStack = currentTokenIndexStack,
		.nextNodeIndexStack = nextNodeIndexStack,
	};

	parseProgram(&parser);
	parseProgram(&parser);

	ListDestroy(&currentTokenIndexStack);
	ListDestroy(&nextNodeIndexStack);
	return (ParsingResult){.nodes = parser.nodes, .errors = parser.errors};
}

#undef INITIAL_NODE_CAPACITY
#undef INITIAL_ERROR_CAPACITY
#undef INITIAL_STACK_CAPACITY
