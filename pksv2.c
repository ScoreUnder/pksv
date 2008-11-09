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
#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
#include "windows.h"
#endif
#include <stdio.h>
#include "sulib.h"
#include "golddef.h"
#include "pokedef.h"
#define DECOMPILE 0
#define RECOMPILE 1
#define TXT       2
#define MOVEMENT  3
#define BRAILLE   4
#define FIRE_RED  0
#define RUBY      1
#define GOLD      2
#define DIAMOND   3
char mode=FIRE_RED;
char IsVerbose=1;
unsigned char search=0xFF; //Free Space character
char eorg=0;
char testing=0;
FILE*LogFile;
char GlobBuf[65536];
signed int PointerToOffset(unsigned int ptr); //prototype
signed int OffsetToPointer(unsigned int offset);
#include "textproc.h"
#include "codeproc.h"

int main(unsigned int argc,char**argv)
{
  char determine_mode[4];
  char command_line=DECOMPILE;
  char*file_name;
  char*export_name;
  FILE*romfile;
  FILE*otherfile;
  unsigned int i;
  unsigned int file_location=0;
  unsigned int decompile_at;
  unsigned int narc;
  i=(unsigned int)strlen(argv[0]);
  while(argv[0][i]!='\\'&&argv[0][i]!='/'&&i!=0)
  {
    i--;
  }
  if(i==0){argv[0][0]=0;}
  argv[0][i+1]=0;
  strcpy(GlobBuf,argv[0]);
  //Argument Processing
  for(i=1;i<argc;i++)
  {
    if(argv[i][0]!='-')
    {
      if(file_location==0)
        file_location=i;
      continue;
    }
    if(!strcmp(argv[i],"-r"))
    {
      command_line=RECOMPILE;
    }
    else if(!strcmp(argv[i],"-t"))
    {
      command_line=TXT;
    }
    else if(!strcmp(argv[i],"-e"))
    {
      command_line=RECOMPILE;
      testing=1;
    }
    else if(!strcmp(argv[i],"-m"))
    {
      command_line=MOVEMENT;
    }
    else if(!strcmp(argv[i],"-b"))
    {
      command_line=BRAILLE;
    }
    else if(!strcmp(argv[i],"-d"))
    {
      command_line=DECOMPILE;
    }
    else if(!strcmp(argv[i],"--help"))
    {
      printf("PKSV - backend used for compiling/decompiling.\n");
    }
  }
  if(file_location==0)
  {
    puts("No file specified.");
    return 1;
  }
  //Filename, etc processing
  file_name=argv[file_location];
  if(command_line!=RECOMPILE)
  {
    sscanf(argv[file_location+1],"%x",&decompile_at);
    export_name=argv[file_location+2];
    if(argc<file_location+2)
    {
      printf("Please specify a filename to export the script to.\n");
      return 1;
    }
  }
  else
  {
    export_name=argv[file_location+1];
    if(argc<file_location+1)
    {
      return 1;
    }
  }
  if(command_line==RECOMPILE)
    romfile=fopen(export_name,"r+b");
  else
    romfile=fopen(file_name,"rb");
  if(!romfile)
  {
    if(command_line==RECOMPILE)
      printf("Unable to open file %s.",export_name);
    else
      printf("Unable to open file %s.",file_name);
    return 1;
  }
  //Determining mode
  fseek(romfile,0xAC,SEEK_SET);
  fread(determine_mode,1,3,romfile);
  if(determine_mode[0]=='A'&&determine_mode[1]=='X')
  {
    mode=RUBY;
  }
  if(determine_mode[0]=='B'&&determine_mode[1]=='P')
  {
    mode=FIRE_RED;
  }
  if(determine_mode[0]=='B'&&determine_mode[1]=='P'&&determine_mode[2]=='E')
  {
    search=0;
  }
  fseek(romfile,0x13F,SEEK_SET);
  fread(determine_mode,1,2,romfile);
  if((determine_mode[0]=='A'&&determine_mode[1]=='A')||(determine_mode[0]=='S'&&determine_mode[1]=='M'))
  {
    mode=GOLD;
    search=0;
  }
  fseek(romfile,0x0,SEEK_SET);
  fread(determine_mode,1,4,romfile);
  if(determine_mode[0]=='N'&&determine_mode[1]=='A'&&determine_mode[2]=='R'&&determine_mode[3]=='C')
  {
    mode=DIAMOND;
    search=0;
  }
  if(command_line==TXT)
  {
    otherfile=fopen(export_name,"wb");
    transtxt(decompile_at,file_name);
    fwrite(trans,1,strlen(trans),otherfile);
    fclose(otherfile);
  }
  else if(command_line==MOVEMENT)
  {
    otherfile=fopen(export_name,"wb");
    transmove(decompile_at,file_name);
    fwrite(trans,1,strlen(trans),otherfile);
    fclose(otherfile);
  }
  else if(command_line==BRAILLE)
  {
    otherfile=fopen(export_name,"wb");
    transbrl(decompile_at,file_name,otherfile);
    fwrite(trans,1,strlen(trans),otherfile);
    fclose(otherfile);
  }
  else if(command_line==DECOMPILE)
  {
    otherfile=fopen(export_name,"wb");
    if(mode==DIAMOND)
    {
      sscanf(argv[file_location+2],"%x",&narc);
      export_name=argv[file_location+3];
      if(argc<file_location+3)
      {
        printf("Please specify a filename to export the script to.\n");
        return 1;
      }
      fclose(otherfile);
      otherfile=fopen(export_name,"wb");
      DecodeProc(romfile,narc,decompile_at,file_name,otherfile);
    }
    else
    {
      DecodeProc(romfile,0,decompile_at,file_name,otherfile);
    }
    fclose(otherfile);
  }
  else if(command_line==RECOMPILE)
  {
    RecodeProc(file_name,export_name);
#ifdef WIN32
    ShellExecute(NULL,NULL,"PokeScrE.log",NULL,NULL,SW_SHOWNORMAL);
#endif
  }
  fclose(romfile);
  return 0;
}
