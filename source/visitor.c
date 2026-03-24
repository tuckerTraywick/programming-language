#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "visitor.h"
#include "object.h"
#include "token.h"
#include "node.h"
#include "list.h"
#include "map.h"

// A buffer used to store the name of the current symbol being worked on. Also used to concatenate
// module names to identifiers. Always null terminated.
static char symbol_name[MAX_SYMBOL_NAME_LENGTH];
static size_t symbol_name_length;

void get_module_name(struct object *object, struct node *module_definition) {
	symbol_name_length = 0;
	// module_definition("module", "a", ".", "b", ";")
	struct node *current_node = object->nodes + module_definition->child_index; // "module"
	current_node = object->nodes + current_node->next_index; // "a"
	while (true) {
		struct token *current_token = object->tokens + current_node->child_index;
		if (current_token->type == TOKEN_TYPE_SEMICOLON) {
			break;
		}
		char *token_text = object->text + current_token->text_index;
		strncpy(symbol_name + symbol_name_length, token_text, current_token->text_length);
		symbol_name_length += current_token->text_length; // We are not counting the null terminator.
		current_node = object->nodes + current_node->next_index;
	}
	symbol_name[symbol_name_length] = '\0';
}

bool initialize_symbols(struct object *object) {
	size_t next_node_index = object->nodes->child_index;
	bool module_declared = false;
	bool error_occurred = false;
	while (next_node_index) {
		// Get the current node and get ready to visit the next node.
		struct node *current_node = object->nodes + next_node_index;
		next_node_index = current_node->next_index;

		if (current_node->type != NODE_TYPE_DEFINITION) {
			continue;
		}
		
		current_node = object->nodes + current_node->child_index;
		// "pub"?, definiton(...)
		// Set the visibility of the definition.
		enum symbol_visibility visibility = SYMBOL_VISIBILITY_PRIVATE;
		if (current_node->type == NODE_TYPE_TOKEN) {
			visibility = SYMBOL_VISIBILITY_PUBLIC;
			current_node = object->nodes + current_node->next_index;
			// definiton(...)
		}

		// Handle module definitions.
		if (current_node->type == NODE_TYPE_MODULE_DEFINITION) {
			if (module_declared) {
				struct compiler_error error = {
					.node_index = current_node - object->nodes,
					.type = COMPILER_ERROR_TYPE_MODULE_ALREADY_DECLEARED
				};
				// TODO: Handle false return value.
				list_push_back(&object->compiler_errors, &error);
				error_occurred = true;
				continue;
			}
			// module_definition("module", "a", ".", "b", ";") 
			module_declared = true;
			get_module_name(object, current_node);

			struct module_symbol symbol = {0};
			// TODO: Handle false return value.
			list_push_back(&object->modules, &symbol);
			size_t symbol_index = list_get_count(&object->modules) - 1;
			struct symbol_handle handle = {
				.type = SYMBOL_TYPE_MODULE,
				.visibility = visibility,
				.index = symbol_index,
			};
			// TODO: Handle false return value.
			map_add(&object->symbols, symbol_name, &handle);
		// Handle all other definitions.
		} else if (current_node->type == NODE_TYPE_TYPE_DEFINITION || current_node->type == NODE_TYPE_VARIABLE_DEFINITION || current_node->type == NODE_TYPE_FUNCTION_DEFINITION) {
			enum node_type type = current_node->type;
			// struct/trait/var/func_definition("struct/trait/var/func", name, ...
			current_node = object->nodes + current_node->child_index;
			// "struct/trait/var/func", name, ...
			current_node = object->nodes + current_node->next_index;
			// name, ...

			// Get the name of the symbol and check if it already exists.
			struct token *token = object->tokens + current_node->child_index;
			char *type_name = object->text + token->text_index;
			symbol_name[symbol_name_length] = '.';
			// TODO: Account for name being too long.
			strncpy(symbol_name + symbol_name_length + 1, type_name, token->text_length);
			symbol_name[symbol_name_length + 1 + token->text_length] = '\0';

			// See if the symbol already exists.
			struct symbol_handle *result = map_get(&object->symbols, symbol_name);
			if (result) {
				struct compiler_error error = {
					.node_index = current_node - object->nodes,
					.type = COMPILER_ERROR_SYMBOL_ALREADY_DEFINED,
				};
				// TODO: Handle false return value.
				list_push_back(&object->compiler_errors, &error);
				error_occurred = true;
				continue;
			}

			// Add the symbol to the correct section.
			if (type == NODE_TYPE_TYPE_DEFINITION) {
				struct type_symbol symbol = {0};
				// TODO: Handle false return value.
				list_push_back(&object->types, &symbol);
				size_t symbol_index = list_get_count(&object->types) - 1;
				struct symbol_handle handle = {
					.type = SYMBOL_TYPE_TYPE,
					.visibility = visibility,
					.index = symbol_index,
				};
				// TODO: Handle falase return value.
				map_add(&object->symbols, symbol_name, &handle);
			} else if (type == NODE_TYPE_VARIABLE_DEFINITION) {
				struct variable_symbol symbol = {0};
				// TODO: Handle false return value.
				list_push_back(&object->variables, &symbol);
				size_t symbol_index = list_get_count(&object->variables) - 1;
				struct symbol_handle handle = {
					.type = SYMBOL_TYPE_VARIABLE,
					.visibility = visibility,
					.index = symbol_index,
				};
				// TODO: Handle falase return value.
				map_add(&object->symbols, symbol_name, &handle);
			} else {
				struct function_symbol symbol = {0};
				// TODO: Handle false return value.
				list_push_back(&object->functions, &symbol);
				size_t symbol_index = list_get_count(&object->functions) - 1;
				struct symbol_handle handle = {
					.type = SYMBOL_TYPE_FUNCTION,
					.visibility = visibility,
					.index = symbol_index,
				};
				// TODO: Handle falase return value.
				map_add(&object->symbols, symbol_name, &handle);
			}
		}
	}
	return error_occurred;
}
