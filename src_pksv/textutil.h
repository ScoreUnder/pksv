#ifndef PKSV_TEXT_UTIL_H
#define PKSV_TEXT_UTIL_H 1

#include <stddef.h>
#include <stdint.h>

__attribute__((nonnull, returns_nonnull)) const char *hex_to_uint32(
    const char *c, size_t maxlen, uint32_t *out);

__attribute__((nonnull, returns_nonnull)) const char *dec_to_uint32(
    const char *c, size_t maxlen, uint32_t *out);

__attribute__((malloc, nonnull, warn_unused_result)) char *
extract_text_interval(const char *start, const char *end);

__attribute__((nonnull)) size_t skip_whitespace(const char *in, size_t pos);

__attribute__((nonnull)) size_t skip_whitespace_and_comma(const char *in,
                                                          size_t pos);

#endif
