#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "binarysearch.h"

void bsearch_init_root(struct bsearch_root *root, bsearch_compare_func *compare,
                       bsearch_copy_func *copy, bsearch_free_key_func *free_key,
                       bsearch_free_val_func *free_val) {
  size_t capacity = 16;
  root->pairs = malloc(sizeof(struct bsearch_kv) * capacity);
  root->compare = compare;
  root->copy = copy;
  root->free_key = free_key;
  root->free_val = free_val;
  root->size = 0;
  root->capacity = capacity;
}

struct bsearch_root *bsearch_create_root(bsearch_compare_func *compare,
                                         bsearch_copy_func *copy,
                                         bsearch_free_key_func *free_key,
                                         bsearch_free_val_func *free_val) {
  struct bsearch_root *root = malloc(sizeof(struct bsearch_root));
  bsearch_init_root(root, compare, copy, free_key, free_val);
  return root;
}

void bsearch_destroy_root(struct bsearch_root *root) {
  bsearch_deinit_root(root);
  free(root);
}

void bsearch_deinit_root(struct bsearch_root *restrict root) {
  for (size_t i = 0; i < root->size; i++) {
    if (root->free_key) root->free_key((void *)root->pairs[i].key);
    if (root->free_val) root->free_val(root->pairs[i].value);
  }
  free(root->pairs);
}

void bsearch_ensure_capacity(struct bsearch_root *root, size_t capacity) {
  if (root->capacity < capacity) {
    capacity = capacity * 2;
    while ((capacity & (capacity - 1)) != 0) {
      capacity = capacity & (capacity - 1);
    }
    root->capacity = capacity;
    root->pairs = realloc(root->pairs, capacity * sizeof(struct bsearch_kv));
  }
}

static size_t bsearch_find(struct bsearch_kv const *kvs, size_t len,
                           void const *key, bsearch_compare_func *compare) {
  size_t left = 0;
  size_t right = len;
  while (left < right) {
    size_t mid = (left + right) / 2;
    int cmp = compare(kvs[mid].key, key);
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

size_t bsearch_upsert(struct bsearch_root *kvs, void const *key, void *value) {
  size_t len = kvs->size;
  size_t index = bsearch_find(kvs->pairs, len, key, kvs->compare);
  if (index < len && kvs->compare(kvs->pairs[index].key, key) == 0) {
    // Overwrite value of existing element
    if (kvs->free_val) kvs->free_val(kvs->pairs[index].value);
    kvs->pairs[index].value = value;
    return index;
  } else {
    bsearch_ensure_capacity(kvs, len + 1);
    if (index < len) {
      memmove(&kvs->pairs[index + 1], &kvs->pairs[index],
              (len - index) * sizeof(struct bsearch_kv));
    }
    void *key_copied = kvs->copy(key);
    kvs->pairs[index] = (struct bsearch_kv){.key = key_copied, .value = value};
    kvs->size++;
    return index;
  }
}

size_t bsearch_find_if_exists(struct bsearch_root *kvs, void const *key) {
  size_t len = kvs->size;
  size_t index = bsearch_find(kvs->pairs, len, key, kvs->compare);
  if (index < len && kvs->compare(kvs->pairs[index].key, key) == 0) {
    return index;
  } else {
    return len;
  }
}
