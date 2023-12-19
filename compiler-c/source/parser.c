#include <assert.h> // assert()
#include <stdbool.h> // bool
#include <stdint.h> // SIZE_MAX
#include <stdio.h> // FILE, fopen(), fclose(), fseek(), frewind(), ftell()
#include <stdlib.h> // malloc(), realloc(), free()
#include <ctype.h> // isdigit(), isalpha(), isalnum(), ispunct()
#include <string.h> // strncmp()
#include "log.h"
#include "parser.h"

#define TOKENS_INITIAL_CAPACITY 3000
#define TOKENS_CAPACITY_INCREMENT 1500

#define ERRORS_INITIAL_CAPACITY 1000
#define ERRORS_CAPACITY_INCREMENT 500

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
    assert((long)bytesRead == fileSize && "Something went wrong while putting the file in a buffer.");
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

bool lexString(char *text, TokenList *tokens, ErrorList *errors) {
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
    listInitialize(tokens, sizeof(struct Token), TOKENS_INITIAL_CAPACITY, TOKENS_CAPACITY_INCREMENT);
    listInitialize(errors, sizeof(struct LexingError), ERRORS_INITIAL_CAPACITY, ERRORS_CAPACITY_INCREMENT);
    struct Token token = {0};
    bool foundOperator = true;

    while (text[token.index]) {
        char ch = text[token.index];
        if (ch == '\n') {
            // Skip newlines.
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
            listAppend(tokens, (char*)(&token));
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
            listAppend(tokens, (char*)(&token));
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
            listAppend(tokens, (char*)(&token));
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
            listAppend(tokens, (char*)(&token));
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
                    listAppend(tokens, (char*)(&token));
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
                listAppend(tokens, (char*)(&token));
                struct LexingError error = {
                    .message="Invalid token.",
                    .index=token.index,
                    .row=token.row,
                    .column=token.column,
                };
                
                while (text[token.index + token.textLength] && !isspace(text[token.index + token.textLength])) {
                    ++token.textLength;
                    ++token.column;
                }
                listAppend(errors, (char*)(&error));
                token.index += token.textLength;
            }
        }
    }

    return !errors->count;
}
