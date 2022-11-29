#ifndef PKSV_ROM_UTIL_H
#define PKSV_ROM_UTIL_H 1

#include <stdio.h>

#define ROM_UNKNOWN 255

#define ROM_BASE_ADDRESS 0x08000000
#define GBA_MAX_ROM_SIZE 0x02000000

// Where we can basically guarantee free space in FR/LG/R/S/E and G/S/C ROMs.
#define FREESPACE_START_GBA 0x00740000
#define FREESPACE_START_GBC 0x00098860

struct rom_mode {
  unsigned type : 8;
  unsigned search : 8;
};

struct rom_mode determine_mode(FILE *romfile);

#endif
