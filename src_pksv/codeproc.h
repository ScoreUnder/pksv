#ifndef PKSV_CODEPROC_H
#define PKSV_CODEPROC_H 1

#include <stdint.h>
#include <stdbool.h>
#include "binarysearch.h"
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

#define Defined2(thing) (WhatIs2(thing), !fail)
#define GetNum(x) GenForFunc(x, &i, Script, defines, c)

extern unsigned char gffs;
unsigned int GenForFunc(char *func, unsigned int *ii, char *Script,
                        struct bsearch_root *defines, codeblock *c);

void Define2(unsigned int otherthing, char *thing);
char *WhatIs2(int thing);

/** Find a consecutive block of free space.
 *  @param rom_search The ROM file to search for free space
 *  @param len The length of the returned block of free space
 *  @param align The alignment of the returned block of free space
 *  @param offset Where to start searching for free space
 *  @param search The byte value considered to be free space
 *  @param free_intervals A collection of definitely-free-space intervals
 *  @return The ROM address of a block of free space of length `len`,
 *          or UINT32_MAX on error.
 */
uint32_t FindFreeSpace(FILE *rom_search, uint32_t len, uint32_t align,
                       uint32_t *offset, uint8_t search,
                       struct bsearch_root *free_intervals);

void LowerCaseAndRemAll0D(char *orig);

signed int PointerToOffset(unsigned int ptr);
signed int OffsetToPointer(unsigned int offset);

bool gsc_are_banks_equal(uint32_t offset1, uint32_t offset2);
uint32_t gsc_next_bank(uint32_t offset);

#endif
