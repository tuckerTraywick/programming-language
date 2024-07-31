#include <stdio.h>
#include "lexer.h"
#include "parser.h"

int main(void) {
    char *text = "1";
    LexingResult lexingResult = lex(text);
    LexingResultPrint(&lexingResult);

    ParsingResult ParsingResult = parse(lexingResult.tokens);
    printf("\n");
    ParsingResultPrint(&ParsingResult);

    LexingResultDestroy(&lexingResult);
    ParsingResultDestroy(&ParsingResult);
    return 0;
}
