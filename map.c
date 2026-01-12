#include <stdio.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

#define max(a, b) (((a) >= (b)) ? (a) : (b))

struct map {
	size_t keys_capacity;
	size_t keys_size;
	char *keys;
	size_t buckets_capacity;
	size_t buckets_count;
	size_t bucket_size;
	size_t *key_indices; // Same capacity as `buckets`.
	char buckets[];
};

// Returned by `probe` to indicate the status of a key in a map.
enum probe_result {
	PROBE_RESULT_KEY_FOUND,
	PROBE_RESULT_MAP_NOT_FULL,
	PROBE_RESULT_MAP_FULL,
};

static const size_t keys_growth_factor = 2;

static struct map *get_header(void **map) {
	return (struct map*)*map - 1;
}

// FNV-1a hash, copied from Wikipedia:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
static size_t hash(char *key) {
	size_t hash = 14695981039346656037ull;
	size_t prime = 1099511628211ull;
	while (*key) {
		hash = (hash^*key)*prime;
		++key;
	}
	return hash;
}

// Finds the corresponding bucket for `key` in `map` and places its index in `bucket`. Returns a
// status indicating whether the key was found, the key was not found and the map has an empty
// bucket, or the key was not found and the map is full.
static enum probe_result probe(void **map, char *key, size_t *bucket_index) {
	struct map *header = get_header(map);
	size_t start_index = hash(key)%header->buckets_capacity;
	size_t index = start_index;
	size_t next_empty_index = 0;

	// Look for a matching key or an empty bucket.
	do {
		if (header->key_indices[index] == 0) {
			if (next_empty_index == 0) {
				next_empty_index = index;
			}
		// If the bucket is full and the key matches, we found the bucket.
		} else if (strcmp(key, header->keys + header->key_indices[index] - 1) == 0) {
			*bucket_index = index;
			return PROBE_RESULT_KEY_FOUND;
		}
		// Linear probing.
		index = (index + 1)%header->buckets_capacity;
	} while (index != start_index);

	if (header->buckets_count < header->buckets_capacity) {
		*bucket_index = next_empty_index;
		return PROBE_RESULT_MAP_NOT_FULL;
	}
	*bucket_index = index;
	return PROBE_RESULT_MAP_FULL;
}

// Returns the index of the key in the string pool if successful, `SIZE_MAX` otherwise.
static size_t map_add_key(void **map, char *key) {
	struct map *header = get_header(map);
	size_t length = strlen(key);
	if (header->keys_capacity < header->keys_size + length) {
		// Reallocate the keys if the given key is too big to fit.
		char *new_keys = realloc(header->keys, max(header->keys_size + length, keys_growth_factor*header->keys_capacity));
		if (!new_keys) {
			return 0;
		}
	}
	strcpy(header->keys + header->keys_size, key);
	header->keys_size += length + 1;
	return header->keys_size - length;
}

void *map_create(size_t buckets_capacity, size_t bucket_size, size_t keys_capacity) {
	struct map *header = malloc(sizeof *header + buckets_capacity*bucket_size);
	if (!header) {
		return NULL;
	}
	*header = (struct map){
		.keys_capacity = keys_capacity,
		.keys = malloc(keys_capacity),
		// .key_indices = malloc(buckets_capacity*sizeof *header->key_indices),
		.buckets_capacity = buckets_capacity,
		.bucket_size = bucket_size,
	};
	if (!header->keys) {
		free(header);
		return NULL;
	}
	header->key_indices = malloc(buckets_capacity*sizeof *header->key_indices);
	if (!header->key_indices) {
		free(header->keys);
		free(header->key_indices);
		return NULL;
	}
	return &header->buckets;
}

void map_destroy_impl(void **map) {
	struct map *header = get_header(map);
	free(header->keys);
	free(header->key_indices);
	free(header);
	*map = NULL;
}

size_t map_get_buckets_capacity_impl(void **map) {
	struct map *header = get_header(map);
	return header->buckets_capacity;
}

bool map_set_buckets_capacity_impl(void **map, size_t capacity) {
	struct map *header = get_header(map);
	if (capacity < header->buckets_count) {
		return false;
	}
	header->keys = realloc(header->keys, capacity*sizeof *header->keys);
	if (!header->keys) {
		return false;
	}
	header = realloc(header, sizeof *header + capacity*header->bucket_size);
	if (!header) {
		return false;
	}
	*map = &header->buckets;
	return true;
}

size_t map_get_buckets_count_impl(void **map) {
	struct map *header = get_header(map);
	return header->buckets_count;
}

size_t map_get_bucket_size_impl(void **map) {
	struct map *header = get_header(map);
	return header->bucket_size;
}

size_t map_get_keys_capacity_impl(void **map) {
	struct map *header = get_header(map);
	return header->keys_capacity;
}

size_t map_get_keys_size_impl(void **map) {
	struct map *header = get_header(map);
	return header->keys_size;
}

bool map_is_empty_impl(void **map) {
	struct map *header = get_header(map);
	return header->buckets_count == 0;
}

bool map_is_not_empty_impl(void **map) {
	struct map *header = get_header(map);
	return header->buckets_count != 0;
}

void *map_get_impl(void **map, char *key) {
	struct map *header = get_header(map);
	size_t bucket_index = 0;
	enum probe_result result = probe(map, key, &bucket_index);
	if (result == PROBE_RESULT_KEY_FOUND) {
		return header->buckets + bucket_index*header->bucket_size;
	}
	return NULL;
}

bool map_set_impl(void **map, char *key, void *value) {
	struct map *header = get_header(map);
	size_t bucket_index = 0;
	enum probe_result result = probe(map, key, &bucket_index);
	if (result == PROBE_RESULT_KEY_FOUND) {
		memcpy(header->buckets + bucket_index*header->bucket_size, value, header->bucket_size);
		++header->buckets_count;
		return true;
	}
	return false;
}

bool map_add_impl(void **map, char *key, void *value) {
	struct map *header = get_header(map);
	size_t bucket_index = 0;
	enum probe_result result = probe(map, key, &bucket_index);
	if (result == PROBE_RESULT_MAP_FULL) {
		// Set capacity and rehash.
	}
	if (result == PROBE_RESULT_MAP_NOT_FULL) {
		// Copy the key.
		size_t key_index = map_add_key(map, key);
		if (key_index == 0) {
			return false;
		}
		header->key_indices[bucket_index] = key_index;
	}
	memcpy(header->buckets + bucket_index*header->bucket_size, value, header->bucket_size);
	++header->buckets_count;
	return true;
}

#undef max
