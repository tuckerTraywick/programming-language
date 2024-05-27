#include <stdint.h>
#include <stddef.h>
#include "symboltable.h"

void initializeSymbolTable(struct SymbolTable *table, size_t capacity) {
    // *table = (struct SymbolTable){
    //     .nodeCapacity = capacity,
    //     .nodeCount = 0,
    //     .nodes = malloc(capacity),
    // };
    // // TODO: Handle failed `malloc()`.
    // assert(table->nodes && "`malloc()` failed.");
}

void destroySymbolTable(struct SymbolTable *table) {
    // free(table->nodes);
    // *table = (struct SymbolTable){0};
}

uint64_t getSymbol(struct SymbolTable *table, char *name) {
    // assert(table->nodeCapacity > 0 && table->nodes && "Table must be initialized.");
    // char *ch = name;
    // struct SymbolTableNode *node = table->nodes;
    // while (true) {
    //     if (*ch == '\0' && node->ch == '\0') {
    //         return node->child;
    //     } else if (*ch == '\0' || node->ch == '\0') {
    //         return 0;
    //     } else if (*ch == node->ch) {
    //         node = table->nodes + node->child;
    //         ++ch;
    //     } else if (node->next) {
    //         node = table->nodes + node->next;
    //     } else {
    //         return 0;
    //     }
    // }
}

void addSymbol(struct SymbolTable *table, char *name, uint64_t offset) {
    // assert(table->nodeCapacity > 0 && table->nodes && "Table must be initialized.");
    // assert(*name != '\0' && "`name` must be at least one character long.");
    // char *ch = name;
    // struct SymbolTableNode *node = table->nodes;

    // // Find the next node to add a child to.
    // while (*ch != '\0') {
    //     /*
    //     if the node matches:
    //         if the node has a child:
    //             advance to the next character
    //             advance to the child
    //         else:
    //             add a new child to the node
    //     */
    //     if (*ch == node->ch) {
    //         ++ch;
    //         node = table->nodes + node->child;
    //     } else if (node->next) {
    //         node = table->nodes + node->next;
    //     } else {
    //         node = addNode(node, *ch);
    //     }
    // }
}
