#ifndef PKSV_LANGUAGE_DEFS_BUILDER_H
#define PKSV_LANGUAGE_DEFS_BUILDER_H 1

#include <stdint.h>
#include <stdbool.h>

#include "language-defs.h"

struct meta {
  const char *name;
  const char *value;
};

struct bytes_list_builder {
  uint8_t *bytes;
  size_t length;
  size_t capacity;
};

struct parser_list_builder {
  struct language *parsers;
  size_t length;
  size_t capacity;
};

struct command_arg_builder {
  size_t size;
  struct parser_list_builder parsers;
  struct lang_or_cmd as_language;
};

struct command_args_list_builder {
  struct command_arg_builder *args;
  size_t length;
  size_t capacity;
};

struct rule_builder {
  struct bytes_list_builder bytes;
  struct command_args_list_builder args;
  struct language oneshot_lang;
  const char *command_name;
  uint8_t attributes;
};

struct meta_list_builder {
  struct meta *metas;
  size_t length;
  size_t capacity;
};

struct rule_list_builder {
  struct rule_builder *rules;
  size_t length;
  size_t capacity;
};

struct language_def_builder {
  struct meta_list_builder metas;
  struct rule_list_builder rules;
};

#endif
