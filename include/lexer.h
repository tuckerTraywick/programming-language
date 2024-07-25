#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdio.h>
#include "list.h"

typedef List TokenList;

typedef List LexingErrorList;

typedef enum TokenType {
	LEXING_ERROR,
	IDENTIFIER,
	NUMBER,
	CHARACTER,
	STRING,
	KEYWORD,
	OPERATOR,
} TokenType;

typedef struct Token {
	TokenType type;
	char *text;
	size_t length;
} Token;

typedef struct LexingError {
	char *message;
	char *text;
	size_t textLength;
} LexingError;

typedef struct LexingResult {
	TokenList tokens;
	LexingErrorList errors;
} LexingResult;

LexingResult lex(char *text, size_t length);

#endif // LEXER_H
