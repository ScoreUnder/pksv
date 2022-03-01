#include <stdint.h>
#include <sys/types.h>

#include "int32_interval.h"
#include "binarysearch.h"

void int32_interval_init_bsearch_root(struct bsearch_root *root) {
  bsearch_init_root(root, bsearch_key_int32cmp, bsearch_key_nocopy, NULL, NULL);
}

void int32_interval_add(struct bsearch_root *restrict root, uint32_t start,
                        uint32_t end) {
  ptrdiff_t s_index = bsearch_find(root, (void *)start);

  if (s_index >= 0) {
    // overwriting? this is user error
    // but let's handle it anyway
    if ((uint32_t)root->pairs[s_index].value < end)
      root->pairs[s_index].value = (void *)end;
  } else {
    size_t index = (size_t) (-s_index - 1);

    // Can merge left?
    if (index > 0 && (uint32_t)root->pairs[index - 1].value >= start) {
      uint32_t new_end = end;

      // Can merge right?
      if ((size_t)index < root->size &&
          (uint32_t)root->pairs[index].key <= new_end) {
        new_end = (uint32_t)root->pairs[index].value;
        bsearch_remove(root, index);
      }
      root->pairs[index - 1].value = (void *)new_end;
    }
    // Can merge right?
    else if ((size_t)index < root->size &&
             (uint32_t)root->pairs[index].key <= end) {
      root->pairs[index].key = (void *)start;
    }
    // No merge
    else {
      bsearch_upsert(root, (void *)start, (void *)end);
    }
  }
}
