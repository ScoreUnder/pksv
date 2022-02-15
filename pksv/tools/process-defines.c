#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

#include "../binarysearch.h"

#define IN_FILE "pokeinc_default.txt"

_Static_assert(sizeof(intptr_t) >= sizeof(uint32_t), "intptr_t must be at least 32 bits");

void strtolower(char *str)
{
    while (*str)
    {
        *str = tolower(*str);
        str++;
    }
}

static int bsearch_key_strcmp(const void *a, const void *b)
{
  return strcmp((const char*)a,(const char*)b);
}

static void* bsearch_key_strdup(const void *a)
{
  return strdup((const char*)a);
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  struct bsearch_root defines;

  FILE *f = fopen(IN_FILE, "r");
  if (!f) {
    perror("opening " IN_FILE);
    return 1;
  }

  bsearch_init_root(&defines, bsearch_key_strcmp, bsearch_key_strdup, free, NULL);

  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    char *saveptr = NULL;
    char *p = strtok_r(line, " \t\r\n", &saveptr);
    if (!p) continue;

    if (!strcmp(p, "#d") || !strcmp(p, "#define")) {
      char *identifier = strtok_r(NULL, " \t\r\n", &saveptr);
      if (!identifier) { fputs("#define without identifier", stderr); return 1; }

      char *value = strtok_r(NULL, " \t\r\n", &saveptr);
      if (!value) { fputs("#define without value", stderr); return 1;}

      strtolower(identifier);
      strtolower(value);

      int base = 10;
      if (value[0] == '0') {
        switch (value[1]) {
          case 'x': base = 16; value += 2; break;
          case 'o': base = 8; value += 2; break;
          case 'b': base = 2; value += 2; break;
        }
      }

      char *endptr;
      uint32_t value_parsed = strtoul(value, &endptr, base);
      if (!endptr || *endptr != '\0') {
        size_t index = bsearch_find_if_exists(&defines, value);
        if (index != defines.size) {
          value_parsed = (uint32_t)(intptr_t) defines.pairs[index].value;
        } else {
          fprintf(stderr, "#define of %s with invalid value: %s\n", identifier, value);
          return 1;
        }
      }

      bsearch_upsert(&defines, identifier, (void*)(intptr_t) value_parsed);
    } else if (!strcmp(p, "#quiet") || !strcmp(p, "#loud")) {
      // ignore verbosity indicator
    } else if (p[0] == '\'') {
      // ignore comment
    } else {
      fprintf(stderr, "Don't know how to handle: %s\n", line);
      return 1;
    }
  }
  fclose(f);

  FILE *outfile = fopen("defines.dat", "wb");
  if (!outfile) {
    perror("opening defines.dat for writing");
    return 1;
  }

  for (size_t i = 0; i < defines.size; i++) {
    fputc(strlen(defines.pairs[i].key), outfile);
    fputs(defines.pairs[i].key, outfile);
    fwrite(&defines.pairs[i].value, sizeof(uint32_t), 1, outfile);
  }

  if (fflush(outfile)) {
    perror("writing to defines.dat");
    return 1;
  }
  fclose(outfile);

  return 0;
}
