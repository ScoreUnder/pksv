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
  unsigned int still_going,arg1,arg2,arg3,arg4,arg5,arg6;
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
  func("#org 0x%X\r\n",(FileZoomPos|0x08000000));
  func("'-----------------------------------\r\n");
  while (still_going)
  {
    ReadFile(fileM,&command,1,&read,NULL);
    if(read>0)
    {
#define GENERIC(x) func("%s\r\n",x)
      arg1=arg2=arg3=arg4=arg5=arg6=0;
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
        ReadFile(fileM,&arg5,4,&read,NULL);
        if(arg1==1||arg1==2){
          ReadFile(fileM,&arg6,4,&read,NULL);
          func("trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
          if((arg6&0x08000000)!=0)
          {
            Do(arg6);
          }
        }
        else
        {
          func("trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }
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
    func("#org 0x%X\r\n= %s\r\n",arg1,transtxt(arg1&0x00ffffff,filename));
  }
  while(!AllDoneMove())
  {
    nl();
    arg1=DoneMove(FindNotDoneMove());
    func("#org 0x%X\r\nM %s\r\n",arg1,transmove(arg1&0x00ffffff,filename));
  }
}

