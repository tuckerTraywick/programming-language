#include <stdio.h>
#include "lexer.h"

int main(void) {
    char *text =
    "pub package example\n"
    "pub func fib(n int32) int32 {\n"
    "    if n < 1 {\n"
    "        return n\n"
    "    }\n"
    "    return fib(n - 1) + fib(n - 2)\n"
    "}";
    LexingResult result = lex(text);
    LexingResultPrint(&result);
    LexingResultDestroy(&result);
    return 0;
}
