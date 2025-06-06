#include <assert.h>
#include <stdio.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "arena.h"

#define STARTING_TOKEN_CAPACITY 2000

#define STARTING_ERROR_CAPACITY 100

Lexer_Result lex(char *text) {
	static char *keywords[] = {
		[TOKEN_TYPE_NAMESPACE] = "namespace",
	};
	static char *operators[] = {
		[TOKEN_TYPE_PLUS] = "+",
	};

	Lexer_Result result = {
		.tokens = arena_create(STARTING_TOKEN_CAPACITY*sizeof (Token)),
		.errors = arena_create(STARTING_ERROR_CAPACITY*sizeof (Lexer_Error)),
	};
	assert(result.tokens && result.errors && "Failed malloc.");

	Token current_token = {0};
	while (*text != '\0') {
		// Skip whitespace.
		if (isspace(*text)) {
			++text;
			++current_token.text_index;
			continue;
		// Lex numbers.
		} else if (isdigit(*text)) {
			do {
				++text;
				++current_token.text_length;
			} while (isdigit(*text));
			current_token.type = TOKEN_TYPE_NUMBER;
		// Lex characters.
		} else if (*text == '\'') {
			do {
				++text;
				++current_token.text_length;
				// TODO: Handle escape characters and check length.
			} while (*text && *text != '\'' && *text != '\r' && *text != '\n');
			if (*text != '\'') {
				Lexer_Error error = {
					.text_index = current_token.text_index,
					.text_length = current_token.text_length,
					.type = LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE,
				};
				// TODO: Handle null return value.
				arena_push(result.errors, &error, sizeof error);
				current_token.text_index += error.text_length;
				current_token.text_length = 0;
				continue;
			}
			++text;
			++current_token.text_length;
			current_token.type = TOKEN_TYPE_CHARACTER;
		// Lex strings.
		} else if (*text == '"') {
			do {
				++text;
				++current_token.text_length;
				// TODO: Handle escape characters and check length.
			} while (*text && *text != '"' && *text != '\r' && *text != '\n');
			if (*text != '"') {
				Lexer_Error error = {
					.text_index = current_token.text_index,
					.text_length = current_token.text_length,
					.type = LEXER_ERROR_TYPE_UNCLOSED_DOUBLE_QUOTE,
				};
				// TODO: Handle null return value.
				arena_push(result.errors, &error, sizeof error);
				current_token.text_index += error.text_length;
				current_token.text_length = 0;
				continue;
			}
			++text;
			++current_token.text_length;
			current_token.type = TOKEN_TYPE_STRING;
		// Lex identifiers and keywords.
		} else if (isalpha(*text) || *text == '_') {
			do {
				++text;
				++current_token.text_length;
			} while (isalnum(*text) || *text == '_');
			current_token.type = TOKEN_TYPE_IDENTIFIER;
			for (size_t i = TOKEN_TYPE_NAMESPACE; i < (sizeof keywords)/(sizeof keywords[0]); ++i) {
				// TODO: Store the lengths of the keywords somewhere to avoid `strlen()`.
				if (strcmp(keywords[i], text - current_token.text_length) == 0) {
					current_token.type = i;
					break;
				}
			}
		// Lex operators.
		} else {
			bool found_operator = false;
			for (size_t i = TOKEN_TYPE_PLUS; i < (sizeof operators)/(sizeof operators[0]); ++i) {
				// TODO: Store the lengths of the operators somewhere to avoid `strlen()`.
				if (strncmp(operators[i], text, strlen(operators[i])) == 0) {
					current_token.type = i;
					current_token.text_length = strlen(operators[i]);
					text += current_token.text_length;
					found_operator = true;
					break;
				}
			}

			if (!found_operator) {
				Lexer_Error error = {
					.text_index = current_token.text_index,
					.text_length = 0,
					.type = LEXER_ERROR_TYPE_UNRECOGNIZED_TOKEN,
				};
				// Keep consuming characters until the next whitespace to recover from the error.
				while (*text && !isspace(*text)) {
					++text;
					++error.text_length;
				}
				// TODO: Handle null return value.
				arena_push(result.errors, &error, sizeof error);
				current_token.text_index += error.text_length;
				current_token.text_length = 0;
				continue;
			}
		}
		// TODO: Handle null return value.
		arena_push(result.tokens, &current_token, sizeof current_token);
		current_token.text_index += current_token.text_length;
		current_token.text_length = 0;
	}
	return result;
}

void Lexer_Result_destroy(Lexer_Result *result) {
	arena_destroy(result->tokens);
	arena_destroy(result->errors);
	*result = (Lexer_Result){0};
}
