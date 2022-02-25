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

  size_t num_defines = fgetvarint(in);
  char **defines = malloc(num_defines * sizeof(char *));
  for (size_t i = 0; i < num_defines; i++) {
    char *name = fgetstr(in);
    uint32_t value = fgetvarint(in);

    if (feof(in) || ferror(in)) {
      free(name);
      break;
    }

    strupper(name);
    defines[i] = name;
    printf("#define %s 0x%08X\n", name, value);
  }

  puts("");

  size_t num_reverse_defines = fgetvarint(in);
  for (size_t i = 0; i < num_reverse_defines; i++) {
    uint32_t value = fgetvarint(in);
    uint32_t name_index = fgetvarint(in);
    char *name = defines[name_index];

    if (feof(in) || ferror(in)) {
      break;
    }

    printf("' 0x%08X defaults to %s\n", value, name);
  }

  if (feof(in)) {
    fprintf(stderr, "Unexpected end-of-file in %s\n", infile);
    return 1;
  }

  if (ferror(in)) {
    perror(infile);
    return 1;
  }

  return 0;
}
