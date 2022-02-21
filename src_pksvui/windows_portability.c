#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>

#include "windows_portability.h"

FILE *windows_handle_as_stdio(HANDLE handle, const char *mode)
{
  if (handle == INVALID_HANDLE_VALUE) {
    errno = EBADF;
    return NULL;
  }

  if (mode[strspn(mode, "rwtb")] != '\0') {
    errno = EINVAL;
    return NULL;
  }

  int modeflags = 0;
  if (strchr(mode, 'r'))
    modeflags |= _O_RDONLY;
  if (strchr(mode, 'w'))
    modeflags |= _O_WRONLY;
  if (strchr(mode, 't'))
    modeflags |= _O_TEXT;
  if (strchr(mode, 'b'))
    modeflags |= _O_BINARY;

  int fd = _open_osfhandle((intptr_t)handle, _O_RDONLY);
  if (fd == -1) {
    CloseHandle(handle);
    errno = EBADF;
    return NULL;
  }

  FILE *fp = _fdopen(fd, mode);
  if (fp == NULL) {
    errno = EBADF;
    _close(fd);
  }

  return fp;
}
