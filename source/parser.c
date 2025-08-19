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
	if (!peek_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return false;
	begin_node(parser, NODE_TYPE_FUNCTION_ARGUMENTS);
		parse_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS);
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
			if (!parse_expression(parser)) return false;
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
	if (!parse_token(parser, TOKEN_TYPE_ASSIGN)) return false;
	if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	return true;
}

static bool parse_type(Parser *parser);

static bool parse_generic_arguments(Parser *parser) {
	begin_node(parser, NODE_TYPE_GENERIC_ARGUMENTS);
		parse_token(parser, TOKEN_TYPE_LEFT_ANGLE_BRACKET);
		while (!peek_token(parser, TOKEN_TYPE_GREATER)) {
			if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
			parse_token(parser, TOKEN_TYPE_COMMA);
		}
		if (!parse_token(parser, TOKEN_TYPE_GREATER)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_ANGLE_BRACKET);
	return end_node(parser);
}

static bool parse_basic_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_BASIC_TYPE);
		parse_token(parser, TOKEN_TYPE_IDENTIFIER);
		while (true) {
			if (parse_token(parser, TOKEN_TYPE_DOT)) {
				if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
			} else if (peek_token(parser, TOKEN_TYPE_LEFT_ANGLE_BRACKET)) {
				if (!parse_generic_arguments(parser)) return false;
			} else {
				break;
			}
		}
	return end_node(parser);
}

static bool parse_mut_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_MUT_TYPE);
		parse_token(parser, TOKEN_TYPE_MUT);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_weak_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_WEAK_TYPE);
		parse_token(parser, TOKEN_TYPE_WEAK);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_owned_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_OWNED_TYPE);
		parse_token(parser, TOKEN_TYPE_OWNED);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_tuple_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_TUPLE_TYPE);
		parse_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS);
		// TODO: Fix this so it correctly distinguishes invalid types and unclosed parenthesis.
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
			if (!parse_type(parser)) break;
			parse_token(parser, TOKEN_TYPE_COMMA);
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS);
	return end_node(parser);
}

static bool parse_function_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_FUNCTION_TYPE);
		parse_token(parser, TOKEN_TYPE_FUNC);
		if (!parse_tuple_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_array_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_ARRAY_TYPE);
		parse_token(parser, TOKEN_TYPE_LEFT_BRACKET);
		if (!peek_token(parser, TOKEN_TYPE_RIGHT_BRACKET) && !parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACKET)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACKET);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_pointer_type(Parser *parser) {
	begin_node(parser, NODE_TYPE_POINTER_TYPE);
		parse_token(parser, TOKEN_TYPE_BITWISE_AND);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_type(Parser *parser) {
	if (peek_token(parser, TOKEN_TYPE_BITWISE_AND)) return parse_pointer_type(parser);
	if (peek_token(parser, TOKEN_TYPE_LEFT_BRACKET)) return parse_array_type(parser);
	if (peek_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return parse_tuple_type(parser);
	if (peek_token(parser, TOKEN_TYPE_FUNC)) return parse_function_type(parser);
	if (peek_token(parser, TOKEN_TYPE_OWNED)) return parse_owned_type(parser);
	if (peek_token(parser, TOKEN_TYPE_WEAK)) return parse_weak_type(parser);
	if (peek_token(parser, TOKEN_TYPE_MUT)) return parse_mut_type(parser);
	if (peek_token(parser, TOKEN_TYPE_IDENTIFIER)) return parse_basic_type(parser);
	return false;
}

static bool parse_embed_statement(Parser *parser) {
	begin_node(parser, NODE_TYPE_EMBED_STATEMENT);
		parse_token(parser, TOKEN_TYPE_EMBED);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_struct_definition(Parser *parser);

static bool parse_trait_definition(Parser *parser);

static bool parse_type_case(Parser *parser) {
	if (peek_token(parser, TOKEN_TYPE_EMBED)) return parse_embed_statement(parser);
	if (peek_token(parser, TOKEN_TYPE_STRUCT)) return parse_struct_definition(parser);
	if (peek_token(parser, TOKEN_TYPE_TRAIT)) return parse_trait_definition(parser);
	if (!peek_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE);
	begin_node(parser, NODE_TYPE_TYPE_CASE);
		parse_token(parser, TOKEN_TYPE_IDENTIFIER);
		if (peek_token(parser, TOKEN_TYPE_ASSIGN) && !parse_assignment_body(parser)) return false;
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_field_definition(Parser *parser) {
	if (peek_token(parser, TOKEN_TYPE_EMBED)) return parse_embed_statement(parser);
	if (!(peek_token(parser, TOKEN_TYPE_IDENTIFIER) || peek_token(parser, TOKEN_TYPE_PUB))) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FIELD_DEFINITION);
	begin_node(parser, NODE_TYPE_FIELD_DEFINITION);
		parse_token(parser, TOKEN_TYPE_PUB);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_trait_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_TRAIT)) return false;
	begin_node(parser, NODE_TYPE_TRAIT_DEFINITION);
		parse_token(parser, TOKEN_TYPE_TRAIT);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		// Parse generic parameters...
		
		// Parse fields.
		if (!parse_token(parser, TOKEN_TYPE_LEFT_BRACE)) {
			if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
			return end_node(parser);
		}
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_field_definition(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FIELD_DEFINITION);
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);

		// Parse cases.
		if (!parse_token(parser, TOKEN_TYPE_CASES)) return end_node(parser);
		if (!parse_token(parser, TOKEN_TYPE_LEFT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_CASES);
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_type_case(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE);
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);
	return end_node(parser);
}

