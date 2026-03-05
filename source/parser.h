#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "object.h"

// Returns true if no parsing or memory errors occurred.
bool parse(struct object *object);

#endif // PARSER_H
