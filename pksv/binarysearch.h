#ifndef PKSV_BINARY_SEARCH_H
#define PKSV_BINARY_SEARCH_H 1

#include <stddef.h>

struct bsearch_kv {
  char const* key;
  void const* value;
};

struct bsearch_root {
  struct bsearch_kv* pairs;
  size_t size;
  size_t capacity;
};

/** @brief Initialise a bsearch_root structure which is already allocated. */
void bsearch_init_root(struct bsearch_root *root);

/** @brief Allocate and initialise a bsearch_root structure. */
struct bsearch_root* bsearch_create_root(void);

/** @brief Free a bsearch_root structure. */
void bsearch_destroy_root(struct bsearch_root* root);

/** @brief Free the contents of a bsearch_root structure, but not the structure itself. */
void bsearch_deinit_root(struct bsearch_root* root);

/** @brief Ensure that the bsearch_root has enough space to hold the given number of elements. */
void bsearch_ensure_capacity(struct bsearch_root* root, size_t capacity);

/** @brief Insert a key/value pair into a bsearch_root. */
size_t bsearch_upsert(struct bsearch_root *kvs, const char *key, void const *value);

/** @brief Find a key in a bsearch_root.
 *  @return the index of the matching entry, or kvs->size if not found.
 */
size_t bsearch_find_if_exists(struct bsearch_root *kvs, char const *key);

#endif
