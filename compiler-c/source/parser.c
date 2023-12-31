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

static void parsePackageStatement() {
    beginNode();
        expect(PACKAGE);
        expect(IDENTIFIER);
        zeroOrMore({
            expect(DOT);
            expect(IDENTIFIER);
        })
    endNode(PACKAGE_STATEMENT);
}

void destroyParsingResult(struct ParsingResult *result) {
    assert(result != NULL && "Must pass a result.");
    free(result->nodes);
    free(result->errors);
    *result = (struct ParsingResult) {0};
}

struct ParsingResult parse(struct Token *tokens, size_t tokensCount) {
    assert(tokens != NULL && "Must pass an array of tokens.");
    struct List nodes = listCreate(struct Node, NODES_INITIAL_CAPACITY);
    struct List errors = listCreate(struct ParsingError, ERRORS_INITIAL_CAPACITY);

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
