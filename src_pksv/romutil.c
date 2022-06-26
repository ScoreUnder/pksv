#include <string.h>
#include <stdio.h>

#include "romutil.h"
#include "pksv.h"

struct rom_mode determine_mode(FILE *romfile) {
  char buf[4];
  // Determine (de)compilation mode
  fseek(romfile, 0xAC, SEEK_SET);
  fread(buf, 1, 3, romfile);
  if (memcmp(buf, "AX", 2) == 0) {
    return (struct rom_mode){.type = RUBY, .search = 0xFF};
  }
  if (memcmp(buf, "BP", 2) == 0) {
    if (buf[2] == 'E') {
      // Emerald actually has 00-byte padding more commonly than FF-byte
      // padding. It seems to be removed content or something, because it still
      // pads to the end of the file with FF bytes.
      return (struct rom_mode){.type = EMERALD, .search = 0};
    } else {
      return (struct rom_mode){.type = FIRE_RED, .search = 0xFF};
    }
  }
  fseek(romfile, 0x13F, SEEK_SET);
  fread(buf, 1, 2, romfile);
  if (memcmp(buf, "AA", 2) == 0 || memcmp(buf, "SM", 2) == 0) {
    return (struct rom_mode){.type = GOLD, .search = 0};
  }
  if (memcmp(buf, "BY", 2) == 0) {
    return (struct rom_mode){.type = CRYSTAL, .search = 0};
  }
  return (struct rom_mode){.type = ROM_UNKNOWN, .search = 0};
}
