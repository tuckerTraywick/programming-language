#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "object.h"

// Returns true if no lexing or memory errors occurred.
bool lex(struct object *object);

#endif // LEXER_H
