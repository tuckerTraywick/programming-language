#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "interpreter.h"

int main(void) {
    // struct Object object = ObjectCreate(8*1024, 100);
    // FILE *file = fopen("example.bin", "wb+");
    
    // ObjectWriteToFile(&object, file);
    // ObjectDestroy(&object);
    
    // object = ObjectReadFromFile(file);
    // ObjectPrint(&object);
    
    // ObjectDestroy(&object);
    // fclose(file);
    SymbolTable table = ListCreate(10, sizeof (struct Symbol));
    ListChar strings = ListCreate(100, 1);
    struct Symbol symbol = {.category=MUTABLE_DATA};

    SymbolTableSetSymbol(&table, &strings, "foo", &symbol);
    SymbolTableSetSymbol(&table, &strings, "bar", &symbol);
    struct Symbol *result = SymbolTableGetSymbol(&table, &strings, "bar");
    printf("name = %s\n", (result) ? (char*)strings.elements + result->name : "NULL");

    ListDestroy(&table);
    ListDestroy(&strings);
    return 0;
}
