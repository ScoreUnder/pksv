#ifndef __HAVE_SULIB
#define __HAVE_SULIB
#include <stdio.h>
#include <stdlib.h>

typedef struct __define {
  char*name;
  unsigned int means;
  struct __define*next;
} definition;

typedef struct __insert {
  unsigned int pos;
  unsigned int IsGoldDyn;
  char*name;
  struct __insert*next;
  struct __insert*prev;
} codeinsert;

typedef struct __block {
  unsigned int allocated,size;
  char*data;
  struct __block*next;
  struct __block*prev;
  char*name;
  struct __insert*insert;
  unsigned int org;
} codeblock;

unsigned int init_codeblock(codeblock*c,char*name)
{
  if (c==NULL)return 0;
  c->data=malloc(128);
  c->allocated=128;
  c->size=0;
  c->next=NULL;
  c->prev=NULL;
  if (name!=NULL)
  {
    c->name=malloc(strlen(name)+1);
    strcpy(c->name,name);
  }
  else
  {
    c->name=NULL;
  }
  c->insert=NULL;
  c->org=0;
#ifdef WIN32
  if (c->data==NULL)
  {
    MessageBox(NULL,"The computer appears to be out of memory.","Error",0x10);
    ExitProcess(0);
  }
#endif
  return (c->data!=NULL);
}

codeblock* rewind_codeblock(codeblock*c)
{
  codeblock*z;
  if (!c)return NULL;
  z=c;
  while (z->prev!=NULL)
  {
    z=z->prev;
  }
  return z;
}

codeblock* end_codeblock(codeblock*c)
{
  codeblock*z;
  z=c;
  while (z->next!=NULL)
  {
    z=z->next;
  }
  return z;
}

codeinsert* rewind_insert(codeinsert*c)
{
  codeinsert*z;
  z=c;
  while (z->prev!=NULL)
  {
    z=z->prev;
  }
  return z;
}

codeinsert* end_insert(codeinsert*c)
{
  codeinsert*z;
  z=c;
  while (z->next!=NULL)
  {
    z=z->next;
  }
  return z;
}
unsigned int GoldDyn=0;
unsigned int add_insert(codeblock*c,unsigned int p,char*n)
{
  codeinsert*z;
  z=c->insert;
  if (z!=NULL)
  {
    z=end_insert(z);
    z->next=malloc(sizeof(codeinsert));
    z=z->next;
    z->pos=p;
    z->next=NULL;
    z->prev=NULL;
    z->name=malloc(strlen(n)+1);
    strcpy(z->name,n);
    if (z==NULL)return 0;
  }
  if (z==NULL) //Create the first insert if needed
  {
    c->insert=malloc(sizeof(codeinsert));
    z=c->insert;
    z->pos=p;
    z->next=NULL;
    z->prev=NULL;
    z->name=malloc(strlen(n)+1);
    strcpy(z->name,n);
    if (z==NULL)return 0;
  }
  z->IsGoldDyn=GoldDyn;
  return 1;
}

void delete_inserts(codeblock*c)
{
  codeinsert*i;
  codeinsert*n;
  i=c->insert;
  c->insert=NULL;
  while (i!=NULL)
  {
    n=i->next;
    free(i->name);
    free(i);
    i=n;
  }
}

void add_insert_name(codeinsert*i,char*x)
{
  i->name=malloc(strlen(x)+1);
  strcpy(i->name,x);
}

void add_code_name(codeblock*i,char*x)
{
  i->name=malloc(strlen(x)+1);
  strcpy(i->name,x);
}

void delete_codeblock(codeblock*c)
{
  codeblock*p;
  codeblock*n;
  if (c==NULL)return;
  delete_inserts(c);
  p=c->prev;
  n=c->next;
  if (p==NULL&&n!=NULL)
  {
    memcpy(c,n,sizeof(codeblock));
    delete_inserts(n);
    free(n->data);
    free(n->name);
    free(n);
  }
  else if (p!=NULL)
  {
    p->next=n;
    if (n!=NULL)
      n->prev=p;
    free(c->data);
    free(c->name);
    free(c);
  }
}

unsigned int ffoff=0;
signed int OffsetToPointer(unsigned int offset);
unsigned int FindFreeSpace(char*romname,unsigned int len);
void calc_org(codeblock*c,unsigned int start,char*file)
{
  register unsigned int a;
  register codeblock*b;
  a=start;
  a|=0x08000000;
  b=rewind_codeblock(c);
  while (b!=NULL)
  {
    if (b->name!=NULL)
    {
      if (dyntype==1)
      {
        b->org=a;
        if ((mode==GOLD||mode==CRYSTAL)&&b->size<0x3FFF)
        {
          while ((OffsetToPointer(b->org)&0xFF)!=(OffsetToPointer(b->org+b->size)&0xFF))
          {
            a++;
            b->org=a;
          }
        }
        a+=b->size;
      }
      else
      {
        b->org=FindFreeSpace(file,b->size);
        if ((mode==GOLD||mode==CRYSTAL)&&b->size<0x3FFF)
        {
          while ((OffsetToPointer(b->org)&0xFF)!=(OffsetToPointer(b->org+b->size)&0xFF))
          {
            ffoff-=b->size-1;
            b->org=FindFreeSpace(file,b->size);
          }
        }
      }
    }
    b=b->next;
  }
}

void process_inserts(codeblock*c) //process inserts for everything.
{
  register codeblock*z;
  register codeblock*y;
  register codeinsert*x;
  unsigned int j;
  y=rewind_codeblock(c);
  while (y!=NULL)
  {
    x=y->insert;
    while (x!=NULL)
    {
      z=rewind_codeblock(c);
      while (z!=NULL)
      {
        if (z->name!=NULL)
          if (!strcmp(z->name,x->name))
          {
            if (x->IsGoldDyn)
            {
              j=OffsetToPointer(z->org);
              memcpy((void*)(y->data+x->pos),&j,3);
            }
            else
              memcpy((void*)(y->data+x->pos),&(z->org),4);
          }
        z=z->next;
      }
      x=x->next;
    }
    delete_inserts(y);
    y=y->next;
  }
}

void add_data(codeblock*c,char*data,unsigned int len)
{
  char*temp;
  while ((c->size)+len>c->allocated)
  {
    c->allocated=(2*c->allocated)+len;
    temp=malloc(c->allocated);
    memcpy(temp,c->data,c->size);
    free(c->data);
    c->data=temp;
  }
  memcpy((c->data)+(c->size),data,len);
  c->size+=len;
}

#endif//__HAVE_SULIB
