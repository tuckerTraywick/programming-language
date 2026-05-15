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
	uint32_t *first_child_index_stack; // Points to a list.
	struct parsing_error *errors;
};

static const size_t initial_nodes_capacity = 5000;

static const size_t initial_errors_capacity = 100;

static const size_t initial_stack_capacity = 100;

// Returns true if no memory errors occurred.
static bool parser_add_sibling(struct parser *parser, uint32_t token_index) {
	struct node *new_node = list_push_back_uninitialized(&parser->nodes);
	if (!new_node) {
		return false;
	}

	if (parser->last_node_index == NODE_NONE) {
		*new_node = (struct node){
			.type = NODE_TYPE_TOKEN,
			.parent_index = NODE_NONE,
			.child_index = token_index,
			.previous_index = NODE_NONE,
			.next_index = NODE_NONE,
		};
		parser->last_node_index = new_node - parser->nodes;
		parser->first_node_index = parser->last_node_index;
		return true;
	}

	struct node *last_node = parser->nodes + parser->last_node_index;
	last_node->next_index = last_node - parser->nodes;
	*new_node = (struct node){
		.type = NODE_TYPE_TOKEN,
		.parent_index = last_node->parent_index,
		.child_index = token_index,
		.previous_index = parser->last_node_index,
		.next_index = NODE_NONE,
	};
	parser->last_node_index = new_node - parser->nodes;
	return true;
}

// Assumes `parser` has at least one child index on its stack.
uint32_t parser_pop_first_child_index(struct parser *parser) {
	uint32_t index = 0;
	assert(list_pop_back(&parser->first_child_index_stack, &index));
	return index;
}

// Returns true if no memory errors occurred. Assumes `parser` has at least one node in the parse
// tree.
static bool parser_end_node(struct parser *parser, enum node_type type) {
	struct node *new_node = list_push_back_uninitialized(&parser->nodes);
	if (!new_node) {
		return false;
	}

	assert(list_get_count(&parser->nodes));
	struct node *first_child = parser->nodes + parser_pop_first_child_index(parser);

	*new_node = (struct node){
		.type = type,
		.parent_index = first_child->parent_index,
		.child_index = first_child - parser->nodes,
		.previous_index = first_child->previous_index,
		.next_index = first_child->next_index,
	};
	first_child->parent_index = new_node - parser->nodes;
	parser->last_node_index = new_node - parser->nodes;

	if (parser->first_node_index == first_child - parser->nodes) {
		parser->first_node_index = new_node - parser->nodes;
	}
	return true;
}

static struct token *parser_get_current_token(struct parser *parser) {
	if (parser->current_token_index < list_get_count(&parser->tokens)) {
		return parser->tokens + parser->current_token_index;
	}
	return NULL;
}

static bool parser_peek_token(struct parser *parser, enum token_type type) {
	struct token *token = parser_get_current_token(parser);
	return token && token->type == type;
}

static bool parser_consume_token(struct parser *parser, enum token_type type) {
	if (parser_peek_token(parser, type)) {
		++parser->current_token_index;
		return parser_add_sibling(parser, parser->current_token_index - 1);
	}
	return false;
}

// Returns true if no memory errors occurred.
static bool parser_consume_token_and_begin_node(struct parser *parser, enum token_type type) {
	return parser_consume_token(parser, type) && list_push_back(&parser->first_child_index_stack, &parser->last_node_index);
}

static bool parser_emit_error_and_recover(struct parser *parser, enum parsing_error_type error_type, enum node_type node_type) {
	struct parsing_error error = {
		.type = error_type,
		// TODO: Set `tokens_index` and `tokens_count`.
	};
	if (!list_push_back(&parser->errors, &error)) {
		return false;
	}

	// Skip tokens until a newline is skipped.
	struct token *token = NULL;
	while ((token = parser_get_current_token(parser))) {
		if (token->type == TOKE_TYPE_NEWLINE) {
			++parser->current_token_index;
			break;
		}
	}
	return parser_end_node(parser, node_type);
}

// static bool parse_line_end(struct parser *parser);

// static bool parse_module_definition(struct parser *parser) {
// 	if (!parser_consume_token(parser, TOKEN_TYPE_MODULE)) return false;
// 	if (!parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER)) return parser_emit_error_and_recover(parser, PARSING_ERROR_TYPE_EXPECTED_MODULE_NAME, NODE_TYPE_MODULE_DEFINITION);
// 	while (parser_consume_token(parser, TOKEN_TYPE_DOT)) {
// 		if (!parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER)) return parser_emit_error_and_recover(parser, PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER_OR_STAR, NODE_TYPE_MODULE_DEFINITION);
// 	}
// 	if (!parse_line_end(parser)) return parser_emit_error_and_recover(parser, PARSING_ERROR_TYPE_EXPECTED_LINE_END, NODE_TYPE_MODULE_DEFINITION);
// 	return parser_end_node(parser, NODE_TYPE_MODULE_DEFINITION);
// }

static bool parse_program(struct parser *parser) {
	if (!parser_consume_token_and_begin_node(parser, TOKEN_TYPE_IDENTIFIER)) return false;
	return parser_end_node(parser, NODE_TYPE_PROGRAM);
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
	parser.first_child_index_stack = list_create(initial_stack_capacity, sizeof *parser.first_child_index_stack);
	if (!parser.first_child_index_stack) {
		goto error3;
	}

	bool result = parse_program(&parser);
	*nodes = parser.nodes;
	*first_node_index = parser.first_node_index;
	*errors = parser.errors;
	list_destroy(&parser.first_child_index_stack);
	return result;

error3:
	list_destroy(&parser.errors);
error2:
	list_destroy(&parser.nodes);
error1:
	return false;
}
