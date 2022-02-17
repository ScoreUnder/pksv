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
#include <stdlib.h>
#include <stdint.h>

#include "codeproc.h"
#include "binarysearch.h"
#include "textutil.h"
#include "textproc.h"
#include "sulib.h"
#include "pksv.h"

const char ARG_END_CHARS[] = "\' ,\n";

unsigned int fail;

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

void pksv_codeproc_reset() {
  // Reset @dynamic numberings
  codenum=0;
  levelnum=0;
  textnum=0;
  movenum=0;
  martnum=0;
  thumbnum=0;
  dwordnum=0;

  // Reset associative arrays
  free(basedef2);
  basedef2 = NULL;

  for (size_t i = 0; i < def_size2; i++) {
    free(defnames2[i]);
  }
  free(defnames2);
  defnames2 = NULL;
  def_size2 = 0;
  def_alloc2 = 0;
}

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

void LowerCaseAndRemAll0D(char*orig)
{
  unsigned char a;

  while ((a = *orig))
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
  }
}

unsigned int FindFreeSpace(char*romname,unsigned int len, struct bsearch_root *defines)
{
  unsigned int filepos;
  unsigned int consecutive=0;
  unsigned char cr;
  uint32_t findfrom = 0;

  size_t index = bsearch_find_if_exists(defines,
      (mode == GOLD || mode == CRYSTAL) ? "findfromgold" : "findfrom"
  );
  if (index != defines->size)
  {
    findfrom = (uint32_t)(intptr_t) defines->pairs[index].value;
  }

  filepos = findfrom + ffoff;

  FILE *RomFile=fopen(romname,"rb");
  if (!RomFile)
  {
    perror("could not reopen rom to search for free space");
    return 0;
  }
  fseek(RomFile,filepos,SEEK_SET);
  while (filepos<0x1000000)
  {
    cr = getc(RomFile);
    filepos++;

    if (cr == search) {
      consecutive++;
    } else {
      consecutive=0;
    }

    if (consecutive>len) {
      // Yes, larger than, because text ends in 0xFF so we need one more byte
      break;
    }
  }
  fclose(RomFile);

  ffoff = filepos - findfrom;
  filepos -= consecutive;
  filepos++;
  return (0x08000000|filepos);
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

static void log_bad_int(const char *where)
{
  char *format = "Unknown value in %s (Value must be integer)\r\n";
  char *buf = malloc(strlen(where) + strlen(format) + 1);
  sprintf(buf, format, where);
  log_txt(buf, strlen(buf));
  free(buf);
}

#define Defined(thing) ((WhatIs(thing)&0)|!fail)
#define chr Script[i]
/// GenForFunc Success
unsigned char gffs;

/** @brief Gets a numeric value for a given pokescript function. */
uint32_t GenForFunc(char*func,
                        pos_int *ppos,
                        char*Script,
                        struct bsearch_root *defines,
                        codeblock*c)
{
  uint32_t result = 0;
  char buf[1024], log_buf[1024];
  gffs=0;
  pos_int i = skip_whitespace(Script, *ppos);

  if (chr=='@'||chr==':')
  {
    size_t j=0;
    while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
    {
      buf[j]=chr;
      i++;
      j++;
    }
    buf[j]=0;
    add_insert(c,c->size,buf);
    sprintf(log_buf,"DYN-> %s\r\n",buf);
    if(IsVerbose)
			log_txt(log_buf,strlen(log_buf));
    gffs=1;
    *ppos=i;
    return 0x08000000;
  }
  else if ((chr >= '0' && chr <= '9') || chr == '$')
  {
    const char *end;
    if ((Script[i] == '0' && Script[i + 1] == 'x') || Script[i] == '$') {
      if (Script[i] == '$') i++;
      else i += 2;

      end = hex_to_uint32(&Script[i], SIZE_MAX, &result);
    } else {
      end = dec_to_uint32(&Script[i], SIZE_MAX, &result);
    }

    i += end - &Script[i];

    if (strchr(ARG_END_CHARS, *end) == NULL && *end != '\0') {
      *ppos=i;
      log_bad_int(func);
      return 0;
    }

    if (IsVerbose) {
      sprintf(log_buf,"   -> 0x%X\r\n", result);
      log_txt(log_buf,strlen(log_buf));
    }

    gffs=1;
    *ppos=i;
    return result;
  }
  else
  {
    size_t j=0;
    while (chr!=' '&&chr!='\n'&&chr!='\''&&chr!=0)
    {
      buf[j]=chr;
      i++;
      j++;
    }
    buf[j]=0;
    *ppos=i;

    size_t index = bsearch_find_if_exists(defines, buf);
    if (index != defines->size)
    {
      gffs=1;
      uint32_t value = (uint32_t)(intptr_t) defines->pairs[index].value;
      if (IsVerbose)
      {
        sprintf(log_buf,"   -> %s\r\n      -> 0x%X\r\n",buf,value);
        log_txt(log_buf,strlen(log_buf));
      }
      return value;
    }
    else
    {
      log_bad_int(func);
      return 0;
    }
  }
}
