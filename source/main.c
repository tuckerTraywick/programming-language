#include <stdio.h>
#include <string.h>

#include "arena.h"
#include "lexer.h"
#include "parser.h"

static void Lexer_Result_print(Lexer_Result *result, char *text) {
	static char *token_type_names[] = {
		[TOKEN_TYPE_NUMBER] = "number",
		[TOKEN_TYPE_CHARACTER] = "character",
		[TOKEN_TYPE_STRING] = "string",
		[TOKEN_TYPE_IDENTIFIER] = "identifier",
		[TOKEN_TYPE_NAMESPACE] = "namespace",
		[TOKEN_TYPE_PLUS] = "plus",
	};
	static char *error_type_names[] = {
		[LEXER_ERROR_TYPE_UNRECOGNIZED_TOKEN] = "Unrecognized token.",
		[LEXER_ERROR_TYPE_UNCLOSED_SINGLE_QUOTE] = "Unclosed single quote.",
		[LEXER_ERROR_TYPE_UNCLOSED_DOUBLE_QUOTE] = "Unclosed double quote.",
	};
	printf("---- TOKENS ----\n");
	for (size_t i = 0; i < arena_get_size(result->tokens)/sizeof *result->tokens; ++i) {
		Token *token = result->tokens + i;
		printf("%s `%.*s`\n", token_type_names[token->type], token->text_length, text + token->text_index);
	}

	printf("\n---- LEXER ERRORS ----\n");
	for (size_t i = 0; i < arena_get_size(result->errors)/sizeof *result->errors; ++i) {
		Lexer_Error *error = result->errors + i;
		printf("%s `%.*s`\n", error_type_names[error->type], error->text_length, text + error->text_index);
	}
}

int main(void) {
	char *text = "\"hello -\"  \n123";
	Lexer_Result result = lex(text);
	Lexer_Result_print(&result, text);
	Lexer_Result_destroy(&result);
	return 0;
}
