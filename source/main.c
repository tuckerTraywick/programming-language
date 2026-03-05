#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "token.h"
#include "node.h"
#include "list.h"
#include "map.h"
#include "lexer.h"
#include "parser.h"
#include "visitor.h"

static void print_token(struct object *object, struct token *token) {
	printf("%s `%.*s`", token_type_names[token->type], (int)token->text_length, object->text + token->text_index);
}

static void print_tokens(struct object *object) {
	for (size_t i = 0; i < list_get_count(&object->tokens); ++i) {
		printf("%-5zu ", i);
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
	printf("%-5zu", node - object->nodes);
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

static void print_object(struct object *object) {
	if (object->header) {

	}
	printf("smybols:\n");
	for (size_t i = 0; i < map_get_buckets_capacity(&object->symbols); ++i) {
		if (map_index_is_full(&object->symbols, i)) {
			char *name = map_get_key(&object->symbols, object->symbols + i);
			struct symbol_handle *symbol = map_get(&object->symbols, name);
			printf("%-5zu%s\n", symbol->offset, name);
		}
	}
}

static void print_compiler_errors(struct object *object) {
	for (size_t i = 0; i < list_get_count(&object->compiler_errors); ++i) {
		struct compiler_error *error = object->compiler_errors + i;
		struct node *node = object->nodes + error->node_index;
		printf("%s [Node %zu: %s]\n", compiler_error_messages[error->type], error->node_index, node_type_names[node->type]);
	}
}

int main(void) {
	struct object *object = object_create();
	if (!object) {
		printf("FAILED TO CREATE OBJECT\n");
		return 1;
	}
	FILE *file = fopen("example.txt", "r");
	if (!file) {
		printf("FAILED TO OPEN FILE\n");
		object_destroy(object);
		return 1;
	}
	object_read_text_from_file(object, file);
	fclose(file);
	
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

	if (!list_is_empty(&object->lexer_errors) || !list_is_empty(&object->parser_errors)) {
		object_destroy(object);
		return 1;
	}
	initialize_symbols(object);
	printf("OBJECT:\n");
	print_object(object);
	printf("\nCOMPILER ERRORS:\n");
	print_compiler_errors(object);

	object_destroy(object);
	return 0;
}
