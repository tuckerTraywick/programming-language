#include <stdbool.h> // bool
#include <stdint.h> // SIZE_MAX
#include <assert.h> // assert()
#include <stdio.h> // FILE, fopen(), fclose(), fseek(), frewind(), ftell()
#include <stdlib.h> // malloc(), realloc(), free()
#include <ctype.h> // isdigit(), isalpha(), isalnum(), ispunct()
#include <string.h> // strcmp()
#include "parser.h"

// Appends `token` to `tokens`. Updates `*tokens` `*tokensCapacity`, and `*tokensCount` as needed.
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
        "pub",
        "priv",
        "const",
        "mut",
        "owned",
        "shared",
        "weak",
        "var",
        "fun",
        "struct",
        "alias",
        "cases",
        "embed",
        "impl",
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
        "until",
        "thru",
        "by",
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
        ".",
        ",",
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
    bool keepLexing = true;

    while (keepLexing && text[token.index] != '\0') {
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
                token.text = text + token.index;
                token.textLength = 0;
                while (isdigit(text[token.index + token.textLength])) {
                    ++token.textLength;
                }
                appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                token.index += token.textLength;
                token.column += token.textLength;
                break;
            case 'A'...'Z':
            case 'a'...'z':
            case '_':
                // Lex an identifier.
                token.type = IDENTIFIER;
                token.text = text + token.index;
                token.textLength = 0;
                while (isalnum(text[token.index + token.textLength])) {
                    ++token.textLength;
                }

                for (size_t i = 0; i < keywordsCount; ++i) {
                    if (strcmp(token.text, keywords[i]) == 0) {
                        token.type = i + PACKAGE;
                        break;
                    }
                }
                appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                token.index += token.textLength;
                token.column += token.textLength;
                break;
            case '!'...'/':
            case ':'...'@':
            case '['...'^':
            case '`':
            case '{'...'~':
                // Lex an operator.
                keepLexing = false;
                for (size_t i = 0; i < operatorsCount; ++i) {
                    if (strcmp(token.text, keywords[i]) == 0) {
                        token.type = i + PLUS_EQUAL;
                        token.textLength = strlen(keywords[i]);
                        appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                        token.index += token.textLength;
                        token.column += token.textLength;
                        keepLexing = true;
                        break;
                    }
                }
                break;
            default:
                token.type = INVALID;
                token.text = text + token.index;
                token.textLength = 0;
                appendToken(&token, &tokens, &tokensCapacity, &tokensCount);
                keepLexing = false;
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
