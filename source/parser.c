#include <stdio.h>

#include <stdint.h>
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
	uint32_t current_node_index;
	bool current_node_is_parent;
} Parser;

static uint32_t prefix_precedences[TOKEN_TYPE_COUNT] = {
	// [TOKEN_TYPE_BOOLEAN_NOT] = 1,
	[TOKEN_TYPE_PLUS] = 300,
	[TOKEN_TYPE_MINUS] = 300,
	// [TOKEN_TYPE_TIMES] = 1,
	// [TOKEN_TYPE_BITWISE_NOT] = 1,
	[TOKEN_TYPE_LEFT_PARENTHESIS] = 2,
	[TOKEN_TYPE_LEFT_BRACKET] = 1,
};

static uint32_t infix_precedences[TOKEN_TYPE_COUNT] = {
	[TOKEN_TYPE_BITWISE_XOR] = 400,
	[TOKEN_TYPE_TIMES] = 200,
	[TOKEN_TYPE_DIVIDE] = 200,
	[TOKEN_TYPE_PLUS] = 100,
	[TOKEN_TYPE_MINUS] = 100,
	[TOKEN_TYPE_LEFT_PARENTHESIS] = 2,
	[TOKEN_TYPE_LEFT_BRACKET] = 1,
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
	return token && token->type == type;
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

static uint32_t peek_prefix_operator(Parser *parser) {
	Token *token = current_token(parser);
	if (token) {
		return prefix_precedences[token->type];
	}
	return 0;
}

static uint32_t parse_prefix_operator(Parser *parser) {
	uint32_t precedence = peek_prefix_operator(parser);
	if (precedence) {
		parse_token(parser, current_token(parser)->type);
		return precedence;
	}
	return 0;
}

static uint32_t peek_infix_operator(Parser *parser) {
	Token *token = current_token(parser);
	if (token) {
		return infix_precedences[token->type];
	}
	return 0;
}

static uint32_t parse_infix_operator(Parser *parser) {
	uint32_t precedence = peek_infix_operator(parser);
	if (precedence) {
		parse_token(parser, current_token(parser)->type);
		return precedence;
	}
	return 0;
}

static bool parse_expression(Parser *parser);

static bool parse_function_arguments(Parser *parser) {
	begin_node(parser, NODE_TYPE_FUNCTION_ARGUMENTS);
	if (!parse_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return false;
	while (parse_expression(parser)) {
		if (!parse_token(parser, TOKEN_TYPE_COMMA)) break;
	}
	if (!parse_token(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS);
	return end_node(parser);
}

static bool parse_array_index(Parser *parser) {
	begin_node(parser, NODE_TYPE_ARRAY_INDEX);
	if (!parse_token(parser, TOKEN_TYPE_LEFT_BRACKET)) return false;
	if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACKET)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACKET);
	return end_node(parser);
}

static bool parse_array(Parser *parser) {
	begin_node(parser, NODE_TYPE_ARRAY);
	if (!parse_token(parser, TOKEN_TYPE_LEFT_BRACKET)) return false;
	while (parse_expression(parser)) {
		if (!parse_token(parser, TOKEN_TYPE_COMMA)) break;
	}
	if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACKET)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACKET);
	return end_node(parser);
}

static bool parse_basic_expression(Parser *parser) {
	return parse_token(parser, TOKEN_TYPE_NUMBER) || parse_token(parser, TOKEN_TYPE_IDENTIFIER);
}

static bool parse_infix_expression(Parser *parser, uint32_t precedence);

