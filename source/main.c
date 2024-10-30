#include <stdio.h>
#include "lexer.h"
#include "parser.h"

int main(void) {
    char *text = "1 2 3 4";
    LexingResult lexingResult = lex(text);
    printf("LEXING\n");
    LexingResultPrint(&lexingResult);

    ParsingResult parsingResult = parse(lexingResult.tokens);
    printf("\nPARSING\n");
    ParsingResultPrint(&parsingResult);
    
    LexingResultDestroy(&lexingResult);
    return 0;
}
