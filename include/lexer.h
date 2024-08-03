#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdio.h>
#include "list.h"

typedef List TokenList;

typedef List LexingErrorList;

// The type of thing a token represents.
typedef enum TokenType {
	INVALID_TOKEN,
	UNCLOSED_SINGLE_QUOTE,
	UNCLOSED_DOUBLE_QUOTE,

	NEWLINE,
	NUMBER,
	CHARACTER,
	STRING,
	IDENTIFIER,

	PACKAGE,
	FROM,
	IMPORT,
	EXPORT,
	VAR,
	FUNC,
	METHOD,
	STRUCT,
	CASES,
	PUB,
	STATIC,
	CONST,
	MUT,
	OWNED,
	WEAK,
	NEW,
	MAKE,
	DROP,
	MOVE,
	DEFER,
	ASSERT,
	WHERE,
	WHILE,
	FOR,
	IF,
	SWITCH,
	MATCH,
	DEFAULT,
	RETURN,
	BREAK,
	CONTINUE,
	AS,
	IS,
	LOGICAL_AND,
	LOGICAL_OR,
	LOGICAL_XOR,
	LOGICAL_NOT,

	INCREMENT,
	PLUS_ASSIGN,
	PLUS,
	DECREMENT,
	MINUS_ASSIGN,
	ARROW,
	MINUS,
	TIMES_ASSIGN,
	TIMES,
	DIVIDE_ASSIGN,
	DIVIDE,
	LEFT_SHIFT_ASSIGN,
	LEFT_SHIFT,
	LESS_EQUAL,
	LESS,
	RIGHT_SHIFT_ASSIGN,
	RIGHT_SHIFT,
	GREATER_EQUAL,
	GREATER,
	BITWISE_AND_ASSIGN,
	BITWISE_AND,
	BITWISE_OR_ASSIGN,
	BITWISE_OR,
	BITWISE_XOR_ASSIGN,
	BITWISE_XOR,
	BITWISE_NOT_ASSIGN,
	BITWISE_NOT,
	EQUAL,
	ASSIGN,
	NOT_EQUAL,
	COMMA,
	DOT,
	SEMICOLON,
	LEFT_PARENTHESIS,
	RIGHT_PARENTHESIS,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	LEFT_BRACE,
	RIGHT_BRACE,

	TOKEN_TYPE_COUNT,
} TokenType;

// A token from the input string. Can be a keyword, operator, literal, or identifier.
typedef struct Token {
	TokenType type;
	size_t length;
	char *text; // Points to the input string. Does not need to be freed.
	size_t index; // Index in the input string of the first character of `text`.
} Token;

// The output of the lexer. Must be destroyed by `LexingResultDestroy()` after use.
typedef struct LexingResult {
	TokenList tokens;
	TokenList errors;
} LexingResult;

// Deallocates a `LexingResult`'s lists and zeros its memory.
void LexingResultDestroy(LexingResult *result);

// Pretty prints a `LexingResult` to the terminal.
void LexingResultPrint(LexingResult *result);

// Splits `text` into tokens.
// TODO: Add a length argument.
LexingResult lex(char *text);

#endif // LEXER_H
