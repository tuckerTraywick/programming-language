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

    FROM,
    IMPORT,
};

// Represents a token lexed from an input stream. Stores information about the token's contents and
// its position in the string being lexed.
struct Token {
    enum TokenType type; // The type of the token.
    char *text; // The text of the token. NOT null-terminated. UNOWNED, DO NOT FREE.
    size_t textLength; // The number of characters in the token.
    size_t index; // The index of the token in the string being lexed.
    size_t row; // The line number of the token in the string being lexed.
    size_t column; // The index of the token relative to the start of the line the token is on.
};

// Represents a mapping from an identifier to a token type.
struct KeywordMapping {
    const char *identifier; // The identifier that is a keyword.
    enum TokenType type; // The type of the token representing that keyword.
};

// Represents the result of lexing. Stores a list of tokens and possibly a list of error messages.
// Must be destroyed by `destroyLexingResult()` after use.
struct LexingResult {
    struct Token *tokens; // The tokens lexed. `NULL` if no tokens were lexed.
    size_t tokensCount; // The number of tokens lexed. 0 if no tokens were lexed.
    char **errorMessages; // The error messages generated while lexing. `NULL` if no errors were encountered.
    size_t errorMessagesCount; // The number of errors encountered. Can be 0.
};

// Represents the type of a node.
enum NodeType {
    PROGRAM,
    PACKAGE_STATEMENT,
};

// Represents a node in a parse tree.
struct Node {
    enum NodeType type; // The type of the node.
    struct Node *children; // The node's children. `NULL` if the node has no children.
    size_t childrenCount; // The number of children the node has. Can be 0.
};

// Represents the result of parsing. Stores a parse tree and possibly a list of error messages. Must
// be destroyed by `destroyParsingResult()` after use.
struct ParsingResult {
    struct Node *tree; // The top node of the syntax tree parsed from the string. `NULL` if a tree wasn't able to be parsed.
    char **errorMessages; // The error messages generated during parsing. `NULL` if no errors were encountered.
    size_t errorMessagesCount; // The number of errors encountered. Can be 0.
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

