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
	{"from", FROM},
	{"import", IMPORT},
	{"export", EXPORT},
	{"var", VAR},
	{"func", FUNC},
	{"method", METHOD},
	{"struct", STRUCT},
	{"cases", CASES},
	{"pub", PUB},
	{"static", STATIC},
	{"const", CONST},
	{"mut", MUT},
	{"owned", OWNED},
	{"weak", WEAK},
	{"new", NEW},
	{"make", MAKE},
	{"drop", DROP},
	{"move", MOVE},
	{"defer", DEFER},
	{"assert", ASSERT},
	{"where", WHERE},
	{"while", WHILE},
	{"for", FOR},
	{"if", IF},
	{"switch", SWITCH},
	{"match", MATCH},
	{"default", DEFAULT},
	{"return", RETURN},
	{"break", BREAK},
	{"continue", CONTINUE},
	{"as", AS},
	{"is", IS},
	{"and", LOGICAL_AND},
	{"or", LOGICAL_OR},
	{"xor", LOGICAL_XOR},
	{"not", LOGICAL_NOT},
};

// All of the operators in the language.
static ReservedWord operators[] = {
	{"++", INCREMENT},
	{"+=", PLUS_ASSIGN},
	{"+", PLUS},
	{"--", DECREMENT},
	{"-=", MINUS_ASSIGN},
	{"->", ARROW},
	{"-", MINUS},
	{"*=", TIMES_ASSIGN},
	{"*", TIMES},
	{"/=", DIVIDE_ASSIGN},
	{"/", DIVIDE},
	{"<<=", LEFT_SHIFT_ASSIGN},
	{"<<", LEFT_SHIFT},
	{"<=", LESS_EQUAL},
	{"<", LESS},
	{">>=", RIGHT_SHIFT_ASSIGN},
	{">>", RIGHT_SHIFT},
	{">=", GREATER_EQUAL},
	{">", GREATER},
	{"==", EQUAL},
	{"=", ASSIGN},
	{"!=", NOT_EQUAL},
	{"&=", BITWISE_AND_ASSIGN},
	{"&", BITWISE_AND},
	{"|=", BITWISE_OR_ASSIGN},
	{"|", BITWISE_OR},
	{"^=", BITWISE_XOR_ASSIGN},
	{"^", BITWISE_XOR},
	{"~=", BITWISE_NOT_ASSIGN},
	{"~", BITWISE_NOT},
	{",", COMMA},
	{".", DOT},
	{";", SEMICOLON},
	{"(", LEFT_PARENTHESIS},
	{")", RIGHT_PARENTHESIS},
	{"[", LEFT_BRACKET},
	{"]", RIGHT_BRACKET},
	{"{", LEFT_BRACE},
	{"}", RIGHT_BRACE},
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
		[INVALID_TOKEN] = "Invalid token.",
		[UNCLOSED_SINGLE_QUOTE] = "Unclosed single quote.",
		[UNCLOSED_DOUBLE_QUOTE] = "Unclosed double quote.",

		[NEWLINE] = "newline",
		[NUMBER] = "number",
		[CHARACTER] = "character",
		[STRING] = "string",
		[IDENTIFIER] = "identifier",

		[PACKAGE] = "package",
		[FROM] = "from",
		[IMPORT] = "import",
		[EXPORT] = "export",
		[VAR] = "var",
		[FUNC] = "func",
		[METHOD] = "method",
		[STRUCT] = "struct",
		[CASES] = "cases",
		[PUB] = "pub",
		[STATIC] = "static",
		[CONST] = "const",
		[MUT] = "mut",
		[OWNED] = "owned",
		[WEAK] = "weak",
		[NEW] = "new",
		[MAKE] = "make",
		[DROP] = "drop",
		[MOVE] = "move",
		[DEFER] = "defer",
		[ASSERT] = "assert",
		[WHERE] = "where",
		[WHILE] = "while",
		[FOR] = "for",
		[IF] = "if",
		[SWITCH] = "switch",
		[MATCH] = "match",
		[DEFAULT] = "default",
		[RETURN] = "return",
		[BREAK] = "break",
		[CONTINUE] = "continue",
		[AS] = "as",
		[IS] = "is",
		[LOGICAL_AND] = "logical and",
		[LOGICAL_OR] = "logical or",
		[LOGICAL_XOR] = "logical xor",
		[LOGICAL_NOT] = "logical not",

		[INCREMENT] = "increment",
		[PLUS_ASSIGN] = "plus assign",
		[PLUS] = "plus",
		[DECREMENT] = "decrement",
		[MINUS_ASSIGN] = "minus assign",
		[ARROW] = "arrow",
		[MINUS] = "minus",
		[TIMES_ASSIGN] = "times assign",
		[TIMES] = "times",
		[DIVIDE_ASSIGN] = "divide assign",
		[DIVIDE] = "divide",
		[LEFT_SHIFT_ASSIGN] = "left shift assign",
		[LEFT_SHIFT] = "left shift",
		[LESS_EQUAL] = "less equal",
		[LESS] = "less",
		[RIGHT_SHIFT_ASSIGN] = "right shift assign",
		[RIGHT_SHIFT] = "right shift",
		[GREATER_EQUAL] = "greater equal",
		[GREATER] = "greater",
		[BITWISE_AND_ASSIGN] = "bitwise and assign",
		[BITWISE_AND] = "bitwise and",
		[BITWISE_OR_ASSIGN] = "bitwise or assign",
		[BITWISE_OR] = "bitwise or",
		[BITWISE_XOR_ASSIGN] = "bitwise xor assign",
		[BITWISE_XOR] = "bitwise xor",
		[BITWISE_NOT_ASSIGN] = "bitwise not assign",
		[BITWISE_NOT] = "bitwise not",
		[COMMA] = "comma",
		[DOT] = "dot",
		[SEMICOLON] = "semicolon",
		[LEFT_PARENTHESIS] = "left parenthesis",
		[RIGHT_PARENTHESIS] = "right parenthesis",
		[LEFT_BRACKET] = "left bracket",
		[RIGHT_BRACKET] = "right bracket",
		[LEFT_BRACE] = "left brace",
		[RIGHT_BRACE] = "right brace",
	};

	printf("%lu TOKENS:\n", result->tokens.count);
	for (size_t i = 0; i < result->tokens.count; ++i) {
		Token *token = (Token*)ListGet(&result->tokens, i);
		if (token->type == NEWLINE) {
			printf("%-3zu `\\n` newline\n", token->index);
		} else {
			printf("%-3zu `%.*s` %s\n", token->index, (int)token->length, token->text, tokenTypeNames[token->type]);
		}
	}

	printf("\n%lu ERRORS:\n", result->errors.count);
	for (size_t i = 0; i < result->errors.count; ++i) {
		Token *error = (Token*)ListGet(&result->errors, i);
		if (error->type == NEWLINE) {
			printf("%-3zu `\\n` newline\n", error->index);
		} else {
			printf("%-3zu `%.*s` %s\n", error->index, (int)error->length, error->text, tokenTypeNames[error->type]);
		}
	}
}

