#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abstract_io.h"


abstract_io *absio_create_write_buffer(size_t initial_size) {
  abstract_io *io = malloc(sizeof(abstract_io));
  io->_type = ABSTRACT_IO_TYPE_BUFFER;
  io->_buffer.data = malloc(initial_size);
  io->_buffer.size = 0;
  io->_buffer.capacity = initial_size;
  return io;
}

abstract_io *absio_create_file(FILE *file) {
  abstract_io *io = malloc(sizeof(abstract_io));
  io->_type = ABSTRACT_IO_TYPE_FILE;
  io->_file = file;
  return io;
}

static void absio_ensure_capacity(abstract_io *io, size_t capacity) {
  if (capacity > io->_buffer.capacity) {
    if (capacity > (SIZE_MAX / 2)) {
      fprintf(stderr, "absio_ensure_capacity: buffer too large. requested size: %zu\n", capacity);
      abort();
    }
    if (io->_buffer.capacity == 0) {
      io->_buffer.capacity = 1;
    }
    while (capacity > io->_buffer.capacity) {
      io->_buffer.capacity *= 2;
    }
    io->_buffer.data = realloc(io->_buffer.data, io->_buffer.capacity);
  }
}

size_t absio_write(abstract_io *io, const void *data, size_t size) {
  switch (io->_type) {
    case ABSTRACT_IO_TYPE_BUFFER: {
      size_t new_size = io->_buffer.size + size;
      absio_ensure_capacity(io, new_size);
      memcpy((char*)io->_buffer.data + io->_buffer.size, data, size);
      io->_buffer.size = new_size;
      return size;
    }
    case ABSTRACT_IO_TYPE_FILE: {
      return fwrite(data, 1, size, io->_file);
    }
  }
  return 0;
}

size_t absio_printf(abstract_io *io, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t size = absio_vprintf(io, format, args);
  va_end(args);
  return size;
}

size_t absio_vprintf(abstract_io *io, const char *format, va_list args) {
  switch (io->_type) {
    case ABSTRACT_IO_TYPE_BUFFER: {
      va_list args_copy;
      va_copy(args_copy, args);
      size_t size = vsnprintf(NULL, 0, format, args_copy);
      va_end(args_copy);
      absio_ensure_capacity(io, io->_buffer.size + size + 1);
      size = vsnprintf((char*)io->_buffer.data + io->_buffer.size, size + 1, format, args);
      io->_buffer.size += size;
      return size;
    }
    case ABSTRACT_IO_TYPE_FILE: {
      return vfprintf(io->_file, format, args);
    }
  }
  abort();
  return 0; // unreachable
}

void absio_putc(abstract_io *io, char c) {
  switch (io->_type) {
    case ABSTRACT_IO_TYPE_BUFFER: {
      absio_ensure_capacity(io, io->_buffer.size + 1);
      ((char*)io->_buffer.data)[io->_buffer.size++] = c;
      break;
    }
    case ABSTRACT_IO_TYPE_FILE: {
      putc(c, io->_file);
      break;
    }
  }
}

off_t absio_tell(abstract_io *io) {
  switch (io->_type) {
    case ABSTRACT_IO_TYPE_BUFFER: {
      off_t offset = (off_t) io->_buffer.size;
      if ((size_t) offset != io->_buffer.size) {
        fprintf(stderr, "absio_tell: buffer too large. size: %zu\n", io->_buffer.size);
        abort();
      }
      return offset;
    }
    case ABSTRACT_IO_TYPE_FILE: {
      return ftello(io->_file);
    }
  }
  return 0;
}

void* absio_take_buffer(abstract_io *io) {
  if (io->_type != ABSTRACT_IO_TYPE_BUFFER) {
    fprintf(stderr, "absio_take_buffer: io is not a buffer\n");
    abort();
  }
  void *data = io->_buffer.data;
  io->_buffer.data = NULL;
  io->_buffer.size = 0;
  io->_buffer.capacity = 0;
  return data;
}

void absio_close(abstract_io *io) {
  switch (io->_type) {
    case ABSTRACT_IO_TYPE_BUFFER: {
      free(io->_buffer.data);
      break;
    }
    case ABSTRACT_IO_TYPE_FILE: {
      fclose(io->_file);
      break;
    }
  }
  free(io);
}

void absio_free(abstract_io *io) {
  switch (io->_type) {
    case ABSTRACT_IO_TYPE_BUFFER: {
      free(io->_buffer.data);
      break;
    }
    case ABSTRACT_IO_TYPE_FILE: {
      break;
    }
  }
  free(io);
}
