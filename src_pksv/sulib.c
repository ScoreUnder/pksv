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

codeblock* end_codeblock(codeblock* c) {
  codeblock* z;
  z = c;
  while (z->next != NULL) {
    z = z->next;
  }
  return z;
}

codeinsert* rewind_insert(codeinsert* c) {
  codeinsert* z;
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
  codeinsert* z;
  z = c->insert;
  if (z != NULL) {
    z = end_insert(z);
    z->next = malloc(sizeof(codeinsert));
    z = z->next;
    z->pos = p;
    z->next = NULL;
    z->prev = NULL;
    z->name = malloc(strlen(n) + 1);
    strcpy(z->name, n);
    if (z == NULL) return 0;
  }
  if (z == NULL)  // Create the first insert if needed
  {
    c->insert = malloc(sizeof(codeinsert));
    z = c->insert;
    z->pos = p;
    z->next = NULL;
    z->prev = NULL;
    z->name = malloc(strlen(n) + 1);
    strcpy(z->name, n);
    if (z == NULL) return 0;
  }
  return 1;
}

void delete_inserts(codeblock* c) {
  codeinsert* i;
  codeinsert* n;
  i = c->insert;
  c->insert = NULL;
  while (i != NULL) {
    n = i->next;
    free(i->name);
    free(i);
    i = n;
  }
}

void add_insert_name(codeinsert* i, char* x) {
  i->name = malloc(strlen(x) + 1);
  strcpy(i->name, x);
}

void add_code_name(codeblock* i, char* x) {
  i->name = malloc(strlen(x) + 1);
  strcpy(i->name, x);
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

void calc_org(codeblock* c, unsigned int start, FILE* rom_search,
              struct bsearch_root* defines) {
  struct bsearch_root free_space;  // TODO: make parameter
  uint32_interval_init_bsearch_root(&free_space);

  const char* findfrom_name =
      (mode == GOLD || mode == CRYSTAL) ? "findfromgold" : "findfrom";
  uint32_t findfrom =
      CAST_pvoid_u32(bsearch_get_val(defines, findfrom_name, (void*)0));

  uint32_t a = ROM_BASE_ADDRESS | start;
  codeblock* b = rewind_codeblock(c);
  while (b != NULL) {
    if (b->name != NULL) {
    retry_for_address : {
      uint32_t min_address = 0;
      uint32_t result = FindFreeSpace(rom_search, b->size, b->align, &findfrom,
                                      &min_address, search, &free_space);
      if (result == UINT32_MAX) {
        fprintf(stderr, "error: could not find free space for %s\n", b->name);
        exit(1);  // TODO: report up the call stack
      }
      if (mode == GOLD || mode == CRYSTAL) {
        if (b->size < 0x4000) {
          if (!gsc_are_banks_equal(result, result + b->size - 1)) {
            // Record the discarded free space interval (we didn't use it)
            uint32_interval_add(&free_space, result, result + b->size);

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
                  b->name);
          exit(1);  // TODO: report up the call stack
        }
      }
      b->org = result;
    }
    }
    b = b->next;
  }
}

void process_inserts(codeblock* c,
                     codelabel* cl)  // process inserts for everything.
{
  register codeblock* z;
  register codeblock* y;
  register codeinsert* x;
  register codelabel* cl2;
  unsigned int j;
  y = rewind_codeblock(c);
  while (y != NULL) {
    x = y->insert;
    while (x != NULL) {
      if (*(x->name) == ':') {
        cl2 = cl;
        while (cl2) {
          if (cl2->name)
            if (!strcmp(cl2->name, x->name)) {
              if (mode == GOLD || mode == CRYSTAL) {
                j = OffsetToPointer((cl2->pos + cl2->block->org) &
                                    0x07FFFFFF) >>
                    8;
                memcpy((void*)(y->data + x->pos), &j, 2);
              } else {
                j = (cl2->pos + cl2->block->org) | 0x08000000;
                memcpy((void*)(y->data + x->pos), &j, 4);
              }
            }
          cl2 = cl2->next;
        }
      } else {
        z = rewind_codeblock(c);
        while (z) {
          if (z->name)
            if (!strcmp(z->name, x->name)) {
              if (mode == GOLD || mode == CRYSTAL) {
                j = OffsetToPointer(z->org & 0x07FFFFFF);
                memcpy((void*)(y->data + x->pos), &j, 3);
              } else
                memcpy((void*)(y->data + x->pos), &(z->org), 4);
            }
          z = z->next;
        }
      }
      x = x->next;
    }
    delete_inserts(y);
    y = y->next;
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
