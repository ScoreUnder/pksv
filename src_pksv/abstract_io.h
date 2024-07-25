/**
 * @file abstract_io.h
 * @brief Abstract I/O interface for writing to a file or a buffer.
 *
 * This is provided because Windows (and perhaps other platforms) has no
 * equivalent to `fopencookie` or `open_memstream` for writing to something
 * other than an OS-level file descriptor.
 */
#include <sys/types.h>
#ifndef PKSV_ABSTRACT_IO_H
#define PKSV_ABSTRACT_IO_H 1

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

// This is a simple abstraction layer for writing to a file or a buffer.
enum abstract_io_type {
  ABSTRACT_IO_TYPE_FILE,
  ABSTRACT_IO_TYPE_BUFFER,
};

typedef struct abstract_io {
  enum abstract_io_type _type;
  union {
    FILE *_file;
    struct {
      void *data;
      size_t size;
      size_t capacity;
    } _buffer;
  };
} abstract_io;

abstract_io *absio_create_write_buffer(size_t initial_size);
abstract_io *absio_create_file(FILE *file);

size_t absio_write(abstract_io *io, const void *data, size_t size);
size_t absio_printf(abstract_io *io, const char *format, ...);
size_t absio_vprintf(abstract_io *io, const char *format, va_list args);
void absio_putc(abstract_io *io, char c);
off_t absio_tell(abstract_io *io);

void* absio_take_buffer(abstract_io *io);

void absio_close(abstract_io *io);
void absio_free(abstract_io *io);

#define ABSIO_WRITE_LITERAL(io, literal) \
  absio_write(io, literal, sizeof(literal) - 1)

#endif
