#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "interpreter.h"

int main(void) {
    struct Object object = ObjectCreate(8*1024, 100);
    FILE *file = fopen("example.bin", "wb+");
    
    ObjectWriteToFile(&object, file);
    ObjectDestroy(&object);
    
    object = ObjectReadFromFile(file);
    ObjectPrint(&object);
    
    ObjectDestroy(&object);
    fclose(file);
    return 0;
}
