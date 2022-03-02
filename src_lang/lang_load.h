#ifndef PKSV_LANG_LOAD_H
#define PKSV_LANG_LOAD_H 1

#include <stdio.h>

#include "language-defs.h"

struct language_cache;  // opaque

/** @brief Creates an empty language cache. */
struct language_cache *create_language_cache(const char *dir);

/** @brief Gets a language from the cache, or loads if not found.
 *
 * @param cache The cache to use.
 * @param dir The directory to load from.
 * @param name The language to load.
 * @return The language, or NULL if not available.
 */
const struct language_def *get_language(struct language_cache *cache,
                                        const char *name);

/** @brief Frees a language cache. */
void destroy_language_cache(struct language_cache *cache);

// Lower-level functions for tools that deal with language files.
struct loaded_lang {
  struct language_def def;
  char **string_table;
};

struct loaded_lang *load_language_from_file(struct language_cache *cache,
                                            FILE *file, const char *name);
void free_loaded_lang(struct loaded_lang *lang);

#endif
