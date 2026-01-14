#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "lexer.h"

// The type of thing a node represents.
enum node_type {
	NODE_TYPE_TOKEN,
	NODE_TYPE_PROGRAM,
	NODE_TYPE_IMPORT_STATEMENT,
	NODE_TYPE_DEFINITION,
	NODE_TYPE_MODULE_DEFINITION,
	NODE_TYPE_VARIABLE_DEFINITION,
	NODE_TYPE_FUNCTION_DEFINITION,
	NODE_TYPE_FUNCTION_PARAMETERS,
	NODE_TYPE_FUNCTION_PARAMETER,
	NODE_TYPE_FUNCTION_ARGUMENTS,
	NODE_TYPE_TYPE_DEFIINITION,
	NODE_TYPE_FIELD_DEFINITION,
	// NODE_TYPE_EMBED_STATEMENT,
	NODE_TYPE_TYPE_CASE,
	NODE_TYPE_BLOCK,
	NODE_TYPE_WHILE_LOOP,
	NODE_TYPE_FOR_LOOP,
	NODE_TYPE_LOOP_VARIABLE,
	NODE_TYPE_IF_STATEMENT,
	NODE_TYPE_RETURN_STATEMENT,
	NODE_TYPE_BREAK_STATEMENT,
	NODE_TYPE_CONTINUE_STATEMENT,
	NODE_TYPE_TYPE,
	NODE_TYPE_POINTER_TYPE,
	NODE_TYPE_ARRAY_TYPE,
	NODE_TYPE_TUPLE_TYPE,
	NODE_TYPE_FUNCTION_TYPE,
	NODE_TYPE_OWNED_TYPE,
	NODE_TYPE_WEAK_TYPE,
	NODE_TYPE_MUT_TYPE,
	NODE_TYPE_BASIC_TYPE,
	NODE_TYPE_GENERIC_ARGUMENTS,
	NODE_TYPE_ARRAY_INDEX,
	NODE_TYPE_ARRAY,
	NODE_TYPE_PREFIX_EXPRESSION,
	NODE_TYPE_INFIX_EXPRESSION,
	NODE_TYPE_COUNT,
};

// A node in the abstract syntax tree. Can point to a token or be a parent for other nodes.
struct node {
	size_t next_index;
	size_t parent_index;
	size_t child_index; // Holds the index of a token if `type` is `NODE_TYPE_TOKEN`.
	enum node_type type;
};

// The type of an error encountered during parsing.
enum parser_error_type {
	PARSER_ERROR_TYPE_INVALID_SYNTAX,
	PARSER_ERROR_TYPE_EXPECTED_DEFINITION,
	PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME,
	PARSER_ERROR_TYPE_EXPECTED_IDENTIFIER,
	PARSER_ERROR_TYPE_EXPECTED_EXPRESSION,
	PARSER_ERROR_TYPE_EXPECTED_FUNCTION_PARAMETERS,
	PARSER_ERROR_TYPE_EXPECTED_FUNCTION_ARGUMENTS,
	PARSER_ERROR_TYPE_EXPECTED_FIELD_DEFINITION,
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

// An error encountered during parsing.
struct parser_error {
	size_t token_index;
	size_t token_count;
	enum parser_error_type type;
};

// A map of node types to their names. Indexed by the type of a node.
extern const char *const node_type_names[];

// A map of parser error types to their messages. Indexed by the type of an error.
extern const char *const parser_error_messages[];

// Parses `tokens` and returns a list of nodes in a syntax tree in `nodes` and a list of errors
// encountered in `errors`. Returns true if no memory errors occurred, false otherwise.
bool parse(struct token *tokens, struct node **nodes, struct parser_error **errors);

#endif // PARSER_H
