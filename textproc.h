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
char trans[65536];

char*transtxt(int howfar,char*file)
{
  unsigned int pt=0;
  unsigned char p;
  const char hex[17]="0123456789ABCDEF";
  int read;
  char still_going;
  HANDLE fileC;
  read=0;
  fileC=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(fileC!=INVALID_HANDLE_VALUE)
  {
    SetFilePointer(fileC,(howfar&0xffffff),NULL,FILE_BEGIN);
    still_going=1;
    while(still_going)
    {
      ReadFile(fileC,&p,1,(DWORD*)&read,NULL);
      if (read==0)
      {
        still_going=0;
      }
      else
      {
        if(p>=0xbb&&p<=0xd4) {p-=0x7a;} //capital letter
        else if(p>=0xd5&&p<=0xee) {p-=0x74;} //lower letter
        else if(p>=0xA1&&p<=0xAA) {p-=0x71;}
        else if(p==0) {p=0x20;} //space
        else if(p==0x1b){p='é';}
        else if(p==0xac){p='?';}
        else if(p==0xad){p='.';}
        else if(p==0xb8){p=',';}
        else if(p==0xb4){p='\'';}
        else if(p==0xfe) // \n
        {
          trans[pt]='\\';
          pt++;
          p='n';
        }
        else if(p==0xb7) // [p]
        {
          trans[pt]='[';
          pt++;
          trans[pt]='p';
          pt++;
          p=']';
        }
        else if(p==0xfa) // \l
        {
          trans[pt]='\\';
          pt++;
          p='l';
        }
        else if(p==0xfb) // \p
        {
          trans[pt]='\\';
          pt++;
          p='p';
        }
        else if(p==0xff)
        {
          p=0; //EOS
          still_going=0;
        }
        else if(p==0xf0) {p=':';}
        else if(p==0xfd)
        {
          trans[pt]='\\';
          pt++;
          trans[pt]='v';
          pt++;
          trans[pt]='\\';
          pt++;
          trans[pt]='h';
          pt++;
          ReadFile(fileC,&p,1,(DWORD*)&read,NULL);
          trans[pt]=hex[(p&0xf0)>>4];
          pt++;
          p=hex[p&0xf];
        }
        else if(p==0xab) {p='!';}
        else if(p==0xb0)
        {
          trans[pt]='[';
          pt++;
          trans[pt]='.';
          pt++;
          p=']';
        }
        else
        {
          trans[pt]='\\';
          pt++;
          trans[pt]='h';
          pt++;
          trans[pt]=hex[(p&0xf0)>>4];
          pt++;
          p=hex[p&0xf];
        }
        if(pt>=65525) //cut smaller for safety
        {
          trans[pt]=0;
          break;
        }
        trans[pt]=p;
        pt++;
      }
    }
    CloseHandle(fileC);
  }
  else
  {
    strcpy(trans,"Error in translating string...");
  }
  return trans;
}
////////////////////////////////////////////////////////////////////////////////////////////
char*transbrl(int howfar,char*file)
{
  unsigned int pt=0;
  unsigned char p;
  int read;
  char still_going;
  HANDLE fileC;
  read=0;
  fileC=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(fileC!=INVALID_HANDLE_VALUE)
  {
    SetFilePointer(fileC,(howfar&0xffffff),NULL,FILE_BEGIN);
    still_going=1;
    printf("        '");
    while(still_going)
    {
      ReadFile(fileC,&p,1,(DWORD*)&read,NULL);
      pt=0;
      if (p==0xff){still_going=0;break;}
      printf("  ");
      if(p&1) printf(".");
      else printf(" ");
      if(p&2) printf(".");
      else printf(" ");
    }
    printf("\n        '");
    SetFilePointer(fileC,(howfar&0xffffff),NULL,FILE_BEGIN);
    still_going=1;
    while(still_going)
    {
      ReadFile(fileC,&p,1,(DWORD*)&read,NULL);
      pt=0;
      if (p==0xff){still_going=0;break;}
      p=p>>2;
      printf("  ");
      if(p&1) printf(".");
      else printf(" ");
      if(p&2) printf(".");
      else printf(" ");
    }
    printf("\n        '");
    SetFilePointer(fileC,(howfar&0xffffff),NULL,FILE_BEGIN);
    still_going=1;
    while(still_going)
    {
      ReadFile(fileC,&p,1,(DWORD*)&read,NULL);
      pt=0;
      if (p==0xff){still_going=0;break;}
      p=p>>4;
      printf("  ");
      if(p&1) printf(".");
      else printf(" ");
      if(p&2) printf(".");
      else printf(" ");
    }
    puts("\n");  // Yes, this prints 2 new lines, not one.
    CloseHandle(fileC);
  }
  else
  {
    puts("          'Error in translating braille...");
  }
  return 0;
}

