/*
    Sophie Daffern (Score_Under)'s PKSV - Pokemon ROM script viewer/editor
    Copyright (C) 2007  Sophie Daffern

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define _POSIX_C_SOURCE 200809L
#include <fmem.h>
#include <stdbool.h>
#include <stdio.h>

#include "codeproc.h"
#include "decompiler.h"
#include "pksv.h"
#include "recompiler.h"
#include "romutil.h"
#include "sulib.h"
#include "version.h"

#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
char dyntype = 1;
HINSTANCE inst = NULL;
bool dyndec = false;
int dynplace = 0;
char mode = FIRE_RED;
bool VersionOverride = false;
char GlobBuf[65536];
FILE* LogFile;
bool IsVerbose = true;
unsigned char search = 0xFF;  // Free Space character
bool eorg = false;
bool testing = false;
HWND UI_WIN;
signed int PointerToOffset(unsigned int ptr);  // prototype
signed int OffsetToPointer(unsigned int offset);
int __stdcall TxtDlg(HWND, UINT, WPARAM, LPARAM);
HWND HW_DLG, HW_TXT;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  RECT rect;
  register char* a;
  if (fdwReason != DLL_PROCESS_ATTACH) return 1;
  if (hinstDLL) inst = hinstDLL;

  GetModuleFileName(inst, GlobBuf, 65536);
  a = strrchr(GlobBuf, '\\');
  if (a)
    a[1] = 0;
  else
    strcpy(GlobBuf, ".\\");
  strcat(GlobBuf, "defines.dat");
  if (defines_dat_location) free(defines_dat_location);
  defines_dat_location = strdup(GlobBuf);

  HW_DLG = CreateDialog(inst, MAKEINTRESOURCE(10), NULL, (DLGPROC)&OffsetDlg);
  HW_TXT = CreateDialog(inst, MAKEINTRESOURCE(11), NULL, (DLGPROC)&TxtDlg);
  UI_WIN = FindWindow("Score_Under_PKSVUI", NULL);
  if (UI_WIN != NULL) {
    GetWindowRect(UI_WIN, &rect);
    rect.top += 40;
    rect.left += 40;
    SetWindowPos(HW_DLG, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_NOSIZE);
    rect.top += 40;
    rect.left += 100;
    SetWindowPos(HW_TXT, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_NOSIZE);
  } else {
    SetWindowPos(HW_DLG, HWND_NOTOPMOST, 200, 200, 0, 0, SWP_NOSIZE);
    SetWindowPos(HW_TXT, HWND_NOTOPMOST, 300, 240, 0, 0, SWP_NOSIZE);
  }
  return 1;
}
__declspec(dllexport) __cdecl int VersionMismatch(char* v) {
  return strcmp(v, ABI_COMPAT_VERSION);
}
__declspec(dllexport) __cdecl int NewMode(int nmode) {
  mode = nmode;
  VersionOverride = 1;
  if (mode == RUBY || mode == FIRE_RED)
    search = 0xFF;
  else
    search = 0;
  return mode;
}
__declspec(dllexport) __cdecl int DetermineMode(char* rom) {
  FILE* romfile;
  VersionOverride = 0;
  if (!(romfile = fopen(rom, "rb"))) {
    return -1;
  }
  struct rom_mode rom_mode = determine_mode(romfile);
  if (rom_mode.type != ROM_UNKNOWN) {
    mode = rom_mode.type;
    search = rom_mode.search;
  }
  fclose(romfile);
  return mode;
}
__declspec(dllexport) __cdecl void SetDynamic(int dyn, int place) {
  dyndec = dyn;
  dynplace = place;
}

void pksv_reset_all() {
  pksv_codeproc_reset();
  pksv_decompiler_reset();
}

static void* memdup(void* ptr, size_t len) {
  void* newptr = malloc(len);
  memcpy(newptr, ptr, len);
  return newptr;
}

#define START_FMEM(vname)                       \
  do {                                          \
  } while (0);                                  \
  fmem vname##_fmem;                            \
  fmem_init(&vname##_fmem);                     \
  FILE* vname = fmem_open(&vname##_fmem, "wt"); \
  do {                                          \
  } while (0)

#define END_FMEM(vname)                                        \
  do {                                                         \
  } while (0);                                                 \
  putc('\0', vname);                                           \
  fflush(vname);                                               \
  char* vname##_str = NULL;                                    \
  size_t vname##_len = 0;                                      \
  fmem_mem(&vname##_fmem, (void**)&vname##_str, &vname##_len); \
  vname##_str = memdup(vname##_str, vname##_len);              \
  fclose(vname);                                               \
  fmem_term(&vname##_fmem);                                    \
  do {                                                         \
  } while (0)

__declspec(dllexport) __cdecl char* decompile(char* fname, int loc, int narc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProc(romfile, mode == DIAMOND ? narc : 0, loc, fname, script_file);

  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompileASM(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcASM(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompileText(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcText(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompileLevel(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcLevel(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompilePointer(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcPointer(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompileMoves(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcMoves(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompileMart(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcMart(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
__declspec(dllexport) __cdecl char* decompileAttacks(char* fname, int loc) {
  register FILE* romfile;

  OutputDebugString("Start decompile...");
  romfile = fopen(fname, "r+b");
  if (!romfile) return "'Error opening ROM";

  START_FMEM(script_file);

  DecodeProcAttacks(romfile, loc, fname, script_file);
  pksv_reset_all();
  OutputDebugString("Finished decompiling");
  fclose(romfile);

  END_FMEM(script_file);
  return script_file_str;
}
int dlg_active, compiling = 0;
__declspec(dllexport) __cdecl int compile(char* fname, char* to_recompile) {
  while (compiling) Sleep(1);
  OutputDebugString("Start compile...");
  compiling = 1;
  testing = 0;
  needdlg = 0;
  ffoff = 0;

  START_FMEM(log_file);

  LogFile = log_file; /* XXX hack */
  RecodeProc(to_recompile, fname);
  LogFile = NULL; /* XXX hack */

  END_FMEM(log_file);
  SetDlgItemText(HW_TXT, 1, log_file_str);
  free(log_file_str);

  pksv_reset_all();
  compiling = 0;
  ShowWindow(HW_TXT, SW_SHOW);
  SetWindowPos(HW_TXT, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
  SetForegroundWindow(HW_TXT);
  SetFocus(GetDlgItem(HW_TXT, 1));
  OutputDebugString("Finished compiling");
  return 0;
}

