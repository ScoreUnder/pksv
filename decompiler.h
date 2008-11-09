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
char gabe=0;
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

FILE* scrf;

void DecodeProc(FILE* fileM,unsigned int narc,unsigned int FileZoomPos,char*filename,FILE*fsend)
{
  unsigned int still_going,arg1,arg2,arg3,arg4,arg5,arg6,arg7,endat;
  unsigned char command;
  size_t read;
  char buf[1024],buf2[1024];
  fseek(fileM,FileZoomPos&0x07ffffff,SEEK_SET);
  still_going=1;
  if((FileZoomPos&0x0000ffff)==0x1337||(FileZoomPos&0x00ffffff)==1337||(FileZoomPos&0x0000ffff)==0x1EE7)
  {
    if((FileZoomPos&0x00ffffff)==1337)
    {
      fprintf(fsend,"'Address 1337 is officially L33T!\r\n");
    }
    else
    {
      fprintf(fsend,"'Address 0x%X is officially L33T!\r\n",(FileZoomPos&0x00ffffff));
    }
  }
  if(mode==GOLD)
  {
    fprintf(fsend,"#org 0x%X\r\n",FileZoomPos);
  }
  else if(mode==DIAMOND)
  {
    fprintf(fsend,"#narc 0x%X 0x%X\r\n",narc,FileZoomPos);
    fseek(fileM,0xE,SEEK_SET);
    arg3=0;
    fread(&arg3,1,2,fileM);
    fseek(fileM,0x10,SEEK_SET);
    fread(&buf,1,4,fileM);
    buf[4]=0;
    arg5=0;
    while(arg5<arg3&&strcmp(buf,"BTAF"))
    {
      arg4=0;
      fread(&arg4,1,4,fileM);
      fseek(fileM,arg4-0x8,SEEK_CUR);
      fread(&buf,1,4,fileM);
      buf[4]=0;
      arg5++;
    }
    if(!strcmp(buf,"BTAF"))
    {
      arg1=0;
      endat=0;
      fseek(fileM,8*(narc+1),SEEK_CUR);
      fread(&arg1,1,4,fileM);
      fread(&endat,1,4,fileM);
      //fprintf(fsend,"'%X %X\r\n",arg1,endat);
      fseek(fileM,0x10,SEEK_SET);
      fread(&buf,1,4,fileM);
      buf[4]=0;
      arg5=0;
      while(arg5<arg3&&strcmp(buf,"GMIF"))
      {
        arg4=0;
        fread(&arg4,1,4,fileM);
        fseek(fileM,arg4-0x8,SEEK_CUR);
        fread(&buf,1,4,fileM);
        buf[4]=0;
        arg5++;
      }
      if(!strcmp(buf,"GMIF"))
      {
        arg2=ftell(fileM);
        endat+=arg2+4;
        //fprintf(fsend,"'%X %X\r\n",arg1+FileZoomPos+4+arg2,endat);
        fseek(fileM,4+arg1+FileZoomPos,SEEK_CUR);
      }
      else
      {
        fprintf(fsend,"'ERROR: Incomplete NARC.\r\n");
        return;
      }
    }
    else
    {
      fprintf(fsend,"'ERROR: Incomplete NARC.\r\n");
      return;
    }
  }
  else
  {
    fprintf(fsend,"#org 0x%X\r\n",(FileZoomPos|0x08000000));
  }
  if(!gabe&&mode==DIAMOND)
  {
    fprintf(fsend,"'gabe_k made DIAMOND scripting possible!\r\n");
    gabe=1;
  }
  fprintf(fsend,"'-----------------------------------\r\n");
  if(mode==DIAMOND)
  {
  while (still_going&&(signed int)ftell(fileM)<(signed int)endat)
  {
    read=fread(&command,1,1,fileM);
    if(read>0)
    {
#define GENERIC(x) fprintf(fsend,"%s\r\n",x)
      arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
      switch(command)
      {
      default:
        fprintf(fsend,"#raw 0x%X\r\n",command);
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
  else if(mode!=GOLD)
  {
  while (still_going)
  {
    read=(signed int)fread(&command,1,1,fileM);
    if(read>0)
    {
      arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
      switch(command)
      {
      case CMD_GIVEMONEY:
        fread(&arg1,1,4,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"givemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_REAPPEARAT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"reappearat 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_MOVESPRITE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"movesprite 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_APPLYMOVEMENTFINISHAT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,4,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        if ((arg2&0xff000000)==0x08000000)
        {
          if (arg1==255)
          {
            fprintf(fsend,"applymovementfinishat PLAYER 0x%X 0x%X 0x%X ' %s\r\n",arg2,arg3,arg4,transmove(arg2,filename));
          }
          else if (arg1==0x800f)
          {
            fprintf(fsend,"applymovementfinishat LASTTALKED 0x%X 0x%X 0x%X ' %s\r\n",arg2,arg3,arg4,transmove(arg2,filename));
          }
          else if (arg1==0x7F)
          {
            fprintf(fsend,"applymovementfinishat CAMERA 0x%X 0x%X 0x%X ' %s\r\n",arg2,arg3,arg4,transmove(arg2,filename));
          }
          else
          {
            fprintf(fsend,"applymovementfinishat 0x%X 0x%X 0x%X 0x%X ' %s\r\n",arg1,arg2,arg3,arg4,transmove(arg2,filename));
          }
          DoMove(arg2);
        }
        else
        {
          if (arg1==255)
          {
            fprintf(fsend,"applymovementfinishat PLAYER 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg2,arg3,arg4);
          }
          else if (arg1==0x800f)
          {
            fprintf(fsend,"applymovementfinishat LASTTALKED 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg2,arg3,arg4);
          }
          else if (arg1==0x7f)
          {
            fprintf(fsend,"applymovementfinishat CAMERA 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg2,arg3,arg4);
          }
          else
          {
            fprintf(fsend,"applymovementfinishat 0x%X 0x%X 0x%X 0x%X ' Not in ROM\r\n",arg1,arg2,arg3,arg4);
          }
        }
        break;
      case CMD_DISAPPEARAT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"disappearat 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_CHECKFURNITURE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkfurniture 0x%X\r\n",arg1);
        break;
      case CMD_TAKEFURNITURE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"takefurniture 0x%X\r\n",arg1);
        break;
      case CMD_CHECKITEMINPC:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"checkiteminpc 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_GIVEITEMTOPC:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"giveitemtopc 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_ADDFURNITURE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"addfurniture 0x%X\r\n",arg1);
        break;
      case CMD_CHECKITEMTYPE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkitemtype 0x%X\r\n",arg1);
        break;
      case CMD_CHECKIFROOMFORFURNITURE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkifroomforfurniture 0x%X\r\n",arg1);
        break;
      case CMD_PLAYSONG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"playsong 0x%X\r\n",arg1);
        break;
      case CMD_FARREAPPEAR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"farreappear 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_FARDISAPPEAR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"fardisappear 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_STOREPOKEMONVAR:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storepokemonvar 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETPOKEMONPP:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"setpokemonpp 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_MULTICHOICE3:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        fread(&arg5,1,1,fileM);
        fprintf(fsend,"multichoice3 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        break;
      case CMD_SHOWYESNO:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"showyesno 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETTRAINERFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"settrainerflag 0x%X\r\n",arg1);
        break;
      case CMD_STOREFIRSTPOKEMON:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"storefirstpokemon 0x%X\r\n",arg1);
        break;
      case CMD_POKEMART2:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"pokemart2 0x%X\r\n",arg1);
        break;
      case CMD_8A:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"CMD_8A 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_FADESCREENDELAY:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"fadescreendelay 0x%X 0x%X",arg1,arg2);
        switch(arg1)
        {
          case 0:
            fprintf(fsend," ' Fade back into the action from black");
            break;
          case 1:
            fprintf(fsend," ' Fade screen to black");
            break;
          case 2:
            fprintf(fsend," ' Fade back into the action from white");
            break;
          case 3:
            fprintf(fsend," ' Fade screen to white");
            break;
        }
        fprintf(fsend,"\r\n");
        break;
      case CMD_SETCATCHLOCATION:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"setcatchlocation 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STOREITEMS:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"storeitems 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_WARPTELEPORT2:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warpteleport2 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warpteleport2 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
        break;
      case CMD_WARPMUTED:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warpmuted 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warpmuted 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARPWALKING:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warpwalking 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warpwalking 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_FALLDOWNHOLE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"falldownhole 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_WARPTELEPORT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warpteleport 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warpteleport 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARP3:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warp3 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warp3 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARPELEVATOR:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warpelevator 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warpelevator 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARP4:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warp4 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warp4 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_WARP5:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warp5 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }else
        {
          fprintf(fsend,"warp5 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_GETPLAYERXY:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"getplayerxy 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_FADESONG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"fadesong 0x%X\r\n",arg1);
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
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"takefromcoincase 0x%X\r\n",arg1);
        break;
      case CMD_MSGBOX2:
        fread(&arg2,1,4,fileM);
        if((arg2&0xff000000)!=0x08000000)
        {
          fprintf(fsend,"msgbox2 0x%X\r\n",arg2);
        }
        else
        {
          fprintf(fsend,"msgbox2 0x%X ' %s\r\n",arg2,transtxt(arg2,filename));
          DoText(arg2);
        }
        if((arg2&0xff000000)!=0x08000000)
        {
          fprintf(fsend,"          'Pointer not in ROM area\r\n");
        }
        break;
      case CMD_B2:
        GENERIC("CMD_B2");
        break;
      case CMD_2D:
        GENERIC("CMD_2D");
        break;
      case CMD_2C:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"CMD_2C 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_RESETVARS:
        GENERIC("resetvars");
        break;
      case CMD_B1:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fread(&arg4,1,2,fileM);
        fprintf(fsend,"CMD_B1 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_SETDOOROPENEDSTATIC:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"setdooropenedstatic 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETDOORCLOSEDSTATIC:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"setdoorclosedstatic 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_RESETSPRITELEVEL:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"resetspritelevel 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_CREATETEMPSPRITE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fread(&arg4,1,2,fileM);
        fread(&arg5,1,1,fileM);
        fread(&arg6,1,1,fileM);
        fprintf(fsend,"createtempsprite 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
        break;
      case CMD_A6:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"CMD_A6 0x%X\r\n",arg1);
        break;
      case CMD_TEMPSPRITEFACE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"tempspriteface 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETOBEDIENCE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"setobedience 0x%X\r\n",arg1);
        break;
      case CMD_CHECKOBEDIENCE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkobedience 0x%X\r\n",arg1);
        break;
      case CMD_COMPAREHIDDENVAR:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"checkobedience 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STOREBOXNAME:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storeboxname 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_WARP6:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,2,fileM);
        fread(&arg5,1,2,fileM);
        fprintf(fsend,"warp6 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        break;
      case CMD_MSGBOXSIGN:
        GENERIC("msgboxsign");
        break;
      case CMD_MSGBOXNORMAL:
        GENERIC("msgboxnormal");
        break;
      case CMD_SETHEALINGPLACE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"sethealingplace 0x%X\r\n",arg1);
        break;
      case CMD_SPRITEFACE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"spriteface 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_RANDOM:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"random 0x%X\r\n",arg1);
        break;
      case CMD_MOVEOFFSCREEN:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"moveoffscreen 0x%X\r\n",arg1);
        break;
      case CMD_CHECKTRAINERFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checktrainerflag 0x%X\r\n",arg1);
        break;
      case CMD_SETDOOROPENED:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"setdooropened 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETDOORCLOSED:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"setdoorclosed 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_DOORCHANGE:
        GENERIC("doorchange");
        break;
      case CMD_CALLSTDIF:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
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
        fprintf(fsend,"if 0x%X callstd 0x%X ' %s\r\n",arg1,arg2,buf);
        break;
      case CMD_SHOWPOKEPIC:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"showpokepic 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_HIDEPOKEPIC:
        fprintf(fsend,"hidepokepic\r\n");
        break;
      case CMD_SETMAPFOOTER:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"setmapfooter 0x%X\r\n",arg1);
        break;
      case CMD_JUMPSTDIF:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
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
        fprintf(fsend,"if 0x%X jumpstd 0x%X ' %s\r\n",arg1,arg2,buf);
        break;
      case CMD_SETWORLDMAPFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"setworldmapflag 0x%X\r\n",arg1);
        break;
      case CMD_LIGHTROOM:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"lightroom 0x%X\r\n",arg1);
        break;
      case CMD_DARKENROOM:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"darkenroom 0x%X\r\n",arg1);
        break;
      case CMD_MOVESPRITE2:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"movesprite2 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_ADDITEM:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"additem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SPRITEBEHAVE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"spritebehave 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_UPDATECOINS:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"updatecoins 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETBYTE2:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"setbyte2 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETFARBYTE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
        fprintf(fsend,"setfarbyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_TEXTCOLOR:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"textcolor 0x%X 'Sets text color (Fire Red).\r\n",arg1);
        break;
      case CMD_FAKEJUMPSTD:
        if(mode==FIRE_RED)
        {
          fread(&arg1,1,1,fileM);
          fprintf(fsend,"fakejumpstd 0x%X\r\n",arg1);
        }
        else
        {
          fread(&arg1,1,4,fileM);
          fprintf(fsend,"pokemart3 0x%X\r\n",arg1);
        }
        break;
      case CMD_COMPAREVARS:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"comparevars %s %s\r\n",buf,buf2);
        break;
      case CMD_COMPAREVARS2:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"comparevars2 %s %s\r\n",buf,buf2);
        break;
      case CMD_SUBTRACTVAR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"subtractvar %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_CLEARTRAINERFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"cleartrainerflag 0x%X\r\n",arg1);
        break;
      case CMD_FE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"CMD_FE 0x%X\r\n",arg1);
        break;
      case CMD_ADDVAR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"addvar %s %s\r\n",buf,buf2);
        break;
      case CMD_96:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"CMD_96 0x%X\r\n",arg1);
        break;
      case CMD_COINCASETOVAR:
        fread(&arg1,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"coincasetovar %s\r\n",buf);
        break;
      case CMD_GIVETOCOINCASE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"givetocoincase %d ' Bear in mind, it's not in hex\r\n",arg1);
        break;
      case CMD_CHECKMONEY:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"checkmoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_PAYMONEY:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"paymoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_UPDATEMONEY:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"updatemoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_HIDEMONEY:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"hidemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_HIDECOINS:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"hidecoins 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SHOWCOINS:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"showcoins 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_MULTICHOICE2:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,2,fileM);
        fprintf(fsend,"multichoice2 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_SHOWMONEY:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"showmoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_DISAPPEAR:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"disappear 0x%X\r\n",arg1);
        break;
      case CMD_REAPPEAR:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"reappear 0x%X\r\n",arg1);
        break;
      case CMD_GIVEEGG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"giveegg 0x%X\r\n",arg1);
        break;
      case CMD_STOREFURNITURE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storefurniture 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STOREATK:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storeatk 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_STORECOMP:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storecomp 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SLOTMACHINE:
        if(mode==FIRE_RED)
        {
          fread(&arg1,1,1,fileM);
          fprintf(fsend,"fakecallstd 0x%X\r\n",arg1);
        }
        else if(mode==RUBY)
        {
          fread(&arg1,1,2,fileM);
          fprintf(fsend,"slotmachine 0x%X\r\n",arg1);
        }
        break;
      case CMD_STOREITEM:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storeitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_DOANIMATION:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"doanimation 0x%X\r\n",arg1);
        break;
      case CMD_CHECKANIMATION:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkanimation 0x%X\r\n",arg1);
        break;
      case CMD_SETANIMATION:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"setanimation 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_HIDEBOX:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        fprintf(fsend,"hidebox 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_STORETEXT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
        if((arg2&0xff000000)!=0x08000000)
        {
          fprintf(fsend,"storetext 0x%X 0x%X 'Text not in ROM area\r\n",arg1,arg2);
        }
        else
        {
          fprintf(fsend,"storetext 0x%X 0x%X ' %s\r\n",arg1,arg2,transtxt(arg2,filename));
          DoText(arg2);
        }
        break;
      case CMD_SOUND:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"sound 0x%X\r\n",arg1);
        break;
      case CMD_STOREVAR:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg2);
        if(arg2==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"storevar 0x%X %s\r\n",arg1,buf);
        break;
      case CMD_DOWEATHER:
        GENERIC("doweather");
        break;
      case CMD_SETWEATHER:
        arg2=1;
        if(mode==FIRE_RED)arg2=2;
        fread(&arg1,1,arg2,fileM);
        fprintf(fsend,"setweather 0x%X\r\n",arg1);
        break;
      case CMD_BATTLE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fprintf(fsend,"battle 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case CMD_SETMAPTILE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fread(&arg3,1,2,fileM);
        fread(&arg4,1,2,fileM);
        fprintf(fsend,"setmaptile 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_END:
        still_going=0;
        GENERIC("end");
        break;
      case CMD_POKEMART:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"pokemart 0x%X\r\n",arg1);
        break;
      case CMD_SHOWMSG:
        GENERIC("showmsg");
        break;
      case CMD_RETURN:
        still_going=0;
        GENERIC("return");
        break;
      case CMD_WRITEBYTETOOFFSET:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
        fprintf(fsend,"writebytetooffset 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CHECKFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkflag 0x%X\r\n",arg1);
        break;
      case CMD_SPECIAL2:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"special2 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"setflag 0x%X\r\n",arg1);
        break;
      case CMD_LOCK:
        GENERIC("lock");
        break;
      case CMD_FACEPLAYER:
        GENERIC("faceplayer");
        break;
      case CMD_C3:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"CMD_C3 0x%X\r\n",arg1);
        break;
      case CMD_JUMPIF:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
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
        fprintf(fsend,"if 0x%X jump 0x%X ' %s\r\n",arg1,arg2,buf);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          fprintf(fsend,"          'Jump not in ROM area\r\n");
        }
        break;
      case CMD_WARP:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg4,1,2,fileM);
          fread(&arg5,1,2,fileM);
          fprintf(fsend,"warp 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        }
        else
        {
          fprintf(fsend,"warp 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        }
        break;
      case CMD_FB:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"CMD_FB 0x%X\r\n",arg1);
        break;
      case CMD_COPYSCRIPTBANKS:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"copyscriptbanks 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COMPAREVARTOFARBYTE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,4,fileM);
        fprintf(fsend,"comparevartofarbyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOVAR:
        fread(&arg1,1,4,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"comparefarbytetovar 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COMPAREFARBYTETOBYTE:
        fread(&arg1,1,4,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"comparefarbytetobyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_COPYBYTE:
        fread(&arg1,1,4,fileM);
        fread(&arg2,1,4,fileM);
        fprintf(fsend,"copybyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SETVAR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"setvar %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_MESSAGE:
        fread(&arg1,1,4,fileM);
        if((arg1&0xff000000)!=0x08000000)
        {
          fprintf(fsend,"message 0x%X 'Text not in ROM area\r\n",arg1);
        }
        else
        {
          fprintf(fsend,"message 0x%X ' %s\r\n",arg1,transtxt(arg1,filename));
          DoText(arg1);
        }
        break;
      case CMD_CALLIF:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
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
        fprintf(fsend,"if 0x%X call 0x%X ' %s\r\n",arg1,arg2,buf);
        if((arg2&0xff000000)==0x08000000)
        {
          Do(arg2);
        }
        else
        {
          fprintf(fsend,"          'Call not in ROM area\r\n");
        }
        break;
      case CMD_CHECKGENDER:
        GENERIC("checkgender");
        break;
      case CMD_COMPARE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"compare %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_APPLYMOVEMENT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,4,fileM);
        if ((arg2&0xff000000)==0x08000000)
        {
          if (arg1==255)
          {
            fprintf(fsend,"applymovement PLAYER 0x%X ' %s\r\n",arg2,transmove(arg2,filename));
          }
          else if (arg1==0x800f)
          {
            fprintf(fsend,"applymovement LASTTALKED 0x%X ' %s\r\n",arg2,transmove(arg2,filename));
          }
          else if (arg1==0x7F)
          {
            fprintf(fsend,"applymovement CAMERA 0x%X ' %s\r\n",arg2,transmove(arg2,filename));
          }
          else
          {
            fprintf(fsend,"applymovement 0x%X 0x%X ' %s\r\n",arg1,arg2,transmove(arg2,filename));
          }
          DoMove(arg2);
        }
        else
        {
          if (arg1==255)
          {
            fprintf(fsend,"applymovement PLAYER 0x%X ' Not in ROM\r\n",arg2);
          }
          else if (arg1==0x800f)
          {
            fprintf(fsend,"applymovement LASTTALKED 0x%X ' Not in ROM\r\n",arg2);
          }
          else if (arg1==0x7f)
          {
            fprintf(fsend,"applymovement CAMERA 0x%X ' Not in ROM\r\n",arg2);
          }
          else
          {
            fprintf(fsend,"applymovement 0x%X 0x%X ' Not in ROM\r\n",arg1,arg2);
          }
        }
        break;
      case CMD_RELEASE:
        GENERIC("release");
        break;
      case CMD_PAUSE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"pause 0x%X\r\n",arg1);
        break;
      case CMD_PAUSEEVENT:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"pauseevent 0x%X\r\n",arg1);
        break;
      case CMD_CALL:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"call 0x%X\r\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          Do(arg1);
        }
        else
        {
          fprintf(fsend,"          'Call not in ROM area\r\n");
        }
        break;
      case CMD_CALLSTD:
        fread(&arg1,1,1,fileM);
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
            strcpy(buf,"Non-closing message");
            break;
          case 5:
            strcpy(buf2,"MSG_YESNO");
            strcpy(buf,"Yes/No message");
            break;
          case 6:
            strcpy(buf2,"MSG_NORMAL");
            strcpy(buf,"Normal message");
            break;
          case 9:
            strcpy(buf2,"MSG_ITEM");
            strcpy(buf,"Shows a message, then... 'Player put the yyy in the zzz!'");
            break;
        }
        fprintf(fsend,"callstd %s ' %s\r\n",buf2,buf);
        break;
      case CMD_MSGBOX:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
        if (arg1==0)
        {
          if((arg2&0xff000000)!=0x08000000)
          {
            fprintf(fsend,"msgbox 0x%X\r\n",arg2);
          }
          else
          {
            fprintf(fsend,"msgbox 0x%X ' %s\r\n",arg2,transtxt(arg2,filename));
            DoText(arg2);
          }
        }
        else
        {
          fprintf(fsend,"loadpointer 0x%X 0x%X\r\n",arg1,arg2);
        }
        if((arg2&0xff000000)!=0x08000000)
        {
          fprintf(fsend,"          'Pointer not in ROM area\r\n");
        }
        break;
      case CMD_STOREPOKEMON:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"storepokemon 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_SPECIAL:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"special 0x%X",arg1);
        if(arg1==0)
        {fprintf(fsend," ' Heal Pokemon");}
        fprintf(fsend,"\r\n");
        break;
      case CMD_FADESCREEN:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"fadescreen 0x%X",arg1);
        switch(arg1)
        {
          case 0:
            fprintf(fsend," ' Fade back into the action from black");
            break;
          case 1:
            fprintf(fsend," ' Fade screen to black");
            break;
          case 2:
            fprintf(fsend," ' Fade back into the action from white");
            break;
          case 3:
            fprintf(fsend," ' Fade screen to white");
            break;
        }
        fprintf(fsend,"\r\n");
        break;
      case CMD_FANFARE:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"fanfare 0x%X\r\n",arg1);
        break;
      case CMD_WAITFANFARE:
        GENERIC("waitfanfare");
        break;
      case CMD_BRAILLE:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"braille 0x%X\r\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename,fsend);
        }
        else
        {
          fprintf(fsend,"          'Braille not in ROM area\r\n");
        }
        break;
      case CMD_D3:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"CMD_D3 0x%X\r\n",arg1);
        if((arg1&0xff000000)==0x08000000)
        {
          transbrl(arg1,filename,fsend);
        }
        else
        {
          fprintf(fsend,"          'Braille not in ROM area\r\n");
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
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"picture 0x%X\r\n",arg1);
        break;
      case CMD_NOP0:
        fprintf(fsend,"nop0 ' #raw 0x0\r\n");
        break;
      case CMD_NOP1:
        fprintf(fsend,"nop1 ' #raw 0x1\r\n");
        break;
      case CMD_JUMP:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"jump 0x%X\r\n",arg1);
        Do(arg1);
        still_going=0;
        break;
      case CMD_JUMPSTD:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"jumpstd 0x%X\r\n",arg1);
        still_going=0;
        break;
      case CMD_KILLSCRIPT:
        GENERIC("killscript");
        break;
      case CMD_SETBYTE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"setbyte 0x%X\r\n",arg1);
        break;
      case CMD_COPYVARIFNOTZERO:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"copyvarifnotzero %s 0x%X\r\n",buf,arg2);
        break;
      case CMD_COPYVAR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        sprintf(buf,"0x%X",arg1);
        sprintf(buf2,"0x%X",arg2);
        if(arg1==0x800D){strcpy(buf,"LASTRESULT");}
        else if(arg1==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg1==0x800F){strcpy(buf,"LASTTALKED");}
        if(arg2==0x800D){strcpy(buf2,"LASTRESULT");}
        else if(arg2==0x800C){strcpy(buf,"PLAYERFACING");}
        else if(arg2==0x800F){strcpy(buf,"LASTTALKED");}
        fprintf(fsend,"copyvar %s %s\r\n",buf,buf2);
        break;
      case CMD_COMPAREVARTOBYTE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"comparevartobyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_LOADBYTEFROMPOINTER:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,4,fileM);
        fprintf(fsend,"loadbytefrompointer 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_REMOVEITEM:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"removeitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_WAITSPECIAL:
        GENERIC("waitspecial");
        break;
      case CMD_WAITCRY:
        GENERIC("waitcry");
        break;
      case CMD_CRYFR:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"cry 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CRY:
        if(mode!=FIRE_RED)
        {
          fread(&arg1,1,1,fileM);
          fread(&arg2,1,2,fileM);
          fprintf(fsend,"cry 0x%X 0x%X\r\n",arg1,arg2);
        }
        else
          GENERIC("checksound");
        break;
      case CMD_PLAYSOUND:
        fread(&arg1,1,2,fileM);
        if(mode==FIRE_RED)
        {
          fread(&arg2,1,1,fileM);
          fprintf(fsend,"playsound 0x%X 0x%X\r\n",arg1,arg2);
        }
        else
        {
          fprintf(fsend,"playsound 0x%X\r\n",arg1);
        }
        break;
      case CMD_FADEDEFAULT:
        if(mode==RUBY)
        {
          fread(&arg1,1,2,fileM);
          fprintf(fsend,"fadesound 0x%X\r\n",arg1);
        }
        else if(mode==FIRE_RED)
        {
          fprintf(fsend,"fadedefault\r\n");
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
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"checkitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CHECKITEMAMOUNT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"checkitemamount 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CHECKATTACK:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkattack 0x%X\r\n",arg1);
        break;
      case CMD_TRAINERBATTLE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fread(&arg3,1,2,fileM);
        fread(&arg4,1,4,fileM);
        if(arg1!=3)
        {
          fread(&arg5,1,4,fileM);
        }
        if(arg1==1||arg1==2){
          fread(&arg6,1,4,fileM);
          fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
          if((arg6&0x08000000)!=0)
          {
            Do(arg6);
          }
        }
        else if (arg1==3)
        {
          fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        }
        else
        {
          fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
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
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        fprintf(fsend,"multichoice 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case CMD_ADDPOKEMON:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,2,fileM);
        fread(&arg4,1,1,fileM);
        fread(&arg5,1,4,fileM);
        fread(&arg6,1,4,fileM);
        fprintf(fsend,"addpokemon 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
        break;
      case CMD_CALLASM:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"callasm 0x%X\r\n",arg1);
        break;
      case CMD_CALLASM2:
        fread(&arg1,1,4,fileM);
        fprintf(fsend,"callasm2 0x%X\r\n",arg1);
        break;
      case CMD_CLEARFLAG:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"clearflag 0x%X\r\n",arg1);
        break;
      case CMD_COMPAREFARBYTETOFARBYTE:
        fread(&arg1,1,4,fileM);
        fread(&arg2,1,4,fileM);
        fprintf(fsend,"comparefarbytetofarbyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case CMD_CLOSEMSG:
        GENERIC("closemsg");
        break;
      default:
        fprintf(fsend,"#raw 0x%X\r\n",command);
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
    read=(signed int)fread(&command,1,1,fileM);
    if(read>0)
    {
#define GENERIC(x) fprintf(fsend,"%s\r\n",x)
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
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"2call 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_3CALL:
        fread(&arg1,1,3,fileM);
        arg2=PointerToOffset(arg1);
        fprintf(fsend,"3call 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_2PTCALL:
        fread(&arg1,1,2,fileM);
        arg3=ftell(fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          fseek(fileM,arg2,SEEK_SET);
          fread(&arg4,1,3,fileM);
          fseek(fileM,arg3,SEEK_SET);
        }
        fprintf(fsend,"2ptcall 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        Do(arg4);
        break;
      case GLD_2JUMP:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"2jump 0x%X ' 0x%X\r\n",arg1,arg2);
        still_going=0;
        Do(arg2);
        break;
      case GLD_PRIORITYJUMP:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"priorityjump 0x%X ' 0x%X\r\n",arg1,arg2);
        still_going=0;
        Do(arg2);
        break;
      case GLD_3JUMP:
        fread(&arg1,1,3,fileM);
        arg2=PointerToOffset(arg1);
        fprintf(fsend,"3jump 0x%X ' 0x%X\r\n",arg1,arg2);
        still_going=0;
        Do(arg2);
        break;
      case GLD_2PTJUMP:
        fread(&arg1,1,2,fileM);
        arg3=ftell(fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          fseek(fileM,arg2,SEEK_SET);
          fread(&arg4,1,3,fileM);
          fseek(fileM,arg3,SEEK_SET);
        }
        fprintf(fsend,"2ptjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        still_going=0;
        Do(arg4);
        break;
      case GLD_PTPRIORITYJUMP:
        fread(&arg1,1,2,fileM);
        arg3=ftell(fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          fseek(fileM,arg2,SEEK_SET);
          fread(&arg4,1,3,fileM);
          fseek(fileM,arg3,SEEK_SET);
        }
        fprintf(fsend,"ptpriorityjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        still_going=0;
        Do(arg4);
        break;
      case GLD_EQBYTE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"if == 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        Do(arg3);
        break;
      case GLD_NEQBYTE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"if != 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        Do(arg3);
        break;
      case GLD_EQZERO:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"if false 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_NEQZERO:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"if true 0x%X ' 0x%X\r\n",arg1,arg2);
        Do(arg2);
        break;
      case GLD_LTBYTE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"if < 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        Do(arg3);
        break;
      case GLD_GTBYTE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"if > 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        Do(arg3);
        break;
      case GLD_JUMPSTD:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"jumpstd 0x%X\r\n",arg1);
        still_going=0;
        break;
      case GLD_CALLSTD:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"callstd 0x%X\r\n",arg1);
        break;
      case GLD_3CALLASM:
        fread(&arg1,1,3,fileM);
        fprintf(fsend,"3callasm 0x%X\r\n",arg1);
        break;
      case GLD_SPECIAL:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"special 0x%X\r\n",arg1);
        break;
      case GLD_2PTCALLASM:
        fread(&arg1,1,2,fileM);
        arg3=ftell(fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        if(arg2!=0xFFFFFFFF)
        {
          fseek(fileM,arg2,SEEK_SET);
          fread(&arg4,1,3,fileM);
          fseek(fileM,arg3,SEEK_SET);
        }
        fprintf(fsend,"2ptcallasm 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
        break;
      case GLD_CHECKMAPTRIGGERS:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"checkmaptriggers 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_DOMAPTRIGGER:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"domaptrigger 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_DOTRIGGER:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"dotrigger 0x%X\r\n",arg1);
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
      case GLD_LOADFONT:
        GENERIC("loadfont");
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
        fread(&arg1,1,3,fileM);
        arg2=PointerToOffset(arg1);
        fprintf(fsend,"passtoengine 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHECKBIT1:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkbit1 0x%X\r\n",arg1);
        break;
      case GLD_SETBIT1:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"setbit1 0x%X\r\n",arg1);
        break;
      case GLD_SETBIT2:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"setbit2 0x%X\r\n",arg1);
        break;
      case GLD_CHECKBIT2:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkbit2 0x%X\r\n",arg1);
        break;
      case GLD_CLEARBIT2:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"clearbit2 0x%X\r\n",arg1);
        break;
      case GLD_CLEARBIT1:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"clearbit1 0x%X\r\n",arg1);
        break;
      case GLD_2WRITETEXT:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"2writetext 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=0xFFFFFFFF)
          DoText(arg2);
        break;
      case GLD_3WRITETEXT:
        fread(&arg1,1,3,fileM);
        arg2=PointerToOffset(arg1);
        fprintf(fsend,"3writetext 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=0xFFFFFFFF)
          DoText(arg2);
        break;
      case GLD_REPEATTEXT:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"repeattext 0x%X\r\n",arg1);
        break;
      case GLD_POKEPIC:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"pokepic 0x%X\r\n",arg1);
        break;
      case GLD_DESCRIBEDECORATION:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"describedecoration 0x%X\r\n",arg1);
        break;
      case GLD_FRUITTREE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"fruittree 0x%X\r\n",arg1);
        break;
      case GLD_SPECIALPHONECALL:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"specialphonecall 0x%X\r\n",arg1);
        break;
      case GLD_FACEPERSON:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"faceperson 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_VARIABLESPRITE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"variablesprite 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_APPLYMOVEMENT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"applymovement 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        if(arg3!=-1)DoMove(arg3);
        break;
      case GLD_APPLYMOVEOTHER:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"applymoveother 0x%X ' 0x%X Applies movement to last talked\r\n",arg1,arg2);
        if(arg2!=-1)DoMove(arg2);
        break;
      case GLD_VERBOSEGIVEITEM:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"verbosegiveitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOADWILDDATA:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"loadwilddata 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOADTRAINER:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"loadtrainer 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CATCHTUTORIAL:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"catchtutorial 0x%X\r\n",arg1);
        break;
      case GLD_TRAINERTEXT:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"trainertext 0x%X\r\n",arg1);
        break;
      case GLD_TRAINERSTATUS:
        fread(&arg1,1,1,fileM);
        *buf2=0;
        switch(arg1){
          case 0:strcpy(buf2," ' Deactivate Trainer");break;
          case 1:strcpy(buf2," ' Activate Trainer");break;
          case 2:strcpy(buf2," ' Check if Trainer is Activated");break;
        }
        fprintf(fsend,"trainerstatus 0x%X%s\r\n",arg1,buf2);
        break;
      case GLD_CLOSETEXT:
        GENERIC("closetext");
        break;
      case GLD_LOADMOVESPRITES:
        GENERIC("loadmovesprites");
        break;
      case GLD_POKEMART:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,2,fileM);
        fprintf(fsend,"pokemart 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_ELEVATOR:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"elevator 0x%X ' 0x%X\r\n",arg1,arg2);
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
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"deactivatefacing 0x%X\r\n",arg1);
        break;
      case GLD_NEWLOADMAP:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"newloadmap 0x%X\r\n",arg1);
        break;
      case GLD_WARPCHECK:
        GENERIC("warpcheck");
        break;
      case GLD_GIVEITEM:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"giveitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_REFRESHSCREEN:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"refreshscreen 0x%X\r\n",arg1);
        break;
      case GLD_C1CELOADBYTE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"C1CEloadbyte 0x%X ' Apparently useless.\r\n",arg1);
        break;
      case GLD_SETLASTTALKED:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"setlasttalked 0x%X\r\n",arg1);
        break;
      case GLD_EARTHQUAKE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"earthquake 0x%X\r\n",arg1);
        break;
      case GLD_LOADVAR:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"loadvar 0x%X\r\n",arg1);
        break;
      case GLD_ADDVAR:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"addvar 0x%X\r\n",arg1);
        break;
      case GLD_RANDOM:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"random 0x%X\r\n",arg1);
        break;
      case GLD_COPYBYTETOVAR:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"copybytetovar 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_COPYVARTOBYTE:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"copyvartobyte 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHANGEBLOCK:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"changeblock 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_SHOWEMOTE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"showemote 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_FOLLOW:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"follow 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_FOLLOWNOTEXACT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"follownotexact 0x%X 0x%X ' Follows, but without mindlessly copying movements.\r\n",arg1,arg2);
        break;
      case GLD_SWAPMAPS:
        fread(&arg1,1,3,fileM);
        fprintf(fsend,"swapmaps 0x%X ' 0x%X\r\n",arg1,PointerToOffset(arg1));
        break;
      case GLD_SPRITEFACE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"spriteface 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_MOVEPERSON:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"moveperson 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_WRITEPERSONLOC:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"writepersonloc 0x%X\r\n",arg1);
        break;
      case GLD_LOADEMOTE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"loademote 0x%X\r\n",arg1);
        break;
      case GLD_STOPFOLLOW:
        GENERIC("stopfollow");
        break;
      case GLD_PLAYSOUND:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"playsound 0x%X\r\n",arg1);
        break;
      case GLD_PLAYMUSIC:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"playmusic 0x%X\r\n",arg1);
        break;
      case GLD_CRY:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"cry 0x%X\r\n",arg1);
        break;
      case GLD_PAUSE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"pause 0x%X\r\n",arg1);
        break;
      case GLD_WARPMOD:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"warpmod 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_WARP:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        fprintf(fsend,"warp 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        break;
      case GLD_WARPFACING:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        fread(&arg5,1,1,fileM);
        fprintf(fsend,"warpfacing 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
        break;
      case GLD_BLACKOUTMOD:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"blackoutmod 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOCATIONTOTEXT:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"locationtotext 0x%X\r\n",arg1);
        break;
      case GLD_DISPLAYLOCATION:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"displaylocation 0x%X\r\n",arg1);
        break;
      case GLD_MONEYTOTEXT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"moneytotext 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_COINSTOTEXT:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"coinstotext 0x%X\r\n",arg1);
        break;
      case GLD_VARTOTEXT:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"vartotext 0x%X\r\n",arg1);
        break;
      case GLD_POKETOTEXT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"poketotext 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_ITEMTOTEXT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"itemtotext 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_TRAINERTOTEXT:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fprintf(fsend,"trainertotext 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
        break;
      case GLD_STRINGTOTEXT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"stringtotext 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
        if(arg3!=-1)
        {
          DoText(arg3);
        }
        break;
      case GLD_STORETEXT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        arg4=PointerToOffset((arg1<<8)|arg2);
        fprintf(fsend,"storetext 0x%X 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3,arg4);
        if(arg4!=-1)
        {
          DoText(arg4);
        }
        break;
      case GLD_MUSICFADEOUT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"musicfadeout 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_WRITECMDQUEUE:
        fread(&arg1,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"writecmdqueue 0x%X ' 0x%X\r\n",arg1,arg3);
        break;
      case GLD_DELCMDQUEUE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"delcmdqueue 0x%X\r\n",arg1);
        break;
      case GLD_JUMPTEXTFACEPLAYER:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"jumptextfaceplayer 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=-1)
          DoText(arg2);
        still_going=0;
        break;
      case GLD_JUMPTEXT:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"jumptext 0x%X ' 0x%X\r\n",arg1,arg2);
        if(arg2!=-1)
          DoText(arg2);
        still_going=0;
        break;
      case GLD_WINLOSSTEXT:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,2,fileM);
        arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        arg4=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"winlosstext 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4);
        if(arg3!=-1)
          DoText(arg3);
        if(arg4!=-1)
          DoText(arg4);
        break;
      case GLD_APPEAR:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"appear 0x%X\r\n",arg1);
        break;
      case GLD_DISAPPEAR:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"disappear 0x%X\r\n",arg1);
        break;
      case GLD_ASKFORPHONENUMBER:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"askforphonenumber 0x%X\r\n",arg1);
        break;
      case GLD_PHONECALL:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"phonecall 0x%X\r\n",arg1);
        break;
      case GLD_TRADE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"trade 0x%X\r\n",arg1);
        break;
      case GLD_TAKEITEM:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"takeitem 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEMONEY:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,3,fileM);
        fprintf(fsend,"givemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_TAKEMONEY:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,3,fileM);
        fprintf(fsend,"takemoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHECKMONEY:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,3,fileM);
        fprintf(fsend,"checkmoney 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVECOINS:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"givecoins 0x%X\r\n",arg1);
        break;
      case GLD_TAKECOINS:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"takecoins 0x%X\r\n",arg1);
        break;
      case GLD_CHECKCOINS:
        fread(&arg1,1,2,fileM);
        fprintf(fsend,"checkcoins 0x%X\r\n",arg1);
        break;
      case GLD_GIVEPHONENUMBER:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"givephonenumber 0x%X\r\n",arg1);
        break;
      case GLD_TAKEPHONENUMBER:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"takephonenumber 0x%X\r\n",arg1);
        break;
      case GLD_CHECKPHONENUMBER:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"checkphonenumber 0x%X\r\n",arg1);
        break;
      case GLD_CHECKITEM:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"checkitem 0x%X\r\n",arg1);
        break;
      case GLD_CHECKTIME:
        fread(&arg1,1,1,fileM);
        arg2=rand()%10;
        if(arg2==0)
          fprintf(fsend,"checktime 0x%X ' Look, an acronym of CheckItem!\r\n",arg1); //Easter Egg
        else
          fprintf(fsend,"checktime 0x%X\r\n",arg1);
        break;
      case GLD_CHECKPOKE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"checkpoke 0x%X\r\n",arg1);
        break;
      case GLD_GIVEEGG:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"giveegg 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEPOKEITEM:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"givepokeitem 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_TAKEIFLETTER:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"takeifletter 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_XYCOMPARE:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
        fprintf(fsend,"xycompare 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_GIVEPOKE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fread(&arg3,1,1,fileM);
        fread(&arg4,1,1,fileM);
        if(arg4==1)
        {
          fread(&arg5,1,2,fileM);
          fread(&arg6,1,2,fileM);
          arg7=(OffsetToPointer(FileZoomPos)&0xFF);
          fprintf(fsend,"givepoke 0x%X 0x%X 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6,
          PointerToOffset(arg7|(arg5<<8)),
          PointerToOffset(arg7|(arg6<<8)));
        }
        else
        {
          fprintf(fsend,"givepoke 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
        }
        break;
      case GLD_LOADMENUDATA:
        fread(&arg1,1,2,fileM);
        arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
        fprintf(fsend,"loadmenudata 0x%X ' 0x%X\r\n",arg1,arg2);
        break;
      case GLD_LOADPOKEDATA:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"loadpokedata 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_CHECKCODE:
        fread(&arg1,1,1,fileM);
        //fread(&arg2,1,1,fileM);
        //fprintf(fsend,"checkcode 0x%X 0x%X\r\n",arg1,arg2);
        fprintf(fsend,"checkcode 0x%X\r\n",arg1);
        break;
      case GLD_WRITEBYTE:
        fread(&arg1,1,2,fileM);
        fread(&arg2,1,1,fileM);
        fprintf(fsend,"writebyte 0x%X 0x%X\r\n",arg1,arg2);
        break;
      case GLD_WRITEVARCODE:
        fread(&arg1,1,1,fileM);
        fprintf(fsend,"writevarcode 0x%X\r\n",arg1);
        break;
      case GLD_WRITECODE:
        fread(&arg1,1,1,fileM);
        fread(&arg2,1,1,fileM);
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
          fprintf(fsend,"writecode 0x3 0x%X%s\r\n",arg2,buf2);
        }
        else
        {
          fprintf(fsend,"writecode 0x%X 0x%X\r\n",arg1,arg2);
        }
        break;
      default:
        fprintf(fsend,"#raw 0x%X\r\n",command);
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
  fprintf(fsend,"\r\n");
  while(!AllDone())
  {
    DecodeProc(fileM,narc,Done(FindNotDone()),filename,fsend);
  }
#define nl() fprintf(fsend,"\r\n")
  while(!AllDoneText())
  {
    nl();
    arg1=DoneText(FindNotDoneText());
    if(mode!=GOLD)
    fprintf(fsend,"#org 0x%X\r\n= %s\r\n",arg1,transtxt(arg1&0x07ffffff,filename));
    else
    fprintf(fsend,"#org 0x%X\r\n%s",arg1,transtxt(arg1&0x07ffffff,filename));
  }
  while(!AllDoneMove())
  {
    nl();
    arg1=DoneMove(FindNotDoneMove());
    fprintf(fsend,"#org 0x%X\r\nM %s\r\n",arg1,transmove(arg1&0x07ffffff,filename));
  }
}

