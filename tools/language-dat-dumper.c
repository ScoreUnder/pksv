#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "binarysearch.h"
#include "lang_load.h"
#include "language-defs.h"

void dump_language(FILE *f, const struct language_def *lang);

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s <language-file> [<output-file>]\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "rb");
  if (f == NULL) {
    perror(argv[1]);
    return 1;
  }

  struct loaded_lang *lang = load_language_from_file(NULL, f, "input");
  fclose(f);

  if (lang == NULL) {
    fprintf(stderr, "Failed to load language from file\n");
    return 1;
  }

  FILE *out = stdout;
  if (argc > 2) {
    out = fopen(argv[2], "wt");
    if (out == NULL) {
      perror(argv[2]);
      return 1;
    }
  }

  dump_language(out, &lang->def);
  fclose(out);
  free_loaded_lang(lang);
  return 0;
}

static int bsearch_key_intptrcmp(const void *a, const void *b) {
  if ((intptr_t)a < (intptr_t)b) return -1;
  if ((intptr_t)a > (intptr_t)b) return 1;
  return 0;
}

void dump_rule(FILE *f, const struct rule *rule);
void dump_string(FILE *f, const char *str);

void dump_language(FILE *f, const struct language_def *lang) {
  char **parents = lang->parents;
  while (*parents != NULL) {
    fprintf(f, "inherit %s\n", *parents++);
  }
  if (lang->parents[0] != NULL) {
    putc('\n', f);
  }

  bool has_meta = false;
  int langtype = lang->meta_flags & METAFLAG_MASK_LANGTYPE;
  if (langtype != 0) {
    has_meta = true;
    switch (langtype) {
      case METAFLAG_LANGTYPE_LINE:
        fprintf(f, "type line\n");
        break;
      case METAFLAG_LANGTYPE_TEXT:
        fprintf(f, "type text\n");
        break;
    }
  }
  if (has_meta) {
    putc('\n', f);
  }

  struct bsearch_root seen_rules;
  bsearch_init_root(&seen_rules, bsearch_key_intptrcmp, bsearch_key_nocopy,
                    NULL, NULL);

  for (size_t i = 0; i < NUM_SPECIAL_RULES; i++) {
    struct rule *rule = lang->special_rules[i];
    if (rule == NULL) continue;
    ptrdiff_t index = bsearch_find(&seen_rules, rule);
    if (index >= 0) continue;
    bsearch_unsafe_insert(&seen_rules, index, rule, NULL);
    dump_rule(f, rule);
    putc('\n', f);
  }

  for (size_t i = 0; i < lang->rules_by_bytes->size; i++) {
    struct rule *rule = lang->rules_by_bytes->pairs[i].value;
    ptrdiff_t index = bsearch_find(&seen_rules, rule);
    if (index >= 0) continue;
    bsearch_unsafe_insert(&seen_rules, index, rule, NULL);
    dump_rule(f, rule);
    putc('\n', f);
  }

  for (size_t i = 0; i < lang->rules_by_command_name->size; i++) {
    struct rule *rule = lang->rules_by_command_name->pairs[i].value;
    ptrdiff_t index = bsearch_find(&seen_rules, rule);
    if (index >= 0) continue;
    bsearch_unsafe_insert(&seen_rules, index, rule, NULL);
    dump_rule(f, rule);
    putc('\n', f);
  }

  if (lang->lookup_bytes != NULL) {
    char buf[2];
    uint8_t *decomp_table = lang->lookup_bytes->decomp;
    for (size_t i = 0; i < 0x100; i++) {
      uint8_t val = decomp_table[i];
      if (val == 0) continue;

      buf[0] = val;
      buf[1] = '\0';

      fprintf(f, "[0x%02x] = ", (uint8_t)i);
      dump_string(f, buf);
      putc('\n', f);
    }

    uint8_t *recomp_table = lang->lookup_bytes->recomp;
    for (size_t i = 0; i < 0x100; i++) {
      uint8_t val = recomp_table[i];
      if (val == 0) continue;
      if (decomp_table[val] == i) continue;

      buf[0] = (uint8_t)i;
      buf[1] = '\0';

      fprintf(f, "[0x%02x] = ", val);
      dump_string(f, buf);
      putc('\n', f);
    }
  }
}

void dump_rule(FILE *f, const struct rule *rule) {
  putc('[', f);
  for (size_t i = 0; i < rule->bytes.length; i++) {
    if (i != 0) putc(' ', f);
    fprintf(f, "0x%02x", rule->bytes.bytes[i]);
  }
  fputs("] = ", f);

  dump_string(f, rule->command_name);
  if (rule->attributes & RULE_ATTR_END) {
    fputs("+end", f);
  }
  if (rule->attributes & RULE_ATTR_TERMINATOR) {
    fputs("+terminator", f);
  }
  if (rule->attributes & RULE_ATTR_NO_TERMINATE) {
    fputs("+no_terminate", f);
  }
  if (rule->attributes & RULE_ATTR_DEFAULT) {
    fputs("+default", f);
  }
  if (rule->attributes & RULE_ATTR_CMP_FLAG) {
    fputs("+cmp_flag", f);
  }
  if (rule->attributes & RULE_ATTR_CMP_INT) {
    fputs("+cmp_int", f);
  }
  if (rule->attributes & RULE_ATTR_BREAK) {
    fputs("+break", f);
  }
  if (rule->attributes & RULE_ATTR_PREFER_BREAK) {
    fputs("+prefer_break", f);
  }

  for (size_t i = 0; i < rule->args.length; i++) {
    struct command_arg *arg = &rule->args.args[i];
    fprintf(f, " %zu", arg->size);

    struct parser_list *parsers = &arg->parsers;
    for (size_t j = 0; j < parsers->length; j++) {
      putc(':', f);
      if (parsers->parsers[j].is_prefixed) {
        putc('*', f);
      }
      dump_string(f, parsers->parsers[j].name);
    }

    bool has_language = arg->as_language.lang.name[0] != '\0' ||
                        arg->as_language.lang.is_prefixed;
    bool has_command = arg->as_language.command[0] != '\0';
    if (has_language || has_command) {
      putc('@', f);
      if (arg->as_language.lang.is_prefixed) {
        putc('*', f);
      }
      if (arg->as_language.lang.name[0] != '\0') {
        dump_string(f, arg->as_language.lang.name);
      }
      if (has_command && has_language) {
        putc(':', f);
      }
      if (has_command) {
        fputs("cmd=", f);
        dump_string(f, arg->as_language.command);
      }
    }
  }

  if (rule->oneshot_lang.name[0] != '\0' || rule->oneshot_lang.is_prefixed) {
    fputs(" !", f);
    if (rule->oneshot_lang.is_prefixed) {
      putc('*', f);
    }
    dump_string(f, rule->oneshot_lang.name);
  }
}

void dump_string(FILE *f, const char *str) {
  bool need_quotes = false;
  if (str[0] != '.' && !isalpha(str[0])) {
    need_quotes = true;
  }
  if (str[0] != '\0') {
    for (size_t i = 1; str[i] != '\0' && !need_quotes; i++) {
      if (!isalnum(str[i]) && str[i] != '_' && str[i] != '.') {
        need_quotes = true;
        break;
      }
    }
  }

  if (!need_quotes) {
    fputs(str, f);
  } else {
    putc('"', f);
    for (size_t i = 0; str[i] != '\0'; i++) {
      if (strchr("\\\"\n", str[i]) != NULL) {
        putc('\\', f);
      }
      putc(str[i], f);
    }
    putc('"', f);
  }
}
