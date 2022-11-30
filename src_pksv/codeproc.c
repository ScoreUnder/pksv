/*
    Sophie Daffern (Score_Under)'s PKSV - Pokemon ROM script viewer/editor
    Copyright (C) 2007  Sophie Daffern

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "codeproc.h"
#include "binarysearch_u32.h"
#include "uint32_interval.h"
#include "binarysearch.h"
#include "textutil.h"
#include "romutil.h"
#include "sulib.h"
#include "pksv.h"

uint32_t FindFreeSpace(FILE *rom_search, uint32_t len, uint32_t align,
                       uint32_t *offset, uint32_t *min_address, uint8_t search,
                       struct bsearch_root *free_intervals) {
  assert(rom_search != NULL);
  // If we let *offset get > *min_address, we might skip potential free space
  // That would need extra logic to handle, so for now...
  assert(*offset <= *min_address);

  if (len == 0) return ROM_BASE_ADDRESS;  // Nothing requested, anything goes?

  uint32_t align_mask = align - 1;
  // Must be a power of two (easier to calculate)
  assert(align != 0);
  assert((align_mask & align) == 0);

  // See if we already have a free interval for this; find the smallest
  // compatible and remove it.
  size_t smallest_index = SIZE_MAX;
  uint32_t smallest_len = UINT32_MAX;
  for (size_t i = 0; i < free_intervals->size; i++) {
    uint32_t interval_start = bsearch_key_u32(free_intervals, i);
    if (interval_start < *min_address) continue;
    uint32_t interval_end = bsearch_val_u32(free_intervals, i);
    uint32_t interval_aligned_start =
        (interval_start + align_mask) & ~align_mask;
    if (interval_aligned_start >= interval_end) continue;
    uint32_t interval_len = interval_end - interval_start;
    uint32_t interval_available_len = interval_end - interval_aligned_start;
    if (interval_len < smallest_len && interval_available_len >= len) {
      smallest_index = i;
      smallest_len = interval_len;
    }
  }

  if (smallest_index != SIZE_MAX) {
    uint32_t result = bsearch_key_u32(free_intervals, smallest_index);
    // Correct for alignment
    result = (result + align_mask) & ~align_mask;
    uint32_interval_remove(free_intervals, result, result + len);
    return ROM_BASE_ADDRESS | result;
  }

  uint32_t filepos = *offset;

  // Text has to end in 0xFF, so request one more byte
  // to make room for a potential terminator byte at the start
  // of space which otherwise seems free
  assert(len < UINT32_MAX);
  len++;
  // ...and cheekily wind back search start pos one byte to compensate
  // (so that the minimum returned address is exactly *offset)
  if (filepos > 0) filepos--;

  fseek(rom_search, filepos, SEEK_SET);

  int chr;
  uint32_t consecutive = 0;
  while ((chr = getc(rom_search)) != EOF) {
    filepos++;

    bool match = chr == search;
    if (!match) {
      // check free intervals
      // TODO: naive algorithm, should skip more than one byte at a time
      ptrdiff_t index = bsearch_find_u32(free_intervals, filepos);
      if (index < -1) index = -index - 2;  // find the interval before
      if (index >= 0 && (size_t)index < free_intervals->size) {
        uint32_t interval_start = bsearch_key_u32(free_intervals, index);
        uint32_t interval_end = bsearch_val_u32(free_intervals, index);
        if (filepos >= interval_start && filepos < interval_end) {
          match = true;
        }
      }
    }

    if (match) {
      if (++consecutive >= len) {
        // Check alignment
        uint32_t result = filepos + 1 - len;
        if ((result & align_mask) == 0) break;
        // TODO: record space before alignment as free
      }
    } else {
      consecutive = 0;
    }
  }
  if (consecutive < len) return UINT32_MAX;

  // Undo length hack
  len--;

  // Record the end position for next time
  *offset = filepos;

  // Get start position
  filepos -= len;

  // Punch a hole in the free intervals if necessary
  uint32_interval_remove(free_intervals, filepos, filepos + len);

  return ROM_BASE_ADDRESS | filepos;
}

// Gold ptr<->offset functions
signed int PointerToOffset(unsigned int ptr) {
  unsigned int pointer = 0;
  unsigned int bank = 0;
  bank = ptr & 0xFF;
  pointer = (ptr & 0xFFFF00) >> 8;
  if (pointer < 0x4000 || pointer > 0x7FFF) return -1;
  pointer &= 0x3FFF;
  pointer |= (bank & 3) << 14;
  bank >>= 2;
  return pointer | (bank << 16);
}
signed int OffsetToPointer(unsigned int offset) {
  unsigned int pointer = 0;
  unsigned int bank = 0;

  if ((offset & 0xFFC00000)) {
    return -1;
  }
  bank = (offset & 0x3FC000) >> 14;
  pointer = (offset & 0x3FFF) | 0x4000;
  return (pointer << 8) | bank;
}

bool gsc_are_banks_equal(uint32_t offset1, uint32_t offset2) {
  return ((offset1 ^ offset2) & 0x3FC000) != 0;
}

uint32_t gsc_next_bank(uint32_t offset) { return (offset & 0x3FC000) + 0x4000; }
