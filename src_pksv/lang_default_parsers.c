#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "binarysearch.h"
#include "lang_parsers.h"
#include "lang_default_parsers.h"
#include "textutil.h"

struct parse_result parse_from_loaded_parser(struct loaded_parser *parser,
                                             const char *token,
                                             size_t token_len) {
  char *s = malloc(token_len + 1);
  memcpy(s, token, token_len);
  s[token_len] = '\0';

  ssize_t index = bsearch_find(&parser->lookup_by_name, s);
  free(s);

  if (index >= 0) {
    return (struct parse_result){
        .type = PARSE_RESULT_VALUE,
        .value = (uint32_t)(intptr_t)parser->lookup_by_name.pairs[index].value,
    };
  } else {
    return (struct parse_result){
        .type = PARSE_RESULT_FAIL,
    };
  }
}

struct parse_result format_from_loaded_parser(struct loaded_parser *parser,
                                              uint32_t value) {
  ssize_t index = bsearch_find(&parser->lookup_by_id, (void *)(intptr_t)value);

  if (index >= 0) {
    return (struct parse_result){
        .type = PARSE_RESULT_TOKEN,
        .token = parser->lookup_by_id.pairs[index].value,
    };
  } else {
    return (struct parse_result){
        .type = PARSE_RESULT_FAIL,
    };
  }
}

struct parse_result default_parse_hex(const char *token, size_t token_len) {
  if ((token_len > 2 && token[0] == '0' &&
       (token[1] == 'x' || token[1] == 'X')) ||
      (token_len > 1 && token[0] == '$')) {
    uint32_t result;
    const char *start = token + (token[0] == '$' ? 1 : 2);
    const char *end = hex_to_uint32(start, token_len, &result);
    if (end == token + token_len) {
      return (struct parse_result){
          .type = PARSE_RESULT_VALUE,
          .value = result,
      };
    }
  }
  return (struct parse_result){
      .type = PARSE_RESULT_FAIL,
  };
}

struct parse_result default_format_hex(
    uint32_t value, struct decompiler_informative_state *state) {
  (void)state;

  char *s = malloc(16);
  sprintf(s, "%#x", value);
  return (struct parse_result){
      .type = PARSE_RESULT_TOKEN,
      .token = s,
  };
}

struct parse_result default_parse_dec(const char *token, size_t token_len) {
  uint32_t result;
  const char *end = dec_to_uint32(token, token_len, &result);
  if (end == token + token_len) {
    return (struct parse_result){
        .type = PARSE_RESULT_VALUE,
        .value = result,
    };
  }
  return (struct parse_result){
      .type = PARSE_RESULT_FAIL,
  };
}

struct parse_result default_format_dec(
    uint32_t value, struct decompiler_informative_state *state) {
  (void)state;

  char *s = malloc(16);
  sprintf(s, "%u", value);
  return (struct parse_result){
      .type = PARSE_RESULT_TOKEN,
      .token = s,
  };
}

struct loaded_or_builtin_parser builtin_parser_hex = {
    .which = PARSER_TYPE_BUILTIN,
    .builtin =
        {
            .parse = default_parse_hex,
            .format = default_format_hex,
        },
};

struct loaded_or_builtin_parser builtin_parser_dec = {
    .which = PARSER_TYPE_BUILTIN,
    .builtin =
        {
            .parse = default_parse_dec,
            .format = default_format_dec,
        },
};

// TODO
struct loaded_or_builtin_parser builtin_parser_address = {0};

struct loaded_or_builtin_parser builtin_parser_condition = {0};
