#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "token.h"
#include "node.h"
#include "list.h"
#include "lexer.h"
#include "parser.h"

static void print_token(struct object *object, struct token *token) {
	printf("%s `%.*s`", token_type_names[token->type], (int)token->text_length, object->text + token->text_index);
}

static void print_tokens(struct object *object) {
	for (size_t i = 0; i < list_get_count(&object->tokens); ++i) {
		printf("%zu ", i);
		print_token(object, object->tokens + i);
		printf("\n");
	}
}

static void print_lexer_error(struct object *object, struct lexer_error *error) {
	printf("%s [Character %zu `%.*s`]", lexer_error_messages[error->type], error->text_index, (int)error->text_length, object->text + error->text_index);
}

static void print_lexer_errors(struct object *object) {
	for (size_t i = 0; i < list_get_count(&object->lexer_errors); ++i) {
		print_lexer_error(object, object->lexer_errors + i);
		printf("\n");
	}
}

static void print_node(struct object *object, struct node *node, size_t depth) {
	for (size_t i = 0; i < depth; ++i) {
		printf("| ");
	}

	if (node->type == NODE_TYPE_TOKEN) {
		struct token *token = object->tokens + node->child_index;
		print_token(object, token);
		printf("\n");
		return;
	}

	printf("%s\n", node_type_names[node->type]);
	if (node->child_index == 0) {
		return;
	}

	size_t node_index = node->child_index;
	do {
		print_node(object, object->nodes + node_index, depth + 1);
		node_index = object->nodes[node_index].next_index;
	} while (node_index);
}

static void print_parser_error(struct object *object, struct parser_error *error) {
	printf("%s [Token %zu: ", parser_error_messages[error->type], error->token_index - 1);
	print_token(object, object->tokens + error->token_index - 1);
	printf("]");
}

static void print_parser_errors(struct object *object) {
	for (size_t i = 0; i < list_get_count(&object->parser_errors); ++i) {
		print_parser_error(object, object->parser_errors + i);
		printf("\n");
	}
}

int main(void) {
	struct object *object = object_create();
	if (!object) {
		printf("FAILED TO CREATE OBJECT\n");
		return 1;
	}
	
	char *text = "module hi;";
	if (!lex(object)) {
		object_destroy(object);
		printf("FAILED LEXING\n");
		return 1;
	}
	printf("TOKENS:\n");
	print_tokens(object);
	printf("\nLEXER ERRORS:\n");
	print_lexer_errors(object);
	printf("\n");

	if (!parse(object)) {
		printf("FAILED PARSING\n");
		object_destroy(object);
		return 1;
	}
	printf("NODES:\n");
	print_node(object, object->nodes, 0);
	printf("\nPARSER ERRORS:\n");
	print_parser_errors(object);
	printf("\n");
	
	object_destroy(object);
	return 0;
}

