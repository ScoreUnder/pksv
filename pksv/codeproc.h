#ifndef PKSV_CODEPROC_H
#define PKSV_CODEPROC_H 1

#include <stdio.h>

#include "sulib.h"

extern int codenum;
extern int levelnum;
extern int textnum;
extern int movenum;
extern int martnum;
extern int thumbnum;
extern int dwordnum;
extern unsigned int fail;

#define Defined2(thing) (WhatIs2(thing),!fail)

#ifndef DLL
#define GetNum(x) GenForFunc(x,&i,LogFile,Script,romfn,c)
#else
#define GetNum(x) GenForFunc(x,&i,Script,romfn,c)
#endif

extern unsigned char gffs;
unsigned int GenForFunc(char*func,
                        unsigned int*ii,
#ifndef DLL
                        FILE* LogFile,
#endif
                        char*Script,
                        char*romfn,
                        codeblock*c);

void Define(char*thing,unsigned int otherthing);
unsigned int WhatIs(char*thing);
void ReDefine(char*thing,int val);

void Define2(unsigned int otherthing,char*thing);
char* WhatIs2(int thing);

void LowerCaseAndRemAll0D(char*orig);

signed int PointerToOffset(unsigned int ptr);
signed int OffsetToPointer(unsigned int offset);

#endif
