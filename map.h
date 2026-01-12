#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define map_destroy(map) (map_destroy_impl((void**)(map)))

#define map_get_buckets_capacity(map) (map_get_buckets_capacity_impl((void**)(map)))

// If you pass a capacity smaller than the map's current size, this function does nothing and
// returns false.
#define map_set_buckets_capacity(map, capacity) (map_set_buckets_capacity_impl((void**)(map, (capacity))))

#define map_get_buckets_count(map) (map_get_buckets_count_impl((void**)(map)))

#define map_get_bucket_size(map) (map_get_bucket_size_impl((void**)(map)))

#define map_get_keys_capacity(map) (map_get_keys_capacity_impl((void**)(map)))

// #define map_set_keys_capacity(map, capacity) (map_set_keys_capacity_impl((void**)(map, (capacity))))

#define map_get_keys_size(map) (map_get_keys_size_impl((void**)(map)))

#define map_is_empty(map) (map_is_empty_impl((void**)(map)))

#define map_is_not_empty(map) (map_is_not_empty_impl((void**)(map)))

#define map_get(map, key) (map_get_impl((void**)(map), (key)))

#define map_set(map, key, value) (map_set_impl((void**)(map), (key), (value)))

#define map_add(map, key, value) (map_add_impl((void**)(map), (key), (value)))

void *map_create(size_t buckets_capacity, size_t bucket_count, size_t keys_capacity);

void map_destroy_impl(void **map);

size_t map_get_buckets_capacity_impl(void **map);

bool map_set_buckets_capacity_impl(void **map, size_t capacity);

size_t map_get_buckets_count_impl(void **map);

size_t map_get_bucket_size_impl(void **map);

size_t map_get_keys_capacity_impl(void **map);

size_t map_get_keys_size_impl(void **map);

bool map_is_empty_impl(void **map);

bool map_is_not_empty_impl(void **map);

void *map_get_impl(void **map, char *key);

bool map_set_impl(void **map, char *key, void *value);

bool map_add_impl(void **map, char *key, void *value);

#endif // MAP_H
