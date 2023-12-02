#include <assert.h> // assert()
#include <stdio.h> // FILE, fopen(), fclose(), fseek(), frewind(), ftell()
#include <stdlib.h> // malloc(), free()
#include "parser.h"

char *readFile(FILE *file) {
    // TODO: Check for file errors.
    // TODO: Figure out a portable way to get the size of the file.
    // Find the size of the file.
    assert(file != NULL && "Must pass a valid file.");
    long startPosition = ftell(file);
    fseek(file, 0, SEEK_END);
    long endPosition = ftell(file);
    fseek(file, startPosition, SEEK_SET);
    long fileSize = endPosition - startPosition;
    assert(fileSize >= 0 && "Something went wrong while finding file size.");

    // Allocate a string to store the text of the file (null-terminated).
    char *text = malloc(fileSize);
    assert(text != NULL && "`malloc()` failed.");

    return NULL;
}

char *openAndReadFile(char *path) {
    assert(path != NULL && "Must pass a path.");
    return NULL;
}

int main(void) {
    return 0;
}
