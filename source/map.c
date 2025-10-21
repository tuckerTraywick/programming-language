#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "map.h"

typedef struct Map_Header {
	size_t capacity;
	size_t size;
	size_t element_size;
	char *keys;
	size_t *key_offsets;
	char data[];
} Map_Header;

static Map_Header *map_get_header(void *map) {
	return (Map_Header*)map - 1;
}

// This map implementation assumes that a completely zero bucket means it is empty.
// static bool map_bucket_is_full(void *map, size_t bucket_index) {
// 	unsigned char *bucket = &map_get_header(map)->data;
// 	for (size_t i = 0; i < map_get_element_size(map); ++i) {
// 		if (bucket + i != 0) {
// 			return true;
// 		}
// 	}
// 	return false;
// }

static size_t hash(char *key) {
	size_t result = 0;
	while (*key) {
		result = (result << 5) + result + *key;
		++key;
	}
	return result;
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

size_t map_get_capacity(void *map) {
	return map_get_header(map)->capacity;
}

size_t map_get_size(void *map) {
	return map_get_header(map)->size;
}

size_t map_get_element_size(void *map) {
	return map_get_header(map)->element_size;
}

bool map_is_full(void *map) {
	return map_get_header(map)->size == map_get_header(map)->capacity;
}

void *map_grow_capacity(void *map, size_t amount) {

}

void *map_set(void *map, char *key, void *value) {
	if (map_is_full(map)) {
		map = map_grow_capacity(map, map_get_capacity(map)/2);
		if (!map) {
			return NULL;
		}
	}
	size_t index = hash(key)%map_get_capacity(map);
	if (map_bucket_is_full(map, index) && strcmp(key, )) {
	}
}
