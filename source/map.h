#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <stdbool.h>

void *map_create(size_t capacity, size_t element_size);

void map_destroy(void *map);

// Returns true if the map has no more empty buckets.
bool map_is_full(void *map);

// Returns a pointer to the value. Returns NULL if the key is not in the map.
void *map_get(void *map, char *key);

// Returns the map. May reallocate the map.
void *map_set(void *map, char *key, void *value);

// Increases the map's capacity and rehashes its buckets.
void *map_grow_capacity(void *map, size_t amount);

#endif // MAP_H
