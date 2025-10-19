#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "map.h"

typedef struct Map_Header {
	size_t capacity;
	size_t size;
	size_t element_size;
	size_t pad;
	char data[];
} Map_Header;

static void *map_get_header(void *map) {
	return (Map_Header*)map - 1;
}

void *map_create(size_t capacity, size_t element_size) {
	Map_Header *map = malloc(sizeof *map + capacity*element_size);
	if (!map) {
		return NULL;
	}
	*map = (Map_Header){
		.capacity = capacity,
		.element_size = element_size,
	};
	return map + 1;
}

void map_destroy(void *map) {
	free(map_get_header(map));
}
