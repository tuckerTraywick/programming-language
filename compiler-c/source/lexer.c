#include <assert.h> // assert()
#include <stdbool.h> // bool
#include <stdio.h> // FILE, fopen(), fclose(), fseek(), frewind(), ftell()
#include <stdlib.h> // malloc(), free()
#include <ctype.h> // isdigit(), isalpha(), isalnum(), ispunct()
#include <string.h> // strncmp()
#include "lexer.h"
#include "list.h"

#define TOKENS_INITIAL_CAPACITY 3000
#define ERRORS_INITIAL_CAPACITY 500

// Returns the max of `a` and `b`. Helper for `lexString()`.
static size_t max(size_t a, size_t b) {
    return (a >= b) ? a : b;
}

char *tokenTypeNames[] = {
    [INVALID_TOKEN] = "Invalid token.",
    [CHARACTER_UNCLOSED_QUOTE] = "Unclosed '.",
    [STRING_UNCLOSED_QUOTE] = "Unclosed \".",

    [NUMBER] = "number",
    [CHARACTER] = "character",
    [STRING] = "string",
    [IDENTIFIER] = "identifier",
    [NEWLINE] = "newline",
};

void destroyLexingResult(struct LexingResult *result) {
    assert(result != NULL && "Must pass a result.");
    free(result->tokens);
    free(result->errors);
    *result = (struct LexingResult){0};
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

struct LexingResult lexString(char *text) {
    // A mapping of keywords to their token types.
    static char* keywords[] = {
        "package",
        "from",
        "import",
        "pub",
        "const",
        "mut",
        "owned",
        "weak",
        "var",
        "fun",
        "struct",
        "cases",
        "embed",
        "embeds",
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
        "this",
        "auto",
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
    struct List tokens = listCreate(struct Token, TOKENS_INITIAL_CAPACITY);
    struct List errors = listCreate(struct Token, ERRORS_INITIAL_CAPACITY);
    struct Token token = {0};

    while (text[token.index]) {
        char ch = text[token.index];
        if (ch == '\n') {
            // Lex a newline, just skip it if the previous token was a newline.
            token.type = NEWLINE;
            token.length = 1;
            size_t newlineCount = 0;
            do {
                ++newlineCount;
            } while (text[token.index + newlineCount] == '\n');

            if (listGet(struct Token, &tokens, tokens.count - 1)->type != NEWLINE) {
                listAppend(&tokens, &token);
            }
            token.index += newlineCount;
            token.row += newlineCount;
            token.column = 0;
        } else if (isspace(ch)) {
            // Skip whitespace.
            ++token.index;
            ++token.column;
        }  else if (ch == '/' && text[token.index + 1] && text[token.index + 1] == '/') {
            // Skip line comments.
            do {
                ++token.index;
                ++token.column;
            } while (text[token.index] && text[token.index] != '\n');
        } else if (isdigit(ch)) {
            // Lex a number.
            token.type = NUMBER;
            token.text = text + token.index;
            token.length = 0;
            do {
                ++token.length;
                ++token.column;
            } while (isdigit(text[token.index + token.length]));            
            listAppend(&tokens, &token);
            token.index += token.length;
        } else if (isalpha(ch) || ch == '_') {
            // Lex an identifier or a keyword.
            token.type = IDENTIFIER;
            token.text = text + token.index;
            token.length = 0;
            do {
                ++token.length;
            } while (isalnum(text[token.index + token.length]));

            for (size_t i = 0; i < keywordsCount; ++i) {
                // TODO: Get rid of `strlen()` here and store the lengths of each keyword in the
                // array.
                if (strncmp(keywords[i], token.text, max(token.length, strlen(keywords[i]))) == 0) {
                    token.type = i + PACKAGE;
                    break;
                }
            }
            listAppend(&tokens, &token);
            token.index += token.length;
            token.column += token.length;
        } else if (ch == '\'') {
            // Lex a character literal.
            token.type = CHARACTER;
            token.text = text + token.index;
            token.length = 0;
            do {
                ++token.length;
            } while (text[token.index + token.length] && text[token.index + token.length] != '\''
                     && text[token.index + token.length] != '\n');

            // Handle an unclosed quote.
            if (text[token.index + token.length] == '\'') {
                ++token.length;
            } else {
                token.type = CHARACTER_UNCLOSED_QUOTE;
                listAppend(&errors, &token);
            }

            // TODO: Check for an escape sequence.
            listAppend(&tokens, &token);
            token.index += token.length;
            token.column += token.length;
        } else if (ch == '"') {
            // Lex a string literal.
            token.type = STRING;
            token.text = text + token.index;
            token.length = 0;
            do {
                ++token.length;
            } while (text[token.index + token.length] && text[token.index + token.length] != '"'
                     && text[token.index + token.length] != '\n');

            // Handle an unclosed quote.
            if (text[token.index + token.length] == '"') {
                ++token.length;
            } else {
                token.type = STRING_UNCLOSED_QUOTE;
                listAppend(&errors, &token);
            }

            // TODO: Check for escape sequences.
            listAppend(&tokens, &token);
            token.index += token.length;
            token.column += token.length;
        } else {
            // Try to lex an operator.
            token.type = INVALID_TOKEN;
            token.text = text + token.index;
            token.length = 0;
            bool foundOperator = false;
            for (size_t i = 0; i < operatorsCount; ++i) {
                // TODO: Get rid of `strlen()`.
                size_t length = strlen(operators[i]);
                if (strncmp(operators[i], token.text, length) == 0) {
                    token.type = i + PLUS_EQUAL;
                    token.length = length;
                    listAppend(&tokens, &token);
                    token.index += token.length;
                    token.column += token.length;
                    foundOperator = true;
                    break;
                }
            }
            
            // If that fails, append and `INVALID_TOKEN` and skip whitespace.
            if (!foundOperator) {
                token.type = INVALID_TOKEN;
                token.text = text + token.index;
                token.length = 0;
                listAppend(&errors, &token);
                
                do {
                    ++token.length;
                } while (text[token.index + token.length] && !isspace(text[token.index + token.length]));
                listAppend(&tokens, &token);
                token.index += token.length;
                token.column += token.length;
            }
        }
    }

    return (struct LexingResult){
        .tokens = (struct Token*)tokens.elements,
        .tokensCount = tokens.count,
        .errors = (struct Token*)errors.elements,
        .errorsCount = errors.count,
    };
}

#undef TOKENS_INITIAL_CAPACITY
#undef ERRORS_INITIAL_CAPACITY
