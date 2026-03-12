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
		strncpy(name + length, token_text, current_token->text_length);
		length += current_token->text_length; // We are not counting the null terminator.
		name[length] = '\0';
		current_node = object->nodes + current_node->next_index;
	}
	return name;
}

bool initialize_symbols(struct object *object) {
	char *module_name = NULL;
	size_t next_node_index = object->nodes->child_index;
	bool error_occurred = false;
	while (next_node_index) {
		// Get the current node and get ready to visit the next node.
		struct node *current_node = object->nodes + next_node_index;
		next_node_index = current_node->next_index;

		if (current_node->type != NODE_TYPE_DEFINITION) {
			continue;
		}
		
		// "pub"?, definiton
		current_node = object->nodes + current_node->child_index;
		// Set the visibility of the definition.
		enum symbol_visibility visibility = SYMBOL_VISIBILITY_PRIVATE;
		if (current_node->type == NODE_TYPE_TOKEN) {
			visibility = SYMBOL_VISIBILITY_PUBLIC;
			// definiton
			current_node = object->nodes + current_node->next_index;
		}

		// Handle module definitions.
		if (current_node->type == NODE_TYPE_MODULE_DEFINITION) {
			if (module_name) {
				struct compiler_error error = {
					.node_index = current_node - object->nodes,
					.type = COMPILER_ERROR_TYPE_MODULE_ALREADY_DECLEARED
				};
				// TODO: Handle false return value.
				list_push_back(&object->compiler_errors, &error);
				error_occurred = true;
			} else {
				// module_definition("module", "a", ".", "b", ";") 
				// TODO: Handle NULL return vaule.
				module_name = get_module_name(object, current_node);
				struct symbol_handle symbol = {
					.type = SYMBOL_TYPE_MODULE,
					.visibility = visibility,
				};
				// TODO: Handle false return value.
				map_add(&object->symbols, module_name, &symbol);
			}
		// Handle type definitions.
		} else if (current_node->type == NODE_TYPE_TYPE_DEFINITION) {
			// type_definition("type", "{", "}", ";")
		}
	}
	return error_occurred;
}
