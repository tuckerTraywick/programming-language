#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "visitor.h"
#include "object.h"
#include "token.h"
#include "node.h"
#include "list.h"
#include "map.h"

// The string returned by this function is statically allocated and is modified in-place on
// successive calls to this function. You must copy it if you want to store it.
char *get_module_name(struct object *object, struct node *module_definition) {
	static char name[MAX_SYMBOL_NAME_LENGTH];
	size_t length = 0;
	// module_definition("module", "a", ".", "b", ";")
	struct node *current_node = object->nodes + module_definition->child_index; // "module"
	current_node = object->nodes + current_node->next_index; // "a"
	while (true) {
		struct token *current_token = object->tokens + current_node->child_index;
		if (current_token->type == TOKEN_TYPE_SEMICOLON) {
			break;
		}
		char *token_text = object->text + current_token->text_index;
		strcpy(name + length, token_text);
		length += current_token->text_length; // We are not counting the null terminator.
		current_node = object->nodes + current_node->next_index;
	}
	return name;
}

bool initialize_symbols(struct object *object, struct compiler_error **errors) {
	struct node *current_node = object->nodes;
	char *module_name = NULL;

	while (current_node) {
		enum symbol_visibility visibility = SYMBOL_VISIBILITY_PRIVATE;
		if (current_node->type == NODE_TYPE_DEFINITION) {
			visibility = SYMBOL_VISIBILITY_PUBLIC;
			current_node = object->nodes + current_node->child_index; // module_definition
		}

		if (current_node->type == NODE_TYPE_MODULE_DEFINITION) {
			if (module_name) {
				struct compiler_error error = {.type = COMPILER_ERROR_TYPE_ALREADY_DEFINED};
				// TODO: Handle false return value.
				list_push_back(errors, &error);
			} else {
				// module_definition("module", "a", ".", "b", ";") 
				// TODO: Handle NULL return vaule.
				char *module_name = get_module_name(object, current_node);
				struct module_symbol symbol = {0};
				// TODO: Handle false return value.
				map_add(&object->symbols, module_name, &symbol);
			}
		} else if (current_node->type == NODE_TYPE_TYPE_DEFINITION) {

		}
	}
}
