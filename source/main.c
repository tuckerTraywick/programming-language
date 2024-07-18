#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "interpreter.h"

int main(void) {
    struct Object object = ObjectCreate(1024, 100);
    
    SymbolTableSetSymbol(
        &object.symbolTable,
        &object.strings,
        FUNCTION,
        "foo",
        0,
        0
    );
    SymbolTableSetSymbol(
        &object.symbolTable,
        &object.strings,
        FUNCTION,
        "bar",
        0,
        0
    );
    SymbolTableReserve(&object.symbolTable, &object.strings, 2);

    FILE *file = fopen("example.bin", "wb+");
    ObjectWriteToFile(&object, file);
    ObjectDestroy(&object);
    object = ObjectReadFromFile(file);
    ObjectPrint(&object);
    fclose(file);

    struct Symbol *result = SymbolTableGetSymbol(&object.symbolTable, &object.strings, "bar");
    printf("name = %s\n", (result) ? (char*)object.strings.elements + result->name : "NULL");

    ObjectDestroy(&object);
    return 0;
}
