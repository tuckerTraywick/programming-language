#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdint.h>
#include "token.h"

enum node_type {
	NODE_TYPE_TOKEN,
	NODE_TYPE_COUNT,
};

struct node {
	enum node_type type;
	uint32_t child_index;
	uint32_t next_index;
};

enum parsing_error_type {
	PARSING_ERROR_TYPE_COUNT,
};

struct parsing_error {
	enum parsing_error_type type;
	uint32_t tokens_index;
	uint32_t tokens_count;
};

// A map of node types to names.
extern const char *const node_type_names[];

// A map of parser error types to error messages.
extern const char *const parsing_error_messages[];

bool parse(char *text, struct token *tokens, struct node **nodes, struct parsing_error **errors);

#endif // NODE_H
