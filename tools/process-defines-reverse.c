#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "stdio_ext.h"

void strupper(char *s) {
  while (*s) {
    *s = toupper(*s);
    s++;
  }
}

int main(int argc, char **argv) {
  const char *infile = "defines.dat";
  if (argc > 1) infile = argv[1];
  FILE *in = fopen(infile, "rb");
  if (!in) {
    perror(infile);
    return 1;
  }

  bool ok = true;
  while (!feof(in)) {
    char *name = fgetstr(in);

    if (feof(in)) {
      free(name);
      break;
    }

    uint32_t value = fgetvarint(in);

    if (feof(in) || ferror(in)) {
      ok = false;
      free(name);
      break;
    }

    strupper(name);
    printf("#define %s 0x%08X\n", name, value);
  }

  if (feof(in) && !ok) {
    fprintf(stderr, "Unexpected end-of-file in %s\n", infile);
    return 1;
  }

  if (ferror(in)) {
    perror(infile);
    return 1;
  }

  return 0;
}
