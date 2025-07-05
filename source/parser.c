#include <stdio.h>

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"
#include "list.h"

#define STARTING_NODE_CAPACITY 1000

#define STARTING_PARSER_ERROR_CAPACITY 100

// The state passed between parsing functions.
typedef struct Parser {
	Token *tokens;
	Node *nodes;
	Parser_Error *errors;
	uint32_t current_token_index;
	uint32_t next_node_index;
} Parser;

static uint32_t prefix_precedences[TOKEN_TYPE_COUNT] = {
	[TOKEN_TYPE_PLUS] = 300,
	[TOKEN_TYPE_MINUS] = 300,
	[TOKEN_TYPE_LEFT_PARENTHESIS] = 1,
};

static uint32_t infix_precedences[TOKEN_TYPE_COUNT] = {
	[TOKEN_TYPE_BITWISE_XOR] = 400,
	[TOKEN_TYPE_TIMES] = 200,
	[TOKEN_TYPE_DIVIDE] = 200,
	[TOKEN_TYPE_PLUS] = 100,
	[TOKEN_TYPE_MINUS] = 100,
};

static void add_leaf(Parser *parser, Node *node) {
	// TODO: Handle null return value.
	parser->nodes = list_push(parser->nodes, node);
	if (parser->next_node_index > 0) {
		parser->nodes[parser->next_node_index - 1].next_index = parser->next_node_index;
	}
	++parser->next_node_index;
}

static uint32_t begin_node(Parser *parser) {
	return parser->next_node_index;
}

static bool end_node(Parser *parser, Node_Type type, uint32_t child_index) {
	if (child_index) {
		parser->nodes[child_index - 1].next_index = parser->next_node_index;
	}
	Node node = {.type = type, .next_index = NODE_END, .child_index = child_index,};
	// TODO: Handle null return value.
	parser->nodes = list_push(parser->nodes, &node);
	++parser->next_node_index;
	return true;
}

static bool emit_error(Parser *parser, Parser_Error_Type type) {
	// TODO: Count number of tokens in error.
	Parser_Error error = {
		.type = type,
		.token_index = parser->current_token_index,
		.token_count = 0,
	};
	// TODO: Handle null return value.
	parser->errors = list_push(parser->errors, &error);
	return false;
}

static Token *current_token(Parser *parser) {
	if (parser->current_token_index < list_get_size(parser->tokens)) {
		return parser->tokens + parser->current_token_index;
	}
	return NULL;
}

// static bool peek_token(Parser *parser, Token_Type type) {
// 	Token *token = current_token(parser);
// 	return token && token->type == type;
// }

static bool parse_token(Parser *parser, Token_Type type) {
	Token *token = current_token(parser);
	if (token && token->type == type) {
		Node node = {.type = NODE_TYPE_TOKEN, .next_index = NODE_END, .child_index = parser->current_token_index};
		add_leaf(parser, &node);
		++parser->current_token_index;
		return true;
	}
	return false;
}

// static uint32_t peek_prefix_operator(Parser *parser) {
// 	Token *token = current_token(parser);
// 	if (token) {
// 		return prefix_precedences[token->type];
// 	}
// 	return 0;
// }

// static uint32_t parse_prefix_operator(Parser *parser) {
// 	uint32_t precedence = peek_prefix_operator(parser);
// 	if (precedence) {
// 		parse_token(parser, current_token(parser)->type);
// 		return precedence;
// 	}
// 	return 0;
// }

// static uint32_t peek_infix_operator(Parser *parser) {
// 	Token *token = current_token(parser);
// 	if (token) {
// 		return infix_precedences[token->type];
// 	}
// 	return 0;
// }

// static uint32_t parse_infix_operator(Parser *parser) {
// 	uint32_t precedence = peek_infix_operator(parser);
// 	if (precedence) {
// 		parse_token(parser, current_token(parser)->type);
// 		return precedence;
// 	}
// 	return 0;
// }

static bool parse_variable_definition(Parser *parser) {
	uint32_t child = begin_node(parser);
	if (!parse_token(parser, TOKEN_TYPE_VAR)) return false;
	return end_node(parser, NODE_TYPE_VARIABLE_DEFINITION, child);
}

static bool parse_program(Parser *parser) {
	uint32_t child = begin_node(parser);
	if (!parse_token(parser, TOKEN_TYPE_PUB)) return false;
	if (!parse_variable_definition(parser)) return false;
	if (!parse_token(parser, TOKEN_TYPE_PUB)) return false;
	return end_node(parser, NODE_TYPE_PROGRAM, child);
}

char *node_type_names[] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
	[NODE_TYPE_VARIABLE_DEFINITION] = "variable definition",
	[NODE_TYPE_FUNCTION_ARGUMENTS] = "function arguments",
	[NODE_TYPE_PREFIX_EXPRESSION] = "prefix expression",
	[NODE_TYPE_INFIX_EXPRESSION] = "infix expression",
};

char *parser_error_messages[] = {
	[PARSER_ERROR_TYPE_INVALID_SYNTAX] = "Invalid syntax.",
	[PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME] = "Expected a module name.",
	[PARSER_ERROR_TYPE_EXPECTED_EXPRESSION] = "Expected an expression.",
	[PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS] = "Unclosed parenthesis.",
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

	parse_program(&parser);

	Parser_Result result = {
		.nodes = parser.nodes,
		.errors = parser.errors,
	};
	return result;
}
