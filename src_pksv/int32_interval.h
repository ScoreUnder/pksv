#ifndef PKSV_INT32_INTERVAL_H
#define PKSV_INT32_INTERVAL_H 1

#include <stdint.h>

#include "binarysearch.h"

void int32_interval_init_bsearch_root(struct bsearch_root *root);
void int32_interval_add(struct bsearch_root *restrict root, uint32_t start,
                        uint32_t end);

#endif
