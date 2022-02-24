#ifndef PKSV_LANG_PARSERS_H
#define PKSV_LANG_PARSERS_H 1

#include "binarysearch.h"

struct loaded_parser {
  struct bsearch_root lookup_by_name;
  struct bsearch_root lookup_by_id;
};

struct loaded_parser *load_definitions(const char *filename);

#endif
