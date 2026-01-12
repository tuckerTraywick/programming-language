#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
// #include "map.h"
#include "lexer.h"
#include "parser.h"

// static void print_node(Node *nodes, Token *tokens, char *text, uint32_t node_index, uint32_t depth) {
// 	Node *node = nodes + node_index;
// 	for (uint32_t i = 0; i < depth; ++i) {
// 		printf("| ");
// 	}

// 	if (node->type == NODE_TYPE_TOKEN) {
// 		Token *token = tokens + node->child_index;
// 		printf("%s `%.*s`", reserved_words[token->type], token->text_length, text + token->text_index);
// 		return;
// 	}
	
// 	printf("%s", node_type_names[node->type]);
// 	uint32_t child = node->child_index;
// 	while (child) {
// 		printf("\n");
// 		print_node(nodes, tokens, text, child, depth + 1);
// 		child = nodes[child].next_index;
// 	}
// }

// static void Parser_Result_print(Parser_Result *result, Token *tokens, char *text) {
// 	printf("\n---- NODES ----\n");
// 	print_node(result->nodes, tokens, text, 0, 0);

// 	printf("\n\n---- PARSER ERRORS ----\n");
// 	for (size_t i = 0; i < list_get_size(result->errors); ++i) {
// 		Parser_Error *error = result->errors + i;
// 		printf("%s (starting at token %d)\n", parser_error_messages[error->type], error->token_index);
// 	}
// }

static void print_token(char *text, struct token *token) {
	printf("`%.*s`", (int)token->text_length, text + token->text_index);
}

static void print_tokens(char *text, struct token *tokens) {
	for (size_t i = 0; i < list_get_buckets_count(&tokens); ++i) {
		print_token(text, tokens + i);
		printf("\n");
	}
}

static void print_lexer_error(char *text, struct lexer_error *error) {
	printf("%s `%.*s`", lexer_error_messages[error->type], (int)error->text_length, text + error->text_index);
}

static void print_lexer_errors(char *text, struct lexer_error *errors) {
	for (size_t i = 0; i < list_get_buckets_count(&errors); ++i) {
		print_lexer_error(text, errors + i);
		printf("\n");
	}
}

int main(void) {
	char *text = "123";
	struct token *tokens = NULL;
	struct lexer_error *lexer_errors = NULL;
	if (!lex(text, &tokens, &lexer_errors)) {
		printf("Failed lexing.\n");
		return 1;
	}
	printf("tokens:\n");
	print_tokens(text, tokens);
	printf("\nlexer errors:\n");
	print_lexer_errors(text, lexer_errors);

	struct node *nodes = NULL;
	struct parser_error *parser_errors = NULL;
	if (!parse(tokens, &nodes, &parser_errors)) {
		printf("Failed parsing.\n");
		list_destroy(&tokens);
		list_destroy(&lexer_errors);
		return 1;
	}
	
	list_destroy(&tokens);
	list_destroy(&lexer_errors);
	list_destroy(&nodes);
	list_destroy(&parser_errors);
	return 0;
}
