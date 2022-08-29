#include <stddef.h>
#include <stdint.h>

#include "uint32_interval.h"
#include "binarysearch_u32.h"
#include "binarysearch.h"

void uint32_interval_init_bsearch_root(struct bsearch_root *root) {
  bsearch_init_root(root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    NULL);
}

void uint32_interval_add(struct bsearch_root *restrict root, uint32_t start,
                         uint32_t end) {
  ptrdiff_t s_index = bsearch_find_u32(root, start);

  if (s_index >= 0) {
    // overwriting? this is user error
    // but let's handle it anyway
    if (bsearch_val_u32(root, s_index) < end)
      bsearch_setval_u32(root, s_index, end);
  } else {
    size_t index = (size_t)(-s_index - 1);

    // Can merge left?
    if (index > 0 && bsearch_val_u32(root, index - 1) >= start) {
      uint32_t new_end = end;

      // Can merge right?
      if (index < root->size && bsearch_key_u32(root, index) <= new_end) {
        new_end = bsearch_val_u32(root, index);
        bsearch_remove(root, index);
      }
      bsearch_setval_u32(root, index - 1, new_end);
    }
    // Can merge right?
    else if (index < root->size && bsearch_key_u32(root, index) <= end) {
      root->pairs[index].key = CAST_u32_pvoid(start);
    }
    // No merge
    else {
      bsearch_upsert_u32(root, start, end);
    }
  }
}
