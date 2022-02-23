#ifndef PKSV_LANGUAGE_DEFS_H
#define PKSV_LANGUAGE_DEFS_H 1

#include <stdint.h>
#include <stdbool.h>

#define LC_TYPE_NONE 0
#define LC_TYPE_LANG 1
#define LC_TYPE_COMMAND 2

#define RULE_ATTR_NONE 0
#define RULE_ATTR_END 1
#define RULE_ATTR_TERMINATOR 2
#define RULE_ATTR_NO_TERMINATE 4

#define METAFLAG_NONE 0
#define METAFLAG_MASK_LANGTYPE 3
#define METAFLAG_LANGTYPE_LINE 1
#define METAFLAG_LANGTYPE_TEXT 2

struct bytes_list {
  const uint8_t *bytes;
  size_t length;
};

struct language {
  const char *name;
  bool is_prefixed;
};

struct parser_list {
  const struct language *parsers;
  size_t length;
};

struct lang_or_cmd {
  int type;
  union {
    struct language lang;
    const char *command;
  };
};

struct command_arg {
  size_t size;
  struct parser_list parsers;
  struct lang_or_cmd as_language;
};

struct command_args_list {
  const struct command_arg *args;
  size_t length;
};

struct rule {
  struct bytes_list bytes;
  struct command_args_list args;
  struct language oneshot_lang;
  const char *command_name;
  uint8_t attributes;
};

struct language_def {
  const struct bsearch_root *rules_by_bytes;
  const struct bsearch_root *rules_by_command_name;
  const char **parents;
  uint8_t meta_flags;
};

#endif
