/*
    Charles Daffern (Score_Under)'s PKSV - Pokemon ROM script viewer/editor
    Copyright (C) 2007  Charles Daffern

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
#ifdef DLL
#define _CRT_SECURE_NO_DEPRECATE
#include "windows.h"
char dyntype=1;
#define INTERNAL_VERSION "2.0.4"
HINSTANCE inst=NULL;
#define malloc(x) GlobalAlloc(GPTR,x)
#define free(x) GlobalFree(x)
#define FIRE_RED  0
#define RUBY      1
#define GOLD      2
#define DIAMOND   3
#define CRYSTAL   4
char mode=FIRE_RED;
char VersionOverride=0;
char GlobBuf[65536];
#include "sulib.h"
#include "golddef.h"
#include "pokedef.h"
#define DECOMPILE 0
#define RECOMPILE 1
#define TXT       2
#define MOVEMENT  3
#define BRAILLE   4
char IsVerbose=1;
unsigned char search=0xFF; //Free Space character
char eorg=0;
char testing=0;
char*decompiled=NULL;
unsigned int allocated=0;
unsigned int dec_len=0;
char*logtxt=NULL;
unsigned int log_allocated;
unsigned int log_size;
HWND UI_WIN;
//char GlobBuf[65536];
signed int PointerToOffset(unsigned int ptr); //prototype
signed int OffsetToPointer(unsigned int offset);
#ifdef WIN32
int OffsetDlg(HWND,UINT,WPARAM,LPARAM);
int TxtDlg(HWND,UINT,WPARAM,LPARAM);
HWND HW_DLG,HW_TXT;
#endif
#include "textproc.h"
#include "codeproc.h"
//Determining mode
char determine_mode[5];
BOOL WINAPI DllMain(HINSTANCE hinstDLL,
                    DWORD fdwReason,
                    LPVOID lpvReserved)
{
  RECT rect;
  register char*a;
  if (fdwReason!=DLL_PROCESS_ATTACH)return 1;
  if (hinstDLL)
    inst=hinstDLL;
  GetModuleFileName(inst,GlobBuf,65536);
  a=strrchr(GlobBuf,'\\');
  if (a)a[1]=0;
  else strcpy(GlobBuf,".\\");
  strcat(GlobBuf,"defines.dat");
  HW_DLG=CreateDialog(inst,MAKEINTRESOURCE(10),NULL,(DLGPROC)&OffsetDlg);
  HW_TXT=CreateDialog(inst,MAKEINTRESOURCE(11),NULL,(DLGPROC)&TxtDlg);
  UI_WIN=FindWindow("Score_Under_PKSVUI",NULL);
  if (UI_WIN!=NULL)
  {
    GetWindowRect(UI_WIN,&rect);
    rect.top+=40;
    rect.left+=40;
    SetWindowPos(HW_DLG,HWND_TOPMOST,rect.left,rect.top,0,0,SWP_NOSIZE);
    rect.top+=40;
    rect.left+=100;
    SetWindowPos(HW_TXT,HWND_TOPMOST,rect.left,rect.top,0,0,SWP_NOSIZE);
  }
  else
  {
    SetWindowPos(HW_DLG,HWND_NOTOPMOST,200,200,0,0,SWP_NOSIZE);
    SetWindowPos(HW_TXT,HWND_NOTOPMOST,300,240,0,0,SWP_NOSIZE);
  }
  return 1;
}
__declspec(dllexport) __cdecl int VersionMismatch(char*v)
{
  return strcmp(v,INTERNAL_VERSION);
}
__declspec(dllexport) __cdecl int NewMode(int nmode)
{
  mode=nmode;
  VersionOverride=1;
  if(mode==RUBY||mode==FIRE_RED)search=0xFF;
  else search=0;
  return mode;
}
__declspec(dllexport) __cdecl int DetermineMode(char*rom)
{
  register FILE*romfile;
  VersionOverride=0;
  if (!(romfile=fopen(rom,"rb")))
  {
    return -1;
  }
  fseek(romfile,0xAC,SEEK_SET);
  fread(determine_mode,1,3,romfile);
  if (determine_mode[0]=='A'&&determine_mode[1]=='X')
  {
    mode=RUBY;
  }
  if (determine_mode[0]=='B'&&determine_mode[1]=='P')
  {
    mode=FIRE_RED;
  }
  if (determine_mode[0]=='B'&&determine_mode[1]=='P'&&determine_mode[2]=='E')
  {
    search=0;
  }
  fseek(romfile,0x13F,SEEK_SET);
  fread(determine_mode,1,2,romfile);
  if ((determine_mode[0]=='A'&&determine_mode[1]=='A')||(determine_mode[0]=='S'&&determine_mode[1]=='M'))
  {
    mode=GOLD;
    search=0;
  }
  if (determine_mode[0]=='B'&&determine_mode[1]=='Y')
  {
    mode=CRYSTAL;
    search=0;
  }
  fseek(romfile,0x0,SEEK_SET);
  fread(determine_mode,1,4,romfile);
  if (determine_mode[0]=='N'&&determine_mode[1]=='A'&&determine_mode[2]=='R'&&determine_mode[3]=='C')
  {
    mode=DIAMOND;
    search=0;
  }
  fclose(romfile);
  return mode;
}
__declspec(dllexport) __cdecl char* decompile(char*fname,int loc,int narc)
{
  register FILE*romfile;

  OutputDebugString("Start decompile...");
  romfile=fopen(fname,"r+b");
  if (!romfile)return NULL;
  decompiled=NULL;
  allocated=dec_len=0;
  if (mode==DIAMOND)
  {
    DecodeProc(romfile,narc,loc,fname);
  }
  else
  {
    DecodeProc(romfile,0,loc,fname);
  }
  OutputDebugString("Finished decompiling");
  fclose(romfile);
  return decompiled;
}
__declspec(dllexport) __cdecl char* decompileASM(char*fname,int loc)
{
  register FILE*romfile;

  OutputDebugString("Start decompile...");
  romfile=fopen(fname,"r+b");
  if (!romfile)return NULL;
  decompiled=NULL;
  allocated=dec_len=0;
  DecodeProcASM(romfile,loc,fname);
  OutputDebugString("Finished decompiling");
  fclose(romfile);
  return decompiled;
}
__declspec(dllexport) __cdecl char* decompileLevel(char*fname,int loc)
{
  register FILE*romfile;

  OutputDebugString("Start decompile...");
  romfile=fopen(fname,"r+b");
  if (!romfile)return NULL;
  decompiled=NULL;
  allocated=dec_len=0;
  DecodeProcLevel(romfile,loc,fname);
  OutputDebugString("Finished decompiling");
  fclose(romfile);
  return decompiled;
}
void ClearDefines()
{
  register definition*a;
  register definition*b;
  a=basedef;
  while (a)
  {
    b=a->next;
    free(a->name);
    free(a);
    a=b;
  }
  basedef=NULL;
}
int dlg_active,compiling=0;
__declspec(dllexport) __cdecl int compile(char*fname,char*to_recompile)
{
  while (compiling)Sleep(1);
  OutputDebugString("Start compile...");
  compiling=1;
  testing=0;
  needdlg=0;
  ffoff=0;

  RecodeProc(to_recompile,fname);

  if (logtxt)
  {
    SetDlgItemText(HW_TXT,1,logtxt);
    GlobalFree(logtxt);
    logtxt=NULL;
    log_allocated=log_size=0;
  }
  ClearDefines();
  compiling=0;
  ShowWindow(HW_TXT,SW_SHOW);
  SetWindowPos(HW_TXT,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
  SetForegroundWindow(HW_TXT);
  SetFocus(GetDlgItem(HW_TXT,1));
  dlg_active=needdlg+1;
  OutputDebugString("Finished compiling");
  /*while (dlg_active&&GetMessage(&msg,NULL,0,0))
  {
  	if (!IsDialogMessage(HW_DLG,&msg)&&!IsDialogMessage(HW_TXT,&msg))
  	{
  		TranslateMessage(&msg);
  		DispatchMessage(&msg);
  	}
  }*/
  return 0;
}
__declspec(dllexport) __cdecl int DebugCompile(char*fname,char*to_recompile)
{
  while (compiling)Sleep(1);
  compiling=1;
  testing=1;
  needdlg=0;
  ffoff=0;

  OutputDebugString("Started compile...");

  RecodeProc(to_recompile,fname);

  OutputDebugString("Finished compile");

  if (logtxt)
  {
    SetDlgItemText(HW_TXT,1,logtxt);
    GlobalFree(logtxt);
    logtxt=NULL;
    log_allocated=log_size=0;
  }

  ClearDefines();

  compiling=0;
  ShowWindow(HW_TXT,SW_SHOW);
  SetWindowPos(HW_TXT,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
  SetForegroundWindow(HW_TXT);
  SetFocus(GetDlgItem(HW_TXT,1));

  dlg_active=needdlg+1;
  /*while (dlg_active&&GetMessage(&msg,NULL,0,0))
  {
  	if (!IsDialogMessage(HW_DLG,&msg)&&!IsDialogMessage(HW_TXT,&msg))
  	{
  		TranslateMessage(&msg);
  		DispatchMessage(&msg);
  	}
  }*/
  return 0;
}
int OffsetDlg(HWND a, UINT msg, WPARAM wParam, LPARAM lParam)
{
  unsigned int selection,len,i;
  char*ptr;
  char*ptr2;
  switch (msg)
  {
  case WM_CLOSE:
    if (a==HW_DLG)
    {
      ShowWindow(a,SW_HIDE);
      dlg_active--;
      if (dlg_active)SetForegroundWindow(HW_TXT);
    }
    break;
  case WM_COMMAND:
    if (a==HW_DLG)
      switch (LOWORD(wParam))
      {
      case 1:
        ShowWindow(a,SW_HIDE);
        dlg_active--;
        if (dlg_active)SetForegroundWindow(HW_TXT);
        break;
      case 2:
        selection=(unsigned int)SendMessage(GetDlgItem(HW_DLG,3),LB_GETCURSEL,0,0);
        if (selection==LB_ERR)break;
        if (OpenClipboard(NULL))
        {
          EmptyClipboard();
          len=(unsigned int)SendMessage(GetDlgItem(HW_DLG,3),LB_GETTEXTLEN,selection,0);
          ptr=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,len+1);
          ptr2=(char*)GlobalLock(ptr);
          SendMessage(GetDlgItem(HW_DLG,3),LB_GETTEXT,selection,(LPARAM)ptr2);
          GlobalUnlock(ptr);
          SetClipboardData(CF_TEXT,ptr);
          CloseClipboard();
        }
        break;
      case 4:
        selection=(unsigned int)SendMessage(GetDlgItem(HW_DLG,3),LB_GETCOUNT,0,0);
        if (selection==LB_ERR)break;
        if (OpenClipboard(NULL))
        {
          EmptyClipboard();
          len=0;
          for (i=0;i<selection;i++)
          {
            len+=2+(unsigned int)SendMessage(GetDlgItem(HW_DLG,3),LB_GETTEXTLEN,i,0);
          }
          ptr=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,len+1);
          ptr2=(char*)GlobalLock(ptr);
          *ptr2=0;
          for (i=0;i<selection;i++)
          {
            SendMessage(GetDlgItem(HW_DLG,3),LB_GETTEXT,i,(LPARAM)ptr2+strlen(ptr2));
            strcat(ptr2,"\r\n");
          }
          GlobalUnlock(ptr);
          SetClipboardData(CF_TEXT,ptr);
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
int TxtDlg(HWND a, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CLOSE:
    if (a==HW_TXT)
    {
      ShowWindow(a,SW_HIDE);
      dlg_active--;
      if (dlg_active)SetForegroundWindow(HW_DLG);
    }
    break;
  case WM_SIZE:
    SetWindowPos(GetDlgItem(HW_TXT,1),HWND_TOP,0,0,LOWORD(lParam),HIWORD(lParam),SWP_NOMOVE);
    break;
  default:
    return 0;
  }
  return 1;
}
#endif
