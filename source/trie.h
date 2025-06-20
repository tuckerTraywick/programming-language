#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Trie_Character {
	bool has_sibling : 1;
	char character : 7;
} Trie_Character;

typedef struct Trie {
	uint32_t *children; // Points to a list.
	uint32_t *siblings; // Points to a list.
	Trie_Character *characters; // Points to a list.
	size_t start_index;
} Trie;

// You must check if this function's return value is valid with `Trie_is_valid()`.
Trie Trie_create(size_t child_capacity, size_t sibling_capacity, size_t character_capacity);

void Trie_destroy(Trie *trie);

bool Trie_is_valid(Trie *trie);

// Returns 0 if key is not in trie.
uint32_t Trie_get(Trie *trie, char *key);

// You must check if this function's return value is valid with `Trie_is_valid()`.
Trie Trie_set(Trie *trie, char *key, uint32_t value);

// Returns a portion of a trie starting at the end of a partial key. You must check this function's
// return value with `Trie_is_valid()`. The return value does NOT need to be destroyed with
// `Trie_destroy()`.
Trie Trie_get_subtrie(Trie *trie, char *key);

#endif // TRIE_H
