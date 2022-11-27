#include <assert.h>
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
      uint32_t real_end = bsearch_val_u32(root, index);
      if (real_end < end) {
        bsearch_setval_u32(root, index, end);
        real_end = end;
      }
      // Remove anything that's now covered by the new interval
      while (index + 1 < root->size &&
             real_end >= bsearch_key_u32(root, index + 1)) {
        uint32_t next_end = bsearch_val_u32(root, index + 1);
        bsearch_remove(root, index + 1);
        if (next_end > real_end) {
          bsearch_setval_u32(root, index, next_end);
          break;  // no more to remove (otherwise, intervals are already
                  // malformed)
        }
      }
    }
    // No merge
    else {
      bsearch_upsert_u32(root, start, end);
    }
  }
}

void uint32_interval_remove(struct bsearch_root *restrict root, uint32_t start,
                            uint32_t end) {
  assert(end >= start);

  ptrdiff_t s_index = bsearch_find_u32(root, start);
  if (s_index >= 0) {
    // Exact "start" match: trim start and put it back into the search tree
    bsearch_replace(root, s_index, CAST_u32_pvoid(end),
                    root->pairs[s_index].value);
  } else {
    size_t index = (size_t)(-s_index - 1);

    while (index < root->size && bsearch_key_u32(root, index) < end) {
      if (bsearch_val_u32(root, index) < end) {
        // Overlap with entire later interval
        bsearch_remove(root, index);
      } else {
        // Overlap with "start" of later interval -> set its start to our end
        bsearch_replace(root, index, CAST_u32_pvoid(end),
                        root->pairs[index].value);
        break;
      }
    }

    if (index > 0) {
      uint32_t prev_key = bsearch_key_u32(root, index - 1);
      uint32_t prev_value = bsearch_val_u32(root, index - 1);
      assert(prev_key < start);
      if (start < prev_value) {
        // There is overlap, cut the interval at the end
        bsearch_setval_u32(root, index - 1, start);
        if (end < prev_value) {
          // Overlap is not complete, restore the remainder as a new interval
          bsearch_upsert_u32(root, end, prev_value);
        }
      }
    }
  }
}
