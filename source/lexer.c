#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"

// The initial amount of tokens to allocate.
#define INITIAL_TOKEN_CAPACITY 500

// The initial amount of errors to allocate.
#define INITIAL_ERROR_CAPACITY 50

// Returns the length of an array.
#define lengthOf(array) (sizeof (array) / sizeof (array)[0])

// A mapping from a string to a type of token.
typedef struct ReservedWord {
	char *text;
	TokenType type;
} ReservedWord;

// All of the keywords in the language.
static ReservedWord keywords[] = {
	{"package", PACKAGE},
};

// All of the operators in the language.
static ReservedWord operators[] = {
	{"+", PLUS},
};

// The message for each error type.
char *errorMessages[] = {
	[INVALID_TOKEN] = "Invalid token.",
};

void LexingResultDestroy(LexingResult *result) {
	ListDestroy(&result->tokens);
	ListDestroy(&result->errors);
	*result = (LexingResult){0};
}

void LexingResultPrint(LexingResult *result) {
	static char *tokenTypeNames[] = {
		[INVALID] = "invalid",
		[IDENTIFIER] = "identifier",
		[NUMBER] = "number",
		[CHARACTER] = "character",
		[STRING] = "string",

		[PACKAGE] = "package",
	};

	printf("%lu TOKENS:\n", result->tokens.count);
	for (size_t i = 0; i < result->tokens.count; ++i) {
		Token *token = (Token*)ListGet(&result->tokens, i);
		printf("\"%.*s\" %s\n", (int)token->length, token->text, tokenTypeNames[token->type]);
	}
}

LexingResult lex(char *text, size_t length) {
	TokenList tokens = ListCreate(INITIAL_TOKEN_CAPACITY, sizeof (Token));
	LexingErrorList errors = ListCreate(INITIAL_ERROR_CAPACITY, sizeof (LexingError));
	char *currentChar = text;
	Token currentToken = {.text=currentChar};
	
	// TODO: Only loop `length` times if `length` != 0.
	while (*currentChar != '\0') {
		if (isblank(*currentChar)) {
			// Skip whitespace.
			++currentChar;
			++currentToken.text;
		} else if (isdigit(*currentChar)) {
			// Lex a number.
			currentToken.type = NUMBER;
			do {
				++currentChar;
			} while (*currentChar != '\0' && isdigit(*currentChar));
		} else if (*currentChar == '\'') {

		} else if (*currentChar == '"') {

		} else if (isalpha(*currentChar) || *currentChar == '_') {
			// Lex an identifier.
			currentToken.type = IDENTIFIER;
			do {
				++currentChar;
			} while (isalnum(*currentChar) || *currentChar == '_');
			
			// TODO: Check if the identifier is a keyword.
		} else if (ispunct(*currentChar)) {
			// Lex an operator.
		}

		if (currentToken.type != INVALID) {
			currentToken.length = currentChar - currentToken.text;
			ListPushBack(&tokens, &currentToken);
			currentToken = (Token){.text=currentChar};
		}
	}

	return (LexingResult){
		.tokens = tokens,
		.errors = errors,
	};
}

#undef INTIAL_TOKEN_CAPACITY
#undef INITIAL_ERROR_CAPACITY
