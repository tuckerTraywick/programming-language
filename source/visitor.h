#ifndef VISITOR_H
#define VISITOR_H

#include <stdbool.h>
#include "object.h"

// Makes a symbol for each definition and makes sure there are no duplicate definitions.
bool initialize_symbols_for_definitions(char *text, struct token *tokens, struct node *nodes, struct object *object, struct compiling_error **errors);

#endif // VISITOR_H
