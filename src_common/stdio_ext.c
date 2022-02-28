#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t fputvarint(uint32_t u32, FILE *stream) {
  uint8_t bytes[5];
  size_t i = 0;
  do {
    bytes[i++] = (u32 & 0x7F) | 0x80;
    u32 >>= 7;
  } while (u32);
  bytes[i - 1] &= 0x7F;
  return fwrite(bytes, 1, i, stream);
}

uint32_t fgetvarint(FILE *stream) {
  int byte;
  uint32_t result = 0;
  int i = 0;
  do {
    byte = fgetc(stream);
    result |= (byte & 0x7F) << i;
    i += 7;
  } while (byte & 0x80 && byte != EOF);
  return result;
}

size_t fput_little_endian(uint32_t u32, size_t size, FILE *stream) {
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return fwrite(&u32, 1, size, stream);
#else
  uint8_t bytes[4];
  for (size_t i = 0; i < size; i++) {
    bytes[i] = u32 & 0xFF;
    u32 >>= 8;
  }
  return fwrite(bytes, 1, size, stream);
#endif
}

uint32_t arr_get_little_endian(uint8_t *arr, size_t size) {
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  uint32_t mask = 0xFFFFFFFF;
  assert(size <= 4);
  if (size < 4) {
    mask >>= (4 - size) * 8;
  }
  return *(uint32_t *)arr & mask;
#else
  uint32_t result = 0;
  while (size--) {
    result <<= 8;
    result |= *arr++;
  }
  return result;
#endif
}

uint32_t fget_little_endian(size_t size, FILE *stream) {
  uint8_t bytes[4];
  size = fread(bytes, 1, size, stream);
  return arr_get_little_endian(bytes, size);
}

size_t fputstr(const char *str, FILE *stream) {
  size_t str_length = strlen(str);
  size_t written = fputvarint(str_length, stream);
  return written + fwrite(str, 1, str_length, stream);
}

char *fgetstr(FILE *stream) {
  size_t str_length = fgetvarint(stream);
  char *str = malloc(str_length + 1);
  fread(str, 1, str_length, stream);
  str[str_length] = '\0';
  return str;
}
