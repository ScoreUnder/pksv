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
#include "isdone.h"

char defs[4096][256];
unsigned int means[4096];
int pos;
void Define(char*thing,unsigned int otherthing)
{
  if(pos>4096){MessageBox(NULL,"Overflow of #DEFINEs - ask Score_Under for custom build\nto handle more #defines.","Error",0x10);return;}
  strcpy(defs[pos],thing);
  means[pos]=otherthing;
  pos++;
  return;
}

unsigned int fail;

char*LowerCase(char*orig)  //This is how you lowercase.
{
  int i=0;
  while(orig[i]!=0)
  {
    if(orig[i]>0x40&&orig[i]<0x5A)
    {
      orig[i]+=0x20;
    }
    i++;
  }
  return orig;
}

unsigned int WhatIs(char*thing)
{
  int i=0;
  fail=0;
  while(i<pos)
  {
    if(!strcmp(defs[i],thing))
    {
      return means[i];
    }
    i++;
  }
  fail=1;
  return 0;
}

char* RemAll0D(char*scr)
{
  int i=0;
  while(i<strlen(scr))
  {
    if(scr[i]==0x0d)
    {
      scr[i]=0x0a;
    }
    if(scr[i]=='\r')
    {
      scr[i]=0x20;
    }
    i++;
  }
  return scr;
}
unsigned int ffoff=0;
unsigned int FindFreeSpace(char*romname,unsigned int len)
{
  HANDLE RomFile;
  unsigned int i,j=0;
  unsigned char cr;
  DWORD read;
  RomFile=CreateFile(romname,GENERIC_READ,FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(mode==GOLD)
  {
    i=WhatIs("findfromgold")+ffoff;
  }
  else
  {
    i=WhatIs("findfrom")+ffoff;
  }
  SetFilePointer(RomFile,i,NULL,FILE_BEGIN);
  while(i<0x1000000)
  {
    ReadFile(RomFile,&cr,1,&read,NULL);
    if(cr==search)
    {j++;}else{j=0;}
    i++;
    if(j>len){break;} //Yes, larger than
  }
  ffoff=i-WhatIs("findfrom");
  i-=j;
  i++;
  CloseHandle(RomFile);
  return (0x08000000|i);
}

//Gold ptr<->offset functions
signed int PointerToOffset(unsigned int ptr)
{
  unsigned int pointer=0;
  unsigned int bank=0;
  unsigned int offset=0;
  bank=ptr&0xFF;
  pointer=(ptr&0xFFFF00)>>8;
  if(pointer<0x4000||pointer>0x7FFF)return -1;
  pointer&=0x3FFF;
  pointer|=(bank&3)<<14;
  bank>>=2;
  return pointer|(bank<<16);
}
signed int OffsetToPointer(unsigned int offset)
{
  unsigned int pointer=0;
  unsigned int bank=0;
  
  bank=((offset&0xFF0000)>>14);
  if(bank>0xFF){return -1;}
  if((offset&0xFF000000)){return -1;}
  pointer=offset&0xFFFF;
  bank|=((pointer&0xF000)>>14);
  pointer&=0x3FFF;
  pointer|=0x4000;
  return (pointer<<8)|bank;
}

#define Defined(thing) ((WhatIs(thing)&&0)||!fail)
#define chr Script[i]
#define GetNum(x) GenForFunc(x,&i,LogFile,Script,romfn)
unsigned char gffs;
unsigned int GenForFunc(char*func,unsigned int*ii,HANDLE LogFile,char*Script,char*romfn) //Generates number for function
{
  unsigned int j=0,k=0,l=0,i;
  DWORD read;
  char buf[1024],buf2[1024],buf3[1024];
  gffs=0;
  i=*ii;
  while(chr==' ')
  {
    i++;
  }
  if((chr>0x2F&&chr<0x3A)||chr=='$'||(chr=='f'&&Script[i+1]=='r'&&Script[i+2]=='e'&&Script[i+3]=='e'&&Script[i+4]=='s'&&Script[i+5]=='p'&&Script[i+6]=='a'&&Script[i+7]=='c'&&Script[i+8]=='e'))
  {
    i++;
    if((chr=='x'&&Script[i-1]=='0')||Script[i-1]=='$'||(chr=='r'&&Script[i-1]=='f'&&Script[i+1]=='e'&&Script[i+2]=='e'&&Script[i+3]=='s'&&Script[i+4]=='p'&&Script[i+5]=='a'&&Script[i+6]=='c'&&Script[i+7]=='e'))
    {
      j=0;
      if(chr=='r'){i--;}
      if(chr=='x'){i++;}
      while(chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
      {
        buf[j]=chr;
        i++;
        j++;
      }
      buf[j]=0;
      j=0;
      k=0;
      l=buf[9];
      buf[9]=0;
      strcpy(buf2,"0123456789abcdef");
      if(!strcmp(buf,"freespace")) // freespace(.+)?
      {
        buf[9]=l;
        if(l==0)
        {
          strcpy(buf3,"You did not specify the free space length - defaulting to 0x100");
          WriteFile(LogFile,buf3,strlen(buf3),&read,NULL);
          k=0x100;
        }
        else
        {
          l=9;
          while(buf[l]!=0)
          {
            k=k<<4;
            j=0;
            while(buf2[j]!=0&&buf2[j]!=buf[l])
            {
              j++;
            }
            if(buf2[j]==0||j>15){break;}
            k|=j;
            l++;
          }
        }
        k=FindFreeSpace(romfn,k);
        gffs=1;
        *ii=i;
        if((!Defined("findfrom")&&mode!=GOLD)||(!Defined("findfromgold")&&mode==GOLD))
        {gffs=0;}
        if (IsVerbose)
        {
          sprintf(buf3,"FS -> 0x%X\r\n",k);
          WriteFile(LogFile,buf3,strlen(buf3),&read,NULL);
        }
        return k;
      }
      buf[9]=l;
      l=0;
      while(buf[l]!=0)
      {
        k=k<<4;
        j=0;
        while(buf2[j]!=0&&buf2[j]!=buf[l])
        {
          j++;
        }
        if(buf2[j]==0||j>15){break;}
        k|=j;
        l++;
      }
      if (IsVerbose)
      {
        sprintf(buf3,"   -> 0x%X\r\n",k);
        WriteFile(LogFile,buf3,strlen(buf3),&read,NULL);
      }
    }
    else
    {
      i--;
      j=0;
      while(chr>0x2F&&chr<0x3A)
      {
        buf2[j]=chr;
        i++;
        j++;
      }
      buf2[j]=0;
      if(buf2[0]==0)
      {
        if(chr=='\n'||chr==0)
        {
          sprintf(buf2,"You need to enter more arguments to %s\r\n",func);
        }
        else
        {
          sprintf(buf2,"Unknown integer value in %s\r\n",func);
        }
        WriteFile(LogFile,buf2,strlen(buf2),&read,NULL);
        return 0;
      }
      k=atoi(buf2);
      if(IsVerbose)
      {
        sprintf(buf3,"   -> 0x%X\r\n",k);
        WriteFile(LogFile,buf3,strlen(buf3),&read,NULL);
      }
    }
  }
  else
  {
    j=0;
    while(chr!=' '&&chr!='\n'&&chr!='\''&&chr!=0)
    {
      buf3[j]=chr;
      i++;
      j++;
    }
    buf3[j]=0;
    if(Defined(buf3)){
      gffs=1;
      *ii=i;
      if(IsVerbose)
      {
        sprintf(buf2,"   -> %s\r\n      -> 0x%X\r\n",buf3,WhatIs(buf3));
        WriteFile(LogFile,buf2,strlen(buf2),&read,NULL);
      }
      return WhatIs(buf3);
    }
    sprintf(buf2,"Unknown value in %s (Value must be integer)\r\n",func);
    WriteFile(LogFile,buf2,strlen(buf2),&read,NULL);
    return 0;
  }
  gffs=1;
  *ii=i;
  return k;
}

#include "decompiler.h"
#include "recompiler.h"
