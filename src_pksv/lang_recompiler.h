#ifndef PKSV_LANG_RECOMPILER_H
#define PKSV_LANG_RECOMPILER_H 1

#include <stdio.h>
#include "language-defs.h"
#include "lang_load.h"
#include "lang_parsers_types.h"

void compile_all(FILE *input_file, FILE *output_file,
                 const struct language_def *start_language,
                 struct language_cache *language_cache,
                 struct parser_cache *parser_cache);

#endif
