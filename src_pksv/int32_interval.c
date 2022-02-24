#include <stdint.h>
#include <sys/types.h>

#include "int32_interval.h"
#include "binarysearch.h"

static void *bsearch_key_nocopy(const void *a) { return (void *)a; }

static int bsearch_key_int32cmp(const void *a, const void *b) {
  _Static_assert(sizeof(int32_t) <= sizeof(void *),
                 "pointer size is too small");
  if ((uint32_t)a < (uint32_t)b) return -1;
  if ((uint32_t)a > (uint32_t)b) return 1;
  return 0;
}

void int32_interval_init_bsearch_root(struct bsearch_root *root) {
  bsearch_init_root(root, bsearch_key_int32cmp, bsearch_key_nocopy, NULL, NULL);
}

void int32_interval_add(struct bsearch_root *restrict root, uint32_t start,
                        uint32_t end) {
  ssize_t index = bsearch_find(root, (void *)start);

  if (index >= 0) {
    // overwriting? this is user error
    // but let's handle it anyway
    if ((uint32_t)root->pairs[index].value < end)
      root->pairs[index].value = (void *)end;
  } else {
    index = -index - 1;

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
