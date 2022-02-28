#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lang_default_parsers.h"
#include "lang_parsers.h"
#include "language-defs.h"
#include "binarysearch.h"
#include "stdio_ext.h"

struct parser_cache {
  struct bsearch_root loaded_parsers;
  char *parser_dir;
};

void deinit_loaded_parser(struct loaded_parser *parser) {
  bsearch_deinit_root(&parser->lookup_by_name);
  bsearch_deinit_root(&parser->lookup_by_id);
}

void destroy_loaded_parser(struct loaded_parser *parser) {
  deinit_loaded_parser(parser);
  free(parser);
}

void bsearch_destroy_loaded_or_builtin_parser(void *ptr) {
  struct loaded_or_builtin_parser *parser = ptr;
  if (parser->which == PARSER_TYPE_LOADED) {
    deinit_loaded_parser(&parser->loaded);
    free(parser);
  }
  // Builtin parsers are statically allocated.
}

struct loaded_parser *load_definitions(const char *filename, bool required) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    if (required) {
      fprintf(stderr, "Cannot open definitions file \"%s\"!\n", filename);
    }
    return NULL;
  }

  struct loaded_parser *result = malloc(sizeof *result);
  bsearch_init_root(&result->lookup_by_name, bsearch_key_strcmp,
                    bsearch_key_strdup, free, NULL);
  // Values of lookup_by_id are not freed because they are all keys in
  // lookup_by_name.
  bsearch_init_root(&result->lookup_by_id, bsearch_key_int32cmp,
                    bsearch_key_nocopy, NULL, NULL);

  size_t num_defines = fgetvarint(f);
  struct bsearch_root *by_name = &result->lookup_by_name;
  bsearch_ensure_capacity(by_name, num_defines);
  for (size_t i = 0; i < num_defines; i++) {
    char *str = fgetstr(f);
    uint32_t value = fgetvarint(f);

    if (feof(f) || ferror(f)) {
      goto error;
    }

    // Note: safe to insert directly because defines.dat is already sorted
    bsearch_unsafe_append(by_name, str, (void *)(intptr_t)value);
  }

  size_t num_reverse_defines = fgetvarint(f);
  struct bsearch_root *by_id = &result->lookup_by_id;
  bsearch_ensure_capacity(by_id, num_reverse_defines);
  for (size_t i = 0; i < num_reverse_defines; i++) {
    uint32_t value = fgetvarint(f);
    uint32_t str_index = fgetvarint(f);
    char *str = result->lookup_by_name.pairs[str_index].key;

    if (feof(f) || ferror(f)) {
      goto error;
    }

    bsearch_unsafe_append(by_id, (void *)(intptr_t)value, str);
  }

  if (feof(f) || ferror(f)) {
    goto error;
  }

  fclose(f);
  return result;

error:
  if (feof(f)) {
    fprintf(stderr, "Definitions file \"%s\" is truncated\n", filename);
  } else if (ferror(f)) {
    fprintf(stderr, "Error while reading definitions file \"%s\"\n", filename);
  }
  fclose(f);
  destroy_loaded_parser(result);
  return NULL;
}

char *get_definitions_file_name(const char *parser_dir, const char *defs_name) {
  static char parser_filename_prefix[] = "defs_";
  static char parser_filename_suffix[] = ".dat";
  static size_t parser_filename_prefix_len = sizeof(parser_filename_prefix) - 1;
  static size_t parser_filename_suffix_len = sizeof(parser_filename_suffix) - 1;

  size_t parser_dir_len = strlen(parser_dir);
  size_t defs_name_len = strlen(defs_name);
  char *parser_filename =
      malloc(parser_dir_len + parser_filename_prefix_len + defs_name_len +
             parser_filename_suffix_len + 2);
  char *fname_pos = parser_filename;
  memcpy(fname_pos, parser_dir, parser_dir_len);
  fname_pos += parser_dir_len;
  *fname_pos++ = '/';
  memcpy(fname_pos, parser_filename_prefix, parser_filename_prefix_len);
  fname_pos += parser_filename_prefix_len;
  memcpy(fname_pos, defs_name, defs_name_len);
  fname_pos += defs_name_len;
  memcpy(fname_pos, parser_filename_suffix, parser_filename_suffix_len);
  fname_pos += parser_filename_suffix_len;
  *fname_pos = '\0';

  return parser_filename;
}

struct loaded_or_builtin_parser *get_parser(struct parser_cache *cache,
                                            const char *defs_name,
                                            bool required) {
  ssize_t index = bsearch_find(&cache->loaded_parsers, defs_name);
  if (index >= 0) {
    return cache->loaded_parsers.pairs[index].value;
  }

  char *parser_filename =
      get_definitions_file_name(cache->parser_dir, defs_name);
  struct loaded_parser *defs = load_definitions(defs_name, required);
  free(parser_filename);
  if (defs) {
    struct loaded_or_builtin_parser *parser = malloc(sizeof *parser);
    parser->which = PARSER_TYPE_LOADED;
    parser->loaded = *defs;
    free(defs);
    bsearch_unsafe_insert(&cache->loaded_parsers, index, strdup(defs_name),
                          parser);
    return parser;
  } else {
    // Negative cache entry
    bsearch_unsafe_insert(&cache->loaded_parsers, index, strdup(defs_name),
                          NULL);
    return NULL;
  }
}

