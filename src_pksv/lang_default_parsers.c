#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "binarysearch.h"
#include "lang_parsers.h"
#include "lang_default_parsers.h"

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

// TODO
struct loaded_or_builtin_parser builtin_parser_hex = {
  0
};
struct loaded_or_builtin_parser builtin_parser_dec = {
  0
};
struct loaded_or_builtin_parser builtin_parser_address = {
  0
};
struct loaded_or_builtin_parser builtin_parser_condition = {
  0
};
