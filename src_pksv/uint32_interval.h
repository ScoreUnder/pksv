#ifndef PKSV_INT32_INTERVAL_H
#define PKSV_INT32_INTERVAL_H 1

#include <stdint.h>
#include <stdbool.h>

#include "binarysearch.h"

void uint32_interval_init_bsearch_root(struct bsearch_root *root);
void uint32_interval_add(struct bsearch_root *restrict root, uint32_t start,
                         uint32_t end);
void uint32_interval_remove(struct bsearch_root *restrict root, uint32_t start,
                            uint32_t end);
bool uint32_interval_intersects(struct bsearch_root *restrict root,
                                uint32_t start, uint32_t end);

#endif
