#include <stdio.h> // FILE, fopen(), fclose()
#define TEST_IMPL
#include "test.h"
#include "parser.h"

void testReadFile() {
    FILE *file = fopen("example.txt", "r");
    test(file != NULL && "Failed to open file.");
    char *text = readFile(file);
    test(text != NULL && "Failed to read file.");
    printf("%s", text);
    fclose(file);
}

int main(void) {
    beginTesting();
        testReadFile();
    endTesting();
    return 0;
}
