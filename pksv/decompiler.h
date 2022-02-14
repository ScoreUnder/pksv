#ifndef PKSV_DECOMPILER_H
#define PKSV_DECOMPILER_H 1

#include <stdio.h>

void DecodeProc(FILE*fileM,
								unsigned int narc,
								unsigned int FileZoomPos,
								char*fname
#ifndef DLL
								,FILE*fsend
#endif
								);

#endif
