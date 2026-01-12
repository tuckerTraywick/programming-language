#include <stdio.h>

#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "list.h"

#define max(a, b) (((a) >= (b)) ? (a) : (b))

static const size_t initial_tokens_capacity = 1000;

static const size_t initial_lexer_errors_capacity = 100;

const char *const reserved_words[] = {
	// Literals
	[TOKEN_TYPE_NUMBER] = "number",
	[TOKEN_TYPE_CHARACTER] = "character",
	[TOKEN_TYPE_STRING] = "string",
	[TOKEN_TYPE_IDENTIFIER] = "identifier",
	// Keywords
	[TOKEN_TYPE_MODULE] = "module",
	[TOKEN_TYPE_IMPORT] = "import",
	[TOKEN_TYPE_VAR] = "var",
	[TOKEN_TYPE_FUNC] = "func",
	[TOKEN_TYPE_METHOD] = "method",
	[TOKEN_TYPE_TYPE] = "type",
	[TOKEN_TYPE_STRUCT] = "struct",
	[TOKEN_TYPE_TRAIT] = "trait",
	[TOKEN_TYPE_CASES] = "cases",
	[TOKEN_TYPE_EMBED] = "embed",
	[TOKEN_TYPE_PUB] = "pub",
	[TOKEN_TYPE_MUT] = "mut",
	[TOKEN_TYPE_OWNED] = "owned",
	[TOKEN_TYPE_WEAK] = "weak",
	[TOKEN_TYPE_AS] = "as",
	[TOKEN_TYPE_IS] = "is",
	[TOKEN_TYPE_BOOLEAN_AND] = "and",
	[TOKEN_TYPE_BOOLEAN_OR] = "or",
	[TOKEN_TYPE_BOOLEAN_XOR] = "xor",
	[TOKEN_TYPE_BOOLEAN_NOT] = "not",
	[TOKEN_TYPE_RETURN] = "return",
	[TOKEN_TYPE_BREAK] = "break",
	[TOKEN_TYPE_CONTINUE] = "continue",
	[TOKEN_TYPE_NEXT] = "next",
	[TOKEN_TYPE_DO] = "do",
	[TOKEN_TYPE_WHILE] = "while",
	[TOKEN_TYPE_FOR] = "for",
	[TOKEN_TYPE_IN] = "in",
	[TOKEN_TYPE_THRU] = "thru",
	[TOKEN_TYPE_UNTIL] = "until",
	[TOKEN_TYPE_BY] = "by",
	[TOKEN_TYPE_IF] = "if",
	[TOKEN_TYPE_ELSE] = "else",
	[TOKEN_TYPE_SWITCH] = "switch",
	[TOKEN_TYPE_CASE] = "case",
	[TOKEN_TYPE_DEFAULT] = "default",
	// Operators
	[TOKEN_TYPE_DOT] = ".",
	[TOKEN_TYPE_COMMA] = ",",
	[TOKEN_TYPE_COLON] = ":",
	[TOKEN_TYPE_SEMICOLON] = ";",
	[TOKEN_TYPE_LEFT_PARENTHESIS] = "(",
	[TOKEN_TYPE_RIGHT_PARENTHESIS] = ")",
	[TOKEN_TYPE_LEFT_BRACKET] = "[",
	[TOKEN_TYPE_RIGHT_BRACKET] = "]",
	[TOKEN_TYPE_LEFT_BRACE] = "{",
	[TOKEN_TYPE_RIGHT_BRACE] = "}",
	[TOKEN_TYPE_PLUS_ASSIGN] = "+=",
	[TOKEN_TYPE_PLUS] = "+",
	[TOKEN_TYPE_ARROW] = "->",
	[TOKEN_TYPE_MINUS_ASSIGN] = "-=",
	[TOKEN_TYPE_MINUS] = "-",
	[TOKEN_TYPE_TIMES_ASSIGN] = "*=",
	[TOKEN_TYPE_TIMES] = "*",
	[TOKEN_TYPE_DIVIDE_ASSIGN] = "/=",
	[TOKEN_TYPE_DIVIDE] = "/",
	[TOKEN_TYPE_MODULUS_ASSIGN] = "%=",
	[TOKEN_TYPE_MODULUS] = "%",
	[TOKEN_TYPE_BITWISE_AND_ASSIGN] = "&=",
	[TOKEN_TYPE_BITWISE_AND] = "&",
	[TOKEN_TYPE_BITWISE_OR_ASSIGN] = "|=",
	[TOKEN_TYPE_BITWISE_OR] = "|",
	[TOKEN_TYPE_BITWISE_XOR_ASSIGN] = "^=",
	[TOKEN_TYPE_BITWISE_XOR] = "^",
	[TOKEN_TYPE_BITWISE_NOT_ASSIGN] = "~=",
	[TOKEN_TYPE_BITWISE_NOT] = "~",
	[TOKEN_TYPE_LEFT_SHIFT_ASSIGN] = "<<=",
	[TOKEN_TYPE_LEFT_SHIFT] = "<<",
	[TOKEN_TYPE_RIGHT_SHIFT_ASSIGN] = ">>=",
	[TOKEN_TYPE_RIGHT_SHIFT] = ">>",
	[TOKEN_TYPE_EQUAL] = "==",
	[TOKEN_TYPE_ASSIGN] = "=",
	[TOKEN_TYPE_NOT_EQUAL] = "!=",
	[TOKEN_TYPE_GREATER_EQUAL] = ">=",
	[TOKEN_TYPE_GREATER] = ">",
	[TOKEN_TYPE_LESS_EQUAL] = "<=",
	[TOKEN_TYPE_LESS] = "<",
	[TOKEN_TYPE_LEFT_ANGLE_BRACKET] = "< bracket",
};

