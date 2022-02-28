#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_load.h"
#include "language-defs.h"
#include "binarysearch.h"
#include "stdio_ext.h"

struct language_cache {
  struct bsearch_root languages;
  char *dir;  // directory containing language files
};

struct loaded_lang {
  struct language_def def;
  char **string_table;
};

static int bsearch_key_cmp_bytes(const void *a, const void *b) {
  const struct bytes_list *al = a;
  const struct bytes_list *bl = b;
  int result = memcmp(al->bytes, bl->bytes,
                      al->length < bl->length ? al->length : bl->length);
  if (result == 0) {
    if (al->length < bl->length) {
      result = -1;
    } else if (al->length > bl->length) {
      result = 1;
    }
  }
  return result;
}

static void *bsearch_key_dup_bytes(const void *a) {
  const struct bytes_list *bytes = a;
  struct bytes_list *bytes2 = malloc(sizeof *bytes2);
  bytes2->bytes = malloc(bytes->length);
  bytes2->length = bytes->length;
  memcpy(bytes2->bytes, bytes->bytes, bytes->length);
  return bytes2;
}

static void bsearch_free_bytes(void *a) {
  struct bytes_list *bytes = a;
  free(bytes->bytes);
  free(bytes);
}

static void bsearch_free_language_rule(void *a) {
  // Note: all strings are pooled; do not free them here.
  struct rule *rule = a;
  free(rule->bytes.bytes);
  for (size_t i = 0; i < rule->args.length; i++) {
    free(rule->args.args[i].parsers.parsers);
  }
  free(rule->args.args);
  free(rule);
}

__attribute__((malloc)) static struct rule *dup_language_rule(
    struct rule *rule) {
  // Note: all strings are pooled; can just have pointers copied.
  struct rule *rule2 = malloc(sizeof *rule2);
  rule2->bytes.length = rule->bytes.length;
  rule2->bytes.bytes = malloc(rule->bytes.length);
  memcpy(rule2->bytes.bytes, rule->bytes.bytes, rule->bytes.length);

  rule2->args.length = rule->args.length;
  rule2->args.args = malloc(rule->args.length * sizeof *rule2->args.args);
  for (size_t i = 0; i < rule->args.length; i++) {
    struct command_arg *arg = &rule->args.args[i];
    struct command_arg *arg2 = &rule2->args.args[i];

    arg2->size = arg->size;
    arg2->as_language = arg->as_language;

    arg2->parsers.length = arg->parsers.length;
    arg2->parsers.parsers =
        malloc(arg->parsers.length * sizeof *arg2->parsers.parsers);
    memcpy(arg2->parsers.parsers, arg->parsers.parsers,
           arg->parsers.length * sizeof *arg2->parsers.parsers);
  }

  rule2->oneshot_lang = rule->oneshot_lang;
  rule2->command_name = rule->command_name;
  rule2->attributes = rule->attributes;
  return rule2;
}

static void free_loaded_lang(struct loaded_lang *lang) {
  free(lang->def.name);
  free(lang->def.parents);
  for (size_t i = 0; i < NUM_SPECIAL_RULES; i++) {
    free(lang->def.special_rules[i]);
  }
  bsearch_destroy_root(lang->def.rules_by_bytes);
  bsearch_destroy_root(lang->def.rules_by_command_name);

  char **stringp = lang->string_table;
  while (*stringp) free(*stringp++);
  free(lang->string_table);

  free(lang);
}

static void bsearch_free_loaded_language(void *data) {
  if (data != NULL) free_loaded_lang(data);
}

static void *void_identity(const void *data) { return (void *)data; }

__attribute__((malloc)) static char *get_lang_filename(const char *dir,
                                                       const char *name) {
  static const char *filename_prefix = "lang_";
  static const char *filename_suffix = ".dat";
  size_t dir_len = strlen(dir);
  size_t name_len = strlen(name);
  size_t filename_prefix_len = strlen(filename_prefix);
  size_t filename_suffix_len = strlen(filename_suffix);

  char *filename = malloc(dir_len + name_len + strlen(filename_prefix) +
                          strlen(filename_suffix) + 2);
  char *pos = filename;
  memcpy(pos, dir, dir_len);
  pos += dir_len;
  *pos++ = '/';
  memcpy(pos, filename_prefix, filename_prefix_len);
  pos += filename_prefix_len;
  memcpy(pos, name, name_len);
  pos += name_len;
  memcpy(pos, filename_suffix, filename_suffix_len);
  pos += filename_suffix_len;
  *pos = '\0';

  return filename;
}

static char *fgettabledstr(char **string_table, size_t string_table_size,
                           FILE *f) {
  uint32_t index = fgetvarint(f);
  if (index >= string_table_size) {
    fprintf(stderr, "Language file contained invalid string index %u\n", index);
    return NULL;
  }
  return string_table[index];
}

static void read_language_name(char **string_table, size_t string_table_len,
                               struct language *lang, FILE *file) {
  lang->name = fgettabledstr(string_table, string_table_len, file);
  lang->is_prefixed = getc(file);
}

void overwrite_special_rule(struct language_def *dest, struct rule *src,
                            size_t type) {
  assert(type < NUM_SPECIAL_RULES);
  if (dest->special_rules[type] != NULL) {
    bsearch_free_language_rule(dest->special_rules[type]);
  }
  dest->special_rules[type] = dup_language_rule(src);
}

