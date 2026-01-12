#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

#define list_destroy(list) (list_destroy_impl((void**)list))

#define list_get_buckets_capacity(list) (list_get_buckets_capacity_impl((void**)(list)))

// Returns true if successful, false if memory error. If you pass a capacity smaller than the list's
// count, the count of the list shrinks to the new capacity.
#define list_set_buckets_capacity(list, capacity) (list_set_buckets_capacity_impl((void**)(list), (capacity)))

#define list_get_buckets_count(list) (list_get_buckets_count_impl((void**)(list)))

// Does not zero new buckets added to the list if given a count greater than the list's current
// count. Returns true if the new count was <= the list's capacity, returns false and does nothing
// otherwise.
#define list_set_buckets_count(list, count) (list_set_buckets_count_impl((void**)(list), (count)))

// #define list_set_buckets_count_zero(list, count) (list_set_buckets_count_zero_impl((void**)(list), (count)))

#define list_is_empty(list) (list_is_empty_impl((void**)(list)))

#define list_is_not_empty(list) (list_is_not_empty_impl((void**)(list)))

#define list_push_back(list, value) (list_push_back_impl((void**)(list), (value)))

#define list_pop_back(list, result) (list_pop_back_impl((void**)(list), (result)))

void *list_create(size_t buckets_capacity, size_t bucket_size);

void list_destroy_impl(void **list);

size_t list_get_buckets_capacity_impl(void **list);

bool list_set_buckets_capacity_impl(void **list, size_t capacity);

size_t list_get_buckets_count_impl(void **list);

bool list_set_buckets_count_impl(void **list, size_t count);

// bool list_set_buckets_count_zero_impl(void **list, size_t count);

bool list_is_empty_impl(void **list);

bool list_is_not_empty_impl(void **list);

bool list_push_back_impl(void **list, void *value);

bool list_pop_back_impl(void **list, void *result);

#endif // LIST_H
