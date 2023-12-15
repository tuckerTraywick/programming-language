#include <stdbool.h>
#include <stddef.h> // size_t
#include <stdio.h> // FILE

// The initial number of tokens to allocate when lexing a file.
#ifndef TOKENS_INITIAL_CAPACITY
    #define TOKENS_INITIAL_CAPACITY 1000
#endif

// The number of additional tokens to allocate when the lexer runs out of space for new tokens.
#ifndef TOKENS_CAPACITY_INCREMENT
    #define TOKENS_CAPACITY_INCREMENT 100
#endif

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
    PRIV,
    CONST,
    MUT,
    OWNED,
    SHARED,
    WEAK,
    VAR,
    FUN,
    STRUCT,
    ALIAS,
    CASES,
    EMBED,
    IMPL,
    PASS,
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
    UNTIL,
    THRU,
    BY,
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

    PLUS_EQUAL,
    MINUS_EQUAL,
    ARROW,
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
    DOT,
    COMMA,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
};

// Represents a token lexed from an input stream. Stores information about the token's contents and
// its position in the string being lexed.
struct Token {
    enum TokenType type;
    char *text; // NOT null-terminated. UNOWNED, DO NOT FREE.
    size_t textLength;
    size_t index;
    size_t row; // Line number.
    size_t column; // Index relative to line start.
};

// Represents the result of lexing. Stores a list of tokens and possibly a list of error messages.
// Must be destroyed by `destroyLexingResult()` after use.
struct LexingResult {
    struct Token *tokens; // `NULL` if no tokens were lexed.
    size_t tokensCount;
    char **errorMessages; // `NULL` if no errors were encountered.
    size_t errorMessagesCount;
};

// Represents the type of a node.
enum NodeType {
    PROGRAM,
    PACKAGE_STATEMENT,
};

// Represents a node in a parse tree.
struct Node {
    enum NodeType type;
    struct Node *children; // `NULL` if the node has no children.
    size_t childrenCount;
};

// Represents the result of parsing. Stores a parse tree and possibly a list of error messages. Must
// be destroyed by `destroyParsingResult()` after use.
struct ParsingResult {
    struct Node *tree; // `NULL` if a tree wasn't able to be parsed.
    char **errorMessages; // `NULL` if no errors were encountered.
    size_t errorMessagesCount;
};

// Allocates a string and reads the contents of `file` into it. Returns `NULL` if an error occurred
// while allocating the string or reading the file. Returns a pointer to the string on success. The
// string must be deallocated with `free()` after use. Does NOT change `file`'s stream position or
// close it.
char *readFile(FILE *file);

// Opens the file at `path` then allocates a string and reads the contents of the file into it.
// Returns `NULL` if an error occurred while allocating the string or opening or reading the file.
// Returns a pointer to the string on success. The string must be deallocated with `free()` after
// use.
char *openAndReadFile(char *path);

// Tries to lex `text` into tokens. `text` must be null terminated. Does not produce tokens for
// newline characters if `ignoreNewlines` is true. 
struct LexingResult lexString(char *text, bool ignoreNewlines);

struct LexingResult lexFile(FILE *file);

struct LexingResult openAndLexFile(char *path);

struct ParsingResult parseTokens(struct Token *tokens, size_t tokensLength);

struct ParsingResult parseString(char *text);

struct ParsingResult parseFile(FILE *file);

struct ParsingResult openAndParseFile(char *path);

// Destroys a `LexingResult` and deallocates its memory.
void destroyLexingResult(struct LexingResult *result);

// Destroys a `ParsingResult` and deallocates its memory.
void destroyParsingResult(struct ParsingResult *result);
