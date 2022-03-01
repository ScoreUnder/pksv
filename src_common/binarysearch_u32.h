#ifndef PKSV_BINARY_SEARCH_U32_H
#define PKSV_BINARY_SEARCH_U32_H 1

#include <stdint.h>

#include "binarysearch.h"

int bsearch_key_uint32cmp(const void *a, const void *b);

_Static_assert(sizeof(uint32_t) <= sizeof(void *),
               "Pointer type is too small.");
#define CAST_pvoid_u32(x) ((uint32_t)(uintptr_t)(x))
#define CAST_u32_pvoid(x) ((void *)(uintptr_t)(x))

uint32_t bsearch_key_u32(const struct bsearch_root *root, size_t index);
uint32_t bsearch_val_u32(const struct bsearch_root *root, size_t index);
void bsearch_setval_u32(struct bsearch_root *root, size_t index,
                        uint32_t value);
#define bsearch_find_u32(root, key) bsearch_find((root), CAST_u32_pvoid(key))
#define bsearch_upsert_u32(root, key, value) \
  bsearch_upsert((root), CAST_u32_pvoid(key), CAST_u32_pvoid(value))

#endif
