#include <stdbool.h> // bool
#include <stdint.h> // SIZE_MAX
#include <assert.h> // assert()
#include <stdio.h> // FILE, fopen(), fclose(), fseek(), frewind(), ftell()
#include <stdlib.h> // malloc(), realloc(), free()
#include <ctype.h> // isdigit(), isalpha(), isalnum(), ispunct()
#include <string.h> // strncmp()
#include "log.h"
#include "parser.h"

// Appends `token` to `tokens`. Updates `*tokens` `*tokensCapacity`, and `*tokensCount` as needed.
// Helper for `lexString()`.
static void appendToken(struct Token* token, struct Token **tokens, size_t *tokensCapacity,
size_t *tokensCount) {
    assert(tokens != NULL && "Must pass an array of tokens.");
    assert(tokensCapacity != NULL && "Must pass a capacity.");
    assert(tokensCount != NULL && "Must pass a count.");

    // Extend the array if needed.
    if (*tokensCount >= *tokensCapacity) {
        *tokensCapacity += TOKENS_CAPACITY_INCREMENT;
        struct Token *newTokens = realloc(*tokens, *tokensCapacity);
        assert(newTokens != NULL && "`realloc()` failed.");
        *tokens = newTokens;
    }

    (*tokens)[*tokensCount] = *token;
    ++*tokensCount;
}

// Returns the max of `a` and `b`. Helper for `lexString()`.
static size_t max(size_t a, size_t b) {
    return (a >= b) ? a : b;
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

    size_t stringSize = fileSize + 1; // `+ 1` to account for the null terminator.
    char *text = malloc(stringSize);
    assert(text != NULL && "`malloc()` failed.");

    size_t bytesRead = fread(text, 1, stringSize, file);
    assert(bytesRead == fileSize && "Something went wrong while putting the file in a buffer.");
    // Replace the EOF.
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
    // A mapping of keywords to their token types.
    static char* keywords[] = {
        "package",
        "from",
        "import",
        "public",
        "private",
        "const",
        "mut",
        "owned",
        "shared",
        "weak",
        "auto",
        "var",
        "struct",
        "cases",
        "embed",
        "pass",
        "return",
        "yield",
        "break",
        "continue",
        "if",
        "else",
        "switch",
        "case",
        "default",
        "fallthrough",
        "for",
        "in",
        "do",
        "while",
        "as",
        "is",
        "isnot",
        "and",
        "or",
        "xor",
        "not",
        "true",
        "false",
    };
    static size_t keywordsCount = (sizeof keywords)/(sizeof keywords[0]);

    // A mapping of operators to their token types.
    static char *operators[] = {
        "+=",
        "+",
        "-=",
        "->",
        "-",
        "*=",
        "*",
        "/=",
        "/",
        "%=",
        "%",
        "&=",
        "&",
        "|=",
        "|",
        "^=",
        "^",
        "~=",
        "~",
        "<<=",
        "<<",
        "<=",
        "<",
        ">>=",
        ">>",
        ">=",
        ">",
        "==",
        "=>",
        "=",
        "!=",
        "...",
        "..",
        ".",
        ",",
        ":",
        ";",
        "(",
        ")",
        "[",
        "]",
        "{",
        "}",
    };
    static size_t operatorsCount = (sizeof operators)/(sizeof operators[0]);

    assert(text != NULL && "Must pass a string.");
    size_t tokensCapacity = TOKENS_INITIAL_CAPACITY;
    size_t tokensCount = 0;
    struct Token *tokens = malloc(tokensCapacity*(sizeof *tokens));
    assert(tokens != NULL && "`malloc()` failed.");
    struct Token token = {0};
    bool foundOperator = true;

    while (text[token.index]) {
        char ch = text[token.index];
        if (ch == '\n') {
            // Maybe lex a newline.
            if (!ignoreNewlines && token.type != NEWLINE) {
                token.type = NEWLINE;
                token.text = text + token.index;
                token.textLength = 1;
                appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
            }
            ++token.index;
            ++token.row;
            token.column = 0;
        } else if (isspace(ch)) {
            // Skip whitespace.
            ++token.index;
            ++token.column;
        }  else if (ch == '/' && text[token.index + 1] && text[token.index + 1] == '/') {
            // Skip line comments.
            while (text[token.index] && text[token.index] != '\n') {
                ++token.index;
                ++token.column;
            }
        } else if (isdigit(ch)) {
            // Lex a number.
            token.type = NUMBER;
            token.text = text + token.index;
            token.textLength = 0;
            while (isdigit(text[token.index + token.textLength])) {
                ++token.textLength;
                ++token.column;
            }
            appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
            token.index += token.textLength;
        } else if (isalpha(ch) || ch == '_') {
            // Lex an identifier or a keyword.
            token.type = IDENTIFIER;
            token.text = text + token.index;
            token.textLength = 0;
            while (isalnum(text[token.index + token.textLength])) {
                ++token.textLength;
                ++token.column;
            }

            for (size_t i = 0; i < keywordsCount; ++i) {
                // TODO: Get rid of `strlen()` here and store the lengths of each keyword in the
                // array.
                if (strncmp(keywords[i], token.text, max(token.textLength, strlen(keywords[i]))) == 0) {
                    token.type = i + PACKAGE;
                    break;
                }
            }
            appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
            token.index += token.textLength;
        } else if (ch == '\'') {
            // Lex a character literal.
            token.type = CHARACTER;
            token.text = text + token.index;
            token.textLength = 0;
            // TODO: Check length and lex escape sequences.
            do {
                ++token.textLength;
                ++token.column;
            } while (text[token.index + token.textLength] != '\'');
            ++token.textLength;
            // TODO: Handle unclosed '.
            // if (text[token.index + token.textLength] != '\'') {
            // }
            appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
            token.index += token.textLength + 1;
        } else if (ch == '"') {
            // Lex a string literal.
            token.type = STRING;
            token.text = text + token.index;
            token.textLength = 0;
            // TODO: Lex escape sequences.
            do {
                ++token.textLength;
                ++token.column;
            } while (text[token.index + token.textLength] != '"');
            ++token.textLength;
            // TODO: Handle unclosed ".
            // if (text[token.index + token.textLength] != '"') {
            // }
            appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
            token.index += token.textLength + 1;
        } else {
            // Try to lex an operator.
            token.type = INVALID;
            token.text = text + token.index;
            token.textLength = 0;
            foundOperator = false;
            for (size_t i = 0; i < operatorsCount; ++i) {
                // TODO: Get rid of `strlen()`.
                size_t length = strlen(operators[i]);
                if (strncmp(operators[i], token.text, length) == 0) {
                    token.type = i + PLUS_EQUAL;
                    token.textLength = length;
                    appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                    token.index += token.textLength;
                    token.column += token.textLength;
                    foundOperator = true;
                    break;
                }
            }
            
            // If that fails, append and invalid token and skip whitespace.
            if (!foundOperator) {
                token.type = INVALID;
                token.text = text + token.index;
                token.textLength = 0;
                while (!isspace(text[token.index + token.textLength])) {
                    ++token.textLength;
                    ++token.column;
                }
                appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                token.index += token.textLength;
            }
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
