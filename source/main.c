#include <stdio.h>
#include "lexer.h"

int main(void) {
    char *text = "123 a";
    LexingResult result = lex(text);
    LexingResultPrint(&result);
    LexingResultDestroy(&result);
    return 0;
}
 