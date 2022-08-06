#ifndef PKSV_LANG_PARSERS_H
#define PKSV_LANG_PARSERS_H 1

#include <stdint.h>
#include <stdbool.h>

#include "binarysearch.h"
#include "language-defs.h"
#include "lang_decompiler_types.h"
#include "lang_parsers_types.h"

struct loaded_parser *load_definitions(const char *name, const char *filename,
                                       bool required);

struct parser_cache *create_parser_cache(const char *parser_dir);
void destroy_parser_cache(struct parser_cache *cache);

struct parse_result parse_for_recomp(struct parser_cache *cache,
                                     struct language_def *lang,
                                     struct parser_list parsers, char *token,
                                     size_t token_len);
struct parse_result format_for_decomp(
    struct parser_cache *cache, const struct language_def *lang,
    const struct parser_list parsers, uint32_t value,
    struct decompiler_informative_state *decstate);

#endif
