#include <stdlib.h> // free()
#include <stdio.h> // FILE, fopen(), fclose(), printf(), puts()
#define TEST_IMPL
#include "test.h"
#include "parser.h"

void testReadFile() {
    FILE *file = fopen("test/example.txt", "r");
    test(file != NULL && "Failed to open file.");
    char *text = readFile(file);
    test(text != NULL && "Failed to read file.");
    printf("file text:\n%s\n", text);
    fclose(file);
    free(text);
}

void testOpenAndReadFile() {
    puts("\n----\n");
    char *text = openAndReadFile("test/example.txt");
    test(text != NULL && "Failed to read file.");
    printf("file text:\n%s\n", text);
    free(text);
}

int main(void) {
    beginTesting();
        testReadFile();
        testOpenAndReadFile();
    endTesting();
    return 0;
}
