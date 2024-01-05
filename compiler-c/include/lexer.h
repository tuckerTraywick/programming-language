#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h> // size_t
#include <stdio.h> // FILE
#include "list.h"

// Represents an error encountered during lexing.
struct LexingError {
    char *message; // Unowned, don't free.
    size_t index;
    size_t row; // Line number.
    size_t column; // Index relative to line start.
};

// Represents the type of a token.
enum TokenType {
    INVALID,
    NUMBER,
    CHARACTER,
    STRING,
    IDENTIFIER,
    NEWLINE,

    PACKAGE,
    FROM,
    IMPORT,
    PUB,
    CONST,
    MUT,
    OWNED,
    WEAK,
    VAR,
    FUN,
    STRUCT,
    CASES,
    EMBED,
    EMBEDS,
    RETURN,
    YIELD,
    BREAK,
    CONTINUE,
    IF,
    ELSE,
    SWITCH,
    CASE,
    DEFAULT,
    FALLTHROUGH,
    FOR,
    IN,
    DO,
    WHILE,
    AS,
    IS,
    ISNOT,
    AND,
    OR,
    XOR,
    NOT,
    T,
    F,
    THIS,
    AUTO,

    PLUS_EQUAL,
    PLUS,
    MINUS_EQUAL,
    THIN_ARROW,
    MINUS,
    TIMES_EQUAL,
    TIMES,
    DIVIDE_EQUAL,
    DIVIDE,
    MODULUS_EQUAL,
    MODULUS,
    BITWISE_AND_EQUAL,
    BITWISE_AND,
    BITWISE_OR_EQUAL,
    BITWISE_OR,
    BITWISE_XOR_EQUAL,
    BITWISE_XOR,
    BITWISE_NOT_EQUAL,
    BITWISE_NOT,
    LEFT_SHIFT_EQUAL,
    LEFT_SHIFT,
    LESS_EQUAL,
    LESS,
    RIGHT_SHIFT_EQUAL,
    RIGHT_SHIFT,
    GREATER_EQUAL,
    GREATER,
    EQUAL,
    FAT_ARROW,
    ASSIGN,
    NOT_EQUAL,
    TRIPLE_DOT,
    DOUBLE_DOT,
    DOT,
    COMMA,
    COLON,
    SEMICOLON,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,

    TOKEN_TYPE_COUNT,
};

// Represents a token lexed from an input stream.
struct Token {
    enum TokenType type;
    char *text; // Not null-terminated. Unowned, don't free.
    size_t length;
    size_t index;
    size_t row; // Line number.
    size_t column; // Index relative to line start.
};

// Represents the result of lexing a string. Stores a list of tokens and a list of errors.
struct LexingResult {
    struct Token *tokens;
    size_t tokensCount;
    struct LexingError *errors;
    size_t errorsCount;
};

// Deallocates a `LexingResult`'s buffers and zeros its memory.
void destroyLexingResult(struct LexingResult *result);

// Reads a file into a string. The string must be deallocated with `free()` after use.
char *readFile(FILE *file);

// Reads the file at `path` into a string. The string must be deallocated with `free()` after use.
char *openAndReadFile(char *path);

// Lexes a string into an array of tokens.
struct LexingResult lexString(char *text);

#endif // LEXER_H
