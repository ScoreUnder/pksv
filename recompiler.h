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
#define log(asd,fgh) WriteFile(LogFile,asd,fgh,&read,NULL)
#define aa(x) else if (!strcmp(buf,x))
#define ec() while(Script[i+1]==' '){i++;}if(Script[i+1]=='\''){while(Script[i+1]!='\n'&&Script[i+1]!=0){i++;}}if(Script[i+1]!='\n'){log("Extra characters on line. Ignoring.\r\n",37);}while(chr!='\n'){i++;}
#define rom(c,s) j=c;WriteFile(RomFile,&j,s,&read,NULL)
#define BASIC(x) rom(x,1)
#define vlog(x,y) vlogProc(LogFile,x,y)
void vlogProc(HANDLE LogFile,char*x,unsigned int y)
{
  DWORD read;
  if (IsVerbose){log(x,y);}
  return;
}
void RecodeProc(char*script,char*rom)
{
  HANDLE CurrFile,LogFile,IncFile,RomFile;
  char*Script;             //Whoops, used the same name for the filename.
                           //Use caps-lock carefully.
  char buf[1024],buf2[1024],buf3[1024];
  unsigned int fs,la,fst,i,line,j,k,l,organised=0,arg1,arg2,arg3,arg4,arg5;
  DWORD read;
  SetLastError(0);
  CurrFile=CreateFile(script,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  RomFile=CreateFile(rom,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  la=GetLastError();
  IncFile=CreateFile("pokeinc.txt",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(IncFile==INVALID_HANDLE_VALUE)
  {
    MessageBox(NULL,"The default includes (pokeinc.txt) could not be opened","Error",0x10);
    CloseHandle(CurrFile);
    return;
  }
  if(la)
  {
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,la,0,buf,1023,NULL);
    MessageBox(NULL,buf,"Error",0x40);
    CloseHandle(IncFile);
    return;
  }
  if(CurrFile!=INVALID_HANDLE_VALUE)
  {
    SetLastError(0);
    fs=GetFileSize(CurrFile,NULL);
    fst=GetFileSize(IncFile,NULL);
    la=GetLastError();
    if(la)
    {
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,la,0,buf,1023,NULL);
      MessageBox(NULL,buf,"Error",0x40);
      if(strcmp(script,""))CloseHandle(CurrFile);
      CloseHandle(IncFile);
      return;
    }
    Script=GlobalAlloc(GPTR,fs+fst+2);
    if(Script==NULL)
    {
      MessageBox(NULL,"GlobalAlloc() Failed to allocate memory for the script.","Error",0x10);
      if(strcmp(script,""))CloseHandle(CurrFile);
      CloseHandle(IncFile);
      return;
    }
    ReadFile(IncFile,Script,fst,&read,NULL);
    strcat(Script,"\n");
    ReadFile(CurrFile,(char*)(Script+fst+1),fs,&read,NULL);
    LogFile=CreateFile("PokeScrE.log",GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(LogFile==NULL)
    {
      MessageBox(NULL,"Failed to open a log.","Error",0x10);
      GlobalFree(Script);
      if(strcmp(script,""))CloseHandle(CurrFile);
      CloseHandle(IncFile);
      return;
    }
    WriteFile(LogFile,"Opened file.\r\n",14,&read,NULL);
    i=0;
    LowerCase(Script);
    RemAll0D(Script);
    while(i<strlen(Script))
    {
      switch(chr) //Behave differently according to char
      {
        case 0:  //E O Script
          return;
        case '\'':
          while(chr!='\n'&&chr!=0)
          {i++;}i--;break;
        case ' ':  //Ignore spaces
        case '\n':
          break;
        default:
          j=0;
          while(chr!=' '&&chr!='\n'&&chr!=0)
          {
            buf[j]=chr;
            i++;
            j++;
          }
          buf[j]=0;
//aa    is just shorthand for  "else if (!strcmp (buf, x))"
//ec    is End Command
//log   is self-explanatory
//rom   writes to rom
//basic is shorthand for rom(x,1)
          if(!strcmp(buf,"#define"))
          {
            vlog("#DEFINE\r\n",9);
            while(chr==' '){i++;}
            if(chr=='\n'||chr==0)
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            j=0;
            while(chr!=' '&&chr!='\n'&&chr!=0)
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0; //Append null
            sprintf(buf2,"   -> %s\r\n",buf);
            vlog(buf2,strlen(buf2));
            while(chr==' '){i++;} //The same old same old.
            if(chr=='\n'||chr==0)
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            k=GetNum("#DEFINE");
            if (!gffs){return;}
            Define(buf,k);
            ec();
          }
          aa("#org")
          {
            vlog("#ORG\r\n",6);
            k=GetNum("#org");
            if(!gffs){return;}
            SetFilePointer(RomFile,k&0x00ffffff,NULL,FILE_BEGIN);
            organised=1;
            ec();
          }
          aa("#raw")
          {
            vlog("#RAW\r\n",6);
            k=GetNum("#raw");
            if(!gffs){return;}
            BASIC(k);
            if(k>255)
            {
              BASIC(k>>8);
              if(k>65535)
              {
                BASIC(k>>16);
                if(k>16777215)
                {
                  BASIC(k>>24);
                }
              }
            }
            ec();
          }
          aa("end")             {BASIC(CMD_END);          ec();} //BASIC() writes 1 byte to ROM
          aa("return")          {BASIC(CMD_RETURN);       ec();}
          aa("nop")             {BASIC(CMD_NOP0);         ec();}
          aa("nop0")            {BASIC(CMD_NOP0);         ec();}
          aa("nop1")            {BASIC(CMD_NOP1);         ec();}
          aa("checkgender")     {BASIC(CMD_CHECKGENDER);  ec();}
          aa("jingle")          {BASIC(CMD_JINGLE);       ec();}
          aa("closemsg2")       {BASIC(CMD_CLOSEMSG2);    ec();}
          aa("faceplayer")      {BASIC(CMD_FACEPLAYER);   ec();}
          aa("lock")            {BASIC(CMD_LOCK);         ec();}
          aa("release")         {BASIC(CMD_RELEASE);      ec();}
          aa("closemsg")        {BASIC(CMD_CLOSEMSG);     ec();}
          aa("waitbutton")      {BASIC(CMD_WAITBUTTON);   ec();}
          aa("killscript")      {BASIC(CMD_KILLSCRIPT);   ec();}
          aa("waitfanfare")     {BASIC(CMD_WAITFANFARE);  ec();}
          aa("doweather")       {BASIC(CMD_DOWEATHER);    ec();}
          aa("waitspecial")     {BASIC(CMD_WAITSPECIAL);  ec();}
          aa("showmsg")         {BASIC(CMD_SHOWMSG);      ec();}
          aa("msgbox")
          {
            vlog("Msgbox\r\n",8);
            arg1=GetNum("msgbox");
            if(!gffs){return;}
            if((arg1&0xff000000)==0)
            {
              arg1|=0x08000000;
              sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
              vlog(buf3,strlen(buf3));
            }
            vlog("Finished msgbox\r\n",17);
            BASIC(CMD_MSGBOX);
            BASIC(0);
            rom(arg1,4);
            ec();
          }
          aa("loadpointer")
          {
            vlog("Loadpointer\r\n",13);
            arg1=GetNum("loadpointer");
            if(!gffs){return;}
            arg2=GetNum("loadpointer");
            if(!gffs){return;}
            if((arg2&0xff000000)==0)
            {
              arg2|=0x08000000;
              sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
              vlog(buf3,strlen(buf3));
            }
            vlog("Finished Loadpointer\r\n",22);
            BASIC(CMD_MSGBOX);
            BASIC(arg1);
            rom(arg2,4);
            ec();
          }
          aa("call")
          {
            vlog("Call\r\n",6);
            arg1=GetNum("call");
            if(!gffs){return;}
            if((arg1&0xff000000)==0)
            {
              arg1|=0x08000000;
              sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
              vlog(buf3,strlen(buf3));
            }
            vlog("Finished Call\r\n",15);
            BASIC(CMD_CALL);
            rom(arg1,4);
            ec();
          }
          aa("jump")
          {
            vlog("Jump\r\n",6);
            arg1=GetNum("jump");
            if(!gffs){return;}
            if((arg1&0xff000000)==0)
            {
              arg1|=0x08000000;
              sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
              vlog(buf3,strlen(buf3));
            }
            vlog("Finished Jump\r\n",15);
            BASIC(CMD_JUMP);
            rom(arg1,4);
            ec();
          }
          aa("setbyte")
          {
            vlog("Setbyte\r\n",9);
            arg1=GetNum("setbyte");
            if(!gffs){return;}
            vlog("Finished Setbyte\r\n",18);
            BASIC(CMD_SETBYTE);
            BASIC(arg1);
            ec();
          }
          aa("callstd")
          {
            vlog("Callstd\r\n",9);
            arg1=GetNum("callstd");
            if(!gffs){return;}
            vlog("Finished Callstd\r\n",18);
            BASIC(CMD_CALLSTD);
            BASIC(arg1);
            ec();
          }
          aa("jumpstd")
          {
            vlog("Jumpstd\r\n",9);
            arg1=GetNum("jumpstd");
            if(!gffs){return;}
            vlog("Finished Jumpstd\r\n",18);
            BASIC(CMD_JUMPSTD);
            BASIC(arg1);
            ec();
          }
          aa("if")
          {
            vlog("If\r\n",4);
            arg1=GetNum("if");
            if(!gffs){return;}
            while(chr==' '){i++;}
            j=0;
            while(chr!=' ')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0;
            if(!strcmp(buf,"jump")||!strcmp(buf,"goto"))
            {
              vlog("   -> JUMP\r\n",12);
              arg2=GetNum("if (jump)");
              if(!gffs){return;}
              if((arg2&0xff000000)==0)
              {
                arg2|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                vlog(buf3,strlen(buf3));
              }
              BASIC(CMD_JUMPIF);
            }
            else if(!strcmp(buf,"call"))
            {
              vlog("   -> CALL\r\n",12);
              arg2=GetNum("if (call)");
              if(!gffs){return;}
              if((arg2&0xff000000)==0)
              {
                arg2|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                vlog(buf3,strlen(buf3));
              }
              BASIC(CMD_CALLIF);
            }
            else
            {
              log("IF was not correctly used. Use if xx JUMP or if xx CALL.\r\n",58);
              return;
            }
            BASIC(arg1);
            rom(arg2,4);
            ec();
          }
          aa("writebytetooffset")
          {
            vlog("   -> WRITEBYTETOOFFSET\r\n",25);
            arg1=GetNum("WRITEBYTETOOFFSET");
            if(!gffs){return;}
            arg2=GetNum("WRITEBYTETOOFFSET");
            if(!gffs){return;}
            BASIC(CMD_WRITEBYTETOOFFSET);
            rom(arg1,1);
            rom(arg2,4);
            ec();
          }
          aa("loadbytefrompointer")
          {
            vlog("   -> LOADBYTEFROMPOINTER\r\n",25);
            arg1=GetNum("LOADBYTEFROMPOINTER");
            if(!gffs){return;}
            arg2=GetNum("LOADBYTEFROMPOINTER");
            if(!gffs){return;}
            BASIC(CMD_LOADBYTEFROMPOINTER);
            rom(arg1,1);
            rom(arg2,4);
            ec();
          }
          aa("special")
          {
            vlog("SPECIAL\r\n",9);
            arg1=GetNum("SPECIAL");
            if(!gffs){return;}
            BASIC(CMD_SPECIAL);
            rom(arg1,2);
            ec();
          }
          aa("special2")
          {
            vlog("SPECIAL2\r\n",10);
            arg1=GetNum("SPECIAL2");
            if(!gffs){return;}
            arg2=GetNum("SPECIAL2");
            if(!gffs){return;}
            BASIC(CMD_SPECIAL2);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("compare")
          {
            vlog("COMPARE\r\n",9);
            arg1=GetNum("COMPARE");
            if(!gffs){return;}
            arg2=GetNum("COMPARE");
            if(!gffs){return;}
            BASIC(CMD_COMPARE);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("copyvar")
          {
            vlog("COPYVAR\r\n",9);
            arg1=GetNum("COPYVAR");
            if(!gffs){return;}
            arg2=GetNum("COPYVAR");
            if(!gffs){return;}
            BASIC(CMD_COPYVAR);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("copyvarifnotzero")
          {
            vlog("COPYVARIFNOTZERO\r\n",9);
            arg1=GetNum("COPYVARIFNOTZERO");
            if(!gffs){return;}
            arg2=GetNum("COPYVARIFNOTZERO");
            if(!gffs){return;}
            BASIC(CMD_COPYVARIFNOTZERO);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("message")
          {
            vlog("MESSAGE\r\n",9);
            arg1=GetNum("MESSAGE");
            if(!gffs){return;}
            BASIC(CMD_MESSAGE);
            if((arg1&0xff000000)==0)
            {
              arg1|=0x08000000;
              sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
              vlog(buf3,strlen(buf3));
            }
            rom(arg1,4);
            ec();
          }
          aa("copyscriptbanks")
          {
            vlog("COPYSCRIPTBANKS\r\n",17);
            arg1=GetNum("COPYSCRIPTBANKS");
            if(!gffs){return;}
            arg2=GetNum("COPYSCRIPTBANKS");
            if(!gffs){return;}
            BASIC(CMD_COPYSCRIPTBANKS);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("copybyte")
          {
            vlog("COPYBYTE\r\n",10);
            arg1=GetNum("COPYBYTE");
            if(!gffs){return;}
            arg2=GetNum("COPYBYTE");
            if(!gffs){return;}
            BASIC(CMD_COPYBYTE);
            rom(arg1,4);
            rom(arg2,4);
            ec();
          }
          aa("setvar")
          {
            vlog("SETVAR\r\n",8);
            arg1=GetNum("SETVAR");
            if(!gffs){return;}
            arg2=GetNum("SETVAR");
            if(!gffs){return;}
            BASIC(CMD_SETVAR);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("comparevartobyte")
          {
            vlog("COMPAREVARTOBYTE\r\n",18);
            arg1=GetNum("COMPAREVARTOBYTE");
            if(!gffs){return;}
            arg2=GetNum("COMPAREVARTOBYTE");
            if(!gffs){return;}
            BASIC(CMD_COMPAREVARTOBYTE);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("comparevartofarbyte")
          {
            vlog("COMPAREVARTOFARBYTE\r\n",21);
            arg1=GetNum("COMPAREVARTOFARBYTE");
            if(!gffs){return;}
            arg2=GetNum("COMPAREVARTOFARBYTE");
            if(!gffs){return;}
            BASIC(CMD_COMPAREVARTOFARBYTE);
            rom(arg1,2);
            rom(arg2,4);
            ec();
          }
          aa("comparefarbytetovar")
          {
            vlog("COMPAREFARBYTETOVAR\r\n",21);
            arg1=GetNum("COMPAREFARBYTETOVAR");
            if(!gffs){return;}
            arg2=GetNum("COMPAREFARBYTETOVAR");
            if(!gffs){return;}
            BASIC(CMD_COMPAREFARBYTETOVAR);
            rom(arg1,4);
            rom(arg2,2);
            ec();
          }
          aa("comparefarbytetobyte")
          {
            vlog("COMPAREFARBYTETOBYTE\r\n",22);
            arg1=GetNum("COMPAREFARBYTETOBYTE");
            if(!gffs){return;}
            arg2=GetNum("COMPAREFARBYTETOBYTE");
            if(!gffs){return;}
            BASIC(CMD_COMPAREFARBYTETOBYTE);
            rom(arg1,4);
            rom(arg2,1);
            ec();
          }
          aa("comparefarbytetofarbyte")
          {
            vlog("COMPAREFARBYTETOFARBYTE\r\n",25);
            arg1=GetNum("COMPAREFARBYTETOFARBYTE");
            if(!gffs){return;}
            arg2=GetNum("COMPAREFARBYTETOFARBYTE");
            if(!gffs){return;}
            BASIC(CMD_COMPAREFARBYTETOFARBYTE);
            rom(arg1,4);
            rom(arg2,4);
            ec();
          }
          aa("fadescreen")
          {
            vlog("FADESCREEN\r\n",12);
            arg1=GetNum("FADESCREEN");
            if(!gffs){return;}
            BASIC(CMD_FADESCREEN);
            rom(arg1,1);
            ec();
          }
          aa("callasm")
          {
            vlog("CALLASM\r\n",9);
            arg1=GetNum("CALLASM");
            if(!gffs){return;}
            BASIC(CMD_CALLASM);
            rom(arg1,4);
            ec();
          }
          aa("pause")
          {
            vlog("PAUSE\r\n",7);
            arg1=GetNum("PAUSE");
            if(!gffs){return;}
            BASIC(CMD_PAUSE);
            rom(arg1,2);
            ec();
          }
          aa("setflag")
          {
            vlog("SETFLAG\r\n",9);
            arg1=GetNum("SETFLAG");
            if(!gffs){return;}
            BASIC(CMD_SETFLAG);
            rom(arg1,2);
            ec();
          }
          aa("clearflag")
          {
            vlog("CLEARFLAG\r\n",11);
            arg1=GetNum("CLEARFLAG");
            if(!gffs){return;}
            BASIC(CMD_CLEARFLAG);
            rom(arg1,2);
            ec();
          }
          aa("checkflag")
          {
            vlog("CHECKFLAG\r\n",11);
            arg1=GetNum("CHECKFLAG");
            if(!gffs){return;}
            BASIC(CMD_CHECKFLAG);
            rom(arg1,2);
            ec();
          }
          else
          {
            sprintf(buf2,"Warning: Unknown command \"%s\"\r\n",buf);
            log(buf2,strlen(buf2));
            while(chr!='\n'&&chr!=0)
            {
              i++;
            }
          }
          break;
      }
      i++;
    }
    ///WriteFile(LogFile,stuff,fs,&read,NULL);
    GlobalFree(Script);
    CloseHandle(LogFile);
    CloseHandle(IncFile);
    if(strcmp(script,""))CloseHandle(CurrFile);
  }
  else
  {
    printf("Error opening script.");
    return;
  }
  return;
}
