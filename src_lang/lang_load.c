#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_load.h"
#include "language-defs.h"
#include "binarysearch.h"
#include "stdio_ext.h"
#include "textutil.h"

struct language_cache {
  struct bsearch_root languages;
  char *dir;  // directory containing language files
};

__attribute__((malloc)) void *malloc_or_null(size_t size) {
  if (size == 0) return NULL;
  return malloc(size);
}

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
  bytes2->bytes = malloc_or_null(bytes->length);
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
  if (rule->refcnt-- == 1) {
    free(rule->bytes.bytes);
    for (size_t i = 0; i < rule->args.length; i++) {
      free(rule->args.args[i].parsers.parsers);
    }
    free(rule->args.args);
    free(rule);
  }
}

static struct rule *ref_language_rule(struct rule *rule) {
  assert(rule->refcnt != UINT8_MAX);
  rule->refcnt++;
  return rule;
}

void free_loaded_lang(struct loaded_lang *lang) {
  free(lang->def.name);
  free(lang->def.parents);
  for (size_t i = 0; i < NUM_SPECIAL_RULES; i++) {
    if (lang->def.special_rules[i] != NULL) {
      bsearch_free_language_rule(lang->def.special_rules[i]);
    }
  }
  bsearch_destroy_root(lang->def.rules_by_bytes);
  bsearch_destroy_root(lang->def.rules_by_command_name);
  if ((lang->def.meta_flags & METAFLAG_MASK_LANGTYPE) ==
      METAFLAG_LANGTYPE_TEXT) {
    free(lang->def.lookup_bytes);
  }

  char **stringp = lang->string_table;
  if (stringp)
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
  dest->special_rules[type] = ref_language_rule(src);
}

static bool is_simple_rule(const struct rule *rule) {
  return rule->bytes.length == 1 &&
         rule->bytes.bytes[0] != 0 /* 0 is reserved */
         && rule->args.length == 0 && rule->oneshot_lang.name[0] == '\0' &&
         strlen(rule->command_name) == 1 && rule->attributes == 0;
}

static bool load_rule_args_from_file(FILE *file, struct loaded_lang *lang,
                                     struct rule *rule,
                                     size_t string_table_len);

#define REASONABLY_LARGE (1024 * 1024)

