#ifndef PKSVUI_WINDOWS_PORTABILITY_H
#define PKSVUI_WINDOWS_PORTABILITY_H 1

#include <windows.h>
#include <stdio.h>

FILE *windows_handle_as_stdio(HANDLE handle, const char *mode);

#endif
