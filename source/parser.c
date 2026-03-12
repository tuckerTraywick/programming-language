#include <stddef.h>
#include <stdbool.h>
#include "parser.h"
#include "object.h"
#include "token.h"
#include "node.h"
#include "list.h"

// The state kept between parsing functions.
struct parser {
	size_t current_token_index;
	size_t current_node_index;
	bool current_node_is_parent;
};

static const size_t prefix_precedences[TOKEN_TYPE_COUNT] = {
	[TOKEN_TYPE_LEFT_PARENTHESIS] = 1601,
	[TOKEN_TYPE_LEFT_BRACKET] = 1602,
	
	[TOKEN_TYPE_PLUS] = 1600,
	[TOKEN_TYPE_MINUS] = 1600,
	[TOKEN_TYPE_TIMES] = 1600,
	[TOKEN_TYPE_BITWISE_NOT] = 1600,

	[TOKEN_TYPE_BOOLEAN_NOT] = 600,
};

static const size_t infix_precedences[TOKEN_TYPE_COUNT] = {
	[TOKEN_TYPE_DOT] = 1800,
	[TOKEN_TYPE_ARROW] = 1800,
	
	[TOKEN_TYPE_AS] = 1750,

	[TOKEN_TYPE_LEFT_ANGLE_BRACKET] = 1701,
	[TOKEN_TYPE_LEFT_BRACKET] = 1702,
	[TOKEN_TYPE_LEFT_PARENTHESIS] = 1703,

	// unary operators

	[TOKEN_TYPE_MODULUS] = 1400,
	[TOKEN_TYPE_DIVIDE] = 1400,
	[TOKEN_TYPE_TIMES] = 1400,

	[TOKEN_TYPE_MINUS] = 1300,
	[TOKEN_TYPE_PLUS] = 1300,
	
	[TOKEN_TYPE_LEFT_SHIFT] = 1200,

	[TOKEN_TYPE_RIGHT_SHIFT] = 1100,

	[TOKEN_TYPE_BITWISE_AND] = 1000,

	[TOKEN_TYPE_BITWISE_XOR] = 900,

	[TOKEN_TYPE_BITWISE_OR] = 800,

	[TOKEN_TYPE_IS] = 700,
	[TOKEN_TYPE_LESS_EQUAL] = 700,
	[TOKEN_TYPE_LESS] = 700,
	[TOKEN_TYPE_GREATER_EQUAL] = 700,
	[TOKEN_TYPE_GREATER] = 700,
	[TOKEN_TYPE_NOT_EQUAL] = 700,
	[TOKEN_TYPE_EQUAL] = 700,
		
	[TOKEN_TYPE_BOOLEAN_AND] = 500,
	
	[TOKEN_TYPE_BOOLEAN_XOR] = 400,
	
	[TOKEN_TYPE_BOOLEAN_OR] = 300,
	
	[TOKEN_TYPE_COMMA] = 200,
	
	[TOKEN_TYPE_MODULUS_ASSIGN] = 100,
	[TOKEN_TYPE_RIGHT_SHIFT_ASSIGN] = 100,
	[TOKEN_TYPE_LEFT_SHIFT_ASSIGN] = 100,
	[TOKEN_TYPE_BITWISE_NOT_ASSIGN] = 100,
	[TOKEN_TYPE_BITWISE_XOR_ASSIGN] = 100,
	[TOKEN_TYPE_BITWISE_OR_ASSIGN] = 100,
	[TOKEN_TYPE_BITWISE_AND_ASSIGN] = 100,
	[TOKEN_TYPE_DIVIDE_ASSIGN] = 100,
	[TOKEN_TYPE_TIMES_ASSIGN] = 100,
	[TOKEN_TYPE_MINUS_ASSIGN] = 100,
	[TOKEN_TYPE_PLUS_ASSIGN] = 100,
	[TOKEN_TYPE_ASSIGN] = 100,
};

static struct token *current_token(struct object *object, struct parser *parser) {
	if (parser->current_token_index >= list_get_count(&object->tokens)) {
		return NULL;
	}
	return object->tokens + parser->current_token_index;
}

static struct node *current_node(struct object *object, struct parser *parser) {
	return object->nodes + parser->current_node_index;
}

static size_t last_node_index(struct object *object, struct parser *parser) {
	return list_get_count(&object->nodes) - 1;
}

