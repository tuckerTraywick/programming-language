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
	IDENTIFIER,
	NUMBER,
	CHARACTER,
	STRING,

	PACKAGE,

	PLUS,

	TOKENTYPE_COUNT,
} TokenType;

// The type of error the lexer encountered.
typedef enum LexingErrorType {
	INVALID_TOKEN,
} LexingErrorType;

// A token from the input string. Can be a keyword, operator, literal, or identifier.
typedef struct Token {
	TokenType type;
	size_t length;
	char *text; // Points to the input string. Does not need to be freed.
} Token;

// An error encountered during lexing. The message describes the reason the token it points to is
// invalid.
typedef struct LexingError {
	LexingErrorType type;
	char *message; // Points to one of the strings in `errorMessages`. Does not need to be freed.
	Token *token; // Points to the list of tokens outputted by the lexer. Does not need to be freed.
} LexingError;

// The output of the lexer. Must be destroyed by `LexingResultDestroy()` after use.
typedef struct LexingResult {
	TokenList tokens;
	LexingErrorList errors;
} LexingResult;

// The error messages that can be generated during lexing.
extern char *errorMessages[];

// Deallocates a `LexingResult`'s lists and zeros its memory.
void LexingResultDestroy(LexingResult *result);

// Pretty prints a `LexingResult` to the terminal.
void LexingResultPrint(LexingResult *result);

// Splits `text` into tokens, stops lexing at index `length`. If `length` is 0, stops lexing at the
// first '\0' character it encounters..
LexingResult lex(char *text, size_t length);

#endif // LEXER_H