LexingResult lex(char *text) {
	TokenList tokens = ListCreate(INITIAL_TOKEN_CAPACITY, sizeof (Token));
	TokenList errors = ListCreate(INITIAL_ERROR_CAPACITY, sizeof (Token));
	char *currentChar = text;
	Token currentToken = {.text=currentChar};
	
	// TODO: Lex newlines.
	while (*currentChar != '\0') {
		if (*currentChar == '\n') {
			// Combine multiple newlines into one token.
			currentToken.type = NEWLINE;
			do {
				++currentChar;
			} while (isspace(*currentChar));
		} else if (isspace(*currentChar)) {
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
			// Lex a character literal.
			currentToken.type = CHARACTER;
			do {
				// TODO: Emit an error for an invalid or incomplete escape sequence.
				if (*currentChar == '\\' && *(currentChar + 1) == '\'') {
					++currentChar;
				}
				++currentChar;
			} while (*currentChar != '\0' && *currentChar != '\n' && *currentChar != '\'');

			if (*currentChar == '\'') {
				++currentChar;
			} else {
				// Emit an error.
				Token error = {
					.type = UNCLOSED_SINGLE_QUOTE,
					.text = currentToken.text,
					.length = currentChar - currentToken.text,
					.index = currentToken.text - text,
				};
				ListPushBack(&errors, &error);
				ListPushBack(&tokens, &error);
				currentToken = (Token){.text=currentChar};
				continue;
			}
		} else if (*currentChar == '"') {
			// Lex a string literal.
			currentToken.type = STRING;
			do {
				// TODO: Emit an error for an invalid or incomplete escape sequence.
				if (*currentChar == '\\' && *(currentChar + 1) == '"') {
					++currentChar;
				}
				++currentChar;
			} while (*currentChar != '\0' && *currentChar != '\n' && *currentChar != '"');

			if (*currentChar == '"') {
				++currentChar;
			} else {
				// Emit an error.
				Token error = {
					.type = UNCLOSED_DOUBLE_QUOTE,
					.text = currentToken.text,
					.length = currentChar - currentToken.text,
					.index = currentToken.text - text,
				};
				ListPushBack(&errors, &error);
				ListPushBack(&tokens, &error);
				currentToken = (Token){.text=currentChar};
				continue;
			}
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

			// Recover to the next token and emit an error if no operator was found.
			if (currentToken.type == INVALID_TOKEN) {
				while (*currentChar != '\0' && *currentChar != ';' && !isspace(*currentChar)) {
					++currentChar;
				}

				// Emit an error.
				Token error = {
					.type = INVALID_TOKEN,
					.text = currentToken.text,
					.length = currentChar - currentToken.text,
					.index = currentToken.text - text,
				};
				ListPushBack(&errors, &error);
				ListPushBack(&tokens, &error);
				currentToken = (Token){.text=currentChar};
				continue;
			}
		} // TODO: else {emit an error for non-printable characters}

		// Append the token just lexed to the list of tokens.
		if (currentToken.type != INVALID_TOKEN) {
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
