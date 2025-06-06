#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

// The type of thing a token represents.
typedef enum Token_Type {
	// Literals
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_CHARACTER,
	TOKEN_TYPE_STRING,
	TOKEN_TYPE_IDENTIFIER,
	// Keywords
	TOKEN_TYPE_NAMESPACE,
	// Operators
	TOKEN_TYPE_PLUS,
} Token_Type;

// A span of characters in the input text.
typedef struct Token {
	uint32_t text_index;
	uint32_t text_length;
	Token_Type type;
} Token;

// The type of an error encountered during lexing.
typedef enum Lexer_Error_Type {
	LEXER_ERROR_TYPE_UNRECOGNIZED_TOKEN,
	LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE,
	LEXER_ERROR_TYPE_UNCLOSED_DOUBLE_QUOTE,
} Lexer_Error_Type;

// An error encountered during lexing.
typedef struct Lexer_Error {
	uint32_t text_index;
	uint32_t text_length;
	Lexer_Error_Type type;
} Lexer_Error;

// The result of lexing a string. Must be destroyed with `Lexer_Result_destroy()`.
typedef struct Lexer_Result {
	Token *tokens; // Points to an arena. Owned by this struct.
	Lexer_Error *errors; // Points to an arena. Owned by this struct.
} Lexer_Result;

// Destroys a `Lexer_Result` and frees its arenas.
void Lexer_Result_destroy(Lexer_Result *result);

// Splits a string into tokens and reports any errors.
Lexer_Result lex(char *text);

#endif // LEXER_H
