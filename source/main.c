#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "lexer.h"
#include "parser.h"

static void Lexer_Result_print(Lexer_Result *result, char *text) {
	printf("---- TOKENS ----\n");
	for (size_t i = 0; i < list_get_size(result->tokens); ++i) {
		Token *token = result->tokens + i;
		printf("%zu %s `%.*s`\n", i, reserved_words[token->type], token->text_length, text + token->text_index);
	}

	printf("\n---- LEXER ERRORS ----\n");
	for (size_t i = 0; i < list_get_size(result->errors); ++i) {
		Lexer_Error *error = result->errors + i;
		printf("%s `%.*s`\n", lexer_error_messages[error->type], error->text_length, text + error->text_index);
	}
}

static void print_node(Node *nodes, Token *tokens, char *text, uint32_t node_index, uint32_t depth) {
	Node *node = nodes + node_index;
	for (uint32_t i = 0; i < depth; ++i) {
		printf("| ");
	}

	if (node->type == NODE_TYPE_TOKEN) {
		Token *token = tokens + node->child_index;
		printf("%s `%.*s`", reserved_words[token->type], token->text_length, text + token->text_index);
		return;
	}
	
	printf("%s", node_type_names[node->type]);
	uint32_t child = node->child_index;
	while (child) {
		printf("\n");
		print_node(nodes, tokens, text, child, depth + 1);
		child = nodes[child].next_index;
	}
}

static void Parser_Result_print(Parser_Result *result, Token *tokens, char *text) {
	printf("\n---- NODES ----\n");
	print_node(result->nodes, tokens, text, 0, 0);

	printf("\n\n---- PARSER ERRORS ----\n");
	for (size_t i = 0; i < list_get_size(result->errors); ++i) {
		Parser_Error *error = result->errors + i;
		printf("%s (starting at token %d)\n", parser_error_messages[error->type], error->token_index);
	}
}

int main(void) {
	char *text = "var a int32 = a->c.d->e().b;";
	Lexer_Result lexer_result = lex(text);
	Lexer_Result_print(&lexer_result, text);

	Parser_Result parser_result = parse(lexer_result.tokens);
	Parser_Result_print(&parser_result, lexer_result.tokens, text);

	Lexer_Result_destroy(&lexer_result);
	Parser_Result_destroy(&parser_result);
	return 0;
}
