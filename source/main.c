#include <stdio.h>
#include "arena.h"
#include "lexer.h"
#include "parser.h"

int main(void) {
	int *numbers = arena_create(5*sizeof *numbers);
	int num = 2;
	int *pushed = arena_push(numbers, &num, sizeof num);
	printf("num = %d\n", *pushed);
	printf("popped %d bytes\n", arena_pop(numbers, sizeof num));
	printf("size = %d, capacity = %d\n", arena_get_size(numbers), arena_get_capacity(numbers));
	arena_destroy(numbers);
	return 0;
}
