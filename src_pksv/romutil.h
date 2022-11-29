#ifndef PKSV_ROM_UTIL_H
#define PKSV_ROM_UTIL_H 1

#include <stdio.h>

#define ROM_UNKNOWN 255

#define ROM_BASE_ADDRESS 0x08000000
#define GBA_MAX_ROM_SIZE 0x02000000

struct rom_mode {
  unsigned type : 8;
  unsigned search : 8;
};

struct rom_mode determine_mode(FILE *romfile);

#endif
