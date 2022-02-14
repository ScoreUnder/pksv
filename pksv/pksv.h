#ifndef PKSV_H
#define PKSV_H 1

#include <stdbool.h>
#include <stdio.h>

extern char GlobBuf[];

extern bool VersionOverride, IsVerbose;
extern bool dyndec;  // Decompile with dynamic names (helps newbies edit scripts without overwriting)
extern bool eorg;  // Erase length of code instead of compiling
extern char mode;
extern int dynplace;
extern char dyntype;
extern unsigned char search;  // Character to look for when searching for free space

#ifndef DLL
extern FILE *LogFile;
#endif

// Pokemon game (de)compilation modes
#define FIRE_RED  0
#define RUBY      1
#define GOLD      2
#define DIAMOND   3
#define CRYSTAL   4

// Operation modes
#define DECOMPILE 0
#define RECOMPILE 1
#define TXT       2
#define MOVEMENT  3
#define BRAILLE   4

#endif
