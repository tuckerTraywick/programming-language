#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "lexer.h"

typedef enum Node_Type {
	NODE_TYPE_TOKEN,
	NODE_TYPE_PROGRAM,
} Node_Type;

typedef struct Node {
	uint32_t next_index;
	uint32_t child_index; // Holds the index of a token if `type` is `NODE_TYPE_TOKEN`.
	Node_Type type;
} Node;

typedef enum Parser_Error_Type {
	INVALID_SYNTAX,
} Parser_Error_Type;

typedef struct Parser_Error {
	uint32_t token_index;
	uint32_t token_count;
	Parser_Error_Type type;
} Parser_Error;

typedef struct Parser_Result {
	Node *nodes; // Points to an arena. Owned by this struct.
	Parser_Error *errors; // Points to an arena. Owned by this struct.
} Parser_Result;

void Parser_Result_destroy(Parser_Result *result);

// `tokens` must point to an arena.
Parser_Result parse(Token *tokens);

#endif // PARSER_H
