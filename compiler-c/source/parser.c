#include <stdbool.h> // bool
#include <assert.h> // assert()
#include <stdio.h> // FILE, fopen(), fclose(), fseek(), frewind(), ftell()
#include <stdlib.h> // malloc(), realloc(), free()
#include <string.h> // memset()
#include <ctype.h> // isdigit(), isalpha(), isalnum()
#include "parser.h"

// Appends `token` to `tokens`. Updates `*tokens` `*tokensCapacity`, and `*tokensCount` as needed.
static void appendToken(struct Token* token, struct Token **tokens, size_t *tokensCapacity,
size_t *tokensCount) {
    assert(tokens != NULL && "Must pass an array of tokens.");
    assert(tokensCapacity != NULL && "Must pass a capacity.");
    assert(tokensCount != NULL && "Must pass a count.");

    if (*tokensCount >= *tokensCapacity) {
        // Extend the array if needed.
        *tokensCapacity += TOKENS_CAPACITY_INCREMENT;
        struct Token *newTokens = realloc(*tokens, *tokensCapacity);
        assert(newTokens != NULL && "`realloc()` failed.");
        *tokens = newTokens;
    }

    // Append the token to the end of the array.
    (*tokens)[*tokensCount] = *token;
    ++*tokensCount;
}

char *readFile(FILE *file) {
    // TODO: Check for file errors.
    // TODO: Figure out a portable way to get the size of the file.
    assert(file != NULL && "Must pass a file.");
    // Find the size of the file.
    long startPosition = ftell(file);
    fseek(file, 0, SEEK_END);
    long endPosition = ftell(file);
    fseek(file, startPosition, SEEK_SET);
    long fileSize = endPosition - startPosition;
    assert(fileSize >= 0 && "Something went wrong while finding file size.");

    // Allocate a string to store the text of the file.
    size_t stringSize = fileSize + 1;
    char *text = malloc(stringSize); // `+ 1` to account for the null terminator.
    assert(text != NULL && "`malloc()` failed.");

    // Read the file into the string and replace the EOF with a null terminator.
    size_t bytesRead = fread(text, 1, stringSize, file);
    assert(bytesRead == fileSize && "Something went wrong while putting the file in a buffer.");
    text[fileSize] = '\0';
    return text;
}

char *openAndReadFile(char *path) {
    assert(path != NULL && "Must pass a path.");
    FILE *file = fopen(path, "r");
    assert(file != NULL && "Failed to open file.");
    char *text = readFile(file);
    assert(text != NULL && "Failed to read file.");
    fclose(file);
    return text;
}

struct LexingResult lexString(char *text, bool ignoreNewlines) {
    assert(text != NULL && "Must pass a string.");
    size_t tokensCapacity = TOKENS_INITIAL_CAPACITY;
    size_t tokensCount = 0;
    struct Token *tokens = malloc(tokensCapacity*(sizeof *tokens));
    assert(tokens != NULL && "`malloc()` failed.");
    struct Token token = {0};

    while (text[token.index] != '\0') {
        char ch = text[token.index];
        switch (ch) {
            case ' ':
            case '\t':
                // Skip whitespace.
                ++token.index;
                ++token.column;
                break;
            case '\r':
                // Skip whitespace.
                ++token.index;
                break;
            case '\n':
                // Maybe lex a newline.
                if (!ignoreNewlines) {
                    token.type = NEWLINE;
                    token.text = text + token.index;
                    token.textLength = 1;
                    appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                }
                ++token.index;
                ++token.row;
                token.column = 0;
                break;
            case '0'...'9':
                // Lex a number.
                token.type = NUMBER;
                token.textLength = 0;
                while (isdigit(text[token.index])) {
                    ++token.index;
                    ++token.column;
                }
                token.index -= token.textLength;
                token.column -= token.textLength;
                appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                token.index += token.textLength;
                token.column += token.textLength;
                break;
            default:
                ++token.index;
                ++token.column;
        }
    }

    return (struct LexingResult) {
        .tokens = tokens,
        .tokensCount = tokensCount,
        .errorMessages = NULL,
        .errorMessagesCount = 0,
    };
}

void destroyLexingResult(struct LexingResult *result) {
    assert(result != NULL && "Must pass a result.");
    free(result->tokens);
    free(result->errorMessages);
    *result = (struct LexingResult) {0};
}
