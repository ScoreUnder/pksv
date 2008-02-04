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

unsigned int chr_count(char chrs,char*string) //cannot use chr, defined as i...
{
  unsigned int i=0;
  while(*string!=0)
  {
    if(*string==chrs)
      i++;
    string++;
  }
  return i;
}

HANDLE scrf;

int(*func)(char*, ...);

int writescr(char*fmt, ...)
{
  DWORD read;
  va_list args;
  int retval;
  char buf[65536];
  va_start(args,fmt);
  retval=vsprintf(buf,fmt,args);
  WriteFile(scrf,buf,strlen(buf),&read,NULL);
  va_end(args);
  return retval;
}

void DecodeProc(HANDLE fileM,unsigned int FileZoomPos,char*filename)
{
  unsigned int still_going,arg1,arg2,arg3,arg4,arg5,arg6,arg7;
  DWORD read;
  unsigned char command;
  char buf[1024],buf2[1024];
  SetFilePointer(fileM,FileZoomPos&0x00ffffff,NULL,FILE_BEGIN);
  still_going=1;
  if(!strcmp(fsend,""))
  {
    func=(typeof(func))printf;
  }
  else
  {
    func=writescr;
  }
  func("\n#org 0x%X\n",FileZoomPos);
  func("'-----------------------------------\n");
  while (still_going)
  {
    ReadFile(fileM,&command,1,&read,NULL);
    if(read>0)
    {
#define GENERIC(x) func("%s\n",x)
      switch(command)
      {
      case CMD_17:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("CMD_17 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_96:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_96 0x%X\n",arg1);
        break;
      case CMD_COINCASETOVAR:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("coincasetovar 0x%X\n",arg1);
        break;
      case CMD_GIVETOCOINCASE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("givetocoincase %d ' Bear in mind, it's not in hex\n",arg1);
        break;
      case CMD_92:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_92 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_91:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_91 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_95:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_95 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_94:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_94 0x%X\n",arg1);
        break;
      case CMD_C1:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_C1 0x%X\n",arg1);
        break;
      case CMD_MULTICHOICE2:
        arg1=0;
        arg2=0;
        arg3=0;
        arg4=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        func("multichoice2 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_93:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("CMD_93 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_DISAPPEAR:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("disappear 0x%X\n",arg1);
        break;
      case CMD_GIVEEGG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("giveegg 0x%X\n",arg1);
        break;
      case CMD_STOREFURNITURE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storefurniture 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_STOREATK:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storeatk 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_STORECOMP:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storecomp 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SLOTMACHINE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("slotmachine 0x%X\n",arg1);
        break;
      case CMD_STOREITEM:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storeitem 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_9D:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("CMD_9D 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_73:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("CMD_73 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_STORETEXT:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if((arg2&0xff000000)!=0x08000000)
        {
          func("storetext 0x%X 0x%X 'Text not in ROM area\n",arg1,arg2);
        }
        else
        {
          func("storetext 0x%X 0x%X ' %s\n",arg1,arg2,transtxt(arg2,filename));
          DoText(arg2);
        }
        break;
      case CMD_2F:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_2F 0x%X\n",arg1);
        break;
      case CMD_STOREVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storevar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_DOWEATHER:
        GENERIC("doweather");
        break;
      case CMD_SETWEATHER:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("setweather 0x%X\n",arg1);
        break;
      case CMD_BATTLE:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("battle 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_SETMAPTILE:
        arg1=0;
        arg2=0;
        arg3=0;
        arg4=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        func("setmaptile 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_END:
        still_going=0;
        GENERIC("end");
        break;
      case CMD_POKEMART:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("pokemart 0x%X\n",arg1);
        break;
      case CMD_SHOWMSG:
        GENERIC("showmsg");
        break;
      case CMD_RETURN:
        still_going=0;
        GENERIC("return");
        break;
      case CMD_WRITEBYTETOOFFSET:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("writebytetooffset 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_CHECKFLAG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkflag 0x%X\n",arg1);
        break;
      case CMD_SPECIAL2:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("special2 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SETFLAG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setflag 0x%X\n",arg1);
        break;
      case CMD_LOCK:
        GENERIC("lock");
        break;
      case CMD_FACEPLAYER:
        GENERIC("faceplayer");
        break;
      case CMD_C3:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("CMD_C3 0x%X\n",arg1);
        break;
      case CMD_JUMPIF:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        switch(arg1)
        {
          case 0:
            strcpy(buf,"Smaller Than");
            break;
          case 1:
            strcpy(buf,"Equal To");
            break;
          case 2:
            strcpy(buf,"Larger Than");
            break;
          case 3:
            strcpy(buf,"Smaller Than or Equal To");
            break;
          case 4:
            strcpy(buf,"Larger Than or Equal To");
            break;
          case 5:
            strcpy(buf,"Not Equal To");
            break;
          default:
            strcpy(buf,"Unknown IF condition");
            break;
        }
        func("if 0x%X jump 0x%X ' %s\n",arg1,arg2,buf);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          func("          'Jump not in ROM area\n");
        }
        break;
      case CMD_WARP:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("warp 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_FB:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("CMD_FB 0x%X\n",arg1);
        break;
      case CMD_COPYSCRIPTBANKS:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("copyscriptbanks 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREVARTOFARBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("comparevartofarbyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("comparefarbytetovar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("comparefarbytetobyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COPYBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("copybyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SETVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("setvar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_MESSAGE:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        if((arg1&0xff000000)!=0x08000000)
        {
          func("message 0x%X 'Text not in ROM area\n",arg1);
        }
        else
        {
          func("message 0x%X ' %s\n",arg1,transtxt(arg1,filename));
          DoText(arg1);
        }
        break;
      case CMD_CALLIF:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        switch(arg1)
        {
          case 0:
            strcpy(buf,"Smaller Than");
            break;
          case 1:
            strcpy(buf,"Equal To");
            break;
          case 2:
            strcpy(buf,"Larger Than");
            break;
          case 3:
            strcpy(buf,"Smaller Than or Equal To");
            break;
          case 4:
            strcpy(buf,"Larger Than or Equal To");
            break;
          case 5:
            strcpy(buf,"Not Equal To");
            break;
          default:
            strcpy(buf,"Unknown IF condition");
            break;
        }
        func("if 0x%X call 0x%X\n",arg1,arg2,buf);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          func("          'Call not in ROM area\n");
        }
        break;
      case CMD_CHECKGENDER:
        GENERIC("checkgender");
        break;
      case CMD_COMPARE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        if (arg1!=0x800d)
        {
          func("compare 0x%X 0x%X\n",arg1,arg2);
        }
        else
        {
          func("compare LASTRESULT 0x%X\n",arg2);
        }
        break;
      case CMD_APPLYMOVEMENT:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if ((arg2&0xff000000)==0x08000000)
        {
          if (arg1==255)
          {
            func("applymovement PLAYER 0x%X ' %s\n",arg2,transmove(arg2,filename));
            DoMove(arg2);
          }
          else
          {
            func("applymovement 0x%X 0x%X ' %s\n",arg1,arg2,transmove(arg2,filename));
            DoMove(arg2);
          }
        }
        else
        {
          if (arg1==255)
          {
            func("applymovement PLAYER 0x%X ' Not in ROM\n",arg2);
          }
          else
          {
            func("applymovement 0x%X 0x%X ' Not in ROM\n",arg1,arg2);
          }
        }
        break;
      case CMD_RELEASE:
        GENERIC("release");
        break;
      case CMD_PAUSE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("pause 0x%X\n",arg1);
        break;
      case CMD_PAUSEEVENT:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("pauseevent 0x%X\n",arg1);
        break;
      case CMD_CALL:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("call 0x%X\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          Do(arg1);
        }
        else
        {
          func("          'Call not in ROM area\n");
        }
        break;
      case CMD_CALLSTD:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        strcpy(buf,"");
        sprintf(buf2,"0x%X",arg1);
        switch(arg1)
        {
          case 0:
            strcpy(buf2,"MSG_OBTAIN");
            strcpy(buf,"Obtained the XXXXXX!");
            break;
          case 1:
            strcpy(buf2,"MSG_FIND");
            strcpy(buf,"PLAYER found one XXXXXX!");
            break;
          case 4:
            strcpy(buf2,"MSG_NOCLOSE");
            strcpy(buf,"Non-closing msg");
            break;
          case 5:
            strcpy(buf2,"MSG_YESNO");
            strcpy(buf,"Yes/No msg");
            break;
          case 6:
            strcpy(buf2,"MSG_NORMAL");
            strcpy(buf,"Normal msg");
            break;
        }
        func("callstd %s ' %s\n",buf2,buf);
        break;
      case CMD_MSGBOX:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if (arg1==0)
        {
          if((arg2&0xff000000)!=0x08000000)
          {
            func("msgbox 0x%X\n",arg2);
          }
          else
          {
            func("msgbox 0x%X ' %s\n",arg2,transtxt(arg2,filename));
            DoText(arg2);
          }
        }
        else
        {
          func("loadpointer 0x%X 0x%X\n",arg1,arg2);
        }
        if((arg2&0xff000000)!=0x08000000)
        {
          func("          'Pointer not in ROM area\n");
        }
        break;
      case CMD_HASHPOKE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("hashpoke 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SPECIAL:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("special 0x%X",arg1);
        if(arg1==0)
        {func(" ' Heal Pokemon");}
        func("\n");
        break;
      case CMD_FADESCREEN:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("fadescreen 0x%X",arg1);
        if(arg1==0)
        {func(" ' Fade back into the action");}
        else if (arg1==1)
        {func(" ' Fade screen to black");}
        func("\n");
        break;
      case CMD_FANFARE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("fanfare 0x%X\n",arg1);
        break;
      case CMD_WAITFANFARE:
        GENERIC("waitfanfare");
        break;
      case CMD_BRAILLE:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("braille 0x%X\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename);
        }
        else
        {
          func("          'Braille not in ROM area\n");
        }
        break;
      case CMD_D3:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("CMD_D3 0x%X\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename);
        }
        else
        {
          func("          'Braille not in ROM area\n");
        }
        break;
      case CMD_WAITBUTTON:
        GENERIC("waitbutton");
        break;
      case CMD_CLOSEMSG:
        GENERIC("closemsg");
        break;
      case CMD_JINGLE:
        GENERIC("jingle");
        break;
      case CMD_PICTURE:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("picture 0x%X\n",arg1);
        break;
      case CMD_NOP0:
        func("nop0 ' #raw 0x0\n");
        break;
      case CMD_NOP1:
        func("nop1 ' #raw 0x1\n");
        break;
      case CMD_JUMP:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("jump 0x%X\n",arg1);
        Do(arg1);
        still_going=0;
        break;
      case CMD_JUMPSTD:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("jumpstd 0x%X\n",arg1);
        still_going=0;
        break;
      case CMD_KILLSCRIPT:
        GENERIC("killscript");
        break;
      case CMD_SETBYTE:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("setbyte 0x%X\n",arg1);
        break;
      case CMD_COPYVARIFNOTZERO:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("copyvarifnotzero 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COPYVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("copyvar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREVARTOBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("comparevartobyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_LOADBYTEFROMPOINTER:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("loadbytefrompointer 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_REMOVEITEM:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("removeitem 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_WAITSPECIAL:
        GENERIC("waitspecial");
        break;
      case CMD_CRY:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("cry 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_PLAYSOUND:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("playsound 0x%X\n",arg1);
        break;
      case CMD_FADESOUND:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("fadesound 0x%X\n",arg1);
        break;
      case CMD_FADEOUT:
        GENERIC("fadeout");
        break;
      case CMD_FADEIN:
        GENERIC("fadein");
        break;
      case CMD_COUNTPOKEMON:
        GENERIC("countpokemon");
        break;
      case CMD_CHECKITEM:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("checkitem 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_TRAINERBATTLE:
        arg1=0;
        arg2=0;
        arg3=0;
        arg4=0;
        arg5=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,4,&read,NULL);
        ReadFile(fileM,&arg5,4,&read,NULL);
        func("trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4,arg5);
        if((arg4&0x08000000)!=0)
        {
          DoText(arg4);
        }
        if((arg5&0x08000000)!=0)
        {
          DoText(arg5);
        }
        break;
      case CMD_MULTICHOICE:
        arg1=0;
        arg2=0;
        arg3=0;
        arg4=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        func("multichoice 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_ADDPOKEMON:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("addpokemon 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_FADEDEFAULT:
        GENERIC("fadedefault");
        break;
      case CMD_CALLASM:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("callasm 0x%X\n",arg1);
        break;
      case CMD_CLEARFLAG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("clearflag 0x%X\n",arg1);
        break;
      case CMD_COMPAREFARBYTETOFARBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("comparefarbytetofarbyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_CLOSEMSG2:
        GENERIC("closemsg2");
        break;
      default:
        func("#raw 0x%X\n",command);
        break;
      }
    }
    else
    {
      puts("'--EOF--");
      still_going=0;
    }
  }
  while(!AllDone())
  {
    DecodeProc(fileM,Done(FindNotDone()),filename);
  }
#define nl() func("\n")
  while(!AllDoneText())
  {
    nl();
    arg1=DoneText(FindNotDoneText());
    func("#org 0x%X\n= %s\n",arg1,transtxt(arg1&0x00ffffff,filename));
  }
  while(!AllDoneMove())
  {
    nl();
    arg1=DoneMove(FindNotDoneMove());
    func("#org 0x%X\nM %s\n",arg1,transmove(arg1&0x00ffffff,filename));
  }
}

