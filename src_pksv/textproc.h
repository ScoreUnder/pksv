#ifndef PKSV_TEXTPROC_H
#define PKSV_TEXTPROC_H 1

#include <stdio.h>
#include <stdint.h>

#include "sulib.h"

extern char trans[];

void log_txt(const char *str, size_t length);

char*transtxt(int howfar, const char*file, size_t word_wrap, uint32_t *resume_out);
char*transbrl(int howfar,const char*file,FILE*fsend);
char*transmove(int howfar,const char*file);
char* transbackstr(char*scrfn,unsigned int pos,codeblock*c);
unsigned int transbackmove(char*script,unsigned int*ii);

#endif
