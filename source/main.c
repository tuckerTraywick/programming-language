#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
// #include "map.h"
#include "lexer.h"
#include "parser.h"

static void print_token(char *text, struct token *token) {
	printf("%s `%.*s`", reserved_words[token->type], (int)token->text_length, text + token->text_index);
}

static void print_tokens(char *text, struct token *tokens) {
	for (size_t i = 0; i < list_get_buckets_count(&tokens); ++i) {
		printf("%zu ", i);
		print_token(text, tokens + i);
		printf("\n");
	}
}

static void print_lexer_error(char *text, struct lexer_error *error) {
	printf("%s [Character %zu `%.*s`]", lexer_error_messages[error->type], error->text_index, (int)error->text_length, text + error->text_index);
}

static void print_lexer_errors(char *text, struct lexer_error *errors) {
	for (size_t i = 0; i < list_get_buckets_count(&errors); ++i) {
		print_lexer_error(text, errors + i);
		printf("\n");
	}
}

static void print_node(char *text, struct token *tokens, struct node *nodes, struct node *node, size_t depth) {
	for (size_t i = 0; i < depth; ++i) {
		printf("| ");
	}

	if (node->type == NODE_TYPE_TOKEN) {
		struct token *token = tokens + node->child_index;
		print_token(text, token);
		printf("\n");
		return;
	}

	printf("%s\n", node_type_names[node->type]);
	if (node->child_index == 0) {
		return;
	}

	size_t node_index = node->child_index;
	do {
		print_node(text, tokens, nodes, nodes + node_index, depth + 1);
		node_index = nodes[node_index].next_index;
	} while (node_index);
}

static void print_parser_error(char *text, struct token *tokens, struct parser_error *error) {
	printf("%s [Token %zu ", parser_error_messages[error->type], error->token_index - 1);
	print_token(text, tokens + error->token_index - 1);
	printf("]");
}

static void print_parser_errors(char *text, struct token *tokens, struct parser_error *errors) {
	for (size_t i = 0; i < list_get_buckets_count(&errors); ++i) {
		print_parser_error(text, tokens, errors + i);
		printf("\n");
	}
}

int main(void) {
	char *text = "module a; import 'std.;\nj";
	struct token *tokens = NULL;
	struct lexer_error *lexer_errors = NULL;
	if (!lex(text, &tokens, &lexer_errors)) {
		printf("FAILED LEXING\n");
		return 1;
	}
	printf("TOKENS:\n");
	print_tokens(text, tokens);
	printf("\nLEXER ERRORS:\n");
	print_lexer_errors(text, lexer_errors);
	printf("\n");

	struct node *nodes = NULL;
	struct parser_error *parser_errors = NULL;
	if (!parse(tokens, &nodes, &parser_errors)) {
		printf("FAILED PARSING\n");
		list_destroy(&tokens);
		list_destroy(&lexer_errors);
		return 1;
	}
	printf("NODES:\n");
	print_node(text, tokens, nodes, nodes, 0);
	printf("\nPARSER ERRORS:\n");
	print_parser_errors(text, tokens, parser_errors);
	printf("\n");
	
	list_destroy(&tokens);
	list_destroy(&lexer_errors);
	list_destroy(&nodes);
	list_destroy(&parser_errors);
	return 0;
}
