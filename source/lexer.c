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
	{"++", INCREMENT},
	{"+", PLUS},
	{";", SEMICOLON},
};

// The message for each error type.
// TODO: Add formatting specifiers to these that take the token as an argument.
char *lexingErrorMessages[] = {
	[INVALID_TOKEN] = "Invalid token.",
};

// Returns true if `string` starts with `prefix`.
static bool startsWith(char *string, char *prefix) {
	while (*prefix != '\0') {
		if (*string != *prefix) {
			return false;
		}
		++prefix;
		++string;
	}
	return true;
}

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

		[INCREMENT] = "increment",
		[PLUS] = "plus",
		[SEMICOLON] = "semicolon",
	};

	printf("%lu TOKENS:\n", result->tokens.count);
	for (size_t i = 0; i < result->tokens.count; ++i) {
		Token *token = (Token*)ListGet(&result->tokens, i);
		printf("%3zu \"%.*s\" %s\n", token->index, (int)token->length, token->text, tokenTypeNames[token->type]);
	}

	printf("\n%lu ERRORS:\n", result->errors.count);
	for (size_t i = 0; i < result->errors.count; ++i) {
		LexingError *error = (LexingError*)ListGet(&result->errors, i);
		printf("%3zu \"%.*s\": %s\n", error->token.index, (int)error->token.length, error->token.text, lexingErrorMessages[error->type]);
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
			
			// Check if the identifier matches any of the keywords.
			for (size_t i = 0; i < lengthOf(keywords); ++i) {
				size_t keywordLength = strlen(keywords[i].text);
				if (keywordLength == (size_t)(currentChar - currentToken.text)
				    && startsWith(currentToken.text, keywords[i].text)) {
					currentToken.type = keywords[i].type;
					currentChar += keywordLength;
					break;
				}
			}
		} else if (ispunct(*currentChar)) {
			// Check if the text matches any of the operators.
			for (size_t i = 0; i < lengthOf(operators); ++i) {
				if (startsWith(currentToken.text, operators[i].text)) {
					currentToken.type = operators[i].type;
					currentChar += strlen(operators[i].text);
					break;
				}
			}

			// If no operator was found, emit an error and recover to the next token.
			if (currentToken.type == INVALID) {
				while (*currentChar != '\0' && *currentChar != ';' && !isblank(*currentChar)) {
					++currentChar;
				}

				LexingError error = {
					.type = INVALID_TOKEN,
					.message = lexingErrorMessages[INVALID_TOKEN],
					.token = {
						.type = INVALID,
						.length = currentChar - currentToken.text,
						.text = currentToken.text,
						.index = currentToken.text - text,
					},
				};
				ListPushBack(&errors, &error);
				ListPushBack(&tokens, &error.token);
				currentToken = (Token){.text=currentChar};
				continue;
			}
		}

		// Append the token just lexed to the list of tokens.
		if (currentToken.type != INVALID) {
			currentToken.length = currentChar - currentToken.text;
			currentToken.index = currentToken.text - text;
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
