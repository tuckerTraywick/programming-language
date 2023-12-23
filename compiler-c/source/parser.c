#include <assert.h> // assert()
#include <stddef.h> // size_t
#include <stdbool.h> // bool
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
    struct ParsingTransition transitions[][TOKEN_TYPE_COUNT] = {
        {[NUMBER]={.action=GOTO, .next=1}},
        {[IDENTIFIER]={.action=REJECT}},
    };

    assert(tokens != NULL && "Must pass an array of tokens.");
    struct List nodes = listCreate(struct Node, NODES_INITIAL_CAPACITY);
    struct List errors = listCreate(struct ParsingError, ERRORS_INITIAL_CAPACITY);
    struct List states = listCreate(size_t, STACK_INITIAL_CAPACITY);

    // Start at the first row of the transition table.
    size_t startState = 0;
    listAppend(&states, &startState, STACK_CAPACITY_INCREMENT);

    // Every tree has at least a parent node.
    struct Node firstNode = {
        .type = PROGRAM,
        .tokens = tokens,
    };
    listAppend(&nodes, &firstNode, NODES_CAPACITY_INCREMENT);

    bool keepParsing = true;
    size_t state = 0;
    size_t tokenIndex = 0;
    struct Node *currentNode = listGet(struct Node, &nodes, 0);
    while (keepParsing && tokenIndex < tokensCount) {
        struct Token token = tokens[tokenIndex];
        struct ParsingTransition transition = transitions[state][token.type];
        struct Node newNode;
        switch (transition.action) {
            case REJECT:
                keepParsing = false;
                break;
            case GOTO:
                newNode = (struct Node) {
                    .type = TOKEN,
                    .parent = currentNode,
                    .tokens = tokens + tokenIndex,
                    .tokensCount = 1,
                };
                listAppend(&nodes, &newNode, NODES_CAPACITY_INCREMENT);
                currentNode->next = listLast(struct Node, &nodes);
                currentNode = currentNode->next;
                state = transition.next;
                break;
        }
    }

    listDestroy(&states);
    return (struct ParsingResult) {
        .nodes = (struct Node*) nodes.elements,
        .nodesCount = nodes.count,
        .errors = (struct ParsingError*) errors.elements,
        .errorsCount = errors.count,
    };
}

#undef NODES_INITIAL_CAPACITY
#undef NODES_CAPACITY_INCREMENT

#undef ERRORS_INITIAL_CAPACITY
#undef ERRORS_CAPACITY_INCREMENT

#undef STACK_INITIAL_CAPACITY
#undef STACK_CAPACITY_INCREMENT
