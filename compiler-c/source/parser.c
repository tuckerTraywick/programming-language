#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdlib.h> // malloc(), free()
#include "parser.h"
#include "lexer.h"
#include "list.h"

#define NODES_INITIAL_CAPACITY 3000
#define NODES_CAPACITY_INCREMENT 1500

#define ERRORS_INITIAL_CAPACITY 500
#define ERRORS_CAPACITY_INCREMENT 100

#define STACK_INITIAL_CAPACITY 3000
#define STACK_CAPACITY_INCREMENT 1500

void destroyParsingResult(struct ParsingResult *result) {
    assert(result != NULL && "Must pass a result.");
    free(result->nodes);
    free(result->errors);
    *result = (struct ParsingResult) {0};
}

struct ParsingResult parse(struct Token *tokens, size_t tokensCount) {
    struct ParsingTransition transitions[][NODE_TYPE_COUNT] = {
        {[NUMBER] = {.action=END_NODE}},
    };

    assert(tokens != NULL && "Must pass an array of tokens.");
    struct List nodes = listCreate(struct Node, NODES_INITIAL_CAPACITY);
    struct List errors = listCreate(struct ParsingError, ERRORS_INITIAL_CAPACITY);
    struct List states = listCreate(size_t, STACK_INITIAL_CAPACITY);

    // Every tree has at least a parent node.
    struct Node firstNode = {
        .type = PROGRAM,
        .children = NULL,
        .tokens = tokens,
        .tokensCount = 0,
    };
    listAppend(&nodes, &firstNode, NODES_CAPACITY_INCREMENT);

    struct Node secondNode = {.type=TOKEN, .children=NULL, .tokens=tokens, tokensCount=1};
    listAppend(&nodes, &secondNode, NODES_CAPACITY_INCREMENT);
    listGet(struct Node, &nodes, 0)->children = listGet(struct Node, &nodes, 1);
    ++listGet(struct Node, &nodes, 0)->childrenCount;

    struct Node thirdNode = {.type=TOKEN, .children=NULL, .tokens=tokens+1, tokensCount=1};
    listAppend(&nodes, &secondNode, NODES_CAPACITY_INCREMENT);
    listGet(struct Node, &nodes, 0)->children[1] = *listGet(struct Node, &nodes, 2);
    ++listGet(struct Node, &nodes, 0)->childrenCount;


    listDestroy(&states);
    return (struct ParsingResult) {
        .nodes = (struct Node*)nodes.elements,
        .nodesCount = nodes.count,
        .errors = (struct ParsingError*)errors.elements,
        .errorsCount = errors.count,
    };
}

#undef NODES_INITIAL_CAPACITY
#undef NODES_CAPACITY_INCREMENT

#undef ERRORS_INITIAL_CAPACITY
#undef ERRORS_CAPACITY_INCREMENT

#undef STACK_INITIAL_CAPACITY
#undef STACK_CAPACITY_INCREMENT
