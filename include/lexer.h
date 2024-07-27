#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdio.h>
#include "list.h"

typedef List TokenList;

typedef List LexingErrorList;

// The type of thing a token represents.
typedef enum TokenType {
	INVALID,
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


	INCREMENT,
	PLUS,
	SEMICOLON,
} TokenType;

// The type of error the lexer encountered.
typedef enum LexingErrorType {
	INVALID_TOKEN,
	UNCLOSED_SINGLE_QUOTE,
	UNCLOSED_DOUBLE_QUOTE,
} LexingErrorType;

// A token from the input string. Can be a keyword, operator, literal, or identifier.
typedef struct Token {
	TokenType type;
	size_t length;
	char *text; // Points to the input string. Does not need to be freed.
	size_t index; // Index in the input string of the first character of `text`.
} Token;

// An error encountered during lexing. The message describes the reason the token it points to is
// invalid.
typedef struct LexingError {
	LexingErrorType type;
	char *message; // Points to one of the strings in `errorMessages`. Does not need to be freed.
	Token token;
} LexingError;

// The output of the lexer. Must be destroyed by `LexingResultDestroy()` after use.
typedef struct LexingResult {
	TokenList tokens;
	LexingErrorList errors;
} LexingResult;

// The error messages that can be generated during lexing.
extern char *lexingErrorMessages[];

// Deallocates a `LexingResult`'s lists and zeros its memory.
void LexingResultDestroy(LexingResult *result);

// Pretty prints a `LexingResult` to the terminal.
void LexingResultPrint(LexingResult *result);

// Splits `text` into tokens.
// TODO: Add a length argument.
LexingResult lex(char *text);

#endif // LEXER_H
