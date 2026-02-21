#include <stdint.h>

struct trie_node {
	uint32_t parent_index;
	uint32_t child_index;
	uint32_t sibling_index;
	char ch;
};

struct trie {
	uint32_t nodes_capacity;
	uint32_t nodes_count;
	struct trie_node *nodes;
	uint32_t buckets_capacity;
	uint32_t buckets_count;
	char buckets[];
};
