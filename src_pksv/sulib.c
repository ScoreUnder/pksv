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

unsigned int add_label(char* name, codeblock* c, unsigned int loc,
                       codelabel** head) {
  char* name2 = strdup(name);
  codelabel* newlabel = malloc(sizeof(codelabel));
  codelabel* tail = *head;
  if (tail)
    while (tail->next) tail = tail->next;
  newlabel->prev = tail;
  newlabel->next = NULL;
  newlabel->name = name2;
  newlabel->pos = loc;
  newlabel->block = c;
  if (tail)
    tail->next = newlabel;
  else
    *head = newlabel;
  return newlabel != NULL;
}

unsigned int init_codeblock(codeblock* c, char* name, int org) {
  if (c == NULL) return 0;
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
  }
  return (c->data != NULL);
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
unsigned int add_insert(codeblock* c, unsigned int p, char* n) {
  codeinsert* newinsert = malloc(sizeof(codeinsert));
  if (newinsert == NULL) return 0;
  codeinsert* last = c->insert;
  if (last != NULL) {
    last = end_insert(last);
    last->next = newinsert;
  } else {
    // Create the first insert if needed
    c->insert = newinsert;
  }
  newinsert->pos = p;
  newinsert->next = NULL;
  newinsert->prev = NULL;
  newinsert->name = strdup(n);
  return 1;
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

void delete_codeblock(codeblock* c) {
  codeblock* p;
  codeblock* n;
  if (c == NULL) return;
  delete_inserts(c);
  p = c->prev;
  n = c->next;
  if (p == NULL && n != NULL) {
    memcpy(c, n, sizeof(codeblock));
    delete_inserts(n);
    free(n->data);
    free(n->name);
    free(n);
  } else if (p != NULL) {
    p->next = n;
    if (n != NULL) n->prev = p;
    free(c->data);
    free(c->name);
    free(c);
  }
}

void calc_org(codeblock* root_block, unsigned int start, FILE* rom_search,
              struct bsearch_root* defines) {
  struct bsearch_root free_space;  // TODO: make parameter
  uint32_interval_init_bsearch_root(&free_space);

  const char* findfrom_name =
      (mode == GOLD || mode == CRYSTAL) ? "findfromgold" : "findfrom";
  uint32_t findfrom =
      CAST_pvoid_u32(bsearch_get_val(defines, findfrom_name, (void*)0));

  for (codeblock* block = root_block; block != NULL; block = block->next) {
    if (block->name != NULL) {
    retry_for_address : {
      uint32_t min_address = 0;
      uint32_t result =
          FindFreeSpace(rom_search, block->size, block->align, &findfrom,
                        &min_address, search, &free_space);
      if (result == UINT32_MAX) {
        fprintf(stderr, "error: could not find free space for %s\n",
                block->name);
        exit(1);  // TODO: report up the call stack
      }
      if (mode == GOLD || mode == CRYSTAL) {
        if (block->size < 0x4000) {
          if (!gsc_are_banks_equal(result, result + block->size - 1)) {
            // Record the discarded free space interval (we didn't use it)
            uint32_interval_add(&free_space, result, result + block->size);

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
      // TODO: why would a label have no name?
      if (label->name != NULL && !strcmp(label->name, insert->name)) {
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

void add_data(codeblock* c, char* data, unsigned int len) {
  if (c->size + len > c->allocated) {
    c->allocated = (2 * c->allocated) + len;
    c->data = realloc(c->data, c->allocated);
  }
  memcpy(&c->data[c->size], data, len);
  c->size += len;
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