static bool parse_struct_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_STRUCT)) return false;
	begin_node(parser, NODE_TYPE_STRUCT_DEFINITION);
		parse_token(parser, TOKEN_TYPE_STRUCT);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		// Parse generic parameters...
		
		// Parse fields.
		if (!parse_token(parser, TOKEN_TYPE_LEFT_BRACE)) {
			if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
			return end_node(parser);
		}
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_field_definition(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FIELD_DEFINITION);
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);

		// Parse cases.
		if (!parse_token(parser, TOKEN_TYPE_CASES)) return end_node(parser);
		if (!parse_token(parser, TOKEN_TYPE_LEFT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_CASES);
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_type_case(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE);
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);
	return end_node(parser);
}

static bool parse_definition(Parser *parser);

static bool parse_block(Parser *parser);

static bool parse_if_statement(Parser *parser) {
	begin_node(parser, NODE_TYPE_IF_STATEMENT);
		parse_token(parser, TOKEN_TYPE_IF);
		if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_block(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
		while (parse_token(parser, TOKEN_TYPE_ELSE)) {
			bool encountered_if = false;
			if (parse_token(parser, TOKEN_TYPE_IF)) {
				if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
				encountered_if = true;
			}
			if (!parse_block(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
			if (!encountered_if) break;
		}
	return end_node(parser);
}

static bool parse_loop_variable(Parser *parser) {
	begin_node(parser, NODE_TYPE_LOOP_VARIABLE);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_for_loop(Parser *parser) {
	begin_node(parser, NODE_TYPE_FOR_LOOP);
		parse_token(parser, TOKEN_TYPE_FOR);
		// TODO: Make this parse either one variable without parenthesis or a tuple of variables in parenthesis like function parameters.
		while (!peek_token(parser, TOKEN_TYPE_IN)) {
			if (!parse_loop_variable(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_LOOP_VARIABLE);
			parse_token(parser, TOKEN_TYPE_COMMA);
		}
		parse_token(parser, TOKEN_TYPE_IN);
		if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_block(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
	return end_node(parser);
}

static bool parse_while_loop(Parser *parser) {
	begin_node(parser, NODE_TYPE_WHILE_LOOP);
		parse_token(parser, TOKEN_TYPE_WHILE);
		if (!parse_expression(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_block(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
	return end_node(parser);
}

static bool parse_block_statement(Parser *parser) {
	if (parse_definition(parser)) return true;
	if (peek_token(parser, TOKEN_TYPE_LEFT_BRACE)) return parse_block(parser);
	if (peek_token(parser, TOKEN_TYPE_WHILE)) return parse_while_loop(parser);
	if (peek_token(parser, TOKEN_TYPE_FOR)) return parse_for_loop(parser);
	if (peek_token(parser, TOKEN_TYPE_IF)) return parse_if_statement(parser);
	if (parse_expression(parser)) {
		if (!parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
		return true;
	}
	return false;
}

static bool parse_block(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_LEFT_BRACE)) return false;
	begin_node(parser, NODE_TYPE_BLOCK);
		parse_token(parser, TOKEN_TYPE_LEFT_BRACE);
		while (!peek_token(parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_block_statement(parser)) break;
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);
	return end_node(parser);
}

static bool parse_function_parameter(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_IDENTIFIER)) return false;
	begin_node(parser, NODE_TYPE_FUNCTION_PARAMETER);
		parse_token(parser, TOKEN_TYPE_IDENTIFIER);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(parser);
}

static bool parse_function_parameters(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return false;
	begin_node(parser, NODE_TYPE_FUNCTION_PARAMETERS);
		parse_token(parser, TOKEN_TYPE_LEFT_PARENTHESIS);
		while (parse_function_parameter(parser)) {
			if (!parse_token(parser, TOKEN_TYPE_COMMA)) break;
		}
		if (!parse_token(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) return emit_error(parser, PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS);
	return end_node(parser);
}

static bool parse_method_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_METHOD)) return false;
	begin_node(parser, NODE_TYPE_METHOD_DEFINITION);
		parse_token(parser, TOKEN_TYPE_METHOD);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_function_parameters(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_block(parser) && !parse_token(parser, TOKEN_TYPE_SEMICOLON)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(parser);
}

static bool parse_function_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_FUNC)) return false;
	begin_node(parser, NODE_TYPE_FUNCTION_DEFINITION);
		parse_token(parser, TOKEN_TYPE_FUNC);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_function_parameters(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_block(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
	return end_node(parser);
}

static bool parse_variable_definition(Parser *parser) {
	if (!peek_token(parser, TOKEN_TYPE_VAR)) return false;
	begin_node(parser, NODE_TYPE_VARIABLE_DEFINITION);
		parse_token(parser, TOKEN_TYPE_VAR);
		if (!parse_token(parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_type(parser)) return emit_error(parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (peek_token(parser, TOKEN_TYPE_ASSIGN) && !parse_assignment_body(parser)) return false;
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
	if (peek_token(parser, TOKEN_TYPE_MODULE)) return parse_module_definition(parser);
	if (peek_token(parser, TOKEN_TYPE_VAR)) return parse_variable_definition(parser);
	if (peek_token(parser, TOKEN_TYPE_FUNC)) return parse_function_definition(parser);
	if (peek_token(parser, TOKEN_TYPE_METHOD)) return parse_method_definition(parser);
	if (peek_token(parser, TOKEN_TYPE_STRUCT)) return parse_struct_definition(parser);
	if (peek_token(parser, TOKEN_TYPE_TRAIT)) return parse_trait_definition(parser);
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
	[NODE_TYPE_FUNCTION_DEFINITION] = "function definition",
	[NODE_TYPE_METHOD_DEFINITION] = "method definition",
	[NODE_TYPE_FUNCTION_PARAMETERS] = "function parameters",
	[NODE_TYPE_FUNCTION_PARAMETER] = "function parameter",
	[NODE_TYPE_FUNCTION_ARGUMENTS] = "function arguments",
	[NODE_TYPE_STRUCT_DEFINITION] = "struct definition",
	[NODE_TYPE_TRAIT_DEFINITION] = "trait definition",
	[NODE_TYPE_FIELD_DEFINITION] = "field definition",
	[NODE_TYPE_EMBED_STATEMENT] = "embed statement",
	[NODE_TYPE_TYPE_CASE] = "type case",
	[NODE_TYPE_BLOCK] = "block",
	[NODE_TYPE_WHILE_LOOP] = "while loop",
	[NODE_TYPE_FOR_LOOP] = "for loop",
	[NODE_TYPE_LOOP_VARIABLE] = "loop variable",
	[NODE_TYPE_IF_STATEMENT] = "if statement",
	[NODE_TYPE_RETURN_STATEMENT] = "return statement",
	[NODE_TYPE_BREAK_STATEMENT] = "break statement",
	[NODE_TYPE_CONTINUE_STATEMENT] = "continue statement",
	[NODE_TYPE_TYPE] = "type",
	[NODE_TYPE_POINTER_TYPE] = "pointer type",
	[NODE_TYPE_ARRAY_TYPE] = "array type",
	[NODE_TYPE_TUPLE_TYPE] = "tuple type",
	[NODE_TYPE_FUNCTION_TYPE] = "function type",
	[NODE_TYPE_OWNED_TYPE] = "owned type",
	[NODE_TYPE_WEAK_TYPE] = "weak type",
	[NODE_TYPE_MUT_TYPE] = "mut type",
	[NODE_TYPE_BASIC_TYPE] = "basic type",
	[NODE_TYPE_GENERIC_ARGUMENTS] = "generic arguments",
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
	[PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS] = "Expected function parameters.",
	[PARSER_ERROR_TYPE_EXPECTED_FUNCTION_ARGUMENTS] = "Expected function arguments.",
	[PARSER_ERROR_TYPE_EXPECTED_BLOCK] = "Expected a brace-enclosed block.",
	[PARSER_ERROR_TYPE_EXPECTED_LOOP_VARIABLE] = "Expected a loop variable.",
	[PARSER_ERROR_TYPE_EXPECTED_IN_STATEMENT] = "Expected an `in` statement.",
	[PARSER_ERROR_TYPE_EXPECTED_FIELD_DEFINITION] = "Expected a field definition.",
	[PARSER_ERROR_TYPE_EXPECTED_CASES] = "Expected cases.",
	[PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE] = "Expected a type case.",
	[PARSER_ERROR_TYPE_EXPECTED_TYPE] = "Expected a type.",
	[PARSER_ERROR_TYPE_EXPECTED_ARRAY_INDEX] = "Expected an array index.",
	[PARSER_ERROR_TYPE_EXPECTED_SEMICOLON] = "Expected a semicolon.",
	[PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS] = "Unclosed parenthesis.",
	[PARSER_ERROR_TYPE_UNCLOSED_BRACKET] = "Unclosed bracket.",
	[PARSER_ERROR_TYPE_UNCLOSED_BRACE] = "Unclosed brace.",
	[PARSER_ERROR_TYPE_UNCLOSED_ANGLE_BRACKET] = "Unclosed angle bracket.",
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
