#ifndef LANG_DEFAULT_PARSERS_H
#define LANG_DEFAULT_PARSERS_H 1

#include <stddef.h>
#include <stdint.h>

#include "lang_parsers_types.h"

struct parse_result parse_from_loaded_parser(struct loaded_parser *parser,
                                             const char *token,
                                             size_t token_len);
struct parse_result format_from_loaded_parser(struct loaded_parser *parser,
                                              uint32_t value);

extern struct loaded_or_builtin_parser builtin_parser_hex;
extern struct loaded_or_builtin_parser builtin_parser_dec;
extern struct loaded_or_builtin_parser builtin_parser_address;
extern struct loaded_or_builtin_parser builtin_parser_condition;

#endif
