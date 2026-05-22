#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lexer.h"

// Sentinel value to indicate a node link is empty.
#define NODE_NONE SIZE_MAX

enum node_type {
	NODE_TYPE_TOKEN,
	NODE_TYPE_PROGRAM,
	NODE_TYPE_DEFINITION,
	NODE_TYPE_MODULE_DEFINITION,
	NODE_TYPE_COUNT,
};

struct node {
	size_t parent_index;
	size_t child_index;
	size_t previous_index;
	size_t next_index;
	enum node_type type;
};

enum parsing_error_type {
	PARSING_ERROR_TYPE_EXPECTED_LINE_END,
	PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER,
	PARSING_ERROR_TYPE_EXPECTED_IDENTIFIER_OR_STAR,
	PARSING_ERROR_TYPE_EXPECTED_DEFINITION,
	PARSING_ERROR_TYPE_EXPECTED_STATEMENT,
	PARSING_ERROR_TYPE_COUNT,
};

struct parsing_error {
	size_t tokens_index;
	size_t tokens_count;
	enum parsing_error_type type;
};

// A map of node types to names.
extern const char *const node_type_names[];

// A map of parser error types to error messages.
extern const char *const parsing_error_messages[];

// Returns true if no errors were emitted.
bool parse(struct token *tokens, struct node **nodes, struct parsing_error **errors);

#endif // PARSER_H
