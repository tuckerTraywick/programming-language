#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
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

static void print_lexing_error(char *text, struct lexing_error *error) {
	printf("%s [Character %u `%.*s`]", lexing_error_messages[error->type], error->text_index, (int)error->text_length, text + error->text_index);
}

static void print_lexing_errors(char *text, struct lexing_error *errors) {
	for (size_t i = 0; i < list_get_count(&errors); ++i) {
		print_lexing_error(text, errors + i);
		printf("\n");
	}
}

static void print_node(char *text, struct token *tokens, struct node *nodes, uint32_t first_node_index, size_t depth) {
	struct node *node = nodes + first_node_index;
	printf("%-5u", first_node_index);
	for (size_t i = 0; i < depth; ++i) {
		printf("| ");
	}

	if (node->type == NODE_TYPE_TOKEN) {
		struct token *token = tokens + node->child_index;
		print_token(text, token);
		printf("  previous=%u, next=%u, parent=%u, child=%u\n", node->previous_index, node->next_index, node->parent_index, node->child_index);
		// printf("\n");
		return;
	}

	printf("%s", node_type_names[node->type]);
	printf("  previous=%u, next=%u, parent=%u, child=%u\n", node->previous_index, node->next_index, node->parent_index, node->child_index);
	if (node->child_index == NODE_NONE) {
		return;
	}

	size_t node_index = node->child_index;
	do {
		print_node(text, tokens, nodes, node_index, depth + 1);
		node_index = nodes[node_index].next_index;
	} while (node_index != NODE_NONE);
}

static void print_parsing_error(char *text, struct token *tokens, struct parsing_error *error) {
	printf("%s [Token %u: ", parsing_error_messages[error->type], error->tokens_index - 1);
	print_token(text, tokens + error->tokens_index - 1);
	printf("]");
}

static void print_parsing_errors(char *text, struct token *tokens, struct parsing_error *errors) {
	for (size_t i = 0; i < list_get_count(&errors); ++i) {
		print_parsing_error(text, tokens, errors + i);
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

// static void print_compiler_errors(struct object *object) {
// 	for (size_t i = 0; i < list_get_count(&object->compiler_errors); ++i) {
// 		struct compiler_error *error = object->compiler_errors + i;
// 		struct node_handle *node = object->nodes + error->node_index;
// 		printf("%s [Node %u: %s]\n", compiler_error_messages[error->type], error->node_index, node_type_names[node->type]);
// 	}
// }

int main(void) {
	// struct object *object = object_create();
	// if (!object) {
	// 	printf("FAILED TO CREATE OBJECT\n");
	// 	return 1;
	// }
	// FILE *file = fopen("example.txt", "r");
	// if (!file) {
	// 	printf("FAILED TO OPEN FILE\n");
	// 	object_destroy(object);
	// 	return 1;
	// }
	// object_read_text_from_file(object, file);
	// fclose(file);
	
	char *text = "module a 1";
	struct token *tokens = NULL;
	struct lexing_error *lexing_errors = NULL;
	lex(text, &tokens, &lexing_errors);
	if (!tokens || !lexing_errors) {
		printf("Memory error.\n");
		// TODO: Cleanup.
		return 1;
	}
	printf("TOKENS:\n");
	printf("tokens count = %zu\n", list_get_count(&tokens));
	print_tokens(text, tokens);
	printf("\nLEXER ERRORS:\n");
	print_lexing_errors(text, lexing_errors);
	printf("\n");
	
	struct node *nodes = NULL;
	struct parsing_error *parsing_errors = NULL;
	if (!parse(tokens, &nodes, &parsing_errors)) {
		printf("FAILED PARSING\n");
		// TODO: Cleanup.
		return 1;
	}
	printf("NODES:\n");
	printf("nodes count = %zu\n", list_get_count(&nodes));
	print_node(text, tokens, nodes, 0, 0);
	printf("\nPARSER ERRORS:\n");
	print_parsing_errors(text, tokens, parsing_errors);
	printf("\n");

	list_destroy(&tokens);
	list_destroy(&lexing_errors);
	list_destroy(&nodes);
	list_destroy(&parsing_errors);
	return 0;
}
