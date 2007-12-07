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
void DecodeProc(HANDLE fileM,unsigned int FileZoomPos,char*filename)
{
  unsigned int still_going,arg1,arg2,arg3,arg4,arg5,arg6,arg7;
  DWORD read;
  unsigned char command;
  char buf[1024],buf2[1024];
  printf("\n#org 0x%X\n",FileZoomPos);
  printf("'-----------------------------------\n");
  SetFilePointer(fileM,FileZoomPos&0x00ffffff,NULL,FILE_BEGIN);
  still_going=1;
  while (still_going)
  {
    ReadFile(fileM,&command,1,&read,NULL);
    if(read>0)
    {
#define GENERIC(x) printf("%s\n",x)
      switch(command)
      {
      case CMD_DISAPPEAR:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("disappear 0x%X\n",arg1);
        break;
      case CMD_GIVEEGG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("giveegg 0x%X\n",arg1);
        break;
      case CMD_STOREFURNITURE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("storefurniture 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_STOREATK:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("storeatk 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_STORECOMP:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("storecomp 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SLOTMACHINE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("slotmachine 0x%X\n",arg1);
        break;
      case CMD_STOREITEM:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("storeitem 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_9D:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        printf("CMD_9D 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_73:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("CMD_73 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_STORETEXT:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if((arg2&0xff000000)!=0x08000000)
        {
          printf("storetext 0x%X 0x%X 'Text not in ROM area\n",arg1,arg2);
        }
        else
        {
          printf("storetext 0x%X 0x%X ' %s\n",arg1,arg2,transtxt(arg2,filename));
          DoText(arg2);
        }
        break;
      case CMD_2F:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("CMD_2F 0x%X\n",arg1);
        break;
      case CMD_STOREVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("storevar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_DOWEATHER:
        GENERIC("doweather");
        break;
      case CMD_SETWEATHER:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        printf("setweather 0x%X\n",arg1);
        break;
      case CMD_BATTLE:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        printf("battle 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
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
        printf("setmaptile 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_END:
        still_going=0;
        GENERIC("end");
        break;
      case CMD_POKEMART:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("pokemart 0x%X\n",arg1);
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
        printf("writebytetooffset 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_CHECKFLAG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("checkflag 0x%X\n",arg1);
        break;
      case CMD_SPECIAL2:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("special2 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SETFLAG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("setflag 0x%X\n",arg1);
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
        printf("CMD_C3 0x%X\n",arg1);
        break;
      case CMD_JUMPIF:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        printf("if 0x%X jump 0x%X\n",arg1,arg2);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          printf("          'Jump not in ROM area\n");
        }
        break;
      case CMD_WARP:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        printf("warp 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_FB:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("CMD_FB 0x%X\n",arg1);
        break;
      case CMD_COPYSCRIPTBANKS:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        printf("copyscriptbanks 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREVARTOFARBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        printf("comparevartofarbyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("comparefarbytetovar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        printf("comparefarbytetobyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COPYBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        printf("copybyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SETVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("setvar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_MESSAGE:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        if((arg1&0xff000000)!=0x08000000)
        {
          printf("message 0x%X 'Text not in ROM area\n",arg1);
        }
        else
        {
          printf("message 0x%X ' %s\n",arg1,transtxt(arg1,filename));
          DoText(arg1);
        }
        break;
      case CMD_CALLIF:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        printf("if 0x%X call 0x%X\n",arg1,arg2);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          printf("          'Call not in ROM area\n");
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
          printf("compare 0x%X 0x%X\n",arg1,arg2);
        }
        else
        {
          printf("compare LASTRESULT 0x%X\n",arg2);
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
            printf("applymovement PLAYER 0x%X ' %s\n",arg2,transmove(arg2,filename));
            DoMove(arg2);
          }
          else
          {
            printf("applymovement 0x%X 0x%X ' %s\n",arg1,arg2,transmove(arg2,filename));
            DoMove(arg2);
          }
        }
        else
        {
          if (arg1==255)
          {
            printf("applymovement PLAYER 0x%X ' Not in ROM\n",arg2);
          }
          else
          {
            printf("applymovement 0x%X 0x%X ' Not in ROM\n",arg1,arg2);
          }
        }
        break;
      case CMD_RELEASE:
        GENERIC("release");
        break;
      case CMD_PAUSE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("pause 0x%X\n",arg1);
        break;
      case CMD_PAUSEEVENT:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("pauseevent 0x%X\n",arg1);
        break;
      case CMD_CALL:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("call 0x%X\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          Do(arg1);
        }
        else
        {
          printf("          'Call not in ROM area\n");
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
        printf("callstd %s ' %s\n",buf2,buf);
        break;
      case CMD_MSGBOX:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if (arg1==0)
        {
          if((arg2&0xff000000)!=0x08000000)
          {
            printf("msgbox 0x%X\n",arg2);
          }
          else
          {
            printf("msgbox 0x%X ' %s\n",arg2,transtxt(arg2,filename));
            DoText(arg2);
          }
        }
        else
        {
          printf("loadpointer 0x%X 0x%X\n",arg1,arg2);
        }
        if((arg2&0xff000000)!=0x08000000)
        {
          printf("          'Pointer not in ROM area\n");
        }
        break;
      case CMD_HASHPOKE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("hashpoke 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_SPECIAL:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("special 0x%X",arg1);
        if(arg1==0)
        {printf(" ' Heal Pokemon");}
        puts("");
        break;
      case CMD_FADESCREEN:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        printf("fadescreen 0x%X",arg1);
        if(arg1==0)
        {printf(" ' Fade back into the action");}
        else if (arg1==1)
        {printf(" ' Fade screen to black");}
        puts("");
        break;
      case CMD_FANFARE:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("fanfare 0x%X\n",arg1);
        break;
      case CMD_WAITFANFARE:
        GENERIC("waitfanfare");
        break;
      case CMD_BRAILLE:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("braille 0x%X\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename);
        }
        else
        {
          printf("          'Braille not in ROM area\n");
        }
        break;
      case CMD_D3:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("CMD_D3 0x%X\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename);
        }
        else
        {
          printf("          'Braille not in ROM area\n");
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
        printf("picture 0x%X\n",arg1);
        break;
      case CMD_NOP0:
        printf("nop0 ' #raw 0x0\n");
        break;
      case CMD_NOP1:
        printf("nop1 ' #raw 0x1\n");
        break;
      case CMD_JUMP:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("jump 0x%X\n",arg1);
        Do(arg1);
        still_going=0;
        break;
      case CMD_JUMPSTD:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        printf("jumpstd 0x%X\n",arg1);
        still_going=0;
        break;
      case CMD_KILLSCRIPT:
        GENERIC("killscript");
        break;
      case CMD_SETBYTE:
        arg1=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        printf("setbyte 0x%X\n",arg1);
        break;
      case CMD_COPYVARIFNOTZERO:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("copyvarifnotzero 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COPYVAR:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("copyvar 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_COMPAREVARTOBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        printf("comparevartobyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_LOADBYTEFROMPOINTER:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        printf("loadbytefrompointer 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_REMOVEITEM:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("removeitem 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_WAITSPECIAL:
        GENERIC("waitspecial");
        break;
      case CMD_CRY:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        printf("cry 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_PLAYSOUND:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("playsound 0x%X\n",arg1);
        break;
      case CMD_FADESOUND:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("fadesound 0x%X\n",arg1);
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
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("checkitem 0x%X\n",arg1);
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
        printf("trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4,arg5);
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
        printf("multichoice 0x%X 0x%X 0x%X 0x%X\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_ADDPOKEMON:
        arg1=0;
        arg2=0;
        arg3=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        printf("addpokemon 0x%X 0x%X 0x%X\n",arg1,arg2,arg3);
        break;
      case CMD_FADEDEFAULT:
        GENERIC("fadedefault");
        break;
      case CMD_CALLASM:
        arg1=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        printf("callasm 0x%X\n",arg1);
        break;
      case CMD_CLEARFLAG:
        arg1=0;
        ReadFile(fileM,&arg1,2,&read,NULL);
        printf("clearflag 0x%X\n",arg1);
        break;
      case CMD_COMPAREFARBYTETOFARBYTE:
        arg1=0;
        arg2=0;
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        printf("comparefarbytetofarbyte 0x%X 0x%X\n",arg1,arg2);
        break;
      case CMD_CLOSEMSG2:
        GENERIC("closemsg2");
        break;
      default:
        printf("#raw 0x%X\n",command);
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
#define nl() puts("")
  while(!AllDoneText())
  {
    nl();
    arg1=DoneText(FindNotDoneText());
    printf("#org 0x%X\n= %s\n",arg1,transtxt(arg1&0x00ffffff,filename));
  }
  while(!AllDoneMove())
  {
    nl();
    arg1=DoneMove(FindNotDoneMove());
    printf("#org 0x%X\nM %s\n",arg1,transmove(arg1&0x00ffffff,filename));
  }
}
