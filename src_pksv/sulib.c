#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sulib.h"
#include "codeproc.h"
#include "binarysearch.h"
#include "binarysearch_u32.h"
#include "uint32_interval.h"
#include "pksv.h"
#include "textutil.h"
#include "romutil.h"

void add_label(const char* name, codeblock* block, codelabel** head) {
  assert(name != NULL);
  assert(block != NULL);
  assert(head != NULL);

  uint32_t loc = block->size;

  char* name2 = strdup(name);
  codelabel* newlabel = malloc(sizeof(codelabel));
  codelabel* tail = *head;
  if (tail)
    while (tail->next) tail = tail->next;
  newlabel->prev = tail;
  newlabel->next = NULL;
  newlabel->name = name2;
  newlabel->pos = loc;
  newlabel->block = block;
  if (tail)
    tail->next = newlabel;
  else
    *head = newlabel;
}

static void init_codeblock(codeblock* c, const char* name, uint32_t org) {
  assert(c != NULL);
  c->data = malloc(128);
  c->allocated = 128;
  c->size = 0;
  c->next = NULL;
  c->align = 1;
  c->prev = NULL;
  if (name != NULL) {
    c->name = strdup(name);
  } else {
    c->name = NULL;
  }
  c->insert = NULL;
  c->org = org;
  if (c->data == NULL) {
    fputs("Out of memory\n", stderr);
    exit(1);
  }
}

codeblock* add_codeblock(codeblock* tail, const char* name, uint32_t org) {
  codeblock* newblock = malloc(sizeof(codeblock));
  init_codeblock(newblock, name, org);
  newblock->prev = tail;
  if (tail != NULL) {
    assert(tail->next == NULL);
    tail->next = newblock;
  }
  return newblock;
}

codeblock* rewind_codeblock(codeblock* c) {
  codeblock* z;
  if (!c) return NULL;
  z = c;
  while (z->prev != NULL) {
    z = z->prev;
  }
  return z;
}

codeinsert* end_insert(codeinsert* c) {
  codeinsert* z;
  z = c;
  while (z->next != NULL) {
    z = z->next;
  }
  return z;
}

void add_insert(codeblock* block, uint32_t pos, const char* name) {
  codeinsert* newinsert = malloc(sizeof(codeinsert));
  if (newinsert == NULL) {
    fputs("Out of memory\n", stderr);
    exit(1);
  }
  codeinsert* last = block->insert;
  if (last != NULL) {
    last = end_insert(last);
    last->next = newinsert;
  } else {
    // Create the first insert if needed
    block->insert = newinsert;
  }
  newinsert->pos = pos;
  newinsert->next = NULL;
  newinsert->prev = last;
  newinsert->name = strdup(name);
}

static void delete_inserts(codeblock* block) {
  codeinsert* next;
  for (codeinsert* curr = block->insert; curr != NULL; curr = next) {
    next = curr->next;
    free(curr->name);
    free(curr);
  }
  block->insert = NULL;
}

void calc_org(codeblock* root_block, FILE* rom_search,
              struct bsearch_root* free_space) {
  if (root_block == NULL) return;
  // TODO: individual findfrom values (track used space)
  uint32_t findfrom = root_block->org;
  for (codeblock* block = root_block; block != NULL; block = block->next) {
    if (block->name != NULL) {
    retry_for_address : {
      uint32_t min_address = 0;
      uint32_t result =
          FindFreeSpace(rom_search, block->size, block->align, &findfrom,
                        &min_address, search, free_space);
      if (result == UINT32_MAX) {
        fprintf(stderr, "error: could not find free space for %s\n",
                block->name);
        exit(1);  // TODO: report up the call stack
      }
      if (mode == GOLD || mode == CRYSTAL) {
        if (block->size < 0x4000) {
          if (!gsc_are_banks_equal(result, result + block->size - 1)) {
            // Record the discarded free space interval (we didn't use it)
            uint32_interval_add(free_space, result, result + block->size);

            // Not enough space in this bank, try the next bank
            findfrom = gsc_next_bank(result);
            min_address = findfrom;

            // TODO: Record skipped free space interval as a result of bank
            // change needs raw rom search to know true extent of free space
            goto retry_for_address;
          }
        } else {
          fprintf(stderr,
                  "Error: Offset %s cannot be written as it is too large.\n",
                  block->name);
          exit(1);  // TODO: report up the call stack
        }
      }
      block->org = result;
    }
    }
  }
}

static uint32_t get_address_for_insert(codeblock* root_block,
                                       codelabel* root_label,
                                       codeinsert* insert) {
  if (insert->name[0] == ':') {
    // Insert is requesting a label
    for (codelabel* label = root_label; label != NULL; label = label->next) {
      if (!strcmp(label->name, insert->name)) {
        return label->pos + label->block->org;
      }
    }
  } else {
    // Insert is requesting a dynamic offset
    for (codeblock* block = root_block; block != NULL; block = block->next) {
      if (block->name != NULL && !strcmp(block->name, insert->name)) {
        return block->org;
      }
    }
  }
  fprintf(stderr, "error: could not find address for %s\n", insert->name);
  exit(1);  // TODO: report up the call stack
}

void process_inserts(codeblock* root_block, codelabel* cl) {
  for (codeblock* y = root_block; y != NULL; y = y->next) {
    for (codeinsert* x = y->insert; x != NULL; x = x->next) {
      uint32_t address = get_address_for_insert(root_block, cl, x);
      size_t address_width = 4;
      if (mode == GOLD || mode == CRYSTAL) {
        address = OffsetToPointer(address & ~ROM_BASE_ADDRESS);
        // TODO: better way to tell these apart?
        // maybe put the address width in the insert?
        if (x->name[0] == ':') {
          // Label; use 2-byte syntax
          address_width = 2;
        } else {
          // Dynamic offset; use 3-byte syntax
          address_width = 3;
        }
      }
      memcpy(y->data + x->pos, &address, address_width);
    }
    delete_inserts(y);
  }
}

void add_data(codeblock* block, const void* data, size_t len) {
  if (block->size + len > block->allocated) {
    block->allocated = (2 * block->allocated) + len;
    block->data = realloc(block->data, block->allocated);
  }
  memcpy(&block->data[block->size], data, len);
  block->size += len;
}

void delete_all_codeblocks(codeblock* first) {
  if (first == NULL) return;
  assert(first->prev == NULL);

  codeblock* next;
  for (codeblock* curr = first; curr != NULL; curr = next) {
    next = curr->next;
    delete_inserts(curr);
    if (curr->data) free(curr->data);
    if (curr->name) free(curr->name);
    free(curr);
  }
}
