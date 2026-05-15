#include <stdio.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "parser.h"
#include "lexer.h"
#include "list.h"

// State shared between all of the parsing rules.
struct parser {
	struct token *tokens; // Points to a list.
	uint32_t current_token_index;
	struct node *nodes; // Points to a list.
	uint32_t last_node_index;
	bool next_node_is_child;
	struct parsing_error *errors; // Points to a list.
};

static const size_t initial_nodes_capacity = 1000;

static const size_t initial_errors_capacity = 100;

static struct token *parser_get_current_token(struct parser *parser) {
	if (parser->current_token_index < list_get_count(&parser->tokens)) {
		return parser->tokens + parser->current_token_index;
	}
	return NULL;
}

static bool parser_add_node(struct parser *parser, struct node *node) {
	struct node *new_node = list_push_back_uninitialized(&parser->nodes);
	if (!new_node) {
		return false;
	}
	*new_node = *node;

	if (parser->last_node_index == NODE_NONE) {
		parser->last_node_index = new_node - parser->nodes;
		return true;
	}

	// Link the node to the previous node.
	struct node *last_node = parser->nodes + parser->last_node_index;
	if (parser->next_node_is_child) {
		parser->next_node_is_child = false;
		new_node->parent_index = parser->last_node_index;
		last_node->child_index = new_node - parser->nodes;
		parser->last_node_index = new_node - parser->nodes;
		return true;
	}
	new_node->previous_index = last_node - parser->nodes;
	new_node->parent_index = last_node->parent_index;
	last_node->next_index = new_node - parser->nodes;
	parser->last_node_index = new_node - parser->nodes;
	return true;
}

static bool parser_begin_node(struct parser *parser, enum node_type type) {
	struct node new_node = {
		.type = type,
		.parent_index = NODE_NONE,
		.child_index = NODE_NONE,
		.previous_index = NODE_NONE,
		.next_index = NODE_NONE,
	};
	if (!parser_add_node(parser, &new_node)) {
		return false;
	}
	parser->next_node_is_child = true;
	return true;
}

static bool parser_end_node(struct parser *parser) {
	struct node *last_node = parser->nodes + parser->last_node_index;
	parser->last_node_index = last_node->parent_index;
	return true;
}

static bool parser_peek_token(struct parser *parser, enum token_type type) {
	struct token *token = parser_get_current_token(parser);
	return token && token->type == type;
}

static bool parser_consume_token(struct parser *parser, enum token_type type) {
	if (!parser_peek_token(parser, type)) {
		return false;
	}
	struct node new_node = {
		.type = NODE_TYPE_TOKEN,
		.parent_index = NODE_NONE,
		.child_index = parser->current_token_index,
		.previous_index = NODE_NONE,
		.next_index = NODE_NONE,
	};
	if (!parser_add_node(parser, &new_node)) {
		return false;
	}
	++parser->current_token_index;
	return true;
}

static bool parser_emit_error(struct parser *parser, enum parsing_error_type type) {
	struct parsing_error *error = NULL;
	error = list_push_back_uninitialized(&parser->errors);
	if (!error) {
		return false;
	}
	*error = (struct parsing_error){
		.type = type,
		.tokens_index = parser->current_token_index,
		// TODO: Set `tokens_count`.
	};

	// Skip tokens until we pass the next newline.
	while (parser->current_token_index < list_get_count(&parser->tokens) && !parser_peek_token(parser, TOKEN_TYPE_NEWLINE)) {
		++parser->current_token_index;
	}
	if (parser_peek_token(parser, TOKEN_TYPE_NEWLINE)) {
		++parser->current_token_index;
	}
	return parser_end_node(parser);
}

static bool parse_module_definition(struct parser *parser) {
	if (!parser_peek_token(parser, TOKEN_TYPE_MODULE)) return false;
	parser_begin_node(parser, NODE_TYPE_MODULE_DEFINITION);
		parser_consume_token(parser, TOKEN_TYPE_MODULE);
		if (!parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER)) return parser_emit_error(parser, PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER);
		while (parser_consume_token(parser, TOKEN_TYPE_DOT)) {
			if (parser_consume_token(parser, TOKEN_TYPE_TIMES)) break;
			if (!parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER)) return parser_emit_error(parser, PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER_OR_STAR);
		}
		if (!parser_consume_token(parser, TOKEN_TYPE_NEWLINE)) return parser_emit_error(parser, PARSING_ERROR_TYPE_EXPECTED_LINE_END);
	return parser_end_node(parser);
}

static bool parse_definition(struct parser *parser) {
	parser_begin_node(parser, NODE_TYPE_DEFINITION);
		parser_consume_token(parser, TOKEN_TYPE_PUB);
		if (parse_module_definition(parser)) return parser_end_node(parser);
	return parser_emit_error(parser, PARSING_ERROR_TYPE_EXPECTED_DEFINITION);
}

static bool parse_program_statement(struct parser *parser) {
	if (parse_definition(parser)) return true;
	return false;
}

static bool parse_program(struct parser *parser) {
	parser_begin_node(parser, NODE_TYPE_PROGRAM);
		while (parser->current_token_index < list_get_count(&parser->tokens)) {
			if (!parse_program_statement(parser)) return parser_emit_error(parser, PARSING_ERROR_TYPE_EXPECTED_STATEMENT);
		}
	return parser_end_node(parser);
}

const char *const node_type_names[NODE_TYPE_COUNT] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
	[NODE_TYPE_DEFINITION] = "definition",
	[NODE_TYPE_MODULE_DEFINITION] = "module definition",
};

const char *const parsing_error_messages[PARSING_ERROR_TYPE_COUNT] = {
	[PARSING_ERROR_TYPE_EXPECTED_LINE_END] = "Expected end of line.",
	[PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER] = "Expected an identifier.",
	[PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER_OR_STAR] = "Expected an identifier or a `*`.",
	[PARSING_ERROR_TYPE_EXPECTED_DEFINITION] = "Expected a definition.",
	[PARSING_ERROR_TYPE_EXPECTED_STATEMENT] = "Expected a satatement.",
};

bool parse(struct token *tokens, struct node **nodes, struct parsing_error **errors) {
	struct parser parser = {
		.tokens = tokens,
		.nodes = list_create(initial_nodes_capacity, sizeof *parser.nodes),
		.last_node_index = NODE_NONE,
	};
	if (!parser.nodes) {
		goto error1;
	}
	parser.errors = list_create(initial_errors_capacity, sizeof *parser.errors);
	if (!parser.errors) {
		goto error2;
	}
	bool result = parse_program(&parser);
	*nodes = parser.nodes;
	*errors = parser.errors;
	return result;

error2:
	list_destroy(&parser.nodes);
error1:
	return false;
}
