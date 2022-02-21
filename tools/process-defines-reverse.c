#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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

  char namebuf[256];
  bool ok = true;
  while (!feof(in)) {
    int c = fgetc(in);
    if (c == EOF) break;

    if (fread(namebuf, 1, c, in) != (size_t)c) {
      ok = false;
      break;
    }
    namebuf[c] = 0;
    strupper(namebuf);

    uint32_t value;
    if (fread(&value, 4, 1, in) != 1) {
      ok = false;
      break;
    }

    printf("#define %s 0x%08X\n", namebuf, value);
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
