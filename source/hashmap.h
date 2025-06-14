int *numbers = list_create(100, sizeof *numbers);
int num = 1;
if (!list_push(&numbers, &num)) {
	// error
}
if (!list_pop(&numbers, &num)) {
	// error
}
// use num




int *numbers = map_create(100, sizeof *numbers);
int num = 1;
if (!map_set(&numbers, "hello", &num)) {
	// error
}
if (!map_get(&numbers, "hello", &num)) {
	// not found
}
// use num
