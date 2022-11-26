#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sulib.h"
#include "codeproc.h"
#include "binarysearch.h"
#include "pksv.h"
#include "textutil.h"

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
  c->align = 0;
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

unsigned int ffoff = 0;
void calc_org(codeblock* c, unsigned int start, char* file,
              struct bsearch_root* defines) {
  register unsigned int a;
  register codeblock* b;
  char buf[1024];
  a = start;
  a |= 0x08000000;
  b = rewind_codeblock(c);
  while (b != NULL) {
    if (b->name != NULL) {
      b->org = FindFreeSpace(file, b->size + b->align - 1, defines);
      if (b->align && b->org % b->align)
        b->org += b->align - (b->org % b->align);
      if ((mode == GOLD || mode == CRYSTAL) && b->size < 0x3FFF) {
        while ((OffsetToPointer(b->org) & 0xFF) !=
               (OffsetToPointer(b->org + b->size) & 0xFF)) {
          ffoff -= b->size - 1;
          b->org = FindFreeSpace(file, b->size, defines);
        }
      }
      else if (mode == GOLD || mode == CRYSTAL) {
        fprintf(stderr, "Error: Offset %s cannot be written as it is too large.\n", b->name);
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
