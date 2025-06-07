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
	if (parser->current_token_index > arena_get_size(parser->tokens)/sizeof *parser->tokens) {
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

char *node_type_names[] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
};

void Parser_Result_destroy(Parser_Result *result) {
	arena_destroy(result->nodes);
	arena_destroy(result->errors);
	*result = (Parser_Result){0};
}

void begin_node(Parser *parser, Node_Type type) {
	uint32_t last_index = last_node_index(parser);
	Node *last_node = parser->nodes + last_index;
	// TODO: Handle null return value.
	Node *new_node = arena_allocate(parser->nodes, sizeof *new_node);
	*new_node = (Node){
		.parent_index = parser->current_parent_index,
	};
	
	if (parser->current_parent_index == last_index) {
		last_node->child_index = last_node_index(parser);
	} else {
		last_node->next_index = last_node_index(parser);
	}
	parser->current_parent_index = last_node_index(parser);
}

bool end_node(Parser *parser);

bool peek_token(Parser *parser);

bool parse_token(Parser *parser);

bool parse_expression(Parser *parser);

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

	begin_node(&parser, NODE_TYPE_TOKEN);

	Parser_Result result = {
		.nodes = parser.nodes,
		.errors = parser.errors,
	};
	return result;
}
