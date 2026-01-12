#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct list_header {
	size_t buckets_capacity;
	size_t buckets_count;
	size_t bucket_size;
	char buckets[];
};

static const int buckets_growth_factor = 2;

static struct list_header *get_header(void **list) {
	return (struct list_header*)*list - 1;
}

void *list_create(size_t buckets_capacity, size_t bucket_size) {
	struct list_header *header = malloc(sizeof *header + buckets_capacity*bucket_size);
	if (!header) {
		return NULL;
	}
	*header = (struct list_header){
		.buckets_capacity = buckets_capacity,
		.bucket_size = bucket_size,
	};
	return &header->buckets;
}

void list_destroy_impl(void **list) {
	struct list_header *header = get_header(list);
	free(header);
	*list = NULL;
}

size_t list_get_buckets_capacity_impl(void **list) {
	struct list_header *header = get_header(list);
	return header->buckets_capacity;
}

bool list_set_buckets_capacity_impl(void **list, size_t capacity) {
	struct list_header *header = get_header(list);
	header = realloc(header, sizeof *header + capacity*header->bucket_size);
	if (!header) {
		return false;
	}
	*list = &header->buckets;
	header->buckets_capacity = capacity;
	if (capacity < header->buckets_count) {
		header->buckets_count = capacity;
	}
	return true;
}

size_t list_get_buckets_count_impl(void **list) {
	struct list_header *header = get_header(list);
	return header->buckets_count;
}

bool list_set_buckets_count_impl(void **list, size_t count) {
	struct list_header *header = get_header(list);
	if (count > header->buckets_capacity) {
		return false;
	}
	header->buckets_count = count;
	return true;
}

// bool list_set_buckets_count_zero_impl(void **list, size_t count) {
// 	struct list_header *header = get_header(list);
// 	size_t amount_to_zero = 0;
// 	if (count > header->buckets_capacity) {
// 		amount_to_zero = count - header->buckets_capacity;
// 	} else {
// 		amount_to_zero = header->buckets_capacity - count;
// 	}
// 	memset(header->buckets + header->buckets_count, 0, amount_to_zero);
// 	header->buckets_count = count;
// 	return true;
// }

bool list_is_empty_impl(void **list) {
	struct list_header *header = get_header(list);
	return header->buckets_count == 0;
}

bool list_is_not_empty_impl(void **list) {
	struct list_header *header = get_header(list);
	return header->buckets_count != 0;
}

bool list_push_back_impl(void **list, void *value) {
	struct list_header *header = get_header(list);
	if (header->buckets_count == header->buckets_capacity && !list_set_buckets_capacity_impl(list, buckets_growth_factor*header->buckets_capacity)) {
		return false;
	}
	memcpy((char*)*list + header->buckets_count*header->bucket_size, value, header->bucket_size);
	++header->buckets_count;
	return true;
}

bool list_pop_back_impl(void **list, void *result) {
	struct list_header *header = get_header(list);
	if (header->buckets_count == 0) {
		return false;
	}
	memcpy(result, (char*)*list + (header->buckets_count - 1)*header->bucket_size, header->bucket_size);
	--header->buckets_count;
	// TODO: Maybe shrink here if needed? Maybe make shrink a separate function you have to call?
	return true;
}
