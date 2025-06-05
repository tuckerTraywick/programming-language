#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>
#include "arena.h"

typedef enum Token_Type {
	// Literals
	NUMBER,
	CHARACTER,
	STRING,
	IDENTIFIER,
	// Keywords
	NAMESPACE,
	// Operators
	PLUS,
} Token_Type;

typedef struct Token {
	uint32_t text_index;
	uint32_t text_length;
	Token_Type type;
} Token;

#endif // LEXER_H
