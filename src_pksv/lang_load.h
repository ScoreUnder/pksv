#ifndef PKSV_LANG_LOAD_H
#define PKSV_LANG_LOAD_H 1

#include "language-defs.h"

struct language_cache;  // opaque

/** @brief Creates an empty language cache. */
struct language_cache *create_language_cache(void);

/** @brief Gets a language from the cache, or loads if not found.
 *
 * @param cache The cache to use.
 * @param dir The directory to load from.
 * @param name The language to load.
 * @return The language, or NULL if not available.
 */
const struct language_def *get_language(struct language_cache *cache, const char *dir, const char *name);

/** @brief Frees a language cache. */
void destroy_language_cache(struct language_cache *cache);

#endif
