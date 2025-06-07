void *arena = arena_create(1024);
int *num = arena_allocate(arena, sizeof (int));
*num = 1;
arena = arena_ensure_empty(arena, sizeof (int));


#define list_push(list, value) do {typeof(*list) temp = value; list_push_impl(list, &temp);} while (0)
#define list_pop(list) (list[list_pop_impl()])
int **list = list_create(100, sizeof **list);
list_push(list, 1);
list_set(list, 0, 1);
int num = list_get(list, 0);
num = list_pop(list);
list_ensure_empty(list, 10);


#define map_add(map, key, value) do {(*(map))[map_add_impl((map), (key))] = value;} while (0)
#define map_get(map, key) ((*(map))[map_get_impl((map), (key))])
int **map = map_create(100, sizeof **map);
map_add(map, "hello", 1);
int num = map_get(map, "hello");
int *num = map_get_address(map, "hello");
if (!num) {
	// error
}
map_ensure_empty(map, 10);
