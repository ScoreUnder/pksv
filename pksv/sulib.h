#ifndef PKSV_SULIB_H
#define PKSV_SULIB_H 1

#include "binarysearch.h"

typedef struct __define {
  char*name;
  unsigned int means;
  struct __define*next;
} definition;

typedef struct __insert {
  unsigned int pos;
  char*name;
  struct __insert*next;
  struct __insert*prev;
} codeinsert;

typedef struct __block {
  unsigned int allocated,size;
  char*data;
  struct __block*next;
  struct __block*prev;
  char*name;
  struct __insert*insert;
  unsigned int org;
  unsigned int align;
} codeblock;

typedef struct __label {
  unsigned int pos;
  char*name;
  codeblock*block;
  struct __label*next;
  struct __label*prev;
} codelabel;

extern unsigned int ffoff;

unsigned int add_label(char*name,codeblock*c,unsigned int loc,codelabel**chain);
unsigned int init_codeblock(codeblock*c,char*name,int org);
codeblock* rewind_codeblock(codeblock*c);
unsigned int add_insert(codeblock*c,unsigned int p,char*n);
void delete_inserts(codeblock*c);
void calc_org(codeblock*c,unsigned int start,char*file, struct bsearch_root *defines);
void process_inserts(codeblock*c,codelabel*cl);
void add_data(codeblock*c,char*data,unsigned int len);

#endif
