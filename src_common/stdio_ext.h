#ifndef PKSV_STDIO_EXT_H
#define PKSV_STDIO_EXT_H 1

#include <stdint.h>
#include <stdio.h>

/** @brief Write a 32-bit integer to a file using variable length encoding.
 *  Has a consistent endianness.
 */
size_t fputvarint(uint32_t u32, FILE *stream);

/** @brief Read a 32-bit integer from a file using variable length encoding. */
uint32_t fgetvarint(FILE *stream);

/** @brief Write an integer to a file in little-endian. */
size_t fput_little_endian(uint32_t u32, size_t size, FILE *stream);

/** @brief Read an integer from a file in little-endian. */
uint32_t fget_little_endian(size_t size, FILE *stream);

/** @brief Write a variable-length string to a file.
 */
size_t fputstr(const char *str, FILE *stream);

/** @brief Read a variable-length string from a file. */
__attribute__((malloc)) char *fgetstr(FILE *stream);

#endif