const char *const lexer_error_messages[] = {
	[LEXER_ERROR_TYPE_UNRECOGNIZED_TOKEN] = "Unrecognized token.",
	[LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE] = "Unclosed single quote.",
	[LEXER_ERROR_TYPE_UNCLOSED_DOUBLE_QUOTE] = "Unclosed double quote.",
};

bool lex(char *text, struct token **tokens, struct lexer_error **errors) {
	*tokens = list_create(initial_tokens_capacity, sizeof (struct token));
	if (!*tokens) {
		return false;
	}
	*errors = list_create(initial_lexer_errors_capacity, sizeof (struct lexer_error));
	if (!*errors) {
		list_destroy(tokens);
		*tokens = NULL;
		return false;
	}

	struct token current_token = {0};
	while (*text != '\0') {
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
				struct lexer_error error = {
					.text_index = current_token.text_index,
					.text_length = current_token.text_length,
					.type = LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE,
				};
				// TODO: Handle null return value.
				list_push_back(errors, &error);
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
				list_push_back(errors, &error);
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
			for (size_t i = TOKEN_TYPE_MODULE; i < TOKEN_TYPE_DOT; ++i) {
				// TODO: Store the lengths of the reserved words somewhere to avoid `strlen()`.
				if (strncmp(reserved_words[i], text - current_token.text_length, max(strlen(reserved_words[i]), current_token.text_length)) == 0) {
					current_token.type = i;
					break;
				}
			}
		// Lex operators.
		} else {
			bool found_operator = false;
			for (size_t i = TOKEN_TYPE_DOT; i < TOKEN_TYPE_LEFT_ANGLE_BRACKET; ++i) {
				// TODO: Store the lengths of the reserved words somewhere to avoid `strlen()`.
				if (strncmp(reserved_words[i], text, strlen(reserved_words[i])) == 0) {
					current_token.type = i;
					current_token.text_length = strlen(reserved_words[i]);
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
				list_push_back(errors, &error);
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
		list_push_back(tokens, &current_token);
		current_token.text_index += current_token.text_length;
		current_token.text_length = 0;
	}
	return true;
}

#undef max
