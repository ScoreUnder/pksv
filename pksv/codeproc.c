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
#include <string.h>

int*basedef=NULL;
char**defnames=NULL;

int def_alloc=0,def_size=0,PCS=0;

void Define(char*thing,unsigned int otherthing)
{
  char*m,*m2;
  
  if(def_alloc>(def_size<<2))
  {
    def_size++;
  }
  else
  {
    m=malloc(def_alloc+512*4);
    m2=malloc(def_alloc+512*4);
    //memcpy(edi,esi,ecx);
    memcpy(m,basedef,def_alloc);
    memcpy(m2,defnames,def_alloc);
    def_size++;
    def_alloc+=512*4;
    if(basedef)
      free(basedef);
    if(defnames)
      free(defnames);
    basedef=(int*)m;
    defnames=(char**)m2;
  }
  m=malloc(strlen(thing)+1);
  strcpy(m,thing);
  defnames[def_size-1]=m;
  basedef[def_size-1]=otherthing;
  return;
}
unsigned int fail;
unsigned int WhatIs(char*thing)
{
  register int i;
  fail=0;
  for(i=0;i<def_size;i++)
    if(!strcmp(thing,defnames[i]))
      return basedef[i];
  fail=1;
  return 0;
}

void ReDefine(char*thing,int val)
{
  register int i;
  fail=0;
  for(i=0;i<def_size;i++)
    if(!strcmp(thing,defnames[i]))
    {
      basedef[i]=val;
      return;
    }
  fail=1;
  return;
}

int*basedef2=NULL;
char**defnames2=NULL;

int def_alloc2=0;
int def_size2=0;

int codenum=0;
int levelnum=0;
int textnum=0;
int movenum=0;
int martnum=0;
int thumbnum=0;
int dwordnum=0;

void Define2(unsigned int otherthing,char*thing)
{
  char*m,*m2;
  
  if(def_alloc2>(def_size2<<2))
  {
    def_size2++;
  }
  else
  {
    m=malloc(def_alloc2+512*4);
    m2=malloc(def_alloc2+512*4);
    //memcpy(edi,esi,ecx);
    memcpy(m,basedef2,def_alloc2);
    memcpy(m2,defnames2,def_alloc2);
    def_size2++;
    def_alloc2+=512*4;
    if(basedef2)
      free(basedef2);
    if(defnames2)
      free(defnames2);
    basedef2=(int*)m;
    defnames2=(char**)m2;
  }
  m=malloc(strlen(thing)+1);
  strcpy(m,thing);
  defnames2[def_size2-1]=m;
  basedef2[def_size2-1]=otherthing;
  return;
}
unsigned int fail2;
char* WhatIs2(int thing)
{
  register int i;
  fail=0;
  for(i=0;i<def_size2;i++)
    if(thing==basedef2[i])
      return defnames2[i];
  fail=1;
  return 0;
}
#define Defined2(thing) (WhatIs2(thing),!fail)

char*LowerCase(char*orig)
{
  register unsigned int i=0;
  while (orig[i]!=0)
  {
    if (orig[i]>='A'&&orig[i]<='Z')
    {
      orig[i]+=0x20;
    }
    i++;
  }
  return orig;
}

char* RemAll0D(char*scr)
{
  register unsigned int i=0;
  while (i<strlen(scr))
  {
    if (scr[i]==0x0d)
    {
      scr[i]=0x0a;
    }
    i++;
  }
  return scr;
}

void LowerCaseAndRemAll0D(char*orig)
{
  register unsigned char a;
  a=*orig;
  while (a)
  {
    if (a>='A'&&a<='Z')
    {
      *orig+=0x20;
    }
    else if (a=='\r')
    {
      *orig='\n';
    }
    else if (a=='\t')
    {
      *orig=' ';
    }
    else if (a=='/'&&orig[1]=='/')
    {
      *orig='\'';
    }
    orig++;
    a=*orig;
  }
}

unsigned int FindFreeSpace(char*romname,unsigned int len)
{
  FILE*RomFile;
  unsigned int i,j=0;
  unsigned char cr;
  RomFile=fopen(romname,"rb");
  if (mode==GOLD)
  {
    i=WhatIs("findfromgold")+ffoff;
  }
  else
  {
    i=WhatIs("findfrom")+ffoff;
  }
  fseek(RomFile,i,SEEK_SET);
  while (i<0x1000000)
  {
    fread(&cr,1,1,RomFile);
    if (cr==search)
    {
      j++;
    } else {
      j=0;
    }
    i++;
    if (j>len) {
      break;    //Yes, larger than, because text ends in 0xFF
    }
  }
  ffoff=i-WhatIs("findfrom");
  i-=j;
  i++;
  fclose(RomFile);
  return (0x08000000|i);
}

//Gold ptr<->offset functions
signed int PointerToOffset(unsigned int ptr)
{
  unsigned int pointer=0;
  unsigned int bank=0;
  bank=ptr&0xFF;
  pointer=(ptr&0xFFFF00)>>8;
  if (pointer<0x4000||pointer>0x7FFF)return -1;
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
  if (bank>0xFF) {
    return -1;
  }
  if ((offset&0xFF000000)) {
    return -1;
  }
  pointer=offset&0xFFFF;
  bank|=((pointer&0xF000)>>14);
  pointer&=0x3FFF;
  pointer|=0x4000;
  return (pointer<<8)|bank;
}

