#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

#include "binarysearch.h"
#include "stdio_ext.h"
#include "textutil.h"

_Static_assert(sizeof(intptr_t) >= sizeof(uint32_t),
               "intptr_t must be at least 32 bits");

void strtolower(char *str) {
  while (*str) {
    *str = tolower(*str);
    str++;
  }
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  struct bsearch_root defines;
  struct bsearch_root defines_by_value;

  char *in_filename = "pokeinc_default.txt";
  if (argc >= 2) {
    in_filename = argv[1];
  }

  FILE *f = fopen(in_filename, "rt");
  if (!f) {
    perror("opening input file");
    return 1;
  }

  bsearch_init_root(&defines, bsearch_key_strcmp, bsearch_key_strdup, free,
                    NULL);
  bsearch_init_root(&defines_by_value, bsearch_key_int32cmp, bsearch_key_nocopy,
                    NULL, NULL);

  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    char *p = strtok(line, " \t\r\n");
    if (!p) continue;

    if (!strcmp(p, "#d") || !strcmp(p, "#define")) {
      char *identifier = strtok(NULL, " \t\r\n");
      if (!identifier) {
        fputs("#define without identifier", stderr);
        return 1;
      }

      char *value = strtok(NULL, " \t\r\n");
      if (!value) {
        fputs("#define without value", stderr);
        return 1;
      }

      strtolower(identifier);
      strtolower(value);

      int base = 10;
      if (value[0] == '0') {
        switch (value[1]) {
          case 'x':
            base = 16;
            value += 2;
            break;
          case 'o':
            base = 8;
            value += 2;
            break;
          case 'b':
            base = 2;
            value += 2;
            break;
        }
      }

      char *endptr;
      uint32_t value_parsed = strtoul(value, &endptr, base);
      if (!endptr || *endptr != '\0') {
        ptrdiff_t index = bsearch_find(&defines, value);
        if (index >= 0) {
          value_parsed = (uint32_t)(intptr_t)defines.pairs[index].value;
        } else {
          fprintf(stderr, "#define of %s with invalid value: %s\n", identifier,
                  value);
          return 1;
        }
      }

      ptrdiff_t idx = bsearch_find(&defines, identifier);
      if (idx >= 0) {
        fprintf(stderr, "Duplicate #define of %s\n", identifier);
        return 1;
      }
      idx = (ptrdiff_t)bsearch_unsafe_insert(&defines, idx, strdup(identifier),
                                           (void *)(intptr_t)value_parsed);
      bsearch_upsert(&defines_by_value, (void *)(intptr_t)value_parsed,
                     defines.pairs[idx].key);
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

  char *out_filename = "defines.dat";
  if (argc >= 3) {
    out_filename = argv[2];
  }
  FILE *outfile = fopen(out_filename, "wb");
  if (!outfile) {
    perror("opening defines.dat for writing");
    return 1;
  }

  fputvarint(defines.size, outfile);
  for (size_t i = 0; i < defines.size; i++) {
    fputstr(defines.pairs[i].key, outfile);
    fputvarint((uint32_t)(intptr_t)defines.pairs[i].value, outfile);
  }

  // Much of this can be derived from existing data, but specifying
  // it explicitly will make loading this file cheaper.
  // Namely, the length and keys are both derivable trivially from
  // the previous array.
  fputvarint(defines_by_value.size, outfile);
  for (size_t i = 0; i < defines_by_value.size; i++) {
    fputvarint((uint32_t)(intptr_t)defines_by_value.pairs[i].key, outfile);
    fputvarint(
        (uint32_t)bsearch_find(&defines, defines_by_value.pairs[i].value),
        outfile);
  }

  if (fflush(outfile)) {
    perror("writing to defines.dat");
    return 1;
  }
  fclose(outfile);

  return 0;
}
