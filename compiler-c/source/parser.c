#include <assert.h> // assert()
#include "parser.h"
#include "lexer.h"
#include "list.h"

#define NODES_INITIAL_CAPACITY 3000
#define NODES_CAPACITY_INCREMENT 1500

#define ERRORS_INITIAL_CAPACITY 1000
#define ERRORS_CAPACITY_INCREMENT 500

struct ParsingResult parse(struct Token *tokens, size_t tokensCount) {
    assert(tokens != NULL && "Must pass an array of tokens.");
    struct List nodes = listCreate(struct Node, NODES_INITIAL_CAPACITY);
    struct ParsingResult result = {0};
    return result;
}

#undef NODES_INITIAL_CAPACITY
#undef NODES_CAPACITY_INCREMENT

#undef ERRORS_INITIAL_CAPACITY
#undef ERRORS_CAPACITY_INCREMENT
