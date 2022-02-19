#ifndef PKSV_CODEPROC_H
#define PKSV_CODEPROC_H 1

#include <stdio.h>

#include "sulib.h"

typedef unsigned int pos_int;  // TODO: change to size_t at some point

/** Characters which are considered the end to an argument in PokeScript */
extern const char ARG_END_CHARS[];

extern int codenum;
extern int levelnum;
extern int textnum;
extern int movenum;
extern int martnum;
extern int thumbnum;
extern int dwordnum;
extern unsigned int fail;

void pksv_codeproc_reset(void);

#define Defined2(thing) (WhatIs2(thing),!fail)
#define GetNum(x) GenForFunc(x,&i,Script,defines,c)

extern unsigned char gffs;
unsigned int GenForFunc(char*func,
                        unsigned int*ii,
                        char*Script,
                        struct bsearch_root *defines,
                        codeblock*c);

void Define2(unsigned int otherthing,char*thing);
char* WhatIs2(int thing);

unsigned int FindFreeSpace(char*romname, unsigned int len, struct bsearch_root *defines);

void LowerCaseAndRemAll0D(char*orig);

signed int PointerToOffset(unsigned int ptr);
signed int OffsetToPointer(unsigned int offset);

#endif
