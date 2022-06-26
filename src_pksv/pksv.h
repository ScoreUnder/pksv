#ifndef PKSV_H
#define PKSV_H 1

#ifdef DLL
#ifdef WIN32
#include <windows.h>
#endif
#endif

#include <stdbool.h>
#include <stdio.h>

extern bool VersionOverride, IsVerbose;
extern bool dyndec;  // Decompile with dynamic names (helps newbies edit scripts
                     // without overwriting)
extern bool eorg;    // Erase length of code instead of compiling
extern char mode;
extern int dynplace;
extern unsigned char
    search;  // Character to look for when searching for free space
extern FILE *LogFile;

#ifdef DLL
#ifdef WIN32
int __stdcall OffsetDlg(HWND, UINT, WPARAM, LPARAM);
extern HWND HW_DLG;
#endif
#endif

// Pokemon game (de)compilation modes
#define FIRE_RED 0
#define RUBY 1
#define GOLD 2
#define CRYSTAL 4
#define EMERALD 5

// Operation modes
#define DECOMPILE 0
#define RECOMPILE 1
#define TXT 2
#define MOVEMENT 3
#define BRAILLE 4

#endif
