#include "trie.h"
#include "list.h"

Trie Trie_create(size_t child_capacity, size_t sibling_capacity, size_t character_capacity) {
	Trie trie = {
		.children = list_create(child_capacity, sizeof *trie.children),
		.siblings = list_create(sibling_capacity, sizeof *trie.siblings),
		.characters = list_create(character_capacity, sizeof *trie.characters),
	};
	if (Trie_is_valid(&trie)) {
		return trie;
	}
	return (Trie){0};
}

void Trie_destroy(Trie *trie) {
	list_destroy(trie->children);
	list_destroy(trie->siblings);
	list_destroy(trie->characters);
	*trie = (Trie){0};
}

bool Trie_is_valid(Trie *trie) {
	return trie->children && trie->siblings && trie->characters;
}
