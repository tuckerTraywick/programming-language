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
} Parser;

// Adds a node to the parse tree.
static void addNode(Parser *parser, SyntaxNode *node) {
	ListPushBack(&parser->nodes, node);
	++parser->nextNodeIndex;
}

// Starts a new parent node in the syntax tree.
static void beginNode(Parser *parser, SyntaxNodeType type) {
	ListPushBack(&parser->currentTokenIndexStack, &parser->currentTokenIndex);
	ListPushBack(&parser->nextNodeIndexStack, &parser->nextNodeIndex);
	SyntaxNode node = {.type = type};
	addNode(parser, &node);
}

// Ends a node in the syntax tree. Designates the next node as a sibling to the parent node being
// ended.
static bool endNode(Parser *parser) {
	ListPopBack(&parser->currentTokenIndexStack, 1, NULL);
	ListPopBack(&parser->nextNodeIndexStack, 1, NULL);
	SyntaxNode node = {.type = END};
	addNode(parser, &node);
	return true;
}

// Returns true if the parser hasn't reached the end of the token stream.
static bool hasTokens(Parser *parser) {
	return parser->currentTokenIndex < parser->tokens.count;
}

// Returns a pointer to the current token being parsed.
static Token *currentToken(Parser *parser) {
	if (hasTokens(parser)) {
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
static bool recover(Parser *parser, TokenType type) {
	while (hasTokens(parser) && currentToken(parser)->type != type) {
		++parser->currentTokenIndex;
	}

	if (hasTokens(parser) && currentToken(parser)->type == type) {
		++parser->currentTokenIndex;
	}
	return true;
}
// Keeps consuming tokens until it sees one of the given type.
static bool recoverUntil(Parser *parser, TokenType type) {
	while (hasTokens(parser) && currentToken(parser)->type != type) {
		++parser->currentTokenIndex;
	}
	return true;
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
		if (!consume(parser, IDENTIFIER)) error(parser, MISSING_PACKAGE_NAME);
		while (consume(parser, DOT)) {
			if (consume(parser, TIMES)) break;
			if (!consume(parser, IDENTIFIER)) error(parser, MISSING_SUBPACKAGE_NAME);
		}
		if (!consume(parser, SEMICOLON)) error(parser, EXPECTED_SEMICOLON) && recover(parser, SEMICOLON);
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

		[END] = "end",

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
				"%-5zu token `%.*s`\n",
				i,
				(int)node->token->length,
				node->token->text
			);
		} else {
			printf(
				"%-5zu %s\n",
				i,
				nodeTypeNames[node->type]
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
