#include <stdio.h>
#include "arena.h"
#include "lexer.h"
#include "parser.h"

int main(void) {
	int *numbers = arena_create(10*sizeof (int));
	int *first = arena_allocate(numbers, sizeof (int));
	*first = 1;
	printf("number = %d\n", *first);
	arena_destroy(numbers);
	return 0;
}
