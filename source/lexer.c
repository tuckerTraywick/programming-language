#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "token.h"
#include "object.h"
#include "list.h"

#define max(a, b) (((a) >= (b)) ? (a) : (b))

bool lex(struct object *object) {
	char *text = object->text;
	struct token current_token = {0};
	while (*text) {
		// Skip whitespace.
		if (isspace(*text)) {
			++text;
			++current_token.text_index;
			continue;
		// Skip line comments.
		} else if (*text == '/' && *(text + 1) == '/') {
			do {
				++text;
				++current_token.text_index;
			} while (*text && *text != '\n');
			continue;
		// Lex numbers.
		} else if (isdigit(*text)) {
			do {
				++text;
				++current_token.text_length;
			} while (*text && isdigit(*text));
			current_token.type = TOKEN_TYPE_NUMBER;
		// Lex characters.
		} else if (*text == '\'') {
			do {
				++text;
				++current_token.text_length;
				// TODO: Handle escape characters and check length.
			} while (*text && *text != '\'' && *text != '\r' && *text != '\n');
			if (*text != '\'') {
				struct lexer_error error = {
					.text_index = current_token.text_index,
					.text_length = current_token.text_length,
					.type = LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE,
				};
				// TODO: Handle null return value.
				list_push_back(&object->lexer_errors, &error);
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
				struct lexer_error error = {
					.text_index = current_token.text_index,
					.text_length = current_token.text_length,
					.type = LEXER_ERROR_TYPE_UNCLOSED_DOUBLE_QUOTE,
				};
				// TODO: Handle null return value.
				list_push_back(&object->lexer_errors, &error);
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
			} while (*text && isalnum(*text) || *text == '_');
			current_token.type = TOKEN_TYPE_IDENTIFIER;
			for (size_t i = TOKEN_TYPE_MODULE; i < TOKEN_TYPE_DOT; ++i) {
				// TODO: Store the lengths of the reserved words somewhere to avoid `strlen()`.
				if (strncmp(token_type_names[i], text - current_token.text_length, max(strlen(token_type_names[i]), current_token.text_length)) == 0) {
					current_token.type = i;
					break;
				}
			}
		// Lex operators.
		} else {
			bool found_operator = false;
			for (size_t i = TOKEN_TYPE_DOT; i < TOKEN_TYPE_LEFT_ANGLE_BRACKET; ++i) {
				// TODO: Store the lengths of the reserved words somewhere to avoid `strlen()`.
				if (strncmp(token_type_names[i], text, strlen(token_type_names[i])) == 0) {
					current_token.type = i;
					current_token.text_length = strlen(token_type_names[i]);
					text += current_token.text_length;
					found_operator = true;
					break;
				}
			}

			if (!found_operator) {
				struct lexer_error error = {
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
				list_push_back(&object->lexer_errors, &error);
				current_token.text_index += error.text_length;
				current_token.text_length = 0;
				continue;
			}

			// Distinguish between less than operator and generic brackets.
			if (current_token.type == TOKEN_TYPE_LESS && current_token.text_index > 0 && *(text - 2) != ' ') {
				current_token.type = TOKEN_TYPE_LEFT_ANGLE_BRACKET;
			}
		}
		// TODO: Handle null return value.
		list_push_back(&object->tokens, &current_token);
		current_token.text_index += current_token.text_length;
		current_token.text_length = 0;
	}
	return true;
}

#undef max