struct loaded_lang *load_language_from_file(struct language_cache *cache,
                                            FILE *file, const char *name) {
  struct loaded_lang *lang = malloc(sizeof *lang);
  lang->def.name = strdup(name);

  // Initialise things early that affect how the language is freed.
  lang->def.parents = NULL;
  lang->string_table = NULL;
  lang->def.meta_flags = 0;
  memset(lang->def.special_rules, 0, sizeof lang->def.special_rules);

  lang->def.rules_by_bytes =
      bsearch_create_root(bsearch_key_cmp_bytes, bsearch_key_dup_bytes,
                          bsearch_free_bytes, bsearch_free_language_rule);
  lang->def.rules_by_command_name = bsearch_create_root(
      bsearch_key_strcmp, void_identity, NULL, bsearch_free_language_rule);

  size_t string_table_len = fgetvarint(file);
  if (string_table_len > REASONABLY_LARGE) {
    fprintf(stderr, "Language file contained too many strings\n");
    goto error;
  }
  lang->string_table =
      malloc(sizeof *lang->string_table * (string_table_len + 1));
  memset(lang->string_table, 0,
         sizeof *lang->string_table * (string_table_len + 1));
  for (size_t i = 0; i < string_table_len; i++) {
    lang->string_table[i] = fngetstr(file, REASONABLY_LARGE);
    if (lang->string_table[i] == NULL || feof(file) || ferror(file)) goto error;
  }

  lang->def.meta_flags = getc(file);

  int langtype = lang->def.meta_flags & METAFLAG_MASK_LANGTYPE;
  if (langtype == METAFLAG_LANGTYPE_TEXT) {
    lang->def.lookup_bytes = malloc(sizeof(struct byte_lookup_tables));
    memset(lang->def.lookup_bytes, 0, sizeof(struct byte_lookup_tables));
  }

  size_t parents_len = fgetvarint(file);
  if (parents_len > REASONABLY_LARGE) {
    fprintf(stderr, "Language file contained too many parents\n");
    goto error;
  }
  lang->def.parents = malloc(sizeof *lang->def.parents * (parents_len + 1));
  for (size_t i = 0; i < parents_len; i++) {
    lang->def.parents[i] =
        fgettabledstr(lang->string_table, string_table_len, file);
    if (lang->def.parents[i] == NULL) goto error;
  }
  lang->def.parents[parents_len] = NULL;

  size_t rules_len = fgetvarint(file);
  if (rules_len > REASONABLY_LARGE) {
    fprintf(stderr, "Language file contained too many rules\n");
    goto error;
  }

  // Null all elements of special rules array.
  memset(&lang->def.special_rules, 0, sizeof lang->def.special_rules);

  if (cache != NULL) {
    // Insert parent rules
    for (size_t i = 0; i < parents_len; i++) {
      const struct language_def *parent =
          get_language(cache, lang->def.parents[i]);
      if (!parent) {
        fprintf(stderr,
                "Error: could not find parent language \"%s\" of \"%s\"\n",
                lang->def.parents[i], name);

        goto error;
      }

      bsearch_ensure_capacity(
          lang->def.rules_by_bytes,
          lang->def.rules_by_bytes->size + parent->rules_by_bytes->size);
      bsearch_ensure_capacity(lang->def.rules_by_command_name,
                              lang->def.rules_by_command_name->size +
                                  parent->rules_by_command_name->size);

      for (size_t j = 0; j < parent->rules_by_bytes->size; j++) {
        struct rule *rule = parent->rules_by_bytes->pairs[j].value;
        bsearch_upsert(lang->def.rules_by_bytes, rule->bytes.bytes,
                       ref_language_rule(rule));
      }

      for (size_t j = 0; j < parent->rules_by_command_name->size; j++) {
        struct rule *rule = parent->rules_by_command_name->pairs[j].value;
        bsearch_upsert(lang->def.rules_by_command_name, rule->command_name,
                       ref_language_rule(rule));
      }

      for (size_t j = 0; j < NUM_SPECIAL_RULES; j++) {
        if (parent->special_rules[j]) {
          overwrite_special_rule(&lang->def, parent->special_rules[j], j);
        }
      }
    }
  }

  bsearch_ensure_capacity(lang->def.rules_by_bytes,
                          lang->def.rules_by_bytes->size + rules_len);
  bsearch_ensure_capacity(lang->def.rules_by_command_name,
                          lang->def.rules_by_command_name->size + rules_len);

  for (size_t i = 0; i < rules_len; i++) {
    if (feof(file)) {
      fprintf(stderr,
              "Error: unexpected end of file while reading language "
              "file\n");
      goto error;
    }

    struct rule *rule = malloc(sizeof *rule);
    rule->args.args = NULL;
    rule->args.length = 0;
    rule->bytes.bytes = NULL;

    rule->refcnt = 1;
    rule->attributes = getc(file);
    rule->bytes.length = fgetvarint(file);
    if (rule->bytes.length > REASONABLY_LARGE) {
      fprintf(stderr, "Rule contained too many bytes\n");
      goto error;
    }
    rule->bytes.bytes = malloc_or_null(rule->bytes.length);
    fread(rule->bytes.bytes, 1, rule->bytes.length, file);
    rule->command_name =
        fgettabledstr(lang->string_table, string_table_len, file);
    if (rule->command_name == NULL) {
    rule_error:
      for (size_t i = 0; i < rule->args.length; i++) {
        free(rule->args.args[i].parsers.parsers);
      }
      free(rule->args.args);
      free(rule->bytes.bytes);
      free(rule);
      goto error;
    }
    read_language_name(lang->string_table, string_table_len,
                       &rule->oneshot_lang, file);
    if (rule->oneshot_lang.name == NULL) goto error;

    if (!load_rule_args_from_file(file, lang, rule, string_table_len)) {
      goto rule_error;
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

    if (langtype == METAFLAG_LANGTYPE_TEXT && is_simple_rule(rule)) {
      struct byte_lookup_tables *lookup = lang->def.lookup_bytes;
      lookup->decomp[rule->bytes.bytes[0]] = rule->command_name[0];
      lookup->recomp[(uint8_t)rule->command_name[0]] = rule->bytes.bytes[0];
      bsearch_free_language_rule(rule);
    } else {
      if (rule->bytes.length > 0) {
        bsearch_upsert(lang->def.rules_by_bytes, &rule->bytes,
                       ref_language_rule(rule));
      }
      if (rule->command_name[0] != '\0') {
        bsearch_upsert(lang->def.rules_by_command_name, rule->command_name,
                       rule);
      } else {
        // Unref "master" copy once
        // If it wasn't inserted into the special rules tables or the
        // rule-by-bytes tables, this will completely delete it.
        bsearch_free_language_rule(rule);
      }
    }
  }

  bsearch_trim_capacity(lang->def.rules_by_bytes);
  bsearch_trim_capacity(lang->def.rules_by_command_name);

  return lang;

error:
  fprintf(stderr, "Error: failed to load language file for \"%s\"\n", name);
  free_loaded_lang(lang);
  return NULL;
}

static bool load_rule_args_from_file(FILE *file, struct loaded_lang *lang,
                                     struct rule *rule,
                                     size_t string_table_len) {
  size_t args_len = fgetvarint(file);
  if (feof(file) || ferror(file)) {
    return false;
  }
  if (args_len > REASONABLY_LARGE) {
    fprintf(stderr, "Language file contained too many arguments\n");
    return false;
  }

  rule->args.args = malloc_or_null(sizeof *rule->args.args * args_len);
  rule->args.length = args_len;

  size_t i;
  bool ok = true;

  for (i = 0; i < args_len; i++) {
    struct command_arg *arg = &rule->args.args[i];
    arg->size = getc(file);

    size_t parsers_len = fgetvarint(file);
    if (parsers_len > REASONABLY_LARGE) {
      fprintf(stderr, "Language file contained too many parsers\n");
      ok = false;
      break;
    }
    arg->parsers.parsers =
        malloc_or_null(sizeof *arg->parsers.parsers * parsers_len);
    arg->parsers.length = parsers_len;
    for (size_t k = 0; k < parsers_len; k++) {
      read_language_name(lang->string_table, string_table_len,
                         &arg->parsers.parsers[k], file);
      if (arg->parsers.parsers[k].name == NULL) {
        ok = false;
        free(arg->parsers.parsers);
        break;
        ;
      }
    }
    if (!ok) break;

    read_language_name(lang->string_table, string_table_len,
                       &arg->as_language.lang, file);
    if (arg->as_language.lang.name == NULL) {
      ok = false;
      free(arg->parsers.parsers);
      break;
    }

    arg->as_language.command =
        fgettabledstr(lang->string_table, string_table_len, file);
    if (arg->as_language.command == NULL) {
      ok = false;
      free(arg->parsers.parsers);
      break;
    }
  }

  if (!ok) {
    for (size_t j = 0; j < i; j++) {
      free(rule->args.args[j].parsers.parsers);
    }
    rule->args.length = 0;
  }

  return ok;
}

static struct loaded_lang *load_language(struct language_cache *cache,
                                         const char *dir, const char *name) {
  char *filename = get_lang_filename(dir, name);
  FILE *file = fopen(filename, "rb");
  free(filename);
  if (!file) {
    return NULL;
  }

  struct loaded_lang *lang = load_language_from_file(cache, file, name);

  fclose(file);
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
  ptrdiff_t index = bsearch_find(&cache->languages, name);
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
