#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdio.h>

typedef enum TokenType {
	INVALID,
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
} Token;

typedef struct TokenList {
	size_t count;
	Token *tokens;
} TokenList;

TokenList lex(char *text, size_t length);

#endif // LEXER_H
