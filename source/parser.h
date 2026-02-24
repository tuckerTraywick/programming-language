#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "token.h"
#include "node.h"

enum parser_error_type {
	PARSER_ERROR_TYPE_INVALID_SYNTAX,
	PARSER_ERROR_TYPE_EXPECTED_DEFINITION,
	PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME,
	PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER,
	PARSER_ERROR_TYPE_EXPECTED_EXPRESSION,
	PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS,
	PARSER_ERROR_TYPE_EXPECTED_FUNCTION_ARGUMENTS,
	PARSER_ERROR_TYPE_EXPECTED_FIELD_DEFINITION,
	PARSER_ERROR_TYPE_EXPECTED_MEMBER_DEFINITION,
	PARSER_ERROR_TYPE_EXPECTED_METHOD_DEFINITION,
	PARSER_ERROR_TYPE_EXPECTED_CASES,
	PARSER_ERROR_TYPE_EXPECTED_TYPE_CASE,
	PARSER_ERROR_TYPE_EXPECTED_BLOCK,
	PARSER_ERROR_TYPE_EXPECTED_LOOP_VARIABLE,
	PARSER_ERROR_TYPE_EXPECTED_IN_STATEMENT,
	PARSER_ERROR_TYPE_EXPECTED_TYPE,
	PARSER_ERROR_TYPE_EXPECTED_ARRAY_INDEX,
	PARSER_ERROR_TYPE_EXPECTED_SEMICOLON,
	PARSER_ERROR_TYPE_UNCLOSED_PARENTHESIS,
	PARSER_ERROR_TYPE_UNCLOSED_BRACKET,
	PARSER_ERROR_TYPE_UNCLOSED_BRACE,
	PARSER_ERROR_TYPE_UNCLOSED_ANGLE_BRACKET,
	PARSER_ERROR_TYPE_COUNT,
};

struct parser_error {
	size_t token_index;
	size_t token_count;
	enum parser_error_type type;
};

// A map of parser error types to their messages. Indexed by the type of an error.
extern const char *const parser_error_messages[];

// Parses `tokens` and returns a list of nodes in a syntax tree in `nodes` and a list of errors
// encountered in `errors`. Returns true if no memory errors occurred, false otherwise.
bool parse(struct token *tokens, struct node **nodes, struct parser_error **errors);

#endif // PARSER_H
