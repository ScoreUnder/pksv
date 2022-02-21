#ifndef PKSV_DECOMPILER_H
#define PKSV_DECOMPILER_H 1

#include <stdio.h>

void pksv_decompiler_reset(void);

void DecodeProc(FILE* fileM, unsigned int narc, unsigned int FileZoomPos,
                char* fname, FILE* fsend);
void DecodeProcASM(FILE* fileM, unsigned int FileZoomPos, char* fname,
                   FILE* fsend);
void DecodeProcText(FILE* fileM, unsigned int FileZoomPos, char* fname,
                    FILE* fsend);
void DecodeProcPointer(FILE* fileM, unsigned int FileZoomPos, char* fname,
                       FILE* fsend);
void DecodeProcLevel(FILE* fileM, unsigned int FileZoomPos, char* fname,
                     FILE* fsend);
void DecodeProcMoves(FILE* fileM, unsigned int FileZoomPos, char* fname,
                     FILE* fsend);
void DecodeProcMart(FILE* fileM, unsigned int FileZoomPos, char* fname,
                    FILE* fsend);
void DecodeProcAttacks(FILE* fileM, unsigned int FileZoomPos, char* fname,
                       FILE* fsend);

#endif