#define Defined(thing) ((WhatIs(thing)&0)|!fail)
#define chr Script[i]
#ifndef DLL
#define GetNum(x) GenForFunc(x,&i,LogFile,Script,romfn,c)
#else
#define GetNum(x) GenForFunc(x,&i,Script,romfn,c)
#endif
unsigned char gffs;
unsigned int GenForFunc(char*func,
                        unsigned int*ii,
#ifndef DLL
                        FILE* LogFile,
#endif
                        char*Script,
                        char*romfn,
                        codeblock*c) //Generates number for function
{
  unsigned int j=0,k=0,l=0,i;
  //unsigned int read;
  char buf[1024],buf2[1024],buf3[1024];
  gffs=0;
  i=*ii;
  while (chr==' ')
  {
    i++;
  }
  if (chr=='@'||chr==':')
  {
    j=0;
    while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
    {
      buf[j]=chr;
      i++;
      j++;
    }
    buf[j]=0;
    add_insert(c,c->size,buf);
    sprintf(buf3,"DYN-> %s\r\n",buf);
    if(IsVerbose)
			log_txt(buf3,strlen(buf3));
    gffs=1;
    *ii=i;
    return 0x08000000;
  }
  if ((chr>0x2F&&chr<0x3A)||chr=='$'||(chr=='f'&&Script[i+1]=='r'&&Script[i+2]=='e'&&Script[i+3]=='e'&&Script[i+4]=='s'&&Script[i+5]=='p'&&Script[i+6]=='a'&&Script[i+7]=='c'&&Script[i+8]=='e'))
  {
    i++;
    if ((chr=='x'&&Script[i-1]=='0')||Script[i-1]=='$'||(chr=='r'&&Script[i-1]=='f'&&Script[i+1]=='e'&&Script[i+2]=='e'&&Script[i+3]=='s'&&Script[i+4]=='p'&&Script[i+5]=='a'&&Script[i+6]=='c'&&Script[i+7]=='e'))
    {
      j=0;
      if (chr=='r') {
        i--;
      }
      if (chr=='x') {
        i++;
      }
      while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
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
      if (!strcmp(buf,"freespace")) // freespace(.+)?
      {
        buf[9]=l;
        if (l==0)
        {
          strcpy(buf3,"You did not specify the free space length - defaulting to 0x100");
          log_txt(buf3,strlen(buf3));
          k=0x100;
        }
        else
        {
          l=9;
          while (buf[l]!=0)
          {
            k=k<<4;
            j=0;
            while (buf2[j]!=0&&buf2[j]!=buf[l])
            {
              j++;
            }
            if (buf2[j]==0||j>15) {
              break;
            }
            k|=j;
            l++;
          }
        }
        k=FindFreeSpace(romfn,k);
        gffs=1;
        *ii=i;
        if ((!Defined("findfrom")&&mode!=GOLD)||(!Defined("findfromgold")&&mode==GOLD))
        {
          gffs=0;
        }
        if (IsVerbose)
        {
          sprintf(buf3,"FS -> 0x%X\r\n",k);
          log_txt(buf3,strlen(buf3));
        }
        return k;
      }
      buf[9]=l;
      l=0;
      while (buf[l]!=0)
      {
        k=k<<4;
        j=0;
        while (buf2[j]!=0&&buf2[j]!=buf[l])
        {
          j++;
        }
        if (buf2[j]==0||j>15) {
          break;
        }
        k|=j;
        l++;
      }
      if (IsVerbose)
      {
        sprintf(buf3,"   -> 0x%X\r\n",k);
        log_txt(buf3,strlen(buf3));
      }
    }
    else
    {
      i--;
      j=0;
      while (chr>0x2F&&chr<0x3A)
      {
        buf2[j]=chr;
        i++;
        j++;
      }
      buf2[j]=0;
      if (buf2[0]==0)
      {
        sprintf(buf2,"You need to enter more arguments to %s\r\n",func);
        log_txt(buf2,strlen(buf2));
        return 0;
      }
      k=atoi(buf2);
      if (IsVerbose)
      {
        sprintf(buf3,"   -> 0x%X\r\n",k);
        log_txt(buf3,strlen(buf3));
      }
    }
  }
  else
  {
    j=0;
    while (chr!=' '&&chr!='\n'&&chr!='\''&&chr!=0)
    {
      buf3[j]=chr;
      i++;
      j++;
    }
    buf3[j]=0;
    if (Defined(buf3))
    {
      gffs=1;
      *ii=i;
      if (IsVerbose)
      {
        sprintf(buf2,"   -> %s\r\n      -> 0x%X\r\n",buf3,WhatIs(buf3));
        log_txt(buf2,strlen(buf2));
      }
      return WhatIs(buf3);
    }
    sprintf(buf2,"Unknown value in %s (Value must be integer)\r\n",func);
    log_txt(buf2,strlen(buf2));
    return 0;
  }
  gffs=1;
  *ii=i;
  return k;
}