// Appends a node to the list of nodes without modifying the current parent index.
static struct node *add_node(struct object *object, struct parser *parser, enum node_type type) {
	struct node new_node = {
		.type = type,
	};
	// TODO: Handle null return value.
	list_push_back(&object->nodes, &new_node);
	if (parser->current_node_is_parent) {
		current_node(object, parser)->child_index = last_node_index(object, parser);
		((struct node*)list_get_back(&object->nodes))->parent_index = parser->current_node_index;
		parser->current_node_is_parent = false;
	} else {
		current_node(object, parser)->next_index = last_node_index(object, parser);
		((struct node*)list_get_back(&object->nodes))->parent_index = current_node(object, parser)->parent_index;
	}
	parser->current_node_index = last_node_index(object, parser);
	return (struct node*)list_get_back(&object->nodes);
}

static void begin_node(struct object *object, struct parser *parser, enum node_type type) {
	add_node(object, parser, type);
	parser->current_node_is_parent = true;
}

static bool end_node(struct object *object, struct parser *parser) {
	parser->current_node_index = current_node(object, parser)->parent_index;
	return true;
}

static bool emit_error(struct object *object, struct parser *parser, enum parser_error_type type) {
	// TODO: Count error tokens.
	struct parser_error error = {
		.type = type,
		.token_index = parser->current_token_index,
		.token_count = 0,
	};
	// TODO: Handle null return value.
	list_push_back(&object->parser_errors, &error);
	return false;
}

static bool peek_token(struct object *object, struct parser *parser, enum token_type type) {
	struct token *token = current_token(object, parser);
	return token && token->type == type;
}

static bool parse_token(struct object *object, struct parser *parser, enum token_type type) {
	if (!peek_token(object, parser, type)) {
		return false;
	}
	struct node *new_node = add_node(object, parser, NODE_TYPE_TOKEN);
	new_node->child_index = parser->current_token_index;
	++parser->current_token_index;
	return true;
}

static size_t peek_prefix_operator(struct object *object, struct parser *parser) {
	struct token *token = current_token(object, parser);
	if (token) {
		return prefix_precedences[token->type];
	}
	return 0;
}

static size_t parse_prefix_operator(struct object *object, struct parser *parser) {
	size_t precedence = peek_prefix_operator(object, parser);
	if (precedence) {
		parse_token(object, parser, current_token(object, parser)->type);
		return precedence;
	}
	return 0;
}

static size_t peek_infix_operator(struct object *object, struct parser *parser) {
	struct token *token = current_token(object, parser);
	if (token) {
		return infix_precedences[token->type];
	}
	return 0;
}

static size_t parse_infix_operator(struct object *object, struct parser *parser) {
	size_t precedence = peek_infix_operator(object, parser);
	if (precedence) {
		parse_token(object, parser, current_token(object, parser)->type);
		return precedence;
	}
	return 0;
}

static bool parse_expression(struct object *object, struct parser *parser);

static bool parse_function_arguments(struct object *object, struct parser *parser) {
	if (!peek_token(object, parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return false;
	begin_node(object, parser, NODE_TYPE_FUNCTION_ARGUMENTS);
		parse_token(object, parser, TOKEN_TYPE_LEFT_PARENTHESIS);
		while (!peek_token(object, parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
			if (!parse_expression(object, parser)) return false;
			if (!parse_token(object, parser, TOKEN_TYPE_COMMA)) break;
		}
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS);
	return end_node(object, parser);
}

static bool parse_array_index(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_ARRAY_INDEX);
	if (!parse_token(object, parser, TOKEN_TYPE_LEFT_BRACKET)) return false;
	if (!parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_BRACKET)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_BRACKET);
	return end_node(object, parser);
}

static bool parse_array(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_ARRAY);
	if (!parse_token(object, parser, TOKEN_TYPE_LEFT_BRACKET)) return false;
	while (parse_expression(object, parser)) {
		if (!parse_token(object, parser, TOKEN_TYPE_COMMA)) break;
	}
	if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_BRACKET)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_BRACKET);
	return end_node(object, parser);
}

static bool parse_basic_expression(struct object *object, struct parser *parser) {
	return parse_token(object, parser, TOKEN_TYPE_NUMBER) || parse_token(object, parser, TOKEN_TYPE_IDENTIFIER);
}

static bool parse_infix_expression(struct object *object, struct parser *parser, size_t precedence);

