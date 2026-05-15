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
	uint32_t first_node_index;
	uint32_t last_node_index;
	uint32_t *parent_node_index_stack; // Points to a list.
	bool next_node_is_child;
	struct parsing_error *errors;
};

static const size_t initial_nodes_capacity = 5000;

static const size_t initial_errors_capacity = 100;

static const size_t initial_stack_capacity = 100;

static struct token *parser_get_current_token(struct parser *parser) {
	if (parser->current_token_index < list_get_count(&parser->tokens)) {
		return parser->tokens + parser->current_token_index;
	}
	return NULL;
}

static uint32_t parser_get_parent_node_index(struct parser *parser) {
	uint32_t *back = list_get_back(&parser->parent_node_index_stack);
	if (!back) {
		return NODE_NONE;
	}
	return *back;
}

static bool parser_add_node(struct parser *parser, struct node *node) {
	struct node *new_node = list_push_back_uninitialized(&parser->nodes);
	if (!new_node) {
		return false;
	}
	*new_node = *node;

	if (parser->first_node_index == NODE_NONE) {
		parser->first_node_index = new_node - parser->nodes;
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
	return list_push_back(&parser->parent_node_index_stack, &parser->last_node_index);
}

static bool parser_end_node(struct parser *parser) {
	uint32_t parent_node_index = NODE_NONE;
	if (!list_pop_back(&parser->parent_node_index_stack, &parent_node_index)) {
		return false;
	}
	parser->last_node_index = parent_node_index;
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

static bool parse_module_definition(struct parser *parser) {
	parser_begin_node(parser, NODE_TYPE_MODULE_DEFINITION);
		if (!parser_consume_token(parser, TOKEN_TYPE_MODULE)) return false;
		parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER);
		parser_begin_node(parser, NODE_TYPE_PROGRAM);
			parser_consume_token(parser, TOKEN_TYPE_NUMBER);
		parser_end_node(parser);
	return parser_end_node(parser);
}

static bool parse_program(struct parser *parser) {
	return parse_module_definition(parser);
}

const char *const node_type_names[NODE_TYPE_COUNT] = {
	[NODE_TYPE_TOKEN] = "token",
	[NODE_TYPE_PROGRAM] = "program",
	[NODE_TYPE_MODULE_DEFINITION] = "module definition",
};

const char *const parsing_error_messages[PARSING_ERROR_TYPE_COUNT] = {
	[PARSING_ERROR_TYPE_EXPECTED_LINE_END] = "Expected end of line.",
	[PARSING_ERROR_TYPE_EXPECTED_MODULE_NAME] = "Expected a module name.",
	[PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER_OR_STAR] = "Expected an identifier or a `*`.",
};

bool parse(struct token *tokens, struct node **nodes, uint32_t *first_node_index, struct parsing_error **errors) {
	struct parser parser = {
		.tokens = tokens,
		.nodes = list_create(initial_nodes_capacity, sizeof *parser.nodes),
		.first_node_index = NODE_NONE,
		.last_node_index = NODE_NONE,
	};
	if (!parser.nodes) {
		goto error1;
	}
	parser.errors = list_create(initial_errors_capacity, sizeof *parser.errors);
	if (!parser.errors) {
		goto error2;
	}
	parser.parent_node_index_stack = list_create(initial_stack_capacity, sizeof *parser.parent_node_index_stack);
	if (!parser.parent_node_index_stack) {
		goto error3;
	}

	bool result = parse_program(&parser);
	*nodes = parser.nodes;
	*first_node_index = parser.first_node_index;
	*errors = parser.errors;
	list_destroy(&parser.parent_node_index_stack);
	return result;

error3:
	list_destroy(&parser.errors);
error2:
	list_destroy(&parser.nodes);
error1:
	return false;
}