static struct loaded_lang *load_language(struct language_cache *cache,
                                         const char *dir, const char *name) {
  char *filename = get_lang_filename(dir, name);
  FILE *file = fopen(filename, "rb");
  free(filename);
  if (!file) {
    return NULL;
  }

  struct loaded_lang *lang = malloc(sizeof *lang);
  lang->def.name = strdup(name);

  size_t string_table_len = fgetvarint(file);
  lang->string_table =
      malloc(sizeof *lang->string_table * (string_table_len + 1));
  for (size_t i = 0; i < string_table_len; i++) {
    lang->string_table[i] = fgetstr(file);
  }
  lang->string_table[string_table_len] = NULL;

  lang->def.meta_flags = getc(file);

  size_t parents_len = fgetvarint(file);
  lang->def.parents = malloc(sizeof *lang->def.parents * (parents_len + 1));
  for (size_t i = 0; i < parents_len; i++) {
    lang->def.parents[i] =
        fgettabledstr(lang->string_table, string_table_len, file);
  }

  size_t rules_len = fgetvarint(file);
  lang->def.rules_by_bytes =
      bsearch_create_root(bsearch_key_cmp_bytes, bsearch_key_dup_bytes,
                          bsearch_free_bytes, bsearch_free_language_rule);
  lang->def.rules_by_command_name = bsearch_create_root(
      bsearch_key_strcmp, void_identity, NULL, bsearch_free_language_rule);

  // Null all elements of special rules array.
  memset(&lang->def.special_rules, 0, sizeof lang->def.special_rules);

  // Insert parent rules
  for (size_t i = 0; i < parents_len; i++) {
    const struct language_def *parent =
        get_language(cache, lang->def.parents[i]);
    if (!parent) {
      fprintf(stderr,
              "Error: could not find parent language \"%s\" of \"%s\"\n",
              lang->def.parents[i], name);

      free_loaded_lang(lang);
      return NULL;
    }

    for (size_t j = 0; j < parent->rules_by_bytes->size; j++) {
      struct rule *rule = parent->rules_by_bytes->pairs[j].value;
      bsearch_upsert(lang->def.rules_by_bytes, rule->bytes.bytes,
                     dup_language_rule(rule));
    }

    for (size_t j = 0; j < parent->rules_by_command_name->size; j++) {
      struct rule *rule = parent->rules_by_command_name->pairs[j].value;
      bsearch_upsert(lang->def.rules_by_command_name, rule->command_name,
                     dup_language_rule(rule));
    }

    for (size_t j = 0; j < NUM_SPECIAL_RULES; j++) {
      if (parent->special_rules[j]) {
        overwrite_special_rule(&lang->def, parent->special_rules[j], j);
      }
    }
  }

  for (size_t i = 0; i < rules_len; i++) {
    struct rule *rule = malloc(sizeof *rule);
    rule->attributes = getc(file);
    rule->bytes.length = fgetvarint(file);
    rule->bytes.bytes = malloc(rule->bytes.length);
    fread(rule->bytes.bytes, 1, rule->bytes.length, file);
    rule->command_name =
        fgettabledstr(lang->string_table, string_table_len, file);
    read_language_name(lang->string_table, string_table_len,
                       &rule->oneshot_lang, file);

    size_t args_len = fgetvarint(file);
    rule->args.args = malloc(sizeof *rule->args.args * args_len);
    rule->args.length = args_len;
    for (size_t j = 0; j < args_len; j++) {
      struct command_arg *arg = &rule->args.args[j];
      arg->size = getc(file);

      size_t parsers_len = fgetvarint(file);
      arg->parsers.parsers = malloc(sizeof *arg->parsers.parsers * parsers_len);
      arg->parsers.length = parsers_len;
      for (size_t k = 0; k < parsers_len; k++) {
        read_language_name(lang->string_table, string_table_len,
                           &arg->parsers.parsers[k], file);
      }

      arg->as_language.type = getc(file);
      switch (arg->as_language.type) {
        case LC_TYPE_LANG:
          read_language_name(lang->string_table, string_table_len,
                             &arg->as_language.lang, file);
          break;
        case LC_TYPE_COMMAND:
          arg->as_language.command =
              fgettabledstr(lang->string_table, string_table_len, file);
          break;
      }
    }

    if (rule->attributes & RULE_ATTR_DEFAULT) {
      overwrite_special_rule(&lang->def, rule, SPECIAL_RULE_DEFAULT);
    }
    if (rule->attributes & RULE_ATTR_TERMINATOR) {
      overwrite_special_rule(&lang->def, rule, SPECIAL_RULE_TERMINATOR);
    }
    if (rule->attributes & RULE_ATTR_NO_TERMINATE) {
      overwrite_special_rule(&lang->def, rule, SPECIAL_RULE_NO_TERMINATE);
    }

    bsearch_upsert(lang->def.rules_by_bytes, &rule->bytes,
                   dup_language_rule(rule));
    bsearch_upsert(lang->def.rules_by_command_name, rule->command_name, rule);
  }

  return lang;
}

struct language_cache *create_language_cache(const char *dir) {
  struct language_cache *cache = malloc(sizeof *cache);
  bsearch_init_root(&cache->languages, bsearch_key_strcmp, bsearch_key_strdup,
                    free, bsearch_free_loaded_language);
  cache->dir = strdup(dir);
  return cache;
}

const struct language_def *get_language(struct language_cache *cache,
                                        const char *name) {
  ssize_t index = bsearch_find(&cache->languages, name);
  if (index < 0) {
    bsearch_upsert(&cache->languages, name,
                   NULL);  // Disallow infinite recursion
    struct loaded_lang *lang = load_language(cache, cache->dir, name);
    if (!lang) return NULL;
    bsearch_upsert(&cache->languages, name, lang);
    return &lang->def;
  }
  struct loaded_lang *lang = cache->languages.pairs[index].value;
  if (lang == NULL) return NULL;
  return &lang->def;
}

void destroy_language_cache(struct language_cache *cache) {
  bsearch_deinit_root(&cache->languages);
  free(cache->dir);
  free(cache);
}
