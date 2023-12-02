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
    printf("fileSize = %lu\n", fileSize);

    // Allocate a string to store the text of the fil.
    char *text = malloc(fileSize + 1); // `+ 1` to account for the null terminator.
    assert(text != NULL && "`malloc()` failed.");

    // Read the file into the string and replace the EOF with a null terminator.
    size_t bytesRead = fread(text, 1, fileSize + 1, file);
    printf("bytesRead = %lu\n", bytesRead);
    assert(bytesRead == fileSize && "Something went wrong while putting the file in a buffer.");
    text[fileSize] = '\0';
    return text;
}

char *openAndReadFile(char *path) {
    assert(path != NULL && "Must pass a path.");
    return NULL;
}
