#include "lexer.h"

typedef struct ReservedToken {
	char *name;
	TokenType type;
} ReservedToken;

static ReservedToken keywords[] = {
	"package",
};

static ReservedToken operators[] = {
	"++",
};

LexingResult lex(char *text, size_t length) {
	if (length == 0) {
		// Lex until the end of the string.
	}
}