char*transmove(int howfar,HANDLE file)
{
  int read;
  char still_going;
  HANDLE fileC;
  unsigned char p;
  read=0;
  char buf[10];
  fileC=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(fileC!=INVALID_HANDLE_VALUE)
  {
    SetFilePointer(fileC,(howfar&0xffffff),NULL,FILE_BEGIN);
    still_going=1;
    strcpy(trans,"");
    while(still_going)
    {
      ReadFile(fileC,&p,1,(DWORD*)&read,NULL);
      if(mode==FIRE_RED)
      {
        switch(p)
        {
          case 254:strcat(trans,"end");still_going=0;break;
          case 0x1:strcat(trans,"look_up");break;
          case 0x2:strcat(trans,"look_left");break;
          case 0x3:strcat(trans,"look_right");break;
          case 0x4:strcat(trans,"look_down");break;
          case 0x8:strcat(trans,"walk_down_vslow");break;
          case 0x9:strcat(trans,"walk_up_vslow");break;
          case 0xA:strcat(trans,"walk_left_vslow");break;
          case 0xB:strcat(trans,"walk_right_vslow");break;
          case 0xC:strcat(trans,"walk_down_slow");break;
          case 0xD:strcat(trans,"walk_up_slow");break;
          case 0xE:strcat(trans,"walk_left_slow");break;
          case 0xF:strcat(trans,"walk_right_slow");break;
          case 0x10:strcat(trans,"walk_down");break;
          case 0x11:strcat(trans,"walk_up");break;
          case 0x12:strcat(trans,"walk_left");break;
          case 0x13:strcat(trans,"walk_right");break;
          case 0x14:strcat(trans,"jump_down2");break;
          case 0x15:strcat(trans,"jump_up2");break;
          case 0x16:strcat(trans,"jump_left2");break;
          case 0x17:strcat(trans,"jump_right2");break;
          case 0x1D:strcat(trans,"run_down");break;
          case 0x1E:strcat(trans,"run_up");break;
          case 0x1F:strcat(trans,"run_left");break;
          case 0x20:strcat(trans,"run_right");break;
          case 0x46:strcat(trans,"look_left_jump_down");break;
          case 0x47:strcat(trans,"look_down_jump_up");break;
          case 0x48:strcat(trans,"look_up_jump_left");break;
          case 0x49:strcat(trans,"look_left_jump_right");break;
          case 0x4A:strcat(trans,"faceplayer");break;
          case 0x4B:strcat(trans,"face_away");break;
          case 0x4E:strcat(trans,"jump_down1");break;
          case 0x4F:strcat(trans,"jump_up1");break;
          case 0x50:strcat(trans,"jump_left1");break;
          case 0x51:strcat(trans,"jump_right1");break;
          case 0x52:strcat(trans,"jump_down");break;
          case 0x53:strcat(trans,"jump_up");break;
          case 0x54:strcat(trans,"jump_left");break;
          case 0x55:strcat(trans,"jump_right");break;
          case 0x56:strcat(trans,"jump_downup");break;
          case 0x57:strcat(trans,"jump_updown");break;
          case 0x58:strcat(trans,"jump_leftright");break;
          case 0x59:strcat(trans,"jump_rightleft");break;
          case 0x60:strcat(trans,"invisible");break;
          case 0x62:strcat(trans,"say_!");break;
          case 0x63:strcat(trans,"say_?");break;
          case 0x64:strcat(trans,"say_X");break;
          case 0x65:strcat(trans,"say_!!");break;
          case 0x66:strcat(trans,"say_:)");break;
          default:sprintf(buf,"raw_%X",p);strcat(trans,buf);break;
        }
      }
      else
      {
        switch(p)
        {
          case 254:strcat(trans,"end");still_going=0;break;
          case 0x54:strcat(trans,"hide");break;
          case 0x55:strcat(trans,"show");break;
          case 0x56:strcat(trans,"alert");break;
          case 0x57:strcat(trans,"question");break;
          case 0x58:strcat(trans,"love");break;
          case 0x5A:strcat(trans,"pokeball");break;
          case 0x10:strcat(trans,"pause0");break;
          case 0x11:strcat(trans,"pause1");break;
          case 0x12:strcat(trans,"pause2");break;
          case 0x13:strcat(trans,"pause3");break;
          case 0x14:strcat(trans,"pause4");break;
          case 0x00:strcat(trans,"look_down");break;
          case 0x01:strcat(trans,"look_up");break;
          case 0x02:strcat(trans,"look_left");break;
          case 0x03:strcat(trans,"look_right");break;
          case 0x04:strcat(trans,"walk_down_vslow");break;
          case 0x05:strcat(trans,"walk_up_vslow");break;
          case 0x06:strcat(trans,"walk_left_vslow");break;
          case 0x07:strcat(trans,"walk_right_vslow");break;
          case 0x08:strcat(trans,"walk_down_slow");break;
          case 0x09:strcat(trans,"walk_up_slow");break;
          case 0x0A:strcat(trans,"walk_left_slow");break;
          case 0x0B:strcat(trans,"walk_right_slow");break;
          case 0x17:strcat(trans,"walk_down");break;
          case 0x18:strcat(trans,"walk_up");break;
          case 0x15:strcat(trans,"walk_left");break;
          case 0x16:strcat(trans,"walk_right");break;
          case 0x2D:strcat(trans,"walk_down_fast");break;
          case 0x2E:strcat(trans,"walk_up_fast");break;
          case 0x2F:strcat(trans,"walk_left_fast");break;
          case 0x30:strcat(trans,"walk_right_fast");break;
          case 0x35:strcat(trans,"run_down");break;
          case 0x36:strcat(trans,"run_up");break;
          case 0x37:strcat(trans,"run_left");break;
          case 0x38:strcat(trans,"run_right");break;
          case 0x7e:strcat(trans,"run_down_fast");break;
          case 0x7f:strcat(trans,"run_up_fast");break;
          case 0x80:strcat(trans,"run_left_fast");break;
          case 0x81:strcat(trans,"run_right_fast");break;
          default:sprintf(buf,"raw_%X",p);strcat(trans,buf);break;
        }
      }
      strcat(trans," ");
    }
    CloseHandle(fileC);
  }
  else
  {
    strcpy(trans,"Error in translating movement data!");
  }
  return trans;
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
char* transbackstr(char*scrfn,DWORD pos,HANDLE romfile)
{
  char*NewSpace,*ret;
  char cch;
  char str[65536];
  unsigned int i=0,j=0,k;
  DWORD read;
  HANDLE scrfile;
  char lb[5]; //Little Buffer
  
  scrfile=CreateFile(scrfn,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(pos!=0xFFFFFFFF)SetFilePointer(scrfile,pos,NULL,FILE_BEGIN);
  else              {MessageBox(NULL,"Darn file pointers. Always spoiling the fun.","Error",0x10);return;}
  if(scrfile==INVALID_HANDLE_VALUE){MessageBox(NULL,"Darn file handles. Always being complicated on purpose.","Error",0x10);return;}
  k=0;
  while(1)
  {
    ReadFile(scrfile,&cch,1,&read,NULL);
    if(read==0){MessageBox(0,"EOF","",0x40);break;}
    if(cch=='\n')break;
    if(cch=='\r')break;
    str[k]=cch;
    k++;
  }
  str[k]=0;
  ret=GlobalAlloc(GPTR,strlen(str)+1);
  strcpy(ret,str);
  NewSpace=GlobalAlloc(GPTR,strlen(str)+1);
  while(i<strlen(str))
  {
    if(str[i]>='A'&&str[i]<='Z'){NewSpace[j]=str[i]+0x7a;}
    else if(str[i]>='a'&&str[i]<='z'){NewSpace[j]=str[i]+0x74;}
    else if(str[i]>='0'&&str[i]<='9'){NewSpace[j]=str[i]+0x71;}
    else if(str[i]==' '){NewSpace[j]=0;}
    else if(str[i]=='é'){NewSpace[j]=0x1B;}
    else if(str[i]=='?'){NewSpace[j]=0xAC;}
    else if(str[i]=='.'){NewSpace[j]=0xAD;}
    else if(str[i]==','){NewSpace[j]=0xB8;}
    else if(str[i]=='\''){NewSpace[j]=0xB4;}
    else if(str[i]=='\\')
    {
      i++;
      if(str[i]=='n'){NewSpace[j]=0xFE;}
      else if (str[i]=='l'){NewSpace[j]=0xFA;}
      else if (str[i]=='p'){NewSpace[j]=0xFB;}
      else if (str[i]=='v'){NewSpace[j]=0xFD;}
      else if (str[i]=='h'){
        i++;lb[0]=str[i];i++;lb[1]=str[i];lb[2]=0;
        sscanf(lb,"%x",&k);
        k=k&0xff;
        NewSpace[j]=k;
      }
      else
      {
        i--;
      }
    }
    else if(str[i]==':'){NewSpace[j]=0xF0;}
    else if(str[i]=='!'){NewSpace[j]=0xAB;}
    else if(str[i]=='[')
    {
      i++;
      if(str[i]=='.')
      {
        i++;
        if(str[i]==']')
        {
          NewSpace[j]=0xB0;
        }else{i--;i--;}
      }
      else if(str[i]=='p')
      {
        i++;
        if(str[i]==']')
        {
          NewSpace[j]=0xB7;
        }else{i--;i--;}
      }else{i--;}
    }
    i++;
    j++;
  }
  NewSpace[j]=0xFF;
  j++;
  WriteFile(romfile,NewSpace,j,&read,NULL);
  GlobalFree(NewSpace);
  CloseHandle(scrfile);
  return ret;
}