static bool parse_prefix_expression(struct object *object, struct parser *parser) {
	size_t precedence = peek_prefix_operator(object, parser);
	if (!precedence) {
		return parse_basic_expression(object, parser);
	}
	if (precedence == prefix_precedences[TOKEN_TYPE_LEFT_PARENTHESIS]) {
		return parse_function_arguments(object, parser);
	}
	if (precedence == prefix_precedences[TOKEN_TYPE_LEFT_BRACKET]) {
		return parse_array(object, parser);
	}

	begin_node(object, parser, NODE_TYPE_PREFIX_EXPRESSION);
		parse_prefix_operator(object, parser);
		if (!parse_infix_expression(object, parser, precedence)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	return end_node(object, parser);
}

static bool parse_type(struct object *object, struct parser *parser);

static bool parse_infix_expression(struct object *object, struct parser *parser, size_t precedence) {
	begin_node(object, parser, NODE_TYPE_INFIX_EXPRESSION);
	if (!parse_prefix_expression(object, parser)) return false;
	size_t new_precedence = 0;
	while ((new_precedence = peek_infix_operator(object, parser)) > precedence) {
		parse_infix_operator(object, parser);
		if (new_precedence == infix_precedences[TOKEN_TYPE_LEFT_PARENTHESIS]) {
			--parser->current_token_index;
			if (!parse_function_arguments(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_ARGUMENTS);
		} else if (new_precedence == infix_precedences[TOKEN_TYPE_LEFT_BRACKET]) {
			--parser->current_token_index;
			if (!parse_array_index(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_ARRAY_INDEX);
		} else if (new_precedence == infix_precedences[TOKEN_TYPE_AS]) {
			if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		} else if (!parse_infix_expression(object, parser, new_precedence)) {
			return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		}
	}
	// TODO: Maybe tidy up tree by removing unnecessary parent node if it only has one child.
	return end_node(object, parser);
}

static bool parse_expression(struct object *object, struct parser *parser) {
	return parse_infix_expression(object, parser, 0);
}

static bool parse_assignment_body(struct object *object, struct parser *parser) {
	parse_token(object, parser, TOKEN_TYPE_ASSIGN);
	if (!parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
	return true;
}

static bool parse_type(struct object *object, struct parser *parser);

static bool parse_generic_arguments(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_GENERIC_ARGUMENTS);
		parse_token(object, parser, TOKEN_TYPE_LEFT_ANGLE_BRACKET);
		while (!peek_token(object, parser, TOKEN_TYPE_GREATER)) {
			if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
			parse_token(object, parser, TOKEN_TYPE_COMMA);
		}
		if (!parse_token(object, parser, TOKEN_TYPE_GREATER)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_ANGLE_BRACKET);
	return end_node(object, parser);
}

static bool parse_basic_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_BASIC_TYPE);
		parse_token(object, parser, TOKEN_TYPE_IDENTIFIER);
		while (true) {
			if (parse_token(object, parser, TOKEN_TYPE_DOT)) {
				if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
			} else if (peek_token(object, parser, TOKEN_TYPE_LEFT_ANGLE_BRACKET)) {
				if (!parse_generic_arguments(object, parser)) return false;
			} else {
				break;
			}
		}
	return end_node(object, parser);
}

static bool parse_mut_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_MUT_TYPE);
		parse_token(object, parser, TOKEN_TYPE_MUT);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_weak_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_WEAK_TYPE);
		parse_token(object, parser, TOKEN_TYPE_WEAK);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_owned_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_OWNED_TYPE);
		parse_token(object, parser, TOKEN_TYPE_OWNED);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_tuple_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_TUPLE_TYPE);
		parse_token(object, parser, TOKEN_TYPE_LEFT_PARENTHESIS);
		// TODO: Fix this so it correctly distinguishes invalid types and unclosed parenthesis.
		while (!peek_token(object, parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
			if (!parse_type(object, parser)) break;
			parse_token(object, parser, TOKEN_TYPE_COMMA);
		}
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS);
	return end_node(object, parser);
}

static bool parse_function_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_FUNCTION_TYPE);
		parse_token(object, parser, TOKEN_TYPE_FUNC);
		if (!parse_tuple_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_array_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_ARRAY_TYPE);
		parse_token(object, parser, TOKEN_TYPE_LEFT_BRACKET);
		if (!peek_token(object, parser, TOKEN_TYPE_RIGHT_BRACKET) && !parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_BRACKET)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_BRACKET);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_pointer_type(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_POINTER_TYPE);
		parse_token(object, parser, TOKEN_TYPE_BITWISE_AND);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_type(struct object *object, struct parser *parser) {
	if (peek_token(object, parser, TOKEN_TYPE_BITWISE_AND)) return parse_pointer_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_LEFT_BRACKET)) return parse_array_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return parse_tuple_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_FUNC)) return parse_function_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_OWNED)) return parse_owned_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_WEAK)) return parse_weak_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_MUT)) return parse_mut_type(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return parse_basic_type(object, parser);
	return false;
}