__declspec(dllexport) __cdecl int DebugCompile(char* fname,
                                               char* to_recompile) {
  while (compiling) Sleep(1);
  compiling = 1;
  testing = 1;
  needdlg = 0;
  ffoff = 0;

  START_FMEM(log_file);

  OutputDebugString("Started compile...");

  RecodeProc(to_recompile, fname);

  OutputDebugString("Finished compile");

  END_FMEM(log_file);
  SetDlgItemText(HW_TXT, 1, log_file_str);
  free(log_file_str);

  pksv_reset_all();

  compiling = 0;
  ShowWindow(HW_TXT, SW_SHOW);
  SetWindowPos(HW_TXT, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
  SetForegroundWindow(HW_TXT);
  SetFocus(GetDlgItem(HW_TXT, 1));
  return 0;
}
int __stdcall OffsetDlg(HWND a, UINT msg, WPARAM wParam, LPARAM lParam) {
  unsigned int selection, len, i;
  char* ptr;
  char* ptr2;
  switch (msg) {
    case WM_CLOSE:
      if (a == HW_DLG) {
        ShowWindow(a, SW_HIDE);
        dlg_active--;
        if (dlg_active) SetForegroundWindow(HW_TXT);
      }
      break;
    case WM_COMMAND:
      if (a == HW_DLG) switch (LOWORD(wParam)) {
          case 1:
            ShowWindow(a, SW_HIDE);
            dlg_active--;
            if (dlg_active) SetForegroundWindow(HW_TXT);
            break;
          case 2:
            selection = (unsigned int)SendMessage(GetDlgItem(HW_DLG, 3),
                                                  LB_GETCURSEL, 0, 0);
            if (selection == LB_ERR) break;
            if (OpenClipboard(NULL)) {
              EmptyClipboard();
              len = (unsigned int)SendMessage(GetDlgItem(HW_DLG, 3),
                                              LB_GETTEXTLEN, selection, 0);
              ptr = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 1);
              ptr2 = (char*)GlobalLock(ptr);
              SendMessage(GetDlgItem(HW_DLG, 3), LB_GETTEXT, selection,
                          (LPARAM)ptr2);
              GlobalUnlock(ptr);
              SetClipboardData(CF_TEXT, ptr);
              CloseClipboard();
            }
            break;
          case 4:
            selection = (unsigned int)SendMessage(GetDlgItem(HW_DLG, 3),
                                                  LB_GETCOUNT, 0, 0);
            if (selection == LB_ERR) break;
            if (OpenClipboard(NULL)) {
              EmptyClipboard();
              len = 0;
              for (i = 0; i < selection; i++) {
                len += 2 + (unsigned int)SendMessage(GetDlgItem(HW_DLG, 3),
                                                     LB_GETTEXTLEN, i, 0);
              }
              ptr = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 1);
              ptr2 = (char*)GlobalLock(ptr);
              *ptr2 = 0;
              for (i = 0; i < selection; i++) {
                SendMessage(GetDlgItem(HW_DLG, 3), LB_GETTEXT, i,
                            (LPARAM)ptr2 + strlen(ptr2));
                strcat(ptr2, "\n");
              }
              GlobalUnlock(ptr);
              SetClipboardData(CF_TEXT, ptr);
              CloseClipboard();
            }
            break;
        }
      break;
    default:
      return 0;
  }
  return 1;
}
int __stdcall TxtDlg(HWND a, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CLOSE:
      if (a == HW_TXT) {
        ShowWindow(a, SW_HIDE);
        dlg_active--;
        if (dlg_active) SetForegroundWindow(HW_DLG);
      }
      break;
    case WM_SIZE:
      SetWindowPos(GetDlgItem(HW_TXT, 1), HWND_TOP, 0, 0, LOWORD(lParam),
                   HIWORD(lParam), SWP_NOMOVE);
      break;
    default:
      return 0;
  }
  return 1;
}
