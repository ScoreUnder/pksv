#include <assert.h>
#include "binarysearch_u32.h"
#include "binarysearch.h"

int bsearch_key_uint32cmp(const void *a, const void *b) {
  _Static_assert(sizeof(int32_t) <= sizeof(void *),
                 "pointer size is too small");
  if (CAST_pvoid_u32(a) < CAST_pvoid_u32(b)) return -1;
  if (CAST_pvoid_u32(a) > CAST_pvoid_u32(b)) return 1;
  return 0;
}

uint32_t bsearch_key_u32(const struct bsearch_root *root, size_t index) {
  return CAST_pvoid_u32(root->pairs[index].key);
}

uint32_t bsearch_val_u32(const struct bsearch_root *root, size_t index) {
  return CAST_pvoid_u32(root->pairs[index].value);
}

void bsearch_setval_u32(struct bsearch_root *root, size_t index,
                        uint32_t value) {
  root->pairs[index].value = CAST_u32_pvoid(value);
}
