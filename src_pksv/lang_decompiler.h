#ifndef PKSV_LANG_DECOMPILER_H
#define PKSV_LANG_DECOMPILER_H 1

#include <stdint.h>
#include <stdio.h>

#include "language-defs.h"
#include "lang_decompiler_types.h"
#include "lang_parsers_types.h"
#include "lang_load.h"

void decompile_all(FILE *input_file, uint32_t start_offset,
                   const struct language_def *start_language,
                   struct language_cache *language_cache,
                   struct parser_cache *parser_cache, FILE *output_file);

#endif
