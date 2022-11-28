#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "uint32_interval.h"
#include "binarysearch_u32.h"
#include "binarysearch.h"

void uint32_interval_init_bsearch_root(struct bsearch_root *root) {
  bsearch_init_root(root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    NULL);
}

static void uint32_interval_merge_ends(struct bsearch_root *restrict root,
                                       size_t index, uint32_t end) {
  uint32_t merged_end = bsearch_val_u32(root, index);
  if (merged_end < end) {
    merged_end = end;
  }
  // Remove and merge with anything that's now covered by the new interval
  while (index + 1 < root->size &&
         merged_end >= bsearch_key_u32(root, index + 1)) {
    uint32_t next_end = bsearch_val_u32(root, index + 1);
    if (next_end > merged_end) merged_end = next_end;
    bsearch_remove(root, index + 1);
  }
  bsearch_setval_u32(root, index, merged_end);
}

void uint32_interval_add(struct bsearch_root *restrict root, uint32_t start,
                         uint32_t end) {
  if (end == start) return;
  assert(end > start);

  ptrdiff_t s_index = bsearch_find_u32(root, start);
  size_t index;

  if (s_index >= 0) {
    index = (size_t)s_index;
  } else {
    index = (size_t)(-s_index - 1);
  }

  // Can merge end with the interval before?
  if (index > 0 && bsearch_val_u32(root, index - 1) >= start) {
    uint32_interval_merge_ends(root, index - 1, end);
  }
  // Can merge beginning with the interval after?
  else if (index < root->size && bsearch_key_u32(root, index) <= end) {
    root->pairs[index].key = CAST_u32_pvoid(start);
    uint32_interval_merge_ends(root, index, end);
  }
  // No merge
  else {
    bsearch_upsert_u32(root, start, end);
  }
}

void uint32_interval_remove(struct bsearch_root *restrict root, uint32_t start,
                            uint32_t end) {
  if (end == start) return;
  assert(end >= start);

  ptrdiff_t s_index = bsearch_find_u32(root, start);
  size_t index;
  if (s_index >= 0) {
    index = (size_t)s_index;
  } else {
    index = (size_t)(-s_index - 1);
  }

  while (index < root->size && bsearch_key_u32(root, index) < end) {
    uint32_t old_end = bsearch_val_u32(root, index);
    if (old_end <= end) {
      // Overlap with entire later interval
      bsearch_remove(root, index);
    } else {
      // Overlap with "start" of later interval -> set its start to our end
      bsearch_replace(root, index, CAST_u32_pvoid(end),
                      CAST_u32_pvoid(old_end));
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

bool uint32_interval_intersects(struct bsearch_root *restrict root,
                                uint32_t start, uint32_t end) {
  if (end == start) return false;  // Empty interval, no intersection
  assert(end > start);

  ptrdiff_t s_index = bsearch_find_u32(root, start);
  size_t index;
  if (s_index >= 0) {
    index = (size_t)s_index;
  } else {
    index = (size_t)(-s_index - 1);
  }

  if (index > 0 && bsearch_val_u32(root, index - 1) > start) return true;
  if (index < root->size && bsearch_key_u32(root, index) < end) return true;
  return false;
}

bool uint32_interval_contains(struct bsearch_root *restrict root,
                              uint32_t start, uint32_t end) {
  if (end == start) return true;  // Empty interval, always contained
  assert(end > start);

  ptrdiff_t s_index = bsearch_find_u32(root, start);
  size_t index;
  if (s_index >= 0) {
    index = (size_t)s_index;
    if (bsearch_val_u32(root, index) >= end) return true;
    return false;
  } else {
    index = (size_t)(-s_index - 1);
    if (index == 0) return false;
    // The previous interval is the only one that could possibly overlap fully,
    // because they are ordered by start
    index--;
    if (bsearch_val_u32(root, index) < end) return false;
    if (bsearch_key_u32(root, index) > start) return false;
    return true;
  }
}
