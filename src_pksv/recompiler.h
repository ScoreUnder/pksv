#ifndef PKSV_RECOMPILER_H
#define PKSV_RECOMPILER_H 1

#include <stdio.h>
#include <stdbool.h>

#define DEFINITIONS_FILE "defines.dat"
#define INCLUDES_FILE "pokeinc.txt"

#ifdef DLL
extern unsigned int needdlg;
#endif

extern bool testing;
extern char *defines_dat_location;
extern char *pokeinc_txt_location;
void RecodeProc(char *script, char *romfn);

#endif
