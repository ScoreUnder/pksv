#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lang_parsers.h"
#include "language-defs.h"
#include "binarysearch.h"
#include "stdio_ext.h"

struct parser_cache {
  struct bsearch_root loaded_parsers;
};

void destroy_loaded_parser(struct loaded_parser *parser) {
  bsearch_deinit_root(&parser->lookup_by_name);
  bsearch_deinit_root(&parser->lookup_by_id);
  free(parser);
}

struct loaded_parser *load_definitions(const char *filename) {
  struct loaded_parser *result = malloc(sizeof *result);
  bsearch_init_root(&result->lookup_by_name, bsearch_key_strcmp,
                    bsearch_key_strdup, free, NULL);
  // Values of lookup_by_id are not freed because they are all keys in
  // lookup_by_name.
  bsearch_init_root(&result->lookup_by_id, bsearch_key_int32cmp,
                    bsearch_key_nocopy, NULL, NULL);

  FILE *f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Cannot open definitions file \"%s\"!\n", filename);
    goto error;
  }

  size_t num_defines = fgetvarint(f);
  for (size_t i = 0; i < num_defines; i++) {
    char *str = fgetstr(f);
    uint32_t value = fgetvarint(f);

    if (feof(f) || ferror(f)) {
      goto error;
    }

    struct bsearch_root *by_name = &result->lookup_by_name;
    bsearch_ensure_capacity(by_name, by_name->size + 1);
    // Note: safe to insert directly because defines.dat is already sorted
    by_name->pairs[by_name->size++] = (struct bsearch_kv){
        .key = str,
        .value = (void *)(intptr_t)value,
    };
  }

  size_t num_reverse_defines = fgetvarint(f);
  for (size_t i = 0; i < num_reverse_defines; i++) {
    uint32_t value = fgetvarint(f);
    uint32_t str_index = fgetvarint(f);
    char *str = result->lookup_by_name.pairs[str_index].key;

    if (feof(f) || ferror(f)) {
      goto error;
    }

    struct bsearch_root *by_id = &result->lookup_by_id;
    bsearch_ensure_capacity(by_id, by_id->size + 1);
    by_id->pairs[by_id->size++] = (struct bsearch_kv){
        .key = (void *)(intptr_t)value,
        .value = str,
    };
  }

  if (feof(f) || ferror(f)) {
    goto error;
  }

  fclose(f);
  return result;

error:
  if (f != NULL) {
    if (feof(f)) {
      fprintf(stderr, "Definitions file \"%s\" is truncated\n", filename);
    } else if (ferror(f)) {
      fprintf(stderr, "Error while reading definitions file \"%s\"\n",
              filename);
    }
    fclose(f);
  }
  destroy_loaded_parser(result);
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

struct parser_cache *create_parser_cache(void) {
  struct parser_cache *cache = malloc(sizeof *cache);
  bsearch_init_root(&cache->loaded_parsers, bsearch_key_strcmp,
                    bsearch_key_strdup, free, destroy_loaded_parser);
  return cache;
}
