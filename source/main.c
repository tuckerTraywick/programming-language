#include <stdio.h>
#include "lexer.h"

int main(void) {
    char *text = "package123 packa";
    LexingResult result = lex(text, 0);
    LexingResultPrint(&result);
    LexingResultDestroy(&result);
    return 0;
}