static bool parse_embed_statement(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_EMBED_STATEMENT);
		parse_token(object, parser, TOKEN_TYPE_EMBED);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_type_definition(struct object *object, struct parser *parser);

static bool parse_type_case(struct object *object, struct parser *parser) {
	if (peek_token(object, parser, TOKEN_TYPE_EMBED)) return parse_embed_statement(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_TYPE)) return parse_type_definition(object, parser);
	if (!peek_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE);
	begin_node(object, parser, NODE_TYPE_TYPE_CASE);
		parse_token(object, parser, TOKEN_TYPE_IDENTIFIER);
		if (peek_token(object, parser, TOKEN_TYPE_ASSIGN) && !parse_assignment_body(object, parser)) return false;
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_function_header(struct object *object, struct parser *parser);

static bool parse_method_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_METHOD_DEFINITION);
		if (!parse_function_header(object, parser)) return false;
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_field_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_FIELD_DEFINITION);
		parse_token(object, parser, TOKEN_TYPE_IDENTIFIER);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_member_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_MEMBER_DEFINITION);
		if (peek_token(object, parser, TOKEN_TYPE_EMBED) && !parse_embed_statement(object, parser)) return false;
		parse_token(object, parser, TOKEN_TYPE_PUB);
		if (peek_token(object, parser, TOKEN_TYPE_IDENTIFIER) && !parse_field_definition(object, parser)) return false;
		if (peek_token(object, parser, TOKEN_TYPE_FUNC) && !parse_method_definition(object, parser)) return false;
	return end_node(object, parser);
}

static bool parse_type_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_TYPE_DEFINITION);
		if (!parse_token(object, parser, TOKEN_TYPE_STRUCT)) parse_token(object, parser, TOKEN_TYPE_TRAIT);
		if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		// TODO: Parse generic parameters.
		
		// Parse fields.
		if (!parse_token(object, parser, TOKEN_TYPE_LEFT_BRACE)) {
			if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
			return end_node(object, parser);
		}
		while (!peek_token(object, parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_member_definition(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_MEMBER_DEFINITION);
		}
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);

		// Parse cases.
		if (!parse_token(object, parser, TOKEN_TYPE_CASES)) return end_node(object, parser);
		if (!parse_token(object, parser, TOKEN_TYPE_LEFT_BRACE)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_CASES);
		while (!peek_token(object, parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_type_case(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE);
		}
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);
	return end_node(object, parser);
}

static bool parse_definition(struct object *object, struct parser *parser);

static bool parse_block(struct object *object, struct parser *parser);

