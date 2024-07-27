#include <stdio.h>
#include "lexer.h"

int main(void) {
    char *text = "1 -;;1()2";
    LexingResult result = lex(text, 0);
    LexingResultPrint(&result);
    LexingResultDestroy(&result);
    return 0;
}
