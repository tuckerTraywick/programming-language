#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"
#include "list.h"

#define STARTING_NODE_CAPACITY 1

#define STARTING_PARSER_ERROR_CAPACITY 1

// The state passed between parsing functions.
typedef struct Parser {
	Token *tokens;
	Node *nodes;
	Parser_Error *errors;
	uint32_t current_token_index;
	uint32_t current_node_index;
	bool current_node_is_parent;
} Parser;

static uint32_t prefix_precedences[] = {
	[TOKEN_TYPE_BOOLEAN_NOT] = 0,
	[TOKEN_TYPE_PLUS] = 0,
	[TOKEN_TYPE_MINUS] = 0,
	[TOKEN_TYPE_TIMES] = 0,
	[TOKEN_TYPE_BITWISE_NOT] = 0,
};

static Token *current_token(Parser *parser) {
	if (parser->current_token_index >= list_get_size(parser->tokens)) {
		return NULL;
	}
	return parser->tokens + parser->current_token_index;
}

static Node *current_node(Parser *parser) {
	return parser->nodes + parser->current_node_index;
}

static uint32_t last_node_index(Parser *parser) {
	return list_get_size(parser->nodes) - 1;
}

// Appends a node to the list of nodes without modifying the current parent index.
static Node *add_node(Parser *parser, Node_Type type) {
	Node new_node = {
		.type = type,
	};
	// TODO: Handle null return value.
	parser->nodes = list_push(parser->nodes, &new_node);
	if (parser->current_node_is_parent) {
		current_node(parser)->child_index = last_node_index(parser);
		list_get_last(parser->nodes)->parent_index = parser->current_node_index;
		parser->current_node_is_parent = false;
	} else {
		current_node(parser)->next_index = last_node_index(parser);
		list_get_last(parser->nodes)->parent_index = current_node(parser)->parent_index;
	}
	parser->current_node_index = last_node_index(parser);
	return list_get_last(parser->nodes);
}

static void begin_node(Parser *parser, Node_Type type) {
	add_node(parser, type);
	parser->current_node_is_parent = true;
}

static bool end_node(Parser *parser) {
	parser->current_node_index = current_node(parser)->parent_index;
	return true;
}

static bool emit_error(Parser *parser, Parser_Error_Type type) {
	// TODO: Count error tokens.
	Parser_Error error = {
		.type = type,
		.token_index = parser->current_token_index,
		.token_count = 0,
	};
	// TODO: Handle null return value.
	parser->errors = list_push(parser->errors, &error);
	return false;
}

static bool peek_token(Parser *parser, Token_Type type) {
	Token *token = current_token(parser);
	return token != NULL && token->type == type;
}

static bool peek_prefix_operator(Parser *parser) {
	Token *token = current_token(parser);
	// return token != NULL && token->type >= 
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

static bool parse_prefix(Parser *parser) {
	if (peek_prefix_operator(parser)) {

	}
}

static void parse_program(Parser *parser) {
	parse_token(parser, TOKEN_TYPE_PUB);
	if (!parse_token(parser, TOKEN_TYPE_MODULE)) return;
	if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME);
}

char *node_type_names[] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
};

char *parser_error_messages[] = {
	[PARSER_ERROR_TYPE_INVALID_SYNTAX] = "Invalid syntax.",
	[PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME] = "Expected a module name.",
};

void Parser_Result_destroy(Parser_Result *result) {
	list_destroy(result->nodes);
	list_destroy(result->errors);
	*result = (Parser_Result){0};
}

Parser_Result parse(Token *tokens) {
	Parser parser = {
		.tokens = tokens,
		.nodes = list_create(STARTING_NODE_CAPACITY, sizeof (Node)),
		.errors = list_create(STARTING_PARSER_ERROR_CAPACITY, sizeof (Parser_Error)),
	};

	Node node = {
		.type = NODE_TYPE_PROGRAM,
	};
	// TODO: Handle null return value.
	parser.nodes = list_push(parser.nodes, &node);
	parser.current_node_is_parent = true;

	parse_program(&parser);

	Parser_Result result = {
		.nodes = parser.nodes,
		.errors = parser.errors,
	};
	return result;
}