static bool parse_if_statement(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_IF_STATEMENT);
		parse_token(object, parser, TOKEN_TYPE_IF);
		if (!parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_block(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
		while (parse_token(object, parser, TOKEN_TYPE_ELSE)) {
			bool encountered_if = false;
			if (parse_token(object, parser, TOKEN_TYPE_IF)) {
				if (!parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
				encountered_if = true;
			}
			if (!parse_block(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
			if (!encountered_if) break;
		}
	return end_node(object, parser);
}

static bool parse_loop_variable(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_LOOP_VARIABLE);
		if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_for_loop(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_FOR_LOOP);
		parse_token(object, parser, TOKEN_TYPE_FOR);
		// TODO: Make this parse either one variable without parenthesis or a tuple of variables in parenthesis like function parameters.
		while (!peek_token(object, parser, TOKEN_TYPE_IN)) {
			if (!parse_loop_variable(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_LOOP_VARIABLE);
			parse_token(object, parser, TOKEN_TYPE_COMMA);
		}
		parse_token(object, parser, TOKEN_TYPE_IN);
		if (!parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_block(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
	return end_node(object, parser);
}

static bool parse_while_loop(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_WHILE_LOOP);
		parse_token(object, parser, TOKEN_TYPE_WHILE);
		if (!parse_expression(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_EXPRESSION);
		if (!parse_block(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
	return end_node(object, parser);
}

static bool parse_block_statement(struct object *object, struct parser *parser) {
	if (parse_definition(object, parser)) return true;
	if (peek_token(object, parser, TOKEN_TYPE_LEFT_BRACE)) return parse_block(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_WHILE)) return parse_while_loop(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_FOR)) return parse_for_loop(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_IF)) return parse_if_statement(object, parser);
	if (parse_expression(object, parser)) {
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
		return true;
	}
	return false;
}

static bool parse_block(struct object *object, struct parser *parser) {
	if (!peek_token(object, parser, TOKEN_TYPE_LEFT_BRACE)) return false;
	begin_node(object, parser, NODE_TYPE_BLOCK);
		parse_token(object, parser, TOKEN_TYPE_LEFT_BRACE);
		while (!peek_token(object, parser, TOKEN_TYPE_RIGHT_BRACE)) {
			if (!parse_block_statement(object, parser)) break;
		}
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_BRACE)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_BRACE);
	return end_node(object, parser);
}

static bool parse_function_parameter(struct object *object, struct parser *parser) {
	if (!peek_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return false;
	begin_node(object, parser, NODE_TYPE_FUNCTION_PARAMETER);
		parse_token(object, parser, TOKEN_TYPE_IDENTIFIER);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return end_node(object, parser);
}

static bool parse_function_parameters(struct object *object, struct parser *parser) {
	if (!peek_token(object, parser, TOKEN_TYPE_LEFT_PARENTHESIS)) return false;
	begin_node(object, parser, NODE_TYPE_FUNCTION_PARAMETERS);
		parse_token(object, parser, TOKEN_TYPE_LEFT_PARENTHESIS);
		while (parse_function_parameter(object, parser)) {
			if (!parse_token(object, parser, TOKEN_TYPE_COMMA)) break;
		}
		if (!parse_token(object, parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) return emit_error(object, parser, PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS);
	return end_node(object, parser);
}

static bool parse_function_header(struct object *object, struct parser *parser) {
	if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
	if (!parse_function_parameters(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS);
	if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
	return true;
}

static bool parse_function_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_FUNCTION_DEFINITION);
		parse_token(object, parser, TOKEN_TYPE_FUNC);
		if (!parse_function_header(object, parser)) return false;
		if (!parse_block(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_BLOCK);
	return end_node(object, parser);
}

static bool parse_variable_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_VARIABLE_DEFINITION);
		parse_token(object, parser, TOKEN_TYPE_VAR);
		if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
		if (!parse_type(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_TYPE);
		if (peek_token(object, parser, TOKEN_TYPE_ASSIGN) && !parse_assignment_body(object, parser)) return false;
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_module_name(struct object *object, struct parser *parser) {
	if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME);
	while (parse_token(object, parser, TOKEN_TYPE_DOT)) {
		if (parse_token(object, parser, TOKEN_TYPE_TIMES)) break;
		if (!parse_token(object, parser, TOKEN_TYPE_IDENTIFIER)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER);
	}
	return true;
}

static bool parse_module_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_MODULE_DEFINITION);
		parse_token(object, parser, TOKEN_TYPE_MODULE);
		if (!parse_module_name(object, parser)) return false;
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_definition_body(struct object *object, struct parser *parser) {
	if (peek_token(object, parser, TOKEN_TYPE_MODULE)) return parse_module_definition(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_VAR)) return parse_variable_definition(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_FUNC)) return parse_function_definition(object, parser);
	if (peek_token(object, parser, TOKEN_TYPE_STRUCT) || peek_token(object, parser, TOKEN_TYPE_TRAIT)) return parse_type_definition(object, parser);
	return false;
}

static bool parse_definition(struct object *object, struct parser *parser) {
	begin_node(object, parser, NODE_TYPE_DEFINITION);
	if (peek_token(object, parser, TOKEN_TYPE_PUB)) {
		parse_token(object, parser, TOKEN_TYPE_PUB);
		if (!parse_definition_body(object, parser)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_DEFINITION);
	}
	if (!parse_definition_body(object, parser)) return false;
	return end_node(object, parser);
}

static bool parse_import_statement(struct object *object, struct parser *parser) {
	if (!peek_token(object, parser, TOKEN_TYPE_IMPORT)) return false;
	begin_node(object, parser, NODE_TYPE_IMPORT_STATEMENT);
		parse_token(object, parser, TOKEN_TYPE_IMPORT);
		if (!parse_module_name(object, parser)) return false;
		if (!parse_token(object, parser, TOKEN_TYPE_SEMICOLON)) return emit_error(object, parser, PARSER_ERROR_TYPE_EXPECTED_SEMICOLON);
	return end_node(object, parser);
}

static bool parse_program(struct object *object, struct parser *parser) {
	while (parser->current_token_index < list_get_count(&object->tokens)) {
		if (parse_import_statement(object, parser)) continue;
		if (!parse_definition(object, parser)) return false;
	}
	return parser->current_token_index == list_get_count(&object->tokens) && list_is_empty(&object->lexer_errors);
}

bool parse(struct object *object) {
	struct parser parser = {0};
	struct node node = {
		.type = NODE_TYPE_PROGRAM,
	};
	// TODO: Handle false return value.
	list_push_back(&object->nodes, &node);
	parser.current_node_is_parent = true;
	parse_program(object, &parser);
	return true;
}
