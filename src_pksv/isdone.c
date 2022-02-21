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

#include "isdone.h"

unsigned int doprocs[1024],doneprocs[2048];
unsigned int dotexts[1024],donetexts[2048];
unsigned int domoves[1024],donemoves[2048];
unsigned int domarts[1024],donemarts[2048];
unsigned int dothumb[1024],donethumb[2048];
unsigned int dodword[1024],donedword[2048];
unsigned int dolevel[1024],donelevel[2048];

void initDoneProcs(void)
{
  memset(doprocs,0,sizeof(doprocs));
  memset(doneprocs,0,sizeof(doneprocs));
  memset(dotexts,0,sizeof(dotexts));
  memset(donetexts,0,sizeof(donetexts));
  memset(domoves,0,sizeof(domoves));
  memset(donemoves,0,sizeof(donemoves));
  memset(domarts,0,sizeof(domarts));
  memset(donemarts,0,sizeof(donemarts));
  memset(dothumb,0,sizeof(dothumb));
  memset(donethumb,0,sizeof(donethumb));
  memset(dodword,0,sizeof(dodword));
  memset(donedword,0,sizeof(donedword));
  memset(dolevel,0,sizeof(dolevel));
  memset(donelevel,0,sizeof(donelevel));
}

unsigned int Do(unsigned int proc)
{
  register int i=0,j=0;
  if (proc==0xFFFFFFFF)return proc;
  while (i<1024)
  {
    if (doprocs[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (doneprocs[j]==proc)
          return proc;
        j++;
      }
      doprocs[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int Done(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (doprocs[i]==proc)
    {
      doprocs[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (doneprocs[i]==0)
    {
      doneprocs[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDone(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (doprocs[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (doneprocs[j]==doprocs[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDone(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (doprocs[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (doneprocs[j]==doprocs[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
        return doprocs[i];
    }
    i++;
  }
  return 0;
}
////////////////////////////////////////////////////////////
unsigned int DoText(unsigned int proc)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dotexts[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (donetexts[j]==proc)
          return proc;
        j++;
      }
      dotexts[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int DoneText(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (dotexts[i]==proc)
    {
      dotexts[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (donetexts[i]==0)
    {
      donetexts[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneText(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (dotexts[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donetexts[j]==dotexts[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneText(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dotexts[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donetexts[j]==dotexts[i])
        {
          break;
        }
        j++;
      }
      if (j == 2048)
        return dotexts[i];
    }
    i++;
  }
  return 0;
}
///////////////////////////////////////////////////////////////////
unsigned int DoMove(unsigned int proc)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (domoves[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (donemoves[j]==proc)
          return proc;
        j++;
      }
      domoves[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int DoneMove(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (domoves[i]==proc)
    {
      domoves[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (donemoves[i]==0)
    {
      donemoves[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneMove(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (domoves[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donemoves[j]==domoves[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneMove(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (domoves[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donemoves[j]==domoves[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
        return domoves[i];
    }
    i++;
  }
  return 0;
}
///////////////////////////////////////////////////////////////////
unsigned int DoMart(unsigned int proc)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (domarts[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (donemarts[j]==proc)
          return proc;
        j++;
      }
      domarts[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int DoneMart(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (domarts[i]==proc)
    {
      domarts[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (donemarts[i]==0)
    {
      donemarts[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneMart(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (domarts[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donemarts[j]==domarts[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneMart(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (domarts[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donemarts[j]==domarts[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
        return domarts[i];
    }
    i++;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
unsigned int DoThumb(unsigned int proc)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dothumb[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (donethumb[j]==proc)
          return proc;
        j++;
      }
      dothumb[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int DoneThumb(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (dothumb[i]==proc)
    {
      dothumb[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (donethumb[i]==0)
    {
      donethumb[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneThumb(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (dothumb[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donethumb[j]==dothumb[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneThumb(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dothumb[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donethumb[j]==dothumb[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
        return dothumb[i];
    }
    i++;
  }
  return 0;
}
///////////////////////////////////////////////////////////////////
unsigned int DoDword(unsigned int proc)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dodword[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (donedword[j]==proc)
          return proc;
        j++;
      }
      dodword[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int DoneDword(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (dodword[i]==proc)
    {
      dodword[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (donedword[i]==0)
    {
      donedword[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneDword(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (dodword[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donedword[j]==dodword[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneDword(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dodword[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donedword[j]==dodword[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
        return dodword[i];
    }
    i++;
  }
  return 0;
}
///////////////////////////////////////////////////////////////////
unsigned int DoLevel(unsigned int proc)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dolevel[i]==0)
    {
      j=0;
      while (j<2048)
      {
        if (donelevel[j]==proc)
          return proc;
        j++;
      }
      dolevel[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned int DoneLevel(unsigned int proc)
{
  register int i=0;
  while (i<1024)
  {
    if (dolevel[i]==proc)
    {
      dolevel[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while (i<2048)
  {
    if (donelevel[i]==0)
    {
      donelevel[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneLevel(void)
{
  register int i=0,j;
  while (i<1024)
  {
    if (dolevel[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donelevel[j]==dolevel[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneLevel(void)
{
  register int i=0,j=0;
  while (i<1024)
  {
    if (dolevel[i]!=0)
    {
      j=0;
      while (j<2048)
      {
        if (donelevel[j]==dolevel[i])
        {
          break;
        }
        j++;
      }
      if (j==2048)
        return dolevel[i];
    }
    i++;
  }
  return 0;
}




