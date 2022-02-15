#ifndef PKSV_TEXTPROC_H
#define PKSV_TEXTPROC_H 1

#include <stdio.h>

#include "sulib.h"

extern char trans[];

void log_txt(char *str, size_t length);

char*transtxt(int howfar,char*file);
char*transbrl(int howfar,char*file,FILE*fsend);
char*transmove(int howfar,char*file);
char* transbackstr(char*scrfn,unsigned int pos,codeblock*c);
unsigned int transbackmove(char*script,unsigned int*ii);

#endif
