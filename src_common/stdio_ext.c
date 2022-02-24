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
  bytes[i-1] &= 0x7F;
  return fwrite(bytes, 1, i, stream);
}

uint32_t fgetvarint(FILE *stream) {
  uint8_t byte;
  uint32_t result = 0;
  int i = 0;
  do {
    byte = fgetc(stream);
    result |= (byte & 0x7F) << i;
    i += 7;
  } while (byte & 0x80);
  return result;
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
