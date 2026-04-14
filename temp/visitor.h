#ifndef VISITOR_H
#define VISITOR_H

#include <stdbool.h>
#include "object.h"

// Creates entries in the object's symbol table for each definition encountered in the global scope.
// Returns true if no errors occurred.
bool initialize_symbols(struct object *object);

bool create_scopes(struct object *object);

#endif // VISITOR_H
