#include <stdio.h>
#include "lexer.h"
#include "parser.h"

int main(void) {
    char *text = "foo";
    LexingResult lexingResult = lex(text);
    LexingResultPrint(&lexingResult);
    LexingResultDestroy(&lexingResult);
    return 0;
}
