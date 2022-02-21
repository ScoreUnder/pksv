#include "textutil.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const char *hex_to_uint32(const char *c, size_t maxlen, uint32_t *out) {
  uint32_t result = 0;
  while (*c && maxlen) {
    uint32_t digit;
    if (*c >= '0' && *c <= '9') {
      digit = *c - '0';
    } else if (*c >= 'a' && *c <= 'f') {
      digit = *c - 'a' + 10;
    } else if (*c >= 'A' && *c <= 'F') {
      digit = *c - 'A' + 10;
    } else {
      break;
    }
    if (result > 0xFFFFFFF) {
      // Check for overflow.
      break;
    }
    result = (result << 4) | digit;
    c++;
    maxlen--;
  }
  *out = result;
  return c;
}

const char *dec_to_uint32(const char *c, size_t maxlen, uint32_t *out) {
  uint32_t result = 0;
  while (*c && maxlen) {
    uint32_t digit;
    if (*c >= '0' && *c <= '9') {
      digit = *c - '0';
    } else {
      break;
    }
    if (result > 0x19999999 || (result == 0x19999999 && digit > 5)) {
      // Check for overflow.
      break;
    }
    result = (result * 10) + digit;
    c++;
    maxlen--;
  }
  *out = result;
  return c;
}

char *extract_text_interval(const char *start, const char *end) {
  size_t len = end - start;
  char *result = malloc(len + 1);
  memcpy(result, start, len);
  result[len] = '\0';
  return result;
}

size_t skip_whitespace(const char *in, size_t pos) {
  while (in[pos] == ' ') pos++;
  return pos;
}

size_t skip_whitespace_and_comma(const char *in, size_t pos) {
  pos = skip_whitespace(in, pos);
  if (in[pos] == ',') {
    pos++;
    pos = skip_whitespace(in, pos);
  }
  return pos;
}
