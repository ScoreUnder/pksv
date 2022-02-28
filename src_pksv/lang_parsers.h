#ifndef PKSV_LANG_PARSERS_H
#define PKSV_LANG_PARSERS_H 1

#include <stdint.h>
#include <stdbool.h>

#include "binarysearch.h"
#include "lang_decompiler.h"
#include "language-defs.h"

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
#define PARSE_RESULT_TOKEN 3
struct parse_result {
  uint_fast8_t type;
  union {
    uint32_t value;  // if parser's PARSE_RESULT_VALUE or formatter's
                     // PARSE_RESULT_LABEL
    char *label;     // if parser's PARSE_RESULT_LABEL
    char *token;     // if formatter's PARSE_RESULT_TOKEN
  };
};

typedef struct parse_result builtin_parser_parse(const char *token,
                                                 size_t token_len);
typedef struct parse_result builtin_parser_format(
    uint32_t value, struct decompiler_informative_state *state);

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

struct loaded_parser *load_definitions(const char *filename, bool required);

struct parser_cache;
struct parser_cache *create_parser_cache(void);
void destroy_parser_cache(struct parser_cache *cache);

struct parse_result parse_for_recomp(struct parser_cache *cache,
                                     struct language_def *lang,
                                     const char *parser_dir,
                                     struct parser_list parsers, char *token,
                                     size_t token_len);
struct parse_result format_for_decomp(
    struct parser_cache *cache, struct language_def *lang,
    const char *parser_dir, struct parser_list parsers, uint32_t value,
    struct decompiler_informative_state *decstate);

#endif
