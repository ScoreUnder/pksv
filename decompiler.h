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
  if((FileZoomPos&0x0000ffff)==0x1337||(FileZoomPos&0x00ffffff)==1337||(FileZoomPos&0x0000ffff)==0x1EE7)
  {
    if((FileZoomPos&0x00ffffff)==1337)
    {
      func("'Address 1337 is officially L33T!\r\n");
    }
    else
    {
      func("'Address 0x%X is officially L33T!\r\n",(FileZoomPos&0x00ffffff));
    }
  }
  if(mode==GOLD)
  {
    func("#org 0x%X\r\n",FileZoomPos);
  }
  else
  {
    func("#org 0x%X\r\n",(FileZoomPos|0x08000000));
  }
  func("'-----------------------------------\r\n");
  if(mode!=GOLD)
  {
  while (still_going)
  {
    ReadFile(fileM,&command,1,&read,NULL);
    if(read>0)
    {
#define GENERIC(x) func("%s\r\n",x)
      arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
      switch(command)
      {
      case CMD_GIVEMONEY:
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("givemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_REAPPEARAT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("reappearat 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_MOVESPRITE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("movesprite 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_APPLYMOVEMENTFINISHAT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        if ((arg2&0xff000000)==0x08000000)
        {
          if (arg1==255)
          {
            func("applymovementfinishat PLAYER 0x%X 0x%X 0x%X ' %s\r\n",arg2,arg3,arg4,transmove(arg2,filename));
          }
          else if (arg1==0x800f)
          {
            func("applymovementfinishat LASTTALKED 0x%X 0x%X 0x%X ' %s\r\n",arg2,arg3,arg4,transmove(arg2,filename));
          }
          else if (arg1==0x7F)
          {
            func("applymovementfinishat CAMERA 0x%X 0x%X 0x%X ' %s\r\n",arg2,arg3,arg4,transmove(arg2,filename));
          }
          else
          {
            func("applymovementfinishat 0x%X 0x%X 0x%X 0x%X ' %s\r\n",arg1,arg2,arg3,arg4,transmove(arg2,filename));
          }
          DoMove(arg2);
        }
        else
        {
          if (arg1==255)
          {
            func("applymovementfinishat PLAYER 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg2,arg3,arg4);
          }
          else if (arg1==0x800f)
          {
            func("applymovementfinishat LASTTALKED 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg2,arg3,arg4);
          }
          else if (arg1==0x7f)
          {
            func("applymovementfinishat CAMERA 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg2,arg3,arg4);
          }
          else
          {
            func("applymovementfinishat 0x%X 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg1,arg2,arg3,arg4);
          }
        }
        break;
      case CMD_DISAPPEARAT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("disappearat 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_CHECKFURNITURE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkfurniture 0x%X\r\n",arg1);
        break;
      case CMD_TAKEFURNITURE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("takefurniture 0x%X\r\n",arg1);
        break;
      case CMD_CHECKITEMINPC:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("checkiteminpc 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_GIVEITEMTOPC:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("giveitemtopc 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_ADDFURNITURE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("addfurniture 0x%X\r\n",arg1);
        break;
      case CMD_CHECKITEMTYPE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkitemtype 0x%X\r\n",arg1);
        break;
      case CMD_CHECKIFROOMFORFURNITURE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkifroomforfurniture 0x%X\r\n",arg1);
        break;
      case CMD_PLAYSONG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("playsong 0x%X\r\n",arg1);
        break;
      case CMD_FARREAPPEAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("farreappear 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_FARDISAPPEAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("fardisappear 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_STOREPOKEMONVAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storepokemonvar 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETPOKEMONPP:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("setpokemonpp 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_MULTICHOICE3:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        ReadFile(fileM,&arg5,1,&read,NULL);
        func("multichoice3 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        break;
      case CMD_SHOWYESNO:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("showyesno 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETTRAINERFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("settrainerflag 0x%X\r\n",arg1);
        break;
      case CMD_STOREFIRSTPOKEMON:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("storefirstpokemon 0x%X\r\n",arg1);
        break;
      case CMD_POKEMART2:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("pokemart2 0x%X\r\n",arg1);
        break;
      case CMD_8A:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("CMD_8A 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_FADESCREENDELAY:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("fadescreendelay 0x%X 0x%X",arg1,arg2);
        switch(arg1)
        {
          case 0:
            func(" ' Fade back into the action from black");
            break;
          case 1:
            func(" ' Fade screen to black");
            break;
          case 2:
            func(" ' Fade back into the action from white");
            break;
          case 3:
            func(" ' Fade screen to white");
            break;
        }
        func("\r\n");
        break;
      case CMD_SETCATCHLOCATION:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("setcatchlocation 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STOREITEMS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("storeitems 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_WARPTELEPORT2:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warpteleport2 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warpteleport2 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
        break;
      case CMD_WARPMUTED:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warpmuted 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warpmuted 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARPWALKING:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warpwalking 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warpwalking 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_FALLDOWNHOLE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("falldownhole 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_WARPTELEPORT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warpteleport 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warpteleport 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARP3:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warp3 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warp3 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARPELEVATOR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warpelevator 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warpelevator 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARP4:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warp4 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warp4 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARP5:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warp5 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          func("warp5 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_GETPLAYERXY:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("getplayerxy 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_FADESONG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("fadesong 0x%X\r\n",arg1);
        break;
      case CMD_EXECUTERAM:
        GENERIC("executeram");
        break;
      case CMD_LASTBATTLE:
        GENERIC("lastbattle");
        break;
      case CMD_RESETWEATHER:
        GENERIC("resetweather");
        break;
      case CMD_STARTWIRELESS:
        GENERIC("startwireless");
        break;
      case CMD_CHOOSECONTESTPOKEMON:
        GENERIC("choosecontestpokemon");
        break;
      case CMD_STARTCONTEST:
        GENERIC("startcontest");
        break;
      case CMD_LASTTRAINERBATTLE:
        GENERIC("lasttrainerbattle");
        break;
      case CMD_ENDTRAINERBATTLE:
        GENERIC("endtrainerbattle");
        break;
      case CMD_ENDTRAINERBATTLE2:
        GENERIC("endtrainerbattle2");
        break;
      case CMD_TAKEFROMCOINCASE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("takefromcoincase 0x%X\r\n",arg1);
        break;
      case CMD_MSGBOX2:
        ReadFile(fileM,&arg2,4,&read,NULL);
        if((arg2&0xff000000)!=0x08000000)
        {
          func("msgbox2 0x%X\r\n",arg2);
        }
        else
        {
          func("msgbox2 0x%X ' %s\r\n",arg2,transtxt(arg2,filename));
          DoText(arg2);
        }
        if((arg2&0xff000000)!=0x08000000)
        {
          func("          'Pointer not in ROM area\r\n");
        }
        break;
      case CMD_B2:
        GENERIC("CMD_B2");
        break;
      case CMD_2D:
        GENERIC("CMD_2D");
        break;
      case CMD_2C:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("CMD_2C 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_RESETVARS:
        GENERIC("resetvars");
        break;
      case CMD_B1:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        func("CMD_B1 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_SETDOOROPENEDSTATIC:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("setdooropenedstatic 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETDOORCLOSEDSTATIC:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("setdoorclosedstatic 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_RESETSPRITELEVEL:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("resetspritelevel 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_CREATETEMPSPRITE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        ReadFile(fileM,&arg5,1,&read,NULL);
        ReadFile(fileM,&arg6,1,&read,NULL);
        func("createtempsprite 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
        break;
      case CMD_A6:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("CMD_A6 0x%X\r\n",arg1);
        break;
      case CMD_TEMPSPRITEFACE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("tempspriteface 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETOBEDIENCE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setobedience 0x%X\r\n",arg1);
        break;
      case CMD_CHECKOBEDIENCE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkobedience 0x%X\r\n",arg1);
        break;
      case CMD_COMPAREHIDDENVAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("checkobedience 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STOREBOXNAME:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storeboxname 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_WARP6:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        ReadFile(fileM,&arg5,2,&read,NULL);
        func("warp6 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        break;
      case CMD_MSGBOXSIGN:
        GENERIC("msgboxsign");
        break;
      case CMD_MSGBOXNORMAL:
        GENERIC("msgboxnormal");
        break;
      case CMD_SETHEALINGPLACE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("sethealingplace 0x%X\r\n",arg1);
        break;
      case CMD_SPRITEFACE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("spriteface 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_RANDOM:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("random 0x%X\r\n",arg1);
        break;
      case CMD_MOVEOFFSCREEN:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("moveoffscreen 0x%X\r\n",arg1);
        break;
      case CMD_CHECKTRAINERFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checktrainerflag 0x%X\r\n",arg1);
        break;
      case CMD_SETDOOROPENED:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("setdooropened 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETDOORCLOSED:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("setdoorclosed 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_DOORCHANGE:
        GENERIC("doorchange");
        break;
      case CMD_CALLSTDIF:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
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
        func("if 0x%X callstd 0x%X ' %s\r\n",arg1,arg2,buf);
        break;
      case CMD_SHOWPOKEPIC:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("showpokepic 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_HIDEPOKEPIC:
        func("hidepokepic\r\n");
        break;
      case CMD_SETMAPFOOTER:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setmapfooter 0x%X\r\n",arg1);
        break;
      case CMD_JUMPSTDIF:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
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
        func("if 0x%X jumpstd 0x%X ' %s\r\n",arg1,arg2,buf);
        break;
      case CMD_SETWORLDMAPFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setworldmapflag 0x%X\r\n",arg1);
        break;
      case CMD_LIGHTROOM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("lightroom 0x%X\r\n",arg1);
        break;
      case CMD_DARKENROOM:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("darkenroom 0x%X\r\n",arg1);
        break;
      case CMD_MOVESPRITE2:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("movesprite2 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_ADDITEM:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("additem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SPRITEBEHAVE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("spritebehave 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_UPDATECOINS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("updatecoins 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETBYTE2:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("setbyte2 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETFARBYTE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("setfarbyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_TEXTCOLOR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("textcolor 0x%X 'Sets text color (Fire Red).\r\n",arg1);
        break;
      case CMD_FAKEJUMPSTD:
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg1,1,&read,NULL);
          func("fakejumpstd 0x%X\r\n",arg1);
        }
        else
        {
          ReadFile(fileM,&arg1,4,&read,NULL);
          func("pokemart3 0x%X\r\n",arg1);
        }
        break;
      case CMD_COMPAREVARS:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        func("comparevars %s %s\r\n",buf,buf2);
        break;
      case CMD_COMPAREVARS2:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        func("comparevars2 %s %s\r\n",buf,buf2);
        break;
      case CMD_SUBTRACTVAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        func("subtractvar %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_CLEARTRAINERFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("cleartrainerflag 0x%X\r\n",arg1);
        break;
      case CMD_FE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("CMD_FE 0x%X\r\n",arg1);
        break;
      case CMD_ADDVAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        func("addvar %s %s\r\n",buf,buf2);
        break;
      case CMD_96:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("CMD_96 0x%X\r\n",arg1);
        break;
      case CMD_COINCASETOVAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        func("coincasetovar %s\r\n",buf);
        break;
      case CMD_GIVETOCOINCASE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("givetocoincase %d ' Bear in mind, it's not in hex\r\n",arg1);
        break;
      case CMD_CHECKMONEY:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("checkmoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_PAYMONEY:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("paymoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_UPDATEMONEY:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("updatemoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_HIDEMONEY:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("hidemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_HIDECOINS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("hidecoins 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SHOWCOINS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("showcoins 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_MULTICHOICE2:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        func("multichoice2 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_SHOWMONEY:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("showmoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_DISAPPEAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("disappear 0x%X\r\n",arg1);
        break;
      case CMD_REAPPEAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("reappear 0x%X\r\n",arg1);
        break;
      case CMD_GIVEEGG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("giveegg 0x%X\r\n",arg1);
        break;
      case CMD_STOREFURNITURE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storefurniture 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STOREATK:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storeatk 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STORECOMP:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storecomp 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SLOTMACHINE:
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg1,1,&read,NULL);
          func("fakecallstd 0x%X\r\n",arg1);
        }
        else if(mode==RUBY)
        {
          ReadFile(fileM,&arg1,2,&read,NULL);
          func("slotmachine 0x%X\r\n",arg1);
        }
        break;
      case CMD_STOREITEM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storeitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_DOANIMATION:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("doanimation 0x%X\r\n",arg1);
        break;
      case CMD_CHECKANIMATION:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkanimation 0x%X\r\n",arg1);
        break;
      case CMD_SETANIMATION:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("setanimation 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_HIDEBOX:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        func("hidebox 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_STORETEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if((arg2&0xff000000)!=0x08000000)
        {
          func("storetext 0x%X 0x%X 'Text not in ROM area\r\n",arg1,arg2);
        }
        else
        {
          func("storetext 0x%X 0x%X ' %s\r\n",arg1,arg2,transtxt(arg2,filename));
          DoText(arg2);
        }
        break;
      case CMD_SOUND:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("sound 0x%X\r\n",arg1);
        break;
      case CMD_STOREVAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg2);
        if(arg2==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        func("storevar 0x%X %s\r\n",arg1,buf);
        break;
      case CMD_DOWEATHER:
        GENERIC("doweather");
        break;
      case CMD_SETWEATHER:
        arg2=1;
        if(mode==FIRE_RED)arg2=2;
        ReadFile(fileM,&arg1,arg2,&read,NULL);
        func("setweather 0x%X\r\n",arg1);
        break;
      case CMD_BATTLE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        func("battle 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_SETMAPTILE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,2,&read,NULL);
        func("setmaptile 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_END:
        still_going=0;
        GENERIC("end");
        break;
      case CMD_POKEMART:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("pokemart 0x%X\r\n",arg1);
        break;
      case CMD_SHOWMSG:
        GENERIC("showmsg");
        break;
      case CMD_RETURN:
        still_going=0;
        GENERIC("return");
        break;
      case CMD_WRITEBYTETOOFFSET:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("writebytetooffset 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CHECKFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkflag 0x%X\r\n",arg1);
        break;
      case CMD_SPECIAL2:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("special2 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setflag 0x%X\r\n",arg1);
        break;
      case CMD_LOCK:
        GENERIC("lock");
        break;
      case CMD_FACEPLAYER:
        GENERIC("faceplayer");
        break;
      case CMD_C3:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("CMD_C3 0x%X\r\n",arg1);
        break;
      case CMD_JUMPIF:
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
        func("if 0x%X jump 0x%X ' %s\r\n",arg1,arg2,buf);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          func("          'Jump not in ROM area\r\n");
        }
        break;
      case CMD_WARP:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg4,2,&read,NULL);
          ReadFile(fileM,&arg5,2,&read,NULL);
          func("warp 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }
        else
        {
          func("warp 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_FB:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("CMD_FB 0x%X\r\n",arg1);
        break;
      case CMD_COPYSCRIPTBANKS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("copyscriptbanks 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COMPAREVARTOFARBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("comparevartofarbyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOVAR:
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("comparefarbytetovar 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOBYTE:
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("comparefarbytetobyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COPYBYTE:
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("copybyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETVAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        func("setvar %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_MESSAGE:
        ReadFile(fileM,&arg1,4,&read,NULL);
        if((arg1&0xff000000)!=0x08000000)
        {
          func("message 0x%X 'Text not in ROM area\r\n",arg1);
        }
        else
        {
          func("message 0x%X ' %s\r\n",arg1,transtxt(arg1,filename));
          DoText(arg1);
        }
        break;
      case CMD_CALLIF:
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
        func("if 0x%X call 0x%X ' %s\r\n",arg1,arg2,buf);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          func("          'Call not in ROM area\r\n");
        }
        break;
      case CMD_CHECKGENDER:
        GENERIC("checkgender");
        break;
      case CMD_COMPARE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        func("compare %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_APPLYMOVEMENT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if ((arg2&0xff000000)==0x08000000)
        {
          if (arg1==255)
          {
            func("applymovement PLAYER 0x%X ' %s\r\n",arg2,transmove(arg2,filename));
          }
          else if (arg1==0x800f)
          {
            func("applymovement LASTTALKED 0x%X ' %s\r\n",arg2,transmove(arg2,filename));
          }
          else if (arg1==0x7F)
          {
            func("applymovement CAMERA 0x%X ' %s\r\n",arg2,transmove(arg2,filename));
          }
          else
          {
            func("applymovement 0x%X 0x%X ' %s\r\n",arg1,arg2,transmove(arg2,filename));
          }
          DoMove(arg2);
        }
        else
        {
          if (arg1==255)
          {
            func("applymovement PLAYER 0x%X ' Not in ROM\r\n",arg2);
          }
          else if (arg1==0x800f)
          {
            func("applymovement LASTTALKED 0x%X ' Not in ROM\r\n",arg2);
          }
          else if (arg1==0x7f)
          {
            func("applymovement CAMERA 0x%X ' Not in ROM\r\n",arg2);
          }
          else
          {
            func("applymovement 0x%X 0x%X ' Not in ROM\r\n",arg1,arg2);
          }
        }
        break;
      case CMD_RELEASE:
        GENERIC("release");
        break;
      case CMD_PAUSE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("pause 0x%X\r\n",arg1);
        break;
      case CMD_PAUSEEVENT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("pauseevent 0x%X\r\n",arg1);
        break;
      case CMD_CALL:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("call 0x%X\r\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          Do(arg1);
        }
        else
        {
          func("          'Call not in ROM area\r\n");
        }
        break;
      case CMD_CALLSTD:
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
        func("callstd %s ' %s\r\n",buf2,buf);
        break;
      case CMD_MSGBOX:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        if (arg1==0)
        {
          if((arg2&0xff000000)!=0x08000000)
          {
            func("msgbox 0x%X\r\n",arg2);
          }
          else
          {
            func("msgbox 0x%X ' %s\r\n",arg2,transtxt(arg2,filename));
            DoText(arg2);
          }
        }
        else
        {
          func("loadpointer 0x%X 0x%X\r\n",arg1,arg2);
        }
        if((arg2&0xff000000)!=0x08000000)
        {
          func("          'Pointer not in ROM area\r\n");
        }
        break;
      case CMD_STOREPOKEMON:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("storepokemon 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SPECIAL:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("special 0x%X",arg1);
        if(arg1==0)
        {func(" ' Heal Pokemon");}
        func("\r\n");
        break;
      case CMD_FADESCREEN:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("fadescreen 0x%X",arg1);
        switch(arg1)
        {
          case 0:
            func(" ' Fade back into the action from black");
            break;
          case 1:
            func(" ' Fade screen to black");
            break;
          case 2:
            func(" ' Fade back into the action from white");
            break;
          case 3:
            func(" ' Fade screen to white");
            break;
        }
        func("\r\n");
        break;
      case CMD_FANFARE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("fanfare 0x%X\r\n",arg1);
        break;
      case CMD_WAITFANFARE:
        GENERIC("waitfanfare");
        break;
      case CMD_BRAILLE:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("braille 0x%X\r\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename);
        }
        else
        {
          func("          'Braille not in ROM area\r\n");
        }
        break;
      case CMD_D3:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("CMD_D3 0x%X\r\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename);
        }
        else
        {
          func("          'Braille not in ROM area\r\n");
        }
        break;
      case CMD_WAITBUTTON:
        GENERIC("waitbutton");
        break;
      case CMD_RELEASEALL:
        GENERIC("releaseall");
        break;
      case CMD_LOCKALL:
        GENERIC("lockall");
        break;
      case CMD_PICTURE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("picture 0x%X\r\n",arg1);
        break;
      case CMD_NOP0:
        func("nop0 ' #raw 0x0\r\n");
        break;
      case CMD_NOP1:
        func("nop1 ' #raw 0x1\r\n");
        break;
      case CMD_JUMP:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("jump 0x%X\r\n",arg1);
        Do(arg1);
        still_going=0;
        break;
      case CMD_JUMPSTD:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("jumpstd 0x%X\r\n",arg1);
        still_going=0;
        break;
      case CMD_KILLSCRIPT:
        GENERIC("killscript");
        break;
      case CMD_SETBYTE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("setbyte 0x%X\r\n",arg1);
        break;
      case CMD_COPYVARIFNOTZERO:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        func("copyvarifnotzero %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_COPYVAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        func("copyvar %s %s\r\n",buf,buf2);
        break;
      case CMD_COMPAREVARTOBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("comparevartobyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_LOADBYTEFROMPOINTER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("loadbytefrompointer 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_REMOVEITEM:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("removeitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_WAITSPECIAL:
        GENERIC("waitspecial");
        break;
      case CMD_WAITCRY:
        GENERIC("waitcry");
        break;
      case CMD_CRYFR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("cry 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CRY:
        if(mode!=FIRE_RED)
        {
          ReadFile(fileM,&arg1,1,&read,NULL);
          ReadFile(fileM,&arg2,2,&read,NULL);
          func("cry 0x%X 0x%X\r\n",arg1,arg2);
        }
        else
          GENERIC("checksound");
        break;
      case CMD_PLAYSOUND:
        ReadFile(fileM,&arg1,2,&read,NULL);
        if(mode==FIRE_RED)
        {
          ReadFile(fileM,&arg2,1,&read,NULL);
          func("playsound 0x%X 0x%X\r\n",arg1,arg2);
        }
        else
        {
          func("playsound 0x%X\r\n",arg1);
        }
        break;
      case CMD_FADEDEFAULT:
        if(mode==RUBY)
        {
          ReadFile(fileM,&arg1,2,&read,NULL);
          func("fadesound 0x%X\r\n",arg1);
        }
        else if(mode==FIRE_RED)
        {
          func("fadedefault\r\n");
        }
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
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("checkitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CHECKITEMAMOUNT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("checkitemamount 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CHECKATTACK:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkattack 0x%X\r\n",arg1);
        break;
      case CMD_TRAINERBATTLE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,4,&read,NULL);
        if(arg1!=3)
        {
          ReadFile(fileM,&arg5,4,&read,NULL);
        }
        if(arg1==1||arg1==2){
          ReadFile(fileM,&arg6,4,&read,NULL);
          func("trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
          if((arg6&0x08000000)!=0)
          {
            Do(arg6);
          }
        }
        else if (arg1==3)
        {
          func("trainerbattle 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        }
        else
        {
          func("trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }
        if((arg4&0xFF000000)==0x08000000)
        {
          DoText(arg4);
        }
        if((arg5&0xFF000000)==0x08000000)
        {
          DoText(arg5);
        }
        break;
      case CMD_MULTICHOICE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        func("multichoice 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_ADDPOKEMON:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,2,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        ReadFile(fileM,&arg5,4,&read,NULL);
        ReadFile(fileM,&arg6,4,&read,NULL);
        func("addpokemon 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
        break;
      case CMD_CALLASM:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("callasm 0x%X\r\n",arg1);
        break;
      case CMD_CALLASM2:
        ReadFile(fileM,&arg1,4,&read,NULL);
        func("callasm2 0x%X\r\n",arg1);
        break;
      case CMD_CLEARFLAG:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("clearflag 0x%X\r\n",arg1);
        break;
      case CMD_COMPAREFARBYTETOFARBYTE:
        ReadFile(fileM,&arg1,4,&read,NULL);
        ReadFile(fileM,&arg2,4,&read,NULL);
        func("comparefarbytetofarbyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CLOSEMSG:
        GENERIC("closemsg");
        break;
      default:
        func("#raw 0x%X\r\n",command);
        break;
      }
    }
    else
    {
      puts("'--EOF--");
      still_going=0;
    }
  }
  }
  else
  {
  while (still_going)
  {
    ReadFile(fileM,&command,1,&read,NULL);
    if(read>0)
    {
#define GENERIC(x) func("%s\r\n",x)
      arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
      switch(command)
      {
      case GLD_END:
        GENERIC("end");
        still_going=0;
        break;
      case GLD_RETURN:
        GENERIC("return");
        still_going=0;
        break;
      case GLD_RELOADANDRETURN:
        GENERIC("reloadandreturn");
        still_going=0;
        break;
      case GLD_2CALL:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("2call 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_3CALL:
        ReadFile(fileM,&arg1,3,&read,NULL);
        arg2=PointerToOffset(arg1);
        func("3call 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_2PTCALL:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg3=SetFilePointer(fileM,0,NULL,FILE_CURRENT);
        arg2=PointerToOffset((OffsetToPointer(arg3)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          SetFilePointer(fileM,arg2,NULL,FILE_BEGIN);
          ReadFile(fileM,&arg4,3,&read,NULL);
          SetFilePointer(fileM,arg3,NULL,FILE_BEGIN);
        }
        func("2ptcall 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        Do(arg4);
        break;
      case GLD_2JUMP:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("2jump 0x%X ' 0x%X\r\n",arg1,arg2);
        still_going=0;
        Do(arg2);
        break;
      case GLD_PRIORITYJUMP:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("priorityjump 0x%X ' 0x%X\r\n",arg1,arg2);
        still_going=0;
        Do(arg2);
        break;
      case GLD_3JUMP:
        ReadFile(fileM,&arg1,3,&read,NULL);
        arg2=PointerToOffset(arg1);
        func("3jump 0x%X ' 0x%X\r\n",arg1,arg2);
        still_going=0;
        Do(arg2);
        break;
      case GLD_2PTJUMP:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg3=SetFilePointer(fileM,0,NULL,FILE_CURRENT);
        arg2=PointerToOffset((OffsetToPointer(arg3)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          SetFilePointer(fileM,arg2,NULL,FILE_BEGIN);
          ReadFile(fileM,&arg4,3,&read,NULL);
          SetFilePointer(fileM,arg3,NULL,FILE_BEGIN);
        }
        func("2ptjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        still_going=0;
        Do(arg4);
        break;
      case GLD_PTPRIORITYJUMP:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg3=SetFilePointer(fileM,0,NULL,FILE_CURRENT);
        arg2=PointerToOffset((OffsetToPointer(arg3)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          SetFilePointer(fileM,arg2,NULL,FILE_BEGIN);
          ReadFile(fileM,&arg4,3,&read,NULL);
          SetFilePointer(fileM,arg3,NULL,FILE_BEGIN);
        }
        func("ptpriorityjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        still_going=0;
        Do(arg4);
        break;
      case GLD_EQBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("if == 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_NEQBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("if != 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_EQZERO:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("if ==0 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_NEQZERO:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("if !=0 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_LTBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("if < 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_GTBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("if > 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_JUMPSTD:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("jumpstd 0x%X\r\n",arg1);
        still_going=0;
        break;
      case GLD_CALLSTD:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("callstd 0x%X\r\n",arg1);
        break;
      case GLD_3CALLASM:
        ReadFile(fileM,&arg1,3,&read,NULL);
        func("3callasm 0x%X\r\n",arg1);
        break;
      case GLD_SPECIAL:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("special 0x%X\r\n",arg1);
        break;
      case GLD_2PTCALLASM:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg3=SetFilePointer(fileM,0,NULL,FILE_CURRENT);
        arg2=PointerToOffset((OffsetToPointer(arg3)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          SetFilePointer(fileM,arg2,NULL,FILE_BEGIN);
          ReadFile(fileM,&arg4,3,&read,NULL);
          SetFilePointer(fileM,arg3,NULL,FILE_BEGIN);
        }
        func("2ptcallasm 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        break;
      case GLD_CHECKMAPTRIGGERS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("checkmaptriggers 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_DOMAPTRIGGER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("domaptrigger 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_DOTRIGGER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("dotrigger 0x%X\r\n",arg1);
        break;
      case GLD_CHECKTRIGGERS:
        GENERIC("checktriggers");
        break;
      case GLD_LOADPIKADATA:
        GENERIC("loadpikadata");
        break;
      case GLD_LOADTRAINERSEEN:
        GENERIC("loadtrainerseen");
        break;
      case GLD_INTERPRETMENU:
        GENERIC("interpretmenu");
        break;
      case GLD_INTERPRETMENU2:
        GENERIC("interpretmenu2");
        break;
      case GLD_POKEPICYESORNO:
        GENERIC("pokepicyesorno");
        break;
      case GLD_HALLOFFAME:
        GENERIC("halloffame");
        break;
      case GLD_CREDITS:
        GENERIC("credits");
        break;
      case GLD_CHECKPHONECALL:
        GENERIC("checkphonecall");
        break;
      case GLD_HANGUP:
        GENERIC("hangup");
        break;
      case GLD_RESETFUNCS:
        GENERIC("resetfuncs");
        break;
      case GLD_FACEPLAYER:
        GENERIC("faceplayer");
        break;
      case GLD_OPENTEXTBOX:
        GENERIC("opentextbox");
        break;
      case GLD_PLAYRAMMUSIC:
        GENERIC("playrammusic");
        break;
      case GLD_PLAYMAPMUSIC:
        GENERIC("playmapmusic");
        break;
      case GLD_WARPSOUND:
        GENERIC("warpsound");
        break;
      case GLD_SPECIALSOUND:
        GENERIC("specialsound");
        break;
      case GLD_RELOADMAPMUSIC:
        GENERIC("reloadmapmusic");
        break;
      case GLD_KEEPTEXTOPEN:
        GENERIC("keeptextopen");
        break;
      case GLD_PASSTOENGINE:
        ReadFile(fileM,&arg1,3,&read,NULL);
        arg2=PointerToOffset(arg1);
        func("passtoengine 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHECKBIT1:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkbit1 0x%X\r\n",arg1);
        break;
      case GLD_SETBIT1:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setbit1 0x%X\r\n",arg1);
        break;
      case GLD_SETBIT2:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("setbit2 0x%X\r\n",arg1);
        break;
      case GLD_CHECKBIT2:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkbit2 0x%X\r\n",arg1);
        break;
      case GLD_CLEARBIT2:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("clearbit2 0x%X\r\n",arg1);
        break;
      case GLD_CLEARBIT1:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("clearbit1 0x%X\r\n",arg1);
        break;
      case GLD_WRITETEXT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("writetext 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=0xFFFFFFFF)
          DoText(arg2);
        break;
      case GLD_WRITETEXTBANK:
        ReadFile(fileM,&arg1,3,&read,NULL);
        arg2=PointerToOffset(arg1);
        func("writetext 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=0xFFFFFFFF)
          DoText(arg2);
        break;
      case GLD_REPEATTEXT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("repeattext 0x%X\r\n",arg1);
        break;
      case GLD_POKEPIC:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("pokepic 0x%X\r\n",arg1);
        break;
      case GLD_DESCRIBEDECORATION:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("describedecoration 0x%X\r\n",arg1);
        break;
      case GLD_FRUITTREE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("fruittree 0x%X\r\n",arg1);
        break;
      case GLD_SPECIALPHONECALL:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("specialphonecall 0x%X\r\n",arg1);
        break;
      case GLD_FACEPERSON:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("faceperson 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_VARIABLESPRITE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("variablesprite 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_APPLYMOVEMENT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        arg3=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg2<<8));
        func("applymovement 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_APPLYMOVEOTHER:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("applymoveother 0x%X ' 0x%X Applies movement to last talked\r\n",arg1,arg2);
        break;
      case GLD_VERBOSEGIVEITEM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("verbosegiveitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOADWILDDATA:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("loadwilddata 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOADTRAINER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("loadtrainer 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CATCHTUTORIAL:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("catchtutorial 0x%X\r\n",arg1);
        break;
      case GLD_TRAINERTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("trainertext 0x%X\r\n",arg1);
        break;
      case GLD_TRAINERSTATUS:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("trainerstatus 0x%X\r\n",arg1);
        break;
      case GLD_CLOSETEXT:
        GENERIC("closetext");
        break;
      case GLD_LOADMOVESPRITES:
        GENERIC("loadmovesprites");
        break;
      case GLD_POKEMART:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        func("pokemart 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_ELEVATOR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("elevator 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_YESORNO:
        GENERIC("yesorno");
        break;
      case GLD_RETURNAFTERBATTLE:
        GENERIC("returnafterbattle");
        break;
      case GLD_WAITBUTTON:
        GENERIC("waitbutton");
        break;
      case GLD_WRITEBACKUP:
        GENERIC("writebackup");
        break;
      case GLD_WILDOFF:
        GENERIC("wildoff");
        break;
      case GLD_WILDON:
        GENERIC("wildon");
        break;
      case GLD_ITEMNOTIFY:
        GENERIC("itemnotify");
        break;
      case GLD_POCKETISFULL:
        GENERIC("pocketisfull");
        break;
      case GLD_STARTBATTLE:
        GENERIC("startbattle");
        break;
      case GLD_CLEARFIGHT:
        GENERIC("clearfight");
        break;
      case GLD_TALKAFTERCANCEL:
        GENERIC("talkaftercancel");
        break;
      case GLD_TALKAFTER:
        GENERIC("talkafter");
        break;
      case GLD_TALKAFTERCHECK:
        GENERIC("talkaftercheck");
        break;
      case GLD_CHECKVER:
        GENERIC("checkver");
        break;
      case GLD_RELOADMAPPART:
        GENERIC("reloadmappart");
        break;
      case GLD_RELOADMAP:
        GENERIC("reloadmap");
        break;
      case GLD_DEACTIVATEFACING:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("deactivatefacing 0x%X\r\n",arg1);
        break;
      case GLD_NEWLOADMAP:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("newloadmap 0x%X\r\n",arg1);
        break;
      case GLD_WARPCHECK:
        GENERIC("warpcheck");
        break;
      case GLD_GIVEITEM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("giveitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_REFRESHSCREEN:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("refreshscreen 0x%X\r\n",arg1);
        break;
      case GLD_C1CELOADBYTE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("C1CEloadbyte 0x%X ' Apparently useless.\r\n",arg1);
        break;
      case GLD_SETLASTTALKED:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("setlasttalked 0x%X\r\n",arg1);
        break;
      case GLD_EARTHQUAKE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("earthquake 0x%X\r\n",arg1);
        break;
      case GLD_LOADVAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("loadvar 0x%X\r\n",arg1);
        break;
      case GLD_ADDVAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("addvar 0x%X\r\n",arg1);
        break;
      case GLD_RANDOM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("random 0x%X\r\n",arg1);
        break;
      case GLD_COPYBYTETOVAR:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("copybytetovar 0x%X\r\n",arg1);
        break;
      case GLD_COPYVARTOBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("copyvartobyte 0x%X\r\n",arg1);
        break;
      case GLD_CHANGEBLOCK:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("changeblock 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_SHOWEMOTE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("showemote 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_FOLLOW:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("follow 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_FOLLOWNOTEXACT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("follownotexact 0x%X 0x%X ' Follows, but without mindlessly copying movements.\r\n",arg1,arg2);
        break;
      case GLD_SWAPMAPS:
        ReadFile(fileM,&arg1,3,&read,NULL);
        func("swapmaps 0x%X ' 0x%X\r\n",arg1,PointerToOffeset(arg1));
        break;
      case GLD_CHANGEPERSONDIR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("changepersondir 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_MOVEPERSON:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("moveperson 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_WRITEPERSONLOC:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("writepersonloc 0x%X\r\n",arg1);
        break;
      case GLD_LOADEMOTE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("loademote 0x%X\r\n",arg1);
        break;
      case GLD_STOPFOLLOW:
        GENERIC("stopfollow");
        break;
      case GLD_PLAYSOUND:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("playsound 0x%X\r\n",arg1);
        break;
      case GLD_PLAYMUSIC:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("playmusic 0x%X\r\n",arg1);
        break;
      case GLD_CRY:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("cry 0x%X\r\n",arg1);
        break;
      case GLD_PAUSE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("pause 0x%X\r\n",arg1);
        break;
      case GLD_WARPMOD:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("warpmod 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_WARP:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        func("warp 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case GLD_WARPFACING:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        ReadFile(fileM,&arg5,1,&read,NULL);
        func("warpfacing 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        break;
      case GLD_BLACKOUTMOD:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("blackoutmod 0x%X 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOCATIONTOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("locationtotext 0x%X\r\n",arg1);
        break;
      case GLD_DISPLAYLOCATION:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("displaylocation 0x%X\r\n",arg1);
        break;
      case GLD_MONEYTOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("moneytotext 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_COINSTOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("coinstotext 0x%X\r\n",arg1);
        break;
      case GLD_VARTOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("vartotext 0x%X\r\n",arg1);
        break;
      case GLD_POKETOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("poketotext 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_ITEMTOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("itemtotext 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_TRAINERTOTEXT:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        func("trainertotext 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_STRINGTOTEXT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        arg3=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("stringtotext 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        if(arg3!=-1)
        {
          DoText(arg3);
        }
        break;
      case GLD_STORETEXT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        arg4=PointerToOffset((arg1<<8)|arg2);
        func("storetext 0x%X 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3,arg4);
        if(arg4!=-1)
        {
          DoText(arg4);
        }
        break;
      case GLD_MUSICFADEOUT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("musicfadeout 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_WRITECMDQUEUE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg3=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("writecmdqueue 0x%X ' 0x%X\r\n",arg1,arg3);
        break;
      case GLD_DELCMDQUEUE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("delcmdqueue 0x%X\r\n",arg1);
        break;
      case GLD_JUMPTEXTFACEPLAYER:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("jumptextfaceplayer 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=-1)
          DoText(arg2);
        still_going=0;
        break;
      case GLD_JUMPTEXT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("jumptext 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=-1)
          DoText(arg2);
        still_going=0;
        break;
      case GLD_WINLOSSTEXT:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,2,&read,NULL);
        arg3=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        arg4=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg2<<8));
        func("winlosstext 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4);
        if(arg3!=-1)
          DoText(arg3);
        if(arg4!=-1)
          DoText(arg4);
        break;
      case GLD_APPEAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("appear 0x%X\r\n",arg1);
        break;
      case GLD_DISAPPEAR:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("disappear 0x%X\r\n",arg1);
        break;
      case GLD_ASKFORPHONENUMBER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("askforphonenumber 0x%X\r\n",arg1);
        break;
      case GLD_PHONECALL:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("phonecall 0x%X\r\n",arg1);
        break;
      case GLD_TRADE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("trade 0x%X\r\n",arg1);
        break;
      case GLD_TAKEITEM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("takeitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEITEM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("giveitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEMONEY:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,3,&read,NULL);
        func("givemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_TAKEMONEY:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,3,&read,NULL);
        func("takemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHECKMONEY:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,3,&read,NULL);
        func("checkmoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVECOINS:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("givecoins 0x%X\r\n",arg1);
        break;
      case GLD_TAKECOINS:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("takecoins 0x%X\r\n",arg1);
        break;
      case GLD_CHECKCOINS:
        ReadFile(fileM,&arg1,2,&read,NULL);
        func("checkcoins 0x%X\r\n",arg1);
        break;
      case GLD_GIVEPHONENUMBER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("givephonenumber 0x%X\r\n",arg1);
        break;
      case GLD_TAKEPHONENUMBER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("takephonenumber 0x%X\r\n",arg1);
        break;
      case GLD_CHECKPHONENUMBER:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("checkphonenumber 0x%X\r\n",arg1);
        break;
      case GLD_CHECKITEM:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("checkitem 0x%X\r\n",arg1);
        break;
      case GLD_CHECKTIME:
        ReadFile(fileM,&arg1,1,&read,NULL);
        arg2=rand()%10;
        if(arg2==0)
          func("checktime 0x%X ' Look, an acronym of CheckItem!\r\n",arg1); //Easter Egg
        else
          func("checktime 0x%X\r\n",arg1);
        break;
      case GLD_CHECKPOKE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("checkpoke 0x%X\r\n",arg1);
        break;
      case GLD_GIVEEGG:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("giveegg 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEPOKEITEM:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("givepokeitem 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_TAKEIFLETTER:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg2<<8));
        func("takeifletter 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_XYCOMPARE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg2<<8));
        func("xycompare 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEPOKE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        ReadFile(fileM,&arg3,1,&read,NULL);
        ReadFile(fileM,&arg4,1,&read,NULL);
        if(arg4==1)
        {
          ReadFile(fileM,&arg5,2,&read,NULL);
          ReadFile(fileM,&arg6,2,&read,NULL);
          arg7=(OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF);
          func("givepoke 0x%X 0x%X 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6,
          PointerToOffset(arg7|(arg5<<8)),
          PointerToOffset(arg7|(arg6<<8)));
        }
        else
        {
          func("givepoke 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        }
        break;
      case GLD_FRUITTREE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("fruittree 0x%X\r\n",arg1);
        break;
      case GLD_LOADMENUDATA:
        ReadFile(fileM,&arg1,2,&read,NULL);
        arg2=PointerToOffset((OffsetToPointer(SetFilePointer(fileM,0,NULL,FILE_CURRENT))&0xFF)|(arg1<<8));
        func("loadmenudata 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOADPOKEDATA:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("loadpokedata 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHECKCODE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("checkcode 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_WRITEBYTE:
        ReadFile(fileM,&arg1,2,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        func("writebyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_WRITEVARCODE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        func("writevarcode 0x%X\r\n",arg1);
        break;
      case GLD_WRITECODE:
        ReadFile(fileM,&arg1,1,&read,NULL);
        ReadFile(fileM,&arg2,1,&read,NULL);
        if(arg1==03)
        {
          *buf2=0;
          switch(arg2)
          {
            case 0:
            case 1:
            case 5:
            case 9:
              strcpy(buf2," ' Normal fight");
              break;
            case 2:
              strcpy(buf2," ' Fight with HIRO's backpic");
              break;
            case 3:
              strcpy(buf2," ' Fight with DUDE's backpic");
              break;
            case 4:
              strcpy(buf2," ' Fight pokemon caught with rod");
              break;
            case 6:
              strcpy(buf2," ' Instant win fight");
              break;
            case 7:
              strcpy(buf2," ' Shiny pokemon fight");
              break;
            case 8:
              strcpy(buf2," ' Tree pokemon fight");
              break;
          }
          func("writecode 0x3 0x%X%s\r\n",arg2,buf2);
        }
        else
        {
          func("writecode 0x%X 0x%X\r\n",arg1,arg2);
        }
        break;
      default:
        func("#raw 0x%X\r\n",command);
        break;
      }
    }
    else
    {
      puts("'--EOF--");
      still_going=0;
    }
  }
  }
  func("\r\n");
  while(!AllDone())
  {
    DecodeProc(fileM,Done(FindNotDone()),filename);
  }
#define nl() func("\r\n")
  while(!AllDoneText())
  {
    nl();
    arg1=DoneText(FindNotDoneText());
    if(mode!=GOLD)
    func("#org 0x%X\r\n= %s\r\n",arg1,transtxt(arg1&0x00ffffff,filename));
    else
    func("#org 0x%X\r\n%s",arg1,transtxt(arg1&0x00ffffff,filename));
  }
  while(!AllDoneMove())
  {
    nl();
    arg1=DoneMove(FindNotDoneMove());
    func("#org 0x%X\r\nM %s\r\n",arg1,transmove(arg1&0x00ffffff,filename));
  }
}