struct parse_result execute_parser_parse(
    struct loaded_or_builtin_parser *generic_parser, const char *token,
    size_t token_len) {
  switch (generic_parser->which) {
    case PARSER_TYPE_LOADED: {
      struct loaded_parser *parser = &generic_parser->loaded;
      return parse_from_loaded_parser(parser, token, token_len);
    }
    case PARSER_TYPE_BUILTIN: {
      struct builtin_parser *parser = &generic_parser->builtin;
      return parser->parse(token, token_len);
    }
    default:
      fprintf(stderr, "Unknown parser type\n");
      return (struct parse_result){.type = PARSE_RESULT_FAIL};
  }
}

struct parse_result execute_parser_format(
    struct loaded_or_builtin_parser *generic_parser, uint32_t value,
    struct decompiler_informative_state *decstate) {
  switch (generic_parser->which) {
    case PARSER_TYPE_LOADED: {
      struct loaded_parser *parser = &generic_parser->loaded;
      return format_from_loaded_parser(parser, value);
    }
    case PARSER_TYPE_BUILTIN: {
      struct builtin_parser *parser = &generic_parser->builtin;
      return parser->format(value, decstate);
    }
    default:
      fprintf(stderr, "Unknown parser type\n");
      return (struct parse_result){.type = PARSE_RESULT_FAIL};
  }
}

struct take_parser_state {
  size_t parser_idx;
  size_t prefix_idx;
};

struct loaded_or_builtin_parser *take_parser(
    struct parser_cache *cache, struct language_def *lang,
    struct parser_list parsers, struct take_parser_state *restrict state) {
  while (state->parser_idx < parsers.length) {
    const char *parser_name = parsers.parsers[state->parser_idx].name;
    bool is_prefixed = parsers.parsers[state->parser_idx].is_prefixed;
    state->parser_idx++;

    struct loaded_or_builtin_parser *generic_parser = NULL;

    if (is_prefixed) {
      size_t parser_name_len = strlen(parser_name);
      char **parents = lang->parents;

      for (; state->prefix_idx == 0 || parents[state->prefix_idx - 1];
           state->prefix_idx++) {
        char *prefix;
        if (state->prefix_idx == 0) {
          prefix = lang->name;
        } else {
          prefix = parents[state->prefix_idx - 1];
        }
        size_t prefix_len = strlen(prefix);

        // Concatenate language or parent name with parser name
        char *next_name = malloc(parser_name_len + prefix_len + 2);
        char *next_name_pos = next_name;
        memcpy(next_name, prefix, prefix_len);
        next_name_pos += prefix_len;
        *next_name_pos++ = '_';
        memcpy(next_name_pos, parser_name, parser_name_len);
        next_name_pos += parser_name_len;
        *next_name_pos = '\0';

        // Load or use this parser
        generic_parser = get_parser(cache, next_name, false);
        free(next_name);  // note: already strdup'd
      }
      state->prefix_idx = 0;
    }

    if (generic_parser == NULL) {
      // Try unprefixed parser, even if prefixed was requested
      generic_parser = get_parser(cache, parser_name, true);
    }

    if (generic_parser != NULL) {
      return generic_parser;
    }
  }

  // Default parsers
  if (state->parser_idx == parsers.length) {
    state->parser_idx++;
    return &builtin_parser_hex;
  }
  state->parser_idx = 0;
  state->prefix_idx = 0;
  return &builtin_parser_dec;
}

struct parse_result parse_for_recomp(struct parser_cache *cache,
                                     struct language_def *lang,
                                     struct parser_list parsers, char *token,
                                     size_t token_len) {
  struct parse_result result = {PARSE_RESULT_FAIL};
  struct take_parser_state state = {0, 0};
  struct loaded_or_builtin_parser *generic_parser;

  do {
    generic_parser = take_parser(cache, lang, parsers, &state);
    if (generic_parser != NULL) {
      result = execute_parser_parse(generic_parser, token, token_len);
    }
  } while (generic_parser != NULL && result.type == PARSE_RESULT_FAIL);

  return result;
}

struct parse_result format_for_decomp(
    struct parser_cache *cache, struct language_def *lang,
    struct parser_list parsers, uint32_t value,
    struct decompiler_informative_state *decstate) {
  struct parse_result result = {PARSE_RESULT_FAIL};
  struct take_parser_state state = {0, 0};
  struct loaded_or_builtin_parser *generic_parser;

  do {
    generic_parser = take_parser(cache, lang, parsers, &state);
    if (generic_parser != NULL) {
      result = execute_parser_format(generic_parser, value, decstate);
    }
  } while (generic_parser != NULL && result.type == PARSE_RESULT_FAIL);

  return result;
}

struct parser_cache *create_parser_cache(const char *parser_dir) {
  struct parser_cache *cache = malloc(sizeof *cache);
  cache->parser_dir = strdup(parser_dir);

  bsearch_init_root(&cache->loaded_parsers, bsearch_key_strcmp,
                    bsearch_key_strdup, free,
                    bsearch_destroy_loaded_or_builtin_parser);

  // Populate cache with builtin parsers.
  bsearch_unsafe_append(&cache->loaded_parsers, strdup("dec"),
                        &builtin_parser_dec);
  bsearch_unsafe_append(&cache->loaded_parsers, strdup("hex"),
                        &builtin_parser_hex);

  return cache;
}

void destroy_parser_cache(struct parser_cache *cache) {
  bsearch_deinit_root(&cache->loaded_parsers);
  free(cache->parser_dir);
  free(cache);
}
