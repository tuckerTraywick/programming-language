#include <stddef.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "list.h"

// The state of the parser.
typedef struct Parser {
	TokenList tokens;
	NodeList nodes;
	NodeList errors;
	Node *currentNode;
	size_t nextTokenIndex;
	size_t nextErrorIndex;
	bool nextNodeIsChild; // Whether the next node to be added should be a child of the previous node.
} Parser;

// The amount of nodes to allocate before parsing.
static const size_t initialNodeCapacity = 1000;

// The amount of errors to allocate before parsing.
static const size_t initialErrorCapacity = 100;

// The name of each type of node.
static char *nodeTypeNames[] = {
	[INVALID_SYNTAX] = "Invalid syntax.",

	[PROGRAM] = "program",
	[STATEMENT] = "statement",
	[PACKAGE_STATEMENT] = "package statement",
	[IMPORT_STATEMENT] = "import statement",
	[VARIABLE_DEFINITION] = "variable definition",
	[STRUCT_DEFINITION] = "struct definition",
	[TYPE] = "type",

	[ATOM] = "atom",
	[EXPRESSION] = "expression",

	[EXPECTED_LINE_END] = "Expected end of line.",
	[MISSING_PACKAGE_NAME] = "Expected a package name.",
	[MISSING_SUBPACKAGE_NAME] = "Expected a subpackage name.",
	[EXPECTED_VARIABLE_NAME] = "Expected a variable name.",
	[EXPECTED_TYPE] = "Expected a type."
};

void ParsingResultDestroy(ParsingResult *result) {
	ListDestroy(&result->nodes);
	ListDestroy(&result->errors);
	*result = (ParsingResult){0};
}

static void printNode(Node *node, size_t level) {
	for (size_t i = 0; i < level; ++i) {
		printf("| ");
	}
	
	if (node->type == TOKEN) {
		printf("token `%.*s`\n", (int)node->tokens[0].length, node->tokens[0].text);
	} else {
		printf("%s\n", nodeTypeNames[node->type]);
		if (node->child) {
			Node *next = node->child;
			while (next) {
				printNode(next, level + 1);
				next = next->next;
			}
		}
	}
}

void ParsingResultPrint(ParsingResult *result) {
	printf("%zu NODES:\n", result->nodes.count);
	printNode(result->nodes.elements, 0);

	printf("\n%zu PARSING ERRORS:\n", result->errors.count);
	for (size_t i = 0; i < result->errors.count; ++i) {
		Node *error = (Node*)ListGet(&result->errors, i);
		printf("%-5zu %s\n", i, nodeTypeNames[error->type]);
	}
}

static void addNode(Parser *parser, Node *node) {
	ListPushBack(&parser->nodes, node);
	Node *newNode = ListBack(&parser->nodes);

	if (parser->nextNodeIsChild) {
		parser->currentNode->child = newNode;
		newNode->parent = parser->currentNode;
	} else {
		parser->currentNode->next = newNode;
		newNode->previous = parser->currentNode;
		newNode->parent = parser->currentNode->parent;
	}
	parser->currentNode = newNode;
	parser->nextNodeIsChild = false;
}

static bool consume(Parser *parser, TokenType type) {
	Token *token = ListGet(&parser->tokens, parser->nextTokenIndex);
	if (!token || token->type != type) {
		return false;
	}
	++parser->nextTokenIndex;

	Node node = {
		.type = TOKEN,
		.tokens = token,
		.tokenCount = 1,
	};
	addNode(parser, &node);
	return true;
}

static bool consumeOperator(Parser *parser, size_t precedence) {
	// Make sure no operator is 0. 0 represents a token not being an operator.
	static size_t infixPrecedences[TOKEN_TYPE_COUNT] = {
		[TIMES] = 100,
		[PLUS] = 200,
	};
	Token *token = ListGet(&parser->tokens, parser->nextTokenIndex);
	if (!token || infixPrecedences[token->type] < precedence) {
		return false;
	}
	++parser->nextTokenIndex;
	return token;
}

static void beginNode(Parser *parser, NodeType type) {
	Node node = {
		.type = type,
		.tokens = ListGet(&parser->tokens, parser->nextTokenIndex),
	};
	addNode(parser, &node);
	parser->nextNodeIsChild = true;
}

static bool endNode(Parser *parser) {
	if (parser->currentNode->parent && !parser->nextNodeIsChild) {
		parser->currentNode = parser->currentNode->parent;
	}
	parser->nextNodeIsChild = false;
	return true;
}

static bool backtrack(Parser *parser) {
	Node *parent = parser->currentNode->parent;
	if (parent->previous) {
		parser->currentNode = parent->previous;
		parser->nextNodeIsChild = false;
	} else {
		parser->currentNode = parent->parent;
		parser->nextNodeIsChild = true;
	}
	return false;
}

static bool parseAtom(Parser *parser) {
	beginNode(parser, ATOM);
	if (!consume(parser, NUMBER)) return backtrack(parser);
	return endNode(parser);
}

static bool parseExpression(Parser *parser, size_t precedence) {
	beginNode(parser, EXPRESSION);
	if (!parseAtom(parser)) return backtrack(parser);
	while (consumeOperator(parser, precedence)) {
		if (!parseExpression(parser, precedence + 1)) return backtrack(parser);
	}
	return endNode(parser);
}

ParsingResult parse(TokenList tokens) {
	Parser parser = {
		.tokens = tokens,
		.nodes = ListCreate(initialNodeCapacity, sizeof (Node)),
		.errors = ListCreate(initialErrorCapacity, sizeof (Node)),
		.currentNode = parser.nodes.elements,
		.nextTokenIndex = 0,
		.nextErrorIndex = 0,
	};
	// TODO: Handle `malloc()` failing.
	// Passing a precedence of 1 so `consumeOperator()` doesn't accidentally consume a normal token.
	beginNode(&parser, PROGRAM);
		parseAtom(&parser);
	endNode(&parser);
	return (ParsingResult){.nodes = parser.nodes, .errors = parser.errors};
}