static bool parse_prefix_expression(Parser *parser) {
	uint32_t precedence = peek_prefix_operator(parser);
	if (!precedence) {
		return parse_basic_expression(parser);
	}
	if (precedence == prefix_precedences[TOKEN_TYPE_LEFT_PARENTHESIS]) {
		return parse_function_arguments(parser);
	}
	if (precedence == prefix_precedences[TOKEN_TYPE_LEFT_BRACKET]) {
		return parse_array(parser);
	}

	begin_node(parser, NODE_TYPE_PREFIX_EXPRESSION);
		parse_prefix_operator(parser);
		if (!parse_infix_expression(parser, precedence)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	return end_node(parser);
}

static bool parse_infix_expression(Parser *parser, uint32_t precedence) {
	begin_node(parser, NODE_TYPE_INFIX_EXPRESSION);
	if (!parse_prefix_expression(parser)) return false;
	uint32_t new_precedence = 0;
	while ((new_precedence = peek_infix_operator(parser)) > precedence) {
		parse_infix_operator(parser);
		printf("precedence = %d\n", precedence);
		if (new_precedence == infix_precedences[TOKEN_TYPE_LEFT_PARENTHESIS]) {
			--parser->current_token_index;
			if (!parse_function_arguments(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_ARGUMENTS);
		} else if (new_precedence == infix_precedences[TOKEN_TYPE_LEFT_BRACKET]) {
			--parser->current_token_index;
			if (!parse_array_index(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_ARRAY_INDEX);
		}  else if (!parse_infix_expression(parser, new_precedence)) {
			return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		}
	}
	// TODO: Maybe tidy up tree by removing unnecessary parent node if it only has one child.
	return end_node(parser);
}

static bool parse_expression(Parser *parser) {
	return parse_infix_expression(parser, 0);
}

static bool parse_assignment_body(Parser *parser) {
	if (!parse_token(parser, TOKEN_TYPE_ASSIGN)) return true;
	if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	return true;
}

static bool parse_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_TYPE);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return false;
	return end_node(parser);
}

static bool parse_variable_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_VAR)) return false;
	begin_node(parser, NODE_TYPE_VARIABLE_DEFINITION);
		parse_token(parser, TOKEN_TYPE_VAR);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_assignment_body(parser)) return false;
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_module_name(Parser *parser) {
	if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return false;
	while (parse_token(parser, TOKEN_TYPE_DOT)) {
		if (parse_token(parser, TOKEN_TYPE_TIMES)) break;
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
	}
	return true;
}

static bool parse_module_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_MODULE)) return false;
	begin_node(parser, NODE_TYPE_MODULE_DEFINITION);
		parse_token(parser, TOKEN_TYPE_MODULE);
		if (!parse_module_name(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME);
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_definition_body(Parser *parser) {
	if (parse_module_definition(parser)) return true;
	if (parse_variable_definition(parser)) return true;
	return false;
}

static bool parse_definition(Parser *parser) {
	if (peek_token(parser, TOKEN_TYPE_PUB)) {
		begin_node(parser, NODE_TYPE_DEFINITION);
		parse_token(parser, TOKEN_TYPE_PUB);
		if (!parse_definition_body(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_DEFINITION);
		return end_node(parser);
	}
	return parse_definition_body(parser);
}

static bool parse_import_statement(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_IMPORT)) return false;
	begin_node(parser, NODE_TYPE_IMPORT_STATEMENT);
		parse_token(parser, TOKEN_TYPE_IMPORT);
		if (!parse_module_name(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME);
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_program(Parser *parser) {
	while (parser->current_token_index < list_get_size(parser->tokens)) {
		if (parse_import_statement(parser)) continue;
		if (!parse_definition(parser)) return false;
	}
	return true;
}

char *node_type_names[] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
	[NODE_TYPE_IMPORT_STATEMENT] = "import statement",
	[NODE_TYPE_DEFINITION] = "definition",
	[NODE_TYPE_MODULE_DEFINITION] = "module definition",
	[NODE_TYPE_VARIABLE_DEFINITION] = "variable definition",
	[NODE_TYPE_FUNCTION_ARGUMENTS] = "function arguments",
	[NODE_TYPE_TYPE] = "type",
	[NODE_TYPE_ARRAY_INDEX] = "array index",
	[NODE_TYPE_ARRAY] = "array",
	[NODE_TYPE_PREFIX_EXPRESSION] = "prefix expression",
	[NODE_TYPE_INFIX_EXPRESSION] = "infix expression",
};

char *parser_error_messages[] = {
	[PARSER_ERROR_TYPE_INVALID_SYNTAX] = "Invalid syntax.",
	[PARSER_ERROR_TYPE_EXPECTED_DEFINITION] = "Expected a definition.",
	[PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME] = "Expected a module name.",
	[PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER] = "Expected an identifier.",
	[PARSER_ERROR_TYPE_EXPECTED_EXPRESSION] = "Expected an expression.",
	[PARSER_ERROR_TYPE_EXPECTED_FUNCTION_ARGUMENTS] = "Expected function arguments.",
	[PARSER_ERROR_TYPE_EXPECTED_TYPE] = "Expected a type.",
	[PARSER_ERROR_TYPE_EXPECTED_ARRAY_INDEX] = "Expected an array index.",
	[PARSER_ERROR_TYPE_EXPECTED_SEMICOLON] = "Expected a semicolon.",
	[PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS] = "Unclosed parenthesis.",
	[PARSER_ERROR_TYPE_UNCLOSED_BRACKET] = "Unclosed bracket.",
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
