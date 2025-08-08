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
	TOKEN_TYPE_MODULE,
	TOKEN_TYPE_IMPORT,
	TOKEN_TYPE_VAR,
	TOKEN_TYPE_FUNC,
	TOKEN_TYPE_METHOD,
	TOKEN_TYPE_TYPE,
	TOKEN_TYPE_STRUCT,
	TOKEN_TYPE_TRAIT,
	TOKEN_TYPE_CASES,
	TOKEN_TYPE_EMBED,
	TOKEN_TYPE_PUB,
	TOKEN_TYPE_MUT,
	TOKEN_TYPE_AS,
	TOKEN_TYPE_IS,
	TOKEN_TYPE_BOOLEAN_AND,
	TOKEN_TYPE_BOOLEAN_OR,
	TOKEN_TYPE_BOOLEAN_XOR,
	TOKEN_TYPE_BOOLEAN_NOT,
	TOKEN_TYPE_RETURN,
	TOKEN_TYPE_BREAK,
	TOKEN_TYPE_CONTINUE,
	TOKEN_TYPE_NEXT,
	TOKEN_TYPE_DO,
	TOKEN_TYPE_WHILE,
	TOKEN_TYPE_FOR,
	TOKEN_TYPE_IN,
	TOKEN_TYPE_THRU,
	TOKEN_TYPE_UNTIL,
	TOKEN_TYPE_BY,
	TOKEN_TYPE_IF,
	TOKEN_TYPE_ELSE,
	TOKEN_TYPE_SWITCH,
	TOKEN_TYPE_CASE,
	TOKEN_TYPE_DEFAULT,
	// Operators
	TOKEN_TYPE_DOT,
	TOKEN_TYPE_COMMA,
	TOKEN_TYPE_COLON,
	TOKEN_TYPE_SEMICOLON,
	TOKEN_TYPE_LEFT_PARENTHESIS,
	TOKEN_TYPE_RIGHT_PARENTHESIS,
	TOKEN_TYPE_LEFT_BRACKET,
	TOKEN_TYPE_RIGHT_BRACKET,
	TOKEN_TYPE_LEFT_BRACE,
	TOKEN_TYPE_RIGHT_BRACE,
	TOKEN_TYPE_PLUS_ASSIGN,
	TOKEN_TYPE_PLUS,
	TOKEN_TYPE_ARROW,
	TOKEN_TYPE_MINUS_ASSIGN,
	TOKEN_TYPE_MINUS,
	TOKEN_TYPE_TIMES_ASSIGN,
	TOKEN_TYPE_TIMES,
	TOKEN_TYPE_DIVIDE_ASSIGN,
	TOKEN_TYPE_DIVIDE,
	TOKEN_TYPE_MODULUS_ASSIGN,
	TOKEN_TYPE_MODULUS,
	TOKEN_TYPE_BITWISE_AND_ASSIGN,
	TOKEN_TYPE_BITWISE_AND,
	TOKEN_TYPE_BITWISE_OR_ASSIGN,
	TOKEN_TYPE_BITWISE_OR,
	TOKEN_TYPE_BITWISE_XOR_ASSIGN,
	TOKEN_TYPE_BITWISE_XOR,
	TOKEN_TYPE_BITWISE_NOT_ASSIGN,
	TOKEN_TYPE_BITWISE_NOT,
	TOKEN_TYPE_LEFT_SHIFT_ASSIGN,
	TOKEN_TYPE_LEFT_SHIFT,
	TOKEN_TYPE_RIGHT_SHIFT_ASSIGN,
	TOKEN_TYPE_RIGHT_SHIFT,
	TOKEN_TYPE_EQUAL,
	TOKEN_TYPE_ASSIGN,
	TOKEN_TYPE_NOT_EQUAL,
	TOKEN_TYPE_GREATER_EQUAL,
	TOKEN_TYPE_GREATER,
	TOKEN_TYPE_LESS_EQUAL,
	TOKEN_TYPE_LESS,
	TOKEN_TYPE_LEFT_ANGLE_BRACKET,
	TOKEN_TYPE_COUNT,
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
	LEXER_ERROR_TYPE_COUNT,
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

// A map from token types to their string values. Indexed by the type of a token.
extern char *reserved_words[];

// A map from lexer error types to their error messages. Indexed by the type of an error.
extern char *lexer_error_messages[];

// Destroys a `Lexer_Result` and frees its arenas.
void Lexer_Result_destroy(Lexer_Result *result);

// Splits a string into tokens and reports any errors. Return value must be destroyed with
// `Lexer_Result_destroy()`.
Lexer_Result lex(char *text);

#endif // LEXER_H
