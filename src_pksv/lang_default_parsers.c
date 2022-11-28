#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "binarysearch_u32.h"
#include "binarysearch.h"
#include "lang_decompiler_types.h"
#include "lang_default_parsers.h"
#include "textutil.h"

struct parse_result parse_from_loaded_parser(struct loaded_parser *parser,
                                             const char *token,
                                             size_t token_len) {
  char *s = malloc(token_len + 1);
  memcpy(s, token, token_len);
  s[token_len] = '\0';

  ptrdiff_t index = bsearch_find(&parser->lookup_by_name, s);
  free(s);

  if (index >= 0) {
    return (struct parse_result){
        .type = PARSE_RESULT_VALUE,
        .value = bsearch_val_u32(&parser->lookup_by_name, index),
    };
  } else {
    return (struct parse_result){
        .type = PARSE_RESULT_FAIL,
    };
  }
}

struct parse_result format_from_loaded_parser(struct loaded_parser *parser,
                                              uint32_t value) {
  ptrdiff_t index = bsearch_find_u32(&parser->lookup_by_id, value);

  if (index >= 0) {
    return (struct parse_result){
        .type = PARSE_RESULT_TOKEN,
        .token = strdup(parser->lookup_by_id.pairs[index].value),
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
    end = hex_to_int32(start, token_len, &result);
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
  end = dec_to_int32(token, token_len, &result);
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

struct parse_result default_parse_address(const char *token, size_t token_len) {
  if (token_len >= 2 && (token[0] == '@' || token[0] == ':')) {
    char *s = malloc(token_len + 1);
    memcpy(s, token, token_len);
    s[token_len] = '\0';
    return (struct parse_result){
        .type = PARSE_RESULT_LABEL,
        .label = s,
    };
  }
  return (struct parse_result){
      .type = PARSE_RESULT_FAIL,
  };
}

struct parse_result default_format_address(
    uint32_t value, struct decompiler_informative_state *state) {
  (void)state;

  return (struct parse_result){
      .type = PARSE_RESULT_LABEL,
      .value = value,
  };
}

struct parse_result default_parse_condition(const char *token,
                                            size_t token_len) {
  int value = -1;
  if (token_len >= 1) {
    switch (token[0]) {
      case '<':
        if (token_len == 1)
          value = 0;
        else if (token_len == 2) {
          if (token[1] == '=')
            value = 3;
          else if (token[1] == '>')
            value = 5;
        }
        break;
      case '=':
        if (token_len == 1 || (token_len == 2 && token[1] == '=')) value = 1;
        break;
      case '>':
        if (token_len == 1)
          value = 2;
        else if (token_len == 2 && token[1] == '=')
          value = 4;
        break;
      case '!':
        if (token_len == 2 && token[1] == '=') value = 5;
        break;
      default:
        if (token_len == 4 && strncmp(token, "true", 4) == 0) {
          value = 1;
        } else if (token_len == 5 && strncmp(token, "false", 5) == 0) {
          value = 0;
        }
        break;
    }
  }
  if (value == -1) {
    return (struct parse_result){
        .type = PARSE_RESULT_FAIL,
    };
  } else {
    return (struct parse_result){
        .type = PARSE_RESULT_VALUE,
        .value = value,
    };
  }
}

struct parse_result default_format_condition(
    uint32_t value, struct decompiler_informative_state *state) {
  char *token = NULL;

  switch (value) {
    case 0:
      token = strdup(state->is_checkflag ? "false" : "<");
      break;
    case 1:
      token = strdup(state->is_checkflag ? "true" : "=");
      break;
    case 2:
      token = strdup(">");
      break;
    case 3:
      token = strdup("<=");
      break;
    case 4:
      token = strdup(">=");
      break;
    case 5:
      token = strdup("!=");
      break;
  }

  if (token == NULL) {
    return (struct parse_result){
        .type = PARSE_RESULT_FAIL,
    };
  } else {
    return (struct parse_result){
        .type = PARSE_RESULT_TOKEN,
        .token = token,
    };
  }
}

struct loaded_or_builtin_parser builtin_parser_hex = {
    .which = PARSER_TYPE_BUILTIN,
    .builtin =
        {
            .name = "hex",
            .parse = default_parse_hex,
            .format = default_format_hex,
        },
};

struct loaded_or_builtin_parser builtin_parser_dec = {
    .which = PARSER_TYPE_BUILTIN,
    .builtin =
        {
            .name = "decimal",
            .parse = default_parse_dec,
            .format = default_format_dec,
        },
};

struct loaded_or_builtin_parser builtin_parser_address = {
    .which = PARSER_TYPE_BUILTIN,
    .builtin =
        {
            .name = "address",
            .parse = default_parse_address,
            .format = default_format_address,
        },
};

struct loaded_or_builtin_parser builtin_parser_condition = {
    .which = PARSER_TYPE_BUILTIN,
    .builtin =
        {
            .name = "condition",
            .parse = default_parse_condition,
            .format = default_format_condition,
        },
};
