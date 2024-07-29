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
	PLUS_EQUALS,
	PLUS,
	DECREMENT,
	MINUS_EQUALS,
	ARROW,
	MINUS,
	TIMES_EQUALS,
	TIMES,
	DIVIDE_EQUALS,
	DIVIDE,
	LEFT_SHIFT_EQUALS,
	LEFT_SHIFT,
	LESS_EQUAL,
	LESS,
	RIGHT_SHIFT_EQUALS,
	RIGHT_SHIFT,
	GREATER_EQUAL,
	GREATER,
	BITWISE_AND_EQUALS,
	BITWISE_AND,
	BITWISE_OR_EQUALS,
	BITWISE_OR,
	BITWISE_XOR_EQUALS,
	BITWISE_XOR,
	BITWISE_NOT_EQUALS,
	BITWISE_NOT,
	COMMA,
	DOT,
	SEMICOLON,
	LEFT_PARENTHESIS,
	RIGHT_PARENTHESIS,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	LEFT_BRACE,
	RIGHT_BRACE,
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
