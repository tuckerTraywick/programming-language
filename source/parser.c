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
	uint32_t current_node_index;
	bool current_node_is_parent;
} Parser;

static Token *current_token(Parser *parser) {
	if (parser->current_token_index >= arena_get_size(parser->tokens)/sizeof *parser->tokens) {
		return NULL;
	}
	return parser->tokens + parser->current_token_index;
}

static Node *current_node(Parser *parser) {
	return parser->nodes + parser->current_node_index;
}

static uint32_t last_node_index(Parser *parser) {
	return arena_get_size(parser->nodes)/sizeof *parser->nodes - 1;
}

static Node *last_node(Parser *parser) {
	return parser->nodes + last_node_index(parser);
}

// Appends a node to the list of nodes without modifying the current parent index.
static Node *add_node(Parser *parser, Node_Type type) {
	// TODO: Handle null return value.
	Node *new_node = arena_allocate(parser->nodes, sizeof *new_node);
	*new_node = (Node){
		.type = type,
	};
	if (parser->current_node_is_parent) {
		current_node(parser)->child_index = last_node_index(parser);
		new_node->parent_index = parser->current_node_index;
		parser->current_node_is_parent = false;
	} else {
		current_node(parser)->next_index = last_node_index(parser);
		new_node->parent_index = current_node(parser)->parent_index;
	}
	parser->current_node_index = last_node_index(parser);
	return new_node;
}

static void begin_node(Parser *parser, Node_Type type) {
	add_node(parser, type);
	parser->current_node_is_parent = true;
}

static bool end_node(Parser *parser) {
	parser->current_node_index = current_node(parser)->parent_index;
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
	Node *new_node = add_node(parser, NODE_TYPE_TOKEN);
	new_node->child_index = parser->current_token_index;
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
	parser.current_node_is_parent = true;

	parse_token(&parser, TOKEN_TYPE_NUMBER);
	begin_node(&parser, NODE_TYPE_PROGRAM);
	begin_node(&parser, NODE_TYPE_PROGRAM);
	parse_token(&parser, TOKEN_TYPE_NUMBER);
	parse_token(&parser, TOKEN_TYPE_NUMBER);
	end_node(&parser);
	end_node(&parser);
	parse_token(&parser, TOKEN_TYPE_NUMBER);

	Parser_Result result = {
		.nodes = parser.nodes,
		.errors = parser.errors,
	};
	return result;
}
