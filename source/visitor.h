#ifndef VISITOR_H
#define VISITOR_H

#include <stdbool.h>
#include "object.h"

// Creates entries in the object's symbol table for each definition encountered in the syntax tree.
// Emits errors for multiple definitions of symbols. Returns true if no errors encountered.
bool initialize_symbols(struct object *object, struct compiler_error **errors);

#endif // VISITOR_H
