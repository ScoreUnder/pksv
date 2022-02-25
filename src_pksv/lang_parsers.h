#ifndef PKSV_LANG_PARSERS_H
#define PKSV_LANG_PARSERS_H 1

#include <stdint.h>
#include <stdbool.h>

#include "binarysearch.h"

struct loaded_parser {
  struct bsearch_root lookup_by_name;
  struct bsearch_root lookup_by_id;
};

/*
Requirements:

parse:
- token
- token length
returns:
- success
- value or label

format:
- value
- is_dyndec?
returns:
- success
- value or label

labels can be resolved after the fact by (de)compiler engine
*/
#define PARSE_RESULT_FAIL 0
#define PARSE_RESULT_VALUE 1
#define PARSE_RESULT_LABEL 2
struct parse_result {
  uint_fast8_t type;
  union {
    uint32_t value;  // if PARSE_RESULT_VALUE or formatter's PARSE_RESULT_LABEL
    char *label;     // if parser's PARSE_RESULT_LABEL
  };
};

typedef struct parse_result builtin_parser_parse(const char *token,
                                                 size_t token_len);
typedef struct parse_result builtin_parser_format(uint32_t value,
                                                  bool is_dyndec);

struct builtin_parser {
  builtin_parser_parse *parse;
  builtin_parser_format *format;
};

#define PARSER_TYPE_LOADED 1
#define PARSER_TYPE_BUILTIN 2

struct loaded_or_builtin_parser {
  uint_fast8_t which;
  union {
    struct loaded_parser loaded;
    struct builtin_parser builtin;
  };
};

struct loaded_parser *load_definitions(const char *filename);

#endif
