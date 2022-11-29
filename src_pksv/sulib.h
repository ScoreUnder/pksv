#ifndef PKSV_SULIB_H
#define PKSV_SULIB_H 1

#include <stdio.h>
#include <stdint.h>
#include "binarysearch.h"

typedef struct __insert {
  uint32_t pos;
  char* name;
  struct __insert* next;
  struct __insert* prev;
} codeinsert;

typedef struct __block {
  size_t allocated, size;
  char* data;
  struct __block* next;
  struct __block* prev;
  char* name;
  struct __insert* insert;
  uint32_t org;
  uint32_t align;
} codeblock;

typedef struct __label {
  uint32_t pos;
  char* name;
  codeblock* block;
  struct __label* next;
  struct __label* prev;
} codelabel;

unsigned int add_label(const char* name, codeblock* c, uint32_t loc,
                       codelabel** head);
codeblock* add_codeblock(codeblock* tail, const char* name, uint32_t org);
codeblock* rewind_codeblock(codeblock* c);
void add_insert(codeblock* block, uint32_t pos, const char* name);
void calc_org(codeblock* root_block, FILE* rom_search,
              struct bsearch_root* free_space);
void process_inserts(codeblock* c, codelabel* cl);
void add_data(codeblock* block, const void* data, size_t len);
void delete_all_codeblocks(codeblock* first);

#endif
