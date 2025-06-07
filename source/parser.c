#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"
#include "arena.h"

#define STARTING_NODE_CAPACITY 2000

#define STARTING_PARSER_ERROR_CAPACITY 100

// The state passed between parsing functions.
typedef struct Parser {
	Token *tokens;
	Node *nodes;
	Parser_Error *errors;
	uint32_t current_token_index;
	uint32_t current_parent_index;
} Parser;

static Token *current_token(Parser *parser) {
	if (parser->current_token_index >= arena_get_size(parser->tokens)/sizeof *parser->tokens) {
		return NULL;
	}
	return parser->tokens + parser->current_token_index;
}

static Node *current_parent(Parser *parser) {
	return parser->nodes + parser->current_parent_index;
}

static uint32_t last_node_index(Parser *parser) {
	return arena_get_size(parser->nodes)/sizeof *parser->nodes - 1;
}

static Node *last_node(Parser *parser) {
	return parser->nodes + last_node_index(parser);
}

// Appends a node to the list of nodes without modifying the current parent index.
static Node *add_node(Parser *parser, Node *node) {
	Node *previous_node = last_node(parser);
	// TODO: Handle null return value.
	Node *new_node = arena_push(parser->nodes, node, sizeof *node);
	if (previous_node == current_parent(parser)) {
		previous_node->child_index = last_node_index(parser);
	} else {
		previous_node->next_index = last_node_index(parser);
	}
	return new_node;
}

static void begin_node(Parser *parser, Node_Type type) {
	Node new_node = {
		.type = type,
		.parent_index = parser->current_parent_index,
	};
	add_node(parser, &new_node);
	parser->current_parent_index = last_node_index(parser);
}

static bool end_node(Parser *parser) {
	parser->current_parent_index = last_node(parser)->parent_index;
	return true;
}

static bool peek_token(Parser *parser, Token_Type type) {
	Token *token = current_token(parser);
	return token != NULL && token->type == type;
}

static bool parse_token(Parser *parser, Token_Type type) {
	if (!peek_token(parser, type)) {
		return false;
	}
	Node node = {
		.type = NODE_TYPE_TOKEN,
		.child_index = parser->current_token_index,
	};
	add_node(parser, &node);
	++parser->current_token_index;
	return true;
}

static bool parse_expression(Parser *parser);

char *node_type_names[] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
};

void Parser_Result_destroy(Parser_Result *result) {
	arena_destroy(result->nodes);
	arena_destroy(result->errors);
	*result = (Parser_Result){0};
}

Parser_Result parse(Token *tokens) {
	Parser parser = {
		.tokens = tokens,
		.nodes = arena_create(STARTING_NODE_CAPACITY*sizeof (Node)),
		.errors = arena_create(STARTING_PARSER_ERROR_CAPACITY*sizeof (Parser_Error)),
	};

	arena_allocate(parser.nodes, sizeof *parser.nodes);
	parser.nodes[0] = (Node){
		.type = NODE_TYPE_PROGRAM,
	};

	parse_token(&parser, TOKEN_TYPE_NUMBER);

	Parser_Result result = {
		.nodes = parser.nodes,
		.errors = parser.errors,
	};
	return result;
}
