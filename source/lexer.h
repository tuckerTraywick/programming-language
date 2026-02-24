#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdbool.h>
#include "token.h"

enum lexer_error_type {
	LEXER_ERROR_TYPE_UNRECOGNIZED_TOKEN,
	LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE,
	LEXER_ERROR_TYPE_UNCLOSED_DOUBLE_QUOTE,
	LEXER_ERROR_TYPE_COUNT,
};

struct lexer_error {
	size_t text_index;
	size_t text_length;
	enum lexer_error_type type;
};

// A map from lexer error types to their error messages. Indexed by the type of an error.
extern const char *const lexer_error_messages[];

// Lexes `text` and returns a list of tokens in `tokens` and a list of errors in `errors`. Returns
// true if no memory errors occurred, false otherwise.
bool lex(char *text, struct token **tokens, struct lexer_error **errors);

#endif // LEXER_H
