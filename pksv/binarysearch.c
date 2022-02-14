#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "binarysearch.h"

void bsearch_init_root(struct bsearch_root *root) {
  size_t capacity = 16;
  root->pairs = malloc(sizeof(struct bsearch_kv) * capacity);
  root->size = 0;
  root->capacity = capacity;
}

struct bsearch_root* bsearch_create_root(void) {
  struct bsearch_root* root = malloc(sizeof(struct bsearch_root));
  bsearch_init_root(root);
  return root;
}

void bsearch_destroy_root(struct bsearch_root* root) {
  bsearch_deinit_root(root);
  free(root);
}

void bsearch_deinit_root(struct bsearch_root* root) {
  for (size_t i = 0; i < root->size; i++) {
    free((void*)root->pairs[i].key);
  }
  free(root->pairs);
}

static void bsearch_ensure_capacity(struct bsearch_root* root, size_t capacity) {
  if (root->capacity < capacity) {
    capacity = capacity * 2;
    while ((capacity & (capacity - 1)) != 0) {
      capacity = capacity & (capacity - 1);
    }
    root->capacity = capacity;
    root->pairs = realloc(root->pairs, capacity * sizeof(struct bsearch_kv));
  }
}

static size_t bsearch_find(struct bsearch_kv const *kvs, size_t len, char const *key) {
  size_t left = 0;
  size_t right = len;
  while (left < right) {
    size_t mid = (left + right) / 2;
    int cmp = strcmp(kvs[mid].key, key);
    if (cmp < 0) {
      left = mid + 1;
    } else if (cmp > 0) {
      right = mid;
    } else {
      return mid;
    }
  }
  return left;
}

size_t bsearch_upsert(struct bsearch_root *kvs, const char *key, void const *value) {
  size_t len = kvs->size;
  size_t index = bsearch_find(kvs->pairs, len, key);
  if (index < len && strcmp(kvs->pairs[index].key, key) == 0) {
    // Overwrite value of existing element
    kvs->pairs[index].value = value;
    return index;
  } else {
    bsearch_ensure_capacity(kvs, len + 1);
    if (index < len) {
      memmove(&kvs->pairs[index + 1], &kvs->pairs[index], (len - index) * sizeof(struct bsearch_kv));
    }
    key = strdup(key);
    kvs->pairs[index] = (struct bsearch_kv) { .key = key, .value = value };
    kvs->size++;
    return index;
  }
}

size_t bsearch_find_if_exists(struct bsearch_root *kvs, char const *key) {
  size_t len = kvs->size;
  size_t index = bsearch_find(kvs->pairs, len, key);
  if (index < len && strcmp(kvs->pairs[index].key, key) == 0) {
    return index;
  } else {
    return len;
  }
}
