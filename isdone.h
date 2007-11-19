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
unsigned int doprocs[1024],doneprocs[2048];
unsigned int dotexts[1024],donetexts[2048];
unsigned int domoves[1024],donemoves[2048];
#define initDoneProcs() {\
ZeroMemory(doprocs,sizeof(doprocs));\
ZeroMemory(doprocs,sizeof(doneprocs));\
ZeroMemory(doprocs,sizeof(dotexts));\
ZeroMemory(doprocs,sizeof(donetexts));\
ZeroMemory(doprocs,sizeof(domoves));\
ZeroMemory(doprocs,sizeof(donemoves));\
}
//for debugging only:
#define crash() __asm("int $0x3\n")

unsigned int Do(unsigned int proc)
{
  int i=0,j=0;
  while(i<1024)
  {
    if(doprocs[i]==0)
    {
      j=0;
      while(j<2048)
      {
        if(doneprocs[j]==proc)
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
  int i=0;
  while(i<1024)
  {
    if(doprocs[i]==proc)
    {
      doprocs[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while(i<2048)
  {
    if(doneprocs[i]==0)
    {
      doneprocs[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDone()
{
  int i=0,j;
  while(i<1024)
  {
    if(doprocs[i]!=0)
    {
      j=0;
      while(j<2048)
      {
        if(doneprocs[j]==doprocs[i])
        {break;}
        j++;
      }
      if(j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDone()
{
  int i=0,j=0;
  while(i<1024)
  {
    if(doprocs[i]!=0)
    {
      j=0;
      while(j<2048)
      {
        if(doneprocs[j]==doprocs[i])
        {break;}
        j++;
      }
      if(doneprocs[j]!=doprocs[i])
        return doprocs[i];
    }
    i++;
  }
  return 0;
}
////////////////////////////////////////////////////////////
unsigned int DoText(unsigned int proc)
{
  int i=0,j=0;
  while(i<1024)
  {
    if(dotexts[i]==0)
    {
      j=0;
      while(j<2048)
      {
        if(donetexts[j]==proc)
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
  int i=0;
  while(i<1024)
  {
    if(dotexts[i]==proc)
    {
      dotexts[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while(i<2048)
  {
    if(donetexts[i]==0)
    {
      donetexts[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneText()
{
  int i=0,j;
  while(i<1024)
  {
    if(dotexts[i]!=0)
    {
      j=0;
      while(j<2048)
      {
        if(donetexts[j]==dotexts[i])
        {break;}
        j++;
      }
      if(j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneText()
{
  int i=0,j=0;
  while(i<1024)
  {
    if(dotexts[i]!=0)
    {
      j=0;
      while(j<2048)
      {
        if(donetexts[j]==dotexts[i])
        {break;}
        j++;
      }
      if(donetexts[j]!=dotexts[i])
        return dotexts[i];
    }
    i++;
  }
  return 0;
}
///////////////////////////////////////////////////////////////////
unsigned int DoMove(unsigned int proc)
{
  int i=0,j=0;
  while(i<1024)
  {
    if(domoves[i]==0)
    {
      j=0;
      while(j<2048)
      {
        if(donemoves[j]==proc)
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
  int i=0;
  while(i<1024)
  {
    if(domoves[i]==proc)
    {
      domoves[i]=0;
      break;
    }
    i++;
  }
  i=0;
  while(i<2048)
  {
    if(donemoves[i]==0)
    {
      donemoves[i]=proc;
      break;
    }
    i++;
  }
  return proc;
}
unsigned char AllDoneMove()
{
  int i=0,j;
  while(i<1024)
  {
    if(domoves[i]!=0)
    {
      j=0;
      while(j<2048)
      {
        if(donemoves[j]==domoves[i])
        {break;}
        j++;
      }
      if(j==2048)
      {
        return 0;
      }
    }
    i++;
  }
  return 1;
}
unsigned int FindNotDoneMove()
{
  int i=0,j=0;
  while(i<1024)
  {
    if(domoves[i]!=0)
    {
      j=0;
      while(j<2048)
      {
        if(donemoves[j]==domoves[i])
        {break;}
        j++;
      }
      if(donemoves[j]!=domoves[i])
        return domoves[i];
    }
    i++;
  }
  return 0;
}

