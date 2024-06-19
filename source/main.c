#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "interpreter.h"

int main(void) {
    struct Object object = ObjectCreate(8*1024, 100);
    ObjectPrint(&object);
    ObjectDestroy(&object);
    return 0;
}
