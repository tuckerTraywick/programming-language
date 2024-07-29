#include <stdio.h>
#include "lexer.h"

int main(void) {
    char *text = "123 1-\n  \n\r\n\r \t a";
    LexingResult result = lex(text);
    LexingResultPrint(&result);
    LexingResultDestroy(&result);
    return 0;
}
