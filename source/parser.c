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
	uint32_t *parent_index_stack; // Points to a list.
	uint32_t *leaf_index_stack; // Points to a list.
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

static bool parser_peek_token(struct parser *parser, enum token_type type) {
	struct token *token = parser_get_current_token(parser);
	return token && token->type == type;
}

static bool parser_skip_token(struct parser *parser, enum token_type type) {
	if (parser_peek_token(parser, type)) {
		++parser->current_token_index;
		return true;
	}
	return false;
}

static struct node *parser_get_current_parent(struct parser *parser) {
	return parser->nodes + *parser->parent_index_stack;
}

static struct node *parser_get_current_leaf(struct parser *parser) {
	return parser->nodes + *parser->leaf_index_stack;
}

static void parser_add_leaf(struct parser *parser, struct node *node) {
	struct node *parent = parser_get_current_parent(parser);
	struct node *leaf = parser_get_current_leaf(parser);
	if (leaf == parent) {
		// Add child.
		node->parent_index = parser->current_parent_index;
		parent->child_index = list_get_count(&parser->nodes);
		parser->current_leaf_index = parent->child_index;
		list_push_back(&parser->nodes, node);
		return;
	}
	// Add sibling.
}

static bool parser_consume_token(struct parser *parser, enum token_type type) {
	if (parser_peek_token(parser, type)) {
		++parser->current_token_index;
		// add node.
		return true;
	}
	return false;
}

static void parser_emit_error_and_recover(struct parser *parser, enum parsing_error_type type) {
	struct parsing_error error = {
		.type = type,
		// TODO: Set `tokens_index` and `tokens_count`.
	};
	list_push_back(&parser->errors, &error);

	// Skip tokens until a newline is skipped.
	struct token *token = NULL;
	while ((token = parser_get_current_token(parser))) {
		if (token->type == TOKE_TYPE_NEWLINE) {
			++parser->current_token_index;
			break;
		}
	}
}

static void parser_begin_node(struct parser *parser) {

}

static void parser_begin_node_type(struct parser *parser, enum node_type type) {

}

static void parser_end_node(struct parser *parser) {

}

static void parser_end_node_type(struct parser *parser, enum node_type type) {

}

static bool parse_line_end(struct parser *parser) {
	return !parser_get_current_token(parser) || parser_skip_token(parser, TOKE_TYPE_NEWLINE);
}

static bool parse_module_definition(struct parser *parser) {
	if (!parser_skip_token(parser, TOKEN_TYPE_MODULE)) return false;
	if (!parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER)) {
		parser_end_node_type(parser, NODE_TYPE_MODULE_DEFINITION);
		parser_emit_error_and_recover(parser, PARSING_ERROR_TYPE_EXPECTED_MODULE_NAME);
		return true;
	}

	while (parser_skip_token(parser, TOKEN_TYPE_DOT)) {
		if (parser_consume_token(parser, TOKEN_TYPE_IDENTIFIER)) continue;
		if (parser_consume_token(parser, TOKEN_TYPE_TIMES)) break;
		parser_end_node_type(parser, NODE_TYPE_MODULE_DEFINITION);
		parser_emit_error_and_recover(parser, PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER_OR_STAR);
		return true;
	}

	if (!parse_line_end(parser)) {
		parser_end_node_type(parser, NODE_TYPE_MODULE_DEFINITION);
		parser_emit_error_and_recover(parser, PARSING_ERROR_TYPE_EXPECTED_LINE_END);
		return true;
	}
	parser_end_node_type(parser, NODE_TYPE_MODULE_DEFINITION);
	return true;
}

static bool parse_definition(struct parser *parser) {
	parser_consume_token(parser, TOKEN_TYPE_PUB);
	if (parse_module_definition(parser)) return true;
	return false;
}

static bool parse_program_statement(struct parser *parser) {
	parser_begin_node(parser);
	if (parse_definition(parser)) return true;
	return false;
}

static bool parse_program(struct parser *parser) {
	while (parse_program_statement(parser)) {}
	return parser_get_current_token(parser) == NULL && list_get_count(&parser->errors) == 0;
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

bool parse(struct token *tokens, struct node **nodes, struct parsing_error **errors) {
	struct parser parser = {
		.tokens = tokens,
		.nodes = list_create(initial_nodes_capacity, sizeof *parser.nodes),
	};
	if (!parser.nodes) {
		goto error1;
	}
	parser.errors = list_create(initial_errors_capacity, sizeof *parser.errors);
	if (!parser.errors) {
		goto error2;
	}
	parser.parent_index_stack = list_create(initial_stack_capacity, sizeof *parser.parent_index_stack);
	if (!parser.parent_index_stack) {
		goto error3;
	}
	parser.leaf_index_stack = list_create(initial_stack_capacity, sizeof *parser.leaf_index_stack);
	if (!parser.leaf_index_stack) {
		goto error4;
	}

	bool result = parse_program(&parser);
	*nodes = parser.nodes;
	*errors = parser.errors;
	return result;

error4:
	list_destroy(&parser.parent_index_stack);
error3:
	list_destroy(&parser.errors);
error2:
	list_destroy(&parser.nodes);
error1:
	return false;
}
