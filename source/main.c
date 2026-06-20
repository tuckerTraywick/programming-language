#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include "list.h"
#include "map.h"

static void print_token(char *text, struct token *token) {
	if (token->type == TOKEN_TYPE_NEWLINE) {
		printf("%s", token_type_names[token->type]);
	} else {
		printf("%s `%.*s`", token_type_names[token->type], (int)token->text_length, text + token->text_index);
	}
}

static void print_tokens(char *text, struct token *tokens) {
	for (size_t i = 0; i < list_get_count(&tokens); ++i) {
		printf("%-5zu ", i);
		print_token(text, tokens + i);
		printf("\n");
	}
}

static void print_lexer_error(char *text, struct lexer_error *error) {
	printf("%s [Character %zu `%.*s`]", lexer_error_messages[error->type], error->text_index, (int)error->text_length, text + error->text_index);
}

static void print_lexer_errors(char *text, struct lexer_error *errors) {
	for (size_t i = 0; i < list_get_count(&errors); ++i) {
		print_lexer_error(text, errors + i);
		printf("\n");
	}
}

static void print_node(char *text, struct token *tokens, struct node *nodes, size_t first_node_index, size_t depth) {
	struct node *node = nodes + first_node_index;
	printf("%-5zu", first_node_index);
	for (size_t i = 0; i < depth; ++i) {
		printf("| ");
	}

	if (node->type == NODE_TYPE_TOKEN) {
		struct token *token = tokens + node->child_index;
		print_token(text, token);
		printf("  previous=%zu, next=%zu, parent=%zu, child=%zu\n", node->previous_index, node->next_index, node->parent_index, node->child_index);
		return;
	}

	printf("%s", node_type_names[node->type]);
	printf("  previous=%zu, next=%zu, parent=%zu, child=%zu\n", node->previous_index, node->next_index, node->parent_index, node->child_index);
	if (node->child_index == NODE_NONE) {
		return;
	}

	size_t node_index = node->child_index;
	do {
		print_node(text, tokens, nodes, node_index, depth + 1);
		node_index = nodes[node_index].next_index;
	} while (node_index != NODE_NONE);
}

static void print_parser_error(char *text, struct token *tokens, struct parser_error *error) {
	printf("%s [Token %zu: ", parser_error_messages[error->type], error->tokens_index - 1);
	print_token(text, tokens + error->tokens_index - 1);
	printf("]");
}

static void print_parser_errors(char *text, struct token *tokens, struct parser_error *errors) {
	for (size_t i = 0; i < list_get_count(&errors); ++i) {
		print_parser_error(text, tokens, errors + i);
		printf("\n");
	}
}

// static void print_object(struct object *object) {
// 	printf("smybols:\n");
// 	for (size_t i = 0; i < map_get_buckets_capacity(&object->symbols); ++i) {
// 		if (map_index_is_full(&object->symbols, i)) {
// 			char *name = map_get_key(&object->symbols, object->symbols + i);
// 			struct symbol_handle *symbol = map_get(&object->symbols, name);
// 			printf("%-5u%s\n", symbol->index, name);
// 		}
// 	}
// }

static void print_compiler_errors(struct node *nodes, struct compiler_error *errors) {
	for (size_t i = 0; i < list_get_count(&errors); ++i) {
		struct compiler_error *error = errors + i;
		struct node *node = nodes + error->node_index;
		printf("%s [Node %zu: %s]\n", compiler_error_messages[error->type], error->node_index, node_type_names[node->type]);
	}
}

int main(void) {
	char *text = "namespace ab .c\npub namespace b";
	printf("TEXT:\n%s\n", text);

	struct token *tokens = NULL;
	struct lexer_error *lexer_errors = NULL;
	lex(text, &tokens, &lexer_errors);
	if (!tokens || !lexer_errors) {
		printf("Memory error.\n");
		// TODO: Cleanup.
		return 1;
	}
	printf("TOKENS:\n");
	printf("tokens count = %zu\n", list_get_count(&tokens));
	print_tokens(text, tokens);
	printf("\nLEXER ERRORS:\n");
	print_lexer_errors(text, lexer_errors);
	printf("\n");
	
	struct node *nodes = NULL;
	struct parser_error *parser_errors = NULL;
	if (!parse(tokens, &nodes, &parser_errors)) {
		printf("FAILED PARSING\n");
		// TODO: Cleanup.
		return 1;
	}
	printf("NODES:\n");
	printf("nodes count = %zu\n", list_get_count(&nodes));
	print_node(text, tokens, nodes, 0, 0);
	printf("\nPARSER ERRORS:\n");
	print_parser_errors(text, tokens, parser_errors);
	printf("\n");

	struct compiler_error *compiler_errors = list_create(10, sizeof *compiler_errors);
	if (!compiler_errors) {
		// TODO: Cleanup.
		fprintf(stderr, "Memory error.\n");
		return 1;
	}
	initialize_symbols(text, tokens, nodes, NULL, &compiler_errors);

	printf("COMPILER ERRORS:\n");
	print_compiler_errors(nodes, compiler_errors);

	list_destroy(&tokens);
	list_destroy(&lexer_errors);
	list_destroy(&nodes);
	list_destroy(&parser_errors);
	list_destroy(&compiler_errors);
	return 0;
}
