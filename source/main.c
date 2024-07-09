#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "interpreter.h"

// int main(void) {
//     struct Object object = ObjectCreate(1024, 100);
    
//     struct Symbol symbol = {.category = MUTABLE_DATA};
//     SymbolTableSetSymbol(&object.symbolTable, &object.strings, "foo", &symbol);
//     SymbolTableSetSymbol(&object.symbolTable, &object.strings, "bar", &symbol);
//     SymbolTableRehash(&object.symbolTable, &object.strings, 2);

//     FILE *file = fopen("example.bin", "wb+");
//     ObjectWriteToFile(&object, file);
//     ObjectDestroy(&object);
//     object = ObjectReadFromFile(file);
//     ObjectPrint(&object);
//     fclose(file);

//     struct Symbol *result = SymbolTableGetSymbol(&object.symbolTable, &object.strings, "foo");
//     printf("name = %s\n", (result) ? (char*)object.strings.elements + result->name : "NULL");

//     ObjectDestroy(&object);
//     return 0;
// }

int main(void) {
    struct List first = ListCreate(10, sizeof (int));
    struct List second = ListCreate(10, sizeof (int));
    
    for (int i = 0; i < 10; ++i) {
        ListPushBack(&first, &i);
    }

    for (int i = 10; i < 20; ++i) {
        ListPushBack(&second, &i);
    }

    ListCombine(&first, &second);

    printf("first  = ");
    for (size_t i = 0; i < first.count; ++i) {
        printf("%d ", *(int*)ListGet(&first, i));
    }

    printf("\nsecond = ");

    for (size_t i = 0; i < second.count; ++i) {
        printf("%d ", *(int*)ListGet(&second, i));
    }

    printf("\n");
    
    ListDestroy(&first);
    ListDestroy(&second);

    return 0;
}
