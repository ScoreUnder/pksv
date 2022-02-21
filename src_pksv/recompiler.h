#ifndef PKSV_RECOMPILER_H
#define PKSV_RECOMPILER_H 1

#include <stdio.h>
#include <stdbool.h>

#ifdef DLL
extern unsigned int needdlg;
#endif

extern bool testing;
extern char *defines_dat_location;
void RecodeProc(char *script, char *romfn);

#endif
