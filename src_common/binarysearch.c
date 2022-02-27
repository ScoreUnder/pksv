#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

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

ssize_t bsearch_find(struct bsearch_root const *restrict root,
                     void const *key) {
  struct bsearch_kv const *kvs = root->pairs;
  size_t left = 0;
  size_t right = root->size;
  while (left < right) {
    size_t mid = (left + right) / 2;
    int cmp = root->compare(kvs[mid].key, key);
    if (cmp < 0) {
      left = mid + 1;
    } else if (cmp > 0) {
      right = mid;
    } else {
      return mid;
    }
  }
  return -left - 1;
}

size_t bsearch_unsafe_insert(struct bsearch_root *restrict root, ssize_t index,
                             void const *key, void *value) {
  assert(index < 0);
  size_t pos_index = (size_t)(-index - 1);
  size_t len = root->size;

#ifndef NDEBUG
  if (pos_index < len) {
    assert(root->compare(root->pairs[pos_index].key, key) > 0);
  }
  if (pos_index > 0) {
    assert(root->compare(root->pairs[pos_index - 1].key, key) < 0);
  }
#endif

  bsearch_ensure_capacity(root, len + 1);
  if (pos_index < len) {
    memmove(&root->pairs[pos_index + 1], &root->pairs[pos_index],
            (len - pos_index) * sizeof(struct bsearch_kv));
  }
  void *key_copied = root->copy(key);
  root->pairs[pos_index] =
      (struct bsearch_kv){.key = key_copied, .value = value};
  root->size++;
  return pos_index;
}

size_t bsearch_upsert(struct bsearch_root *restrict kvs, void const *key,
                      void *value) {
  ssize_t index = bsearch_find(kvs, key);
  if (index >= 0) {
    // Overwrite value of existing element
    if (kvs->free_val) kvs->free_val(kvs->pairs[index].value);
    kvs->pairs[index].value = value;
    return index;
  } else {
    return bsearch_unsafe_insert(kvs, index, key, value);
  }
}

void bsearch_remove(struct bsearch_root *restrict root, size_t index) {
  if (root->free_key) root->free_key((void *)root->pairs[index].key);
  if (root->free_val) root->free_val(root->pairs[index].value);
  memmove(&root->pairs[index], &root->pairs[index + 1],
          (root->size - index - 1) * sizeof(struct bsearch_kv));
  root->size--;
}

// Convenience functions:
int bsearch_key_strcmp(const void *a, const void *b) {
  return strcmp((const char *)a, (const char *)b);
}

void *bsearch_key_strdup(const void *a) { return strdup((const char *)a); }

void *bsearch_key_nocopy(const void *a) { return (void *)a; }

int bsearch_key_int32cmp(const void *a, const void *b) {
  _Static_assert(sizeof(int32_t) <= sizeof(void *),
                 "pointer size is too small");
  if ((uint32_t)a < (uint32_t)b) return -1;
  if ((uint32_t)a > (uint32_t)b) return 1;
  return 0;
}
