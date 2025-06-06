#include <stdio.h>
#include <string.h>

#include "arena.h"
#include "lexer.h"
#include "parser.h"

static void Lexer_Result_print(Lexer_Result *result, char *text) {
	printf("---- TOKENS ----\n");
	for (size_t i = 0; i < arena_get_size(result->tokens)/sizeof *result->tokens; ++i) {
		Token *token = result->tokens + i;
		printf("%s `%.*s`\n", reserved_words[token->type], token->text_length, text + token->text_index);
	}

	printf("\n---- LEXER ERRORS ----\n");
	for (size_t i = 0; i < arena_get_size(result->errors)/sizeof *result->errors; ++i) {
		Lexer_Error *error = result->errors + i;
		printf("%s `%.*s`\n", lexer_error_messages[error->type], error->text_length, text + error->text_index);
	}
}

int main(void) {
	char *text = "+=";
	Lexer_Result result = lex(text);
	Lexer_Result_print(&result, text);
	Lexer_Result_destroy(&result);
	return 0;
}
