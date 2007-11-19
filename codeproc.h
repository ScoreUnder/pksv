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

char defs[512][256];
unsigned int means[512];
int pos;
void Define(char*thing,unsigned int otherthing)
{
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
    i++;
  }
  return scr;
}

#define Defined(thing) ((WhatIs(thing)&&0)||!fail)
#define chr Script[i]
#define GetNum(x) GenForFunc(x,&i,LogFile,Script)
unsigned char gffs;
unsigned int GenForFunc(char*func,unsigned int*ii,HANDLE LogFile,char*Script) //Generates number for function
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
  if((chr>0x2F&&chr<0x3A)||chr=='$')
  {
    i++;
    if((chr=='x'&&Script[i-1]=='0')||Script[i-1]=='$')
    {
      if(chr=='x'){i++;}
      j=0;
      while(chr!=' '&&chr!='\n'&&chr!=0)
      {
        buf[j]=chr;
        i++;
        j++;
      }
      buf[j]=0;
      j=0;
      k=0;
      l=0;
      strcpy(buf2,"0123456789abcdef");
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
        sprintf(buf2,"Unknown integer value in %s\r\n",func);
        WriteFile(LogFile,buf2,strlen(buf2),&read,NULL);
        return 0;
      }
      k=atoi(buf2);
#ifdef VERBOSE_RECOMPILE
      sprintf(buf3,"   -> 0x%X\r\n",k);
      WriteFile(LogFile,buf3,strlen(buf3),&read,NULL);
#endif
    }
  }
  else
  {
    j=0;
    while(chr!=' '&&chr!='\n'&&chr!='\r'&&chr!=0)
    {
      buf3[j]=chr;
      i++;
      j++;
    }
    buf3[j]=0;
    if(Defined(buf3)){
      gffs=1;
      *ii=i;
#ifdef VERBOSE_RECOMPILE
      sprintf(buf2,"   -> %s\r\n      -> 0x%X\r\n",buf3,WhatIs(buf3));
      WriteFile(LogFile,buf2,strlen(buf2),&read,NULL);
#endif
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
