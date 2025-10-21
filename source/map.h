#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct Bucket {
	size_t key_offset;
	char data[];
} Bucket;

typedef struct Map {
	size_t buckets_capacity;
	size_t buckets_size;
	size_t value_size;
	Bucket *buckets;
	size_t keys_capacity;
	size_t keys_size;
	char *keys;
} Map;

void *map_create(size_t capacity, size_t element_size);

void map_destroy(void *map);

size_t map_get_capacity(void *map);

size_t map_get_size(void *map);

size_t map_get_element_size(void *map);

// Returns true if the map has no more empty buckets.
bool map_is_full(void *map);

// Increases the map's capacity and rehashes its buckets.
void *map_grow_capacity(void *map, size_t amount);

// Returns a pointer to the value. Returns NULL if the key is not in the map.
void *map_get(void *map, char *key);

// Returns the map. May reallocate the map.
void *map_set(void *map, char *key, void *value);

#endif // MAP_H
