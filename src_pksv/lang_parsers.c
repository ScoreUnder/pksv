#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "language-defs.h"
#include "binarysearch.h"
#include "stdio_ext.h"

struct loaded_parser {
  struct bsearch_root lookup_by_name;
  struct bsearch_root lookup_by_id;
};

struct parser_cache {
  struct bsearch_root loaded_parsers;
};


struct bsearch_root *load_definitions(const char *filename) {
  struct bsearch_root *defines =
      bsearch_create_root(bsearch_key_strcmp, bsearch_key_strdup, free, NULL);

  FILE *f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Cannot open definitions file \"%s\"!\n", filename);
    goto error;
  }

  while (!feof(f)) {
    char *str = fgetstr(f);
    uint32_t value = fgetvarint(f);

    if (feof(f) || ferror(f)) {
      free(str);
      if (feof(f)) {
        fprintf(stderr, "Definitions file \"%s\" is truncated\n", filename);
      } else if (ferror(f)) {
        fprintf(stderr, "Error while reading definitions file \"%s\"\n", filename);
      }
      goto error;
      break;
    }

    bsearch_ensure_capacity(defines, defines->size + 1);
    // Note: safe to insert directly because defines.dat is already sorted
    defines->pairs[defines->size++] = (struct bsearch_kv){
        .key = str,
        .value = (void *)(intptr_t)value,
    };
  }
  fclose(f);
  return defines;

error:
  if (f != NULL) fclose(f);
  bsearch_destroy_root(defines);
  return NULL;
}

bool parse_for_decomp(struct language_def *lang, const char *parser_dir,
                      struct parser_list parsers, char *token,
                      size_t token_len) {
  for (size_t i = 0; i < parsers.length; i++) {
    const char *parser_name = parsers.parsers[i].name;
    bool is_prefixed = parsers.parsers[i].is_prefixed;

    if (is_prefixed) {
      size_t parser_name_len = strlen(parser_name);
      char **parents = lang->parents;

      char *prefix = lang->name;
      do {
        size_t prefix_len = strlen(prefix);

        char *next_name = malloc(parser_name_len + prefix_len + 2);
        memcpy(next_name, prefix, prefix_len);
        next_name[prefix_len] = '_';
        memcpy(next_name + prefix_len + 1, parser_name, parser_name_len);
        next_name[prefix_len + parser_name_len + 1] = '\0';



        free(next_name);
        prefix = *parents++;
      } while (prefix != NULL);
    }
  }
}
