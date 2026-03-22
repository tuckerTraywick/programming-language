#ifndef VISITOR_H
#define VISITOR_H

#include <stdbool.h>
#include "object.h"

// Creates entries in the object's symbol table for each definition encountered in the syntax tree.
// Returns true if no errors were encountered.
bool initialize_symbols(struct object *object);

#endif // VISITOR_H
