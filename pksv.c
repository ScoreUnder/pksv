/*
    THIS IS DEPRECATED. See pksv2.c

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
/*#define FIRE_RED 0
#define RUBY     1
#define GOLD     2
#define DIAMOND  3
int(*func)(char*, ...);
char IsVerbose=1;
char filearg;
char DontShowLog=0;
char error=0;
char type=0;
char GlobBuf[65536];
char mode=FIRE_RED;
unsigned char search=0xFF;
//Sorry for the horrible code.
#include <stdio.h>
#include <windows.h>
HANDLE LogFile;
#include "pokedef.h"
#include "golddef.h"
#include "diamonddef.h"
char eorg=0;
#include "textproc.h" //ORDER IS IMPORTANT
char fsend[65536];
#include "codeproc.h"

//If you don't like this, just
//#define argc ac
//and
//#define argv av
int main(int ac,char**av)
{
  HANDLE fileM;
  char fileName[65536];
  OPENFILENAME ofn;
  unsigned int FileZoomPos;
  int i;
  unsigned int tmp;
  unsigned char pspec,fspec,cline,narc;
  char determineMode[4];
  DWORD read;
  fileName[0]=(char)0;
  pspec=0;
  fspec=0;
  narc=0;
  initDoneProcs();
  i=strlen(av[0]);
  while(av[0][i]!='\\'&&av[0][i]!='/'&&i!=0)
  {
    i--;
  }
  if(i==0){av[0][i]=0;}
  av[0][i+1]=0;
  strcpy(GlobBuf,av[0]);
  ZeroMemory(&ofn,sizeof(OPENFILENAME));
  ofn.lStructSize=sizeof(OPENFILENAME);
  ofn.hInstance=GetModuleHandle(NULL);
  ofn.lpstrFilter="Gameboy Advance roms\0*.gba;*.agb\0Gameboy roms\0*.gb;*.gbc\0";
  ofn.lpstrFile=fileName;
  ofn.nMaxFile=65535;
  ofn.lpstrTitle="Choose a rom";
  ofn.Flags=OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_SHAREAWARE;
  fspec=(ac>1);
  pspec=(ac>2);
  tmp=0;
  cline=0;
  while(tmp<ac)
  {
    if(strcmp(av[tmp],"\"")&&strcmp(av[tmp],""))
    {
      if(av[tmp][0]=='"')
      {
        av[tmp]++;
        av[tmp][strlen(av[tmp])-1]=0;
      }
    }
    if(!strcmp(av[tmp],"-ver")||!strcmp(av[tmp],"--ver")||!strcmp(av[tmp],"--version"))
    {
printf("Charles Daffern (Score_Under)'s PKSV:\n\tPokemon Script Viewer\n\
Version 2008/02/22 23:09\n\n\
This program is licensed under the GNU General Public License version 3.\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; read the license for details.\n\
The license should be stored in license.txt. If not, go to\n<http://www.gnu.org/licenses/gpl.html>");
      return 0;
    }
    else if(!strcmp(av[tmp],"-t"))
    {
      type=1;
    }
    else if(!strcmp(av[tmp],"-noshowtext"))
    {
      DontShowLog=1;
    }
    else if(!strcmp(av[tmp],"-b"))
    {
      type=2;
    }
    else if(!strcmp(av[tmp],"-m"))
    {
      type=3;
    }
    else if(!strcmp(av[tmp],"-q"))
    {
      IsVerbose=0;
    }
    else if(!strcmp(av[tmp],"-?")||!strcmp(av[tmp],"-h")||!strcmp(av[tmp],"--help"))
    {
      cline|=1;
    }
    else if(!strcmp(av[tmp],"-r"))
    {
      if(tmp==1)
      {
        cline|=2;
      }
      else
      {
        cline|=4;
        cline&=0x0f;
        cline|=tmp<<4;
      }
    }
    else if(av[tmp][0]=='-')
    {
      cline|=8;
      cline&=0x0f;
      cline|=tmp<<4;  //W00t! Serious compression - Error code AND command line pos in one char
    }
    tmp++;
  }
  if(cline&1)
  {
    printf(
"PKSV  Copyright (C) 2007  Charles Daffern\n\
This program comes with ABSOLUTELY NO WARRANTY; for details pass argument `-ver'.\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; pass argument `-ver' for details.\n\
\n\
Usage:\n\t\
-h, --help, -?      Show this message.\n\t\
-q                  Quiet compile mode (show only errors)\n\t\
-r SCRIPT [FILE]    Recompile SCRIPT into FILE or to stdout.\n\t\
FILE ADDRESS        Decompile FILE at ADDRESS.\n\t\
FILE                Using FILE, ask for address to decompile at.\n\t\
[-t|-b|-m]          Decode as [text|braille|movements]");
    return 0;
  }
  if(cline&8)
  {
    printf("Invalid argument: %s",av[(cline&0xf0)>>4]);
    return 0;
  }
  if(cline&4)
  {
    printf("Argument %s must be at start.",av[(cline&0xf0)>>4]);
    return 0;
  }
  if(fspec)
  strcpy(fileName,av[1]);
  if(pspec)
  sscanf(av[2],"%x",&FileZoomPos);
  if((cline&2)&&ac>2) //If bit 2 of cline is set and arg count is larger than 2
  {
    filearg=1;
    while(filearg<ac)
    {
      if(av[filearg][0]!='-')
      {
        break;
      }
      filearg++;
    }
    if(filearg+1==ac)
    {
      *fileName=0;
      if(!GetOpenFileName(&ofn))return 0;
    }else{
      strcpy(fileName,av[filearg+1]);
    }
    fileM=CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    SetFilePointer(fileM,0xAC,NULL,FILE_BEGIN);
    ZeroMemory(determineMode,sizeof(determineMode));
    ReadFile(fileM,&determineMode,3,&read,NULL);
    if(determineMode[0]=='A'&&determineMode[1]=='X')
    {
      mode=RUBY;
    }
    if(determineMode[0]=='B'&&determineMode[1]=='P'&&determineMode[2]=='E')
    {
      search=0;
    }
    SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
    ReadFile(fileM,&determineMode,3,&read,NULL);
    if((determineMode[0]=='A'&&determineMode[1]=='A')||(determineMode[0]=='S'&&determineMode[1]=='M'))
    {
      mode=GOLD;
      search=0;
    }
    SetFilePointer(fileM,0x0,NULL,FILE_BEGIN);
    ReadFile(fileM,&determineMode,4,&read,NULL);
    if(determineMode[0]=='N'&&determineMode[1]=='A'&&determineMode[2]=='R'&&determineMode[3]=='C')
    {
      mode=DIAMOND;
      search=0;
    }
    CloseHandle(fileM);
    RecodeProc(av[filearg],fileName);
    if(!DontShowLog)
      ShellExecute(NULL,NULL,"PokeScrE.log",NULL,NULL,SW_SHOWNORMAL);
    return 0;
  }
  else if(cline&2) //if there was a syntax error
  {
    printf("Syntax error: use syntax pokescriptview -r SCRIPT FILE\n");
    return 1;
  }
  if(!fspec)puts("Pokemon Script Viewer - Designed for Pokemon Fire Red\nBy Charles Daffern.\n\n\
This program comes with ABSOLUTELY NO WARRANTY; for details pass argument `--ver'.\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; pass argument `--ver' for details.\n\nPass argument --help for help.\n\n");
  if(!fspec)
  { //Not likely to be using cmd line.

    printf("Enter a file to send the script to, (relative to PKSV folder, w/o extension please)\nor leave blank to see on-screen: ");
    gets(fsend); //waah not the gets! NOT THE GETS!!!!!!!!oneone
    if(strcmp(fsend,"")&&strcmp(fsend,"\""))
    {
      strcat(fsend,".pks");
      if(fsend[0]=='"'&&fsend[strlen(fsend)-1]=='"')
      {
        fsend[strlen(fsend)-1]=0;
        scrf=CreateFile(fsend+1,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
      }
      else
        scrf=CreateFile(fsend,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    }
    printf("Please choose a file to open.\n");
  }
  if(fspec!=0||GetOpenFileName(&ofn))
  {
    if(!fspec)puts("Ok...");
    fileM=CreateFile(ofn.lpstrFile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    //Wow, opening a file is complicated.
    SetFilePointer(fileM,0xAC,NULL,FILE_BEGIN);
    ZeroMemory(determineMode,sizeof(determineMode));
    ReadFile(fileM,&determineMode,3,&read,NULL);
    if(determineMode[0]=='A'&&determineMode[1]=='X')
    {
      mode=RUBY;
    }
    if(determineMode[0]=='B'&&determineMode[1]=='P'&&determineMode[2]=='E')
    {
      search=0;
    }
    SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
    ReadFile(fileM,&determineMode,3,&read,NULL);
    if((determineMode[0]=='A'&&determineMode[1]=='A')||(determineMode[0]=='S'&&determineMode[1]=='M'))
    { //SM = Pokemon Pri[SM]... a silly lockout hack
      mode=GOLD;
      search=0;
    }
    SetFilePointer(fileM,0x0,NULL,FILE_BEGIN);
    ReadFile(fileM,&determineMode,4,&read,NULL);
    if(determineMode[0]=='N'&&determineMode[1]=='A'&&determineMode[2]=='R'&&determineMode[3]=='C')
    {
      mode=DIAMOND;
      search=0;
    }
    //Go to AC, get 2 bytes, see if they make AX - then it's r/s.
    if(fileM!=INVALID_HANDLE_VALUE)
    {
      if(!pspec)
      {
        printf("File opened, preparing for scan.\nWithout typing the $ before the number,\nPlease enter the address to scan: ");
        FileZoomPos=0xffffffff;
        scanf("%x",&FileZoomPos);
      }
      if(FileZoomPos!=0xffffffff)
      {
        if((ac>3&&mode!=DIAMOND)||(ac>4&&mode==DIAMOND)){
          if(mode==DIAMOND)
          strcpy(fsend,av[4]);
          else
          strcpy(fsend,av[3]);
          scrf=CreateFile(fsend,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);}
        FileZoomPos=FileZoomPos&0x00ffffff;
        if(!pspec)printf("You chose 0x%X\n",FileZoomPos);
        if(type==0)
        {
          if(mode==DIAMOND)
          {
            pspec=1;
            sscanf(av[3],"%x",&FileZoomPos);
            sscanf(av[2],"%x",&narc);
            DecodeProc(fileM,narc,FileZoomPos,ofn.lpstrFile);
            CloseHandle(fileM);
            CloseHandle(scrf);
            return error;
          }
          else
          {
            DecodeProc(fileM,0,FileZoomPos,ofn.lpstrFile);
          }
        }
        else if(type==1)
        {
          transtxt(FileZoomPos,ofn.lpstrFile);
        }
        else if(type==2)
        {
          transbrl(FileZoomPos,ofn.lpstrFile);
        }
        else if(type==3)
        {
          transmove(FileZoomPos,ofn.lpstrFile);
        }
        if(type>0){puts(trans);}
      }
      else
      {
        puts("Enter a hex number! (Too late now!)");
        puts("Press any key to exit.");
      }
      CloseHandle(fileM);
    }
    else
    {
      puts("o.O! Could not open file.");
      error=1;
    }
  }
  else
  {
    puts("Ok, DON'T choose a file, then. See if I care.");
    error=1;
  }
  if(strcmp(fsend,""))
  {
    CloseHandle(scrf);
    printf("Press any key to exit.\n");
  }
  if(!pspec)
  {
    getch();
  }
  return error;
}

*/
