#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "lexer.h"

// What type of thing a node represents.
typedef enum Node_Type {
	NODE_TYPE_TOKEN,
	NODE_TYPE_PROGRAM,
	NODE_TYPE_PREFIX_EXPRESSION,
	NODE_TYPE_INFIX_EXPRESSION,
	NODE_TYPE_COUNT,
} Node_Type;

// A node in the abstract syntax tree. Can point to a token.
typedef struct Node {
	uint32_t next_index;
	uint32_t parent_index;
	uint32_t child_index; // Holds the index of a token if `type` is `NODE_TYPE_TOKEN`.
	Node_Type type;
} Node;

// The type of an error encountered during parsing.
typedef enum Parser_Error_Type {
	PARSER_ERROR_TYPE_INVALID_SYNTAX,
	PARSER_ERROR_TYPE_EXPECTED_MODULE_NAME,
	PARSER_ERROR_TYPE_EXPECTED_EXPRESSION,
	PARSER_ERROR_TYPE_COUNT,
} Parser_Error_Type;

// An error encountered during parsing.
typedef struct Parser_Error {
	uint32_t token_index;
	uint32_t token_count;
	Parser_Error_Type type;
} Parser_Error;

// The result of parsing a list of tokens. Must be destroyed with `Parser_Result_destroy()`.
typedef struct Parser_Result {
	Node *nodes; // Points to an arena. Owned by this struct.
	Parser_Error *errors; // Points to an arena. Owned by this struct.
} Parser_Result;

// A map of node types to their names. Indexed by the type of a node.
extern char *node_type_names[];

// A map of parser error types to their messages. Indexed by the type of an error.
extern char *parser_error_messages[];

// Destroys a `Parser_Result` and frees its arenas.
void Parser_Result_destroy(Parser_Result *result);

// Turns a list of tokens into an abstract syntax tree and reports any errors encountered. Return
// value must be destroyed with `Parser_Result_destroy()`. `tokens` must point to an arena.
Parser_Result parse(Token *tokens);

#endif // PARSER_H
