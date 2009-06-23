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

#define aa(x) else if (!strcmp(buf,x))
#ifndef DLL
#define ec() e_c(Script,&i,LogFile)
#define vlog(x) vlogProc(LogFile,x)
#else
#define vlog(x) vlogProc(x)
#define ec() e_c(Script,&i)
#endif
#define rom(x,s) {if(eorg){j=0xFFFFFFFF;}else{j=x;}if(c!=NULL)add_data(c,(char*)&j,s);}
#define BASIC(x) rom(x,1)
void e_c(char*Script,int*ii
#ifndef DLL
         ,FILE*LogFile
#endif
        )
{
  register unsigned int i=*ii;
  while (Script[i+1]==' ') {
    i++;
  }
  if (Script[i+1]=='\'') {
    while (Script[i+1]!='\n'&&Script[i+1]!=0) {
      i++;
    }
  }
  if (Script[i+1]!='\n'&&chr!='\n') {
    log("Extra characters on line. Ignoring.\r\n",37);
  }
  while (chr!='\n') {
    i++;
  }
  *ii=i;
  return;
}
void vlogProc(
#ifndef DLL
  FILE* LogFile,
#endif
  char*x)
{
  if (IsVerbose)
  {
    log(x,strlen(x));
  }
  return;
}
#ifdef WIN32
unsigned int needdlg=0;
#endif
void DoDefines()
{
  register FILE*f;
  char buf[500];
  unsigned char l;
  unsigned int a;
  register unsigned int fl;
  register unsigned int i;
#ifdef WIN32
  OutputDebugString("Started Defines");
#endif
  f=fopen("defines.dat","rb");
  if (!f) {
    f=fopen(GlobBuf,"rb");
  }
  if (!f) {
    log("Cannot open defines.dat!\r\n",26);
    return;
  }
  fseek(f,0,SEEK_END);
  fl=ftell(f);
  fseek(f,0,SEEK_SET);
  for (i=0;i<fl;)
  {
    fread(&l,1,1,f);
    fread(buf,1,l,f);
    fread(&a,1,4,f);
    Define(buf,a);
    i+=8+l;
  }
  fclose(f);
#ifdef WIN32
  OutputDebugString("Finished Defines, starting main compiler loop");
#endif
}
void RecodeProc(char*script,char*romfn)
{
#ifndef DLL
  FILE*CurrFile;
#endif
  FILE*IncFile;
  FILE*RomFile;
#ifdef WIN32
  char*strings;
#endif
  char*Script;             //Whoops, used the same name for the filename.
  //Use caps-lock carefully.
  char buf[1024],buf2[1024],buf3[1024];
  void*temp_ptr;
  unsigned int start=0;
  unsigned int dynu=0;
  unsigned int
#ifndef DLL
  fs,
#endif
  fst,i,j,k,l,arg1,arg2,arg3,arg4,arg5,arg6;//,arg7;
  codeblock*c;
  codeblock*d;
  c=NULL;
  RomFile=fopen(romfn,"r+b");
  if (RomFile==NULL)
  {
    strcat(romfn,".gba");
    RomFile=fopen(romfn,"r+b");
  }
  DoDefines();
#ifndef DLL
  strcat(GlobBuf,"pokeinc.txt");
  IncFile=fopen(GlobBuf,"rb");
#else
  IncFile=fopen("pokeinc.txt","rb");
#endif
  /*if(IncFile==NULL)
  {
    printf("Cannot open pokeinc.txt\n");
  #ifdef WIN32
    MessageBox(NULL,"The default includes (pokeinc.txt) could not be opened","Error",0x10);
  #endif
  #ifndef DLL
    fclose(CurrFile);
  #endif
    return;
  }*/
  if (RomFile==NULL)
  {
    printf("Cannot open ROM\n");
#ifdef WIN32
    MessageBox(NULL,"The ROM file could not be opened","Error",0x10);
#endif
    if (IncFile)
      fclose(IncFile);
#ifndef DLL
    fclose(CurrFile);
#endif
    return;
  }
#ifndef DLL
  CurrFile=fopen(script,"rb");
  if (CurrFile!=NULL)
  {
    fseek(CurrFile,0,SEEK_END);
    fs=ftell(CurrFile);
    fseek(CurrFile,0,SEEK_SET);
#endif

    if (IncFile)
    {
      fseek(IncFile,0,SEEK_END);
      fst=ftell(IncFile);
      fseek(IncFile,0,SEEK_SET);
    } else {
      fst=0;
    }
#ifndef DLL
    Script=malloc(fs+fst+6);
#else
    Script=malloc(strlen(script)+fst+6);
#endif
    if (Script==NULL)
    {
#ifdef WIN32
      MessageBox(NULL,"malloc() Failed to allocate memory for the script.","Error",0x10);
#endif
#ifndef DLL
      if (strcmp(script,""))fclose(CurrFile);
#endif
      fclose(IncFile);
      return;
    }
    memset(Script,0,strlen(script)+fst+6);
    if (IncFile)
      fread(Script,1,fst,IncFile);
    else
      *Script=0;
    strcat(Script,"\n\n");
#ifndef DLL
    fread((char*)(Script+fst+2),1,fs,CurrFile);
#else
    strcpy(Script+fst+2,script);
#endif
    strcat(Script,"\n\n");

#ifndef DLL
    LogFile=fopen("PokeScrE.log","wb");
    if (LogFile==NULL)
    {
      printf("Failed to open a logfile\n");
#ifdef WIN32
      MessageBox(NULL,"Failed to open a log.","Error",0x10);
#endif
      free(Script);
      if (strcmp(script,""))fclose(CurrFile);
      if (IncFile)
        fclose(IncFile);
      return;
    }
    fprintf(LogFile,"Opened file.\r\n");
#else
    log("Opened file.\r\n",14);
#endif

    i=0;
    LowerCaseAndRemAll0D(Script);

    if (mode==DIAMOND)
    {
      while (i<strlen(Script))
      {
        switch (chr) //Behave differently according to char
        {
        case 0:  //E O Script
          return;
        case '\'':
          while (chr!='\n'&&chr!=0)
          {
            i++;
          }i--;
          break;
        case ' ':  //Ignore spaces
        case '\n':
          break;
        default:
          j=0;
          while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
          {
            buf[j]=chr;
            i++;
            j++;
          }
          buf[j]=0;
          if (!strcmp(buf,"#define")||!strcmp(buf,"#d"))
          {
            vlog("#DEFINE\r\n");
            while (chr==' ') {
              i++;
            }
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            j=0;
            while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0; //Append null
            sprintf(buf2,"   -> %s\r\n",buf);
            vlog(buf2);
            while (chr==' ') {
              i++;    //The same old same old.
            }
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            k=GetNum("#DEFINE");
            if (!gffs) {
              return;
            }
            Define(buf,k);
            ec();
          }
          aa("#narc")
          {
            vlog("#NARC\r\n");
            arg1=GetNum("#NARC");
            if (!gffs) {
              return;
            }
            arg2=GetNum("#NARC");
            if (!gffs) {
              return;
            }
            fseek(RomFile,0xE,SEEK_SET);
            arg3=0;
            fread(&arg3,1,2,RomFile);
            fseek(RomFile,0x10,SEEK_SET);
            fread(&buf,1,4,RomFile);
            buf[4]=0;
            k=0;
            while (k<arg3&&strcmp(buf,"BTAF"))
            {
              arg4=0;
              fread(&arg4,1,4,RomFile);
              fseek(RomFile,arg4-0x8,SEEK_CUR);
              fread(&buf,1,4,RomFile);
              buf[4]=0;
              k++;
            }
            if (!strcmp(buf,"BTAF"))
            {
              fseek(RomFile,8*(arg1+1),SEEK_CUR);
              fread(&arg1,1,4,RomFile);
              fseek(RomFile,0x10,SEEK_SET);
              fread(&buf,1,4,RomFile);
              buf[4]=0;
              k=0;
              while (k<arg3&&strcmp(buf,"GMIF"))
              {
                arg4=0;
                fread(&arg4,1,4,RomFile);
                fseek(RomFile,arg4-0x8,SEEK_CUR);
                fread(&buf,1,4,RomFile);
                buf[4]=0;
                k++;
              }
              if (!strcmp(buf,"GMIF"))
              {
                fseek(RomFile,4+arg1+arg2,SEEK_CUR);
              }
              else
              {
                log("Incomplete NARC.\r\n",18);
                return;
              }
            }
            else
            {
              log("Incomplete NARC.\r\n",18);
              return;
            }
            ec();
          }
          aa("#quiet")
          {
            IsVerbose=0;
            ec();
          }
          aa("#loud")
          {
            IsVerbose=1;
            ec();
          }
          aa("#gs")
          {
            mode=GOLD;
            goto gsc;
          }
          aa("#c")
          {
            mode=CRYSTAL;
            goto cry;
          }
          aa("#frlg")
          {
            mode=FIRE_RED;
            goto frlg;
          }
          aa("#rse")
          {
            mode=RUBY;
            goto rse;
          }
          aa("#dp")
          {
            mode=DIAMOND;
dp:
            ec();
          }
          aa("#raw")
          {
            vlog("#RAW\r\n");
            k=GetNum("#RAW");
            if (!gffs) {
              return;
            }
            BASIC(k);
            if (k>255)
            {
              BASIC(k>>8);
              if (k>65535)
              {
                BASIC(k>>16);
                if (k>16777215)
                {
                  BASIC(k>>24);
                }
              }
            }
            ec();
          }
          aa("#ptr")
          {
            vlog("#PTR\r\n");
            k=GetNum("#PTR");
            if (!gffs) {
              return;
            }
            rom(k|0x08000000,4);
            ec();
          }
          aa("wildbattle")
          {
            vlog("WILDBATTLE\r\n");
            arg1=GetNum("WILDBATTLE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WILDBATTLE");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WILDBATTLE");
            if (!gffs) {
              return;
            }
            BASIC(0xBC);
            BASIC(0x00);
            BASIC(0x06);
            BASIC(0x00);
            BASIC(0x01);
            BASIC(0x00);
            BASIC(0x01);
            BASIC(0x00);
            BASIC(0x00);
            BASIC(0x00);
            BASIC(0xBD);
            BASIC(0x00);
            BASIC(0xDE);
            BASIC(0x00);
            BASIC(0x00);
            BASIC(0x80);
            BASIC(0x96);
            BASIC(0x00);
            BASIC(0x00);
            BASIC(0x80);
            BASIC(0x05);
            BASIC(0x00);
            BASIC(0x00);
            BASIC(0x00);
            BASIC(0x0C);
            BASIC(0x80);
            BASIC(0x1E);
            BASIC(0x00);
            BASIC(arg1);
            BASIC(0x01);
            rom(arg2,2);
            BASIC(arg3);
            ec();
          }
          aa("m")
          {
            vlog("Movement data...\r\n");
            add_data(c,trans,transbackmove(Script,&i));
            ec();
          }
          aa("=")
          {
            vlog("[STRING]\r\n");
            if (chr==' ') {
              i++;
            }
            else {
              log("Should have a space after the =\r\n",33);
            }
            if (IncFile)
            {
              fseek(IncFile,0,SEEK_END);
              arg6=ftell(IncFile);
            } else
              arg6=0;
            temp_ptr=transbackstr(script,i-arg6-2,c);
            while (chr!='\n'&&chr!=0) {
              i++;
            }
            sprintf(buf2,"   -> %s\r\n",(char*)temp_ptr);
            free(temp_ptr);
            vlog(buf2);
          }
          aa(".")
          {
            vlog("[BINARY]\r\n   ->");
            while (chr==' ') {
              i++;
            }
            k=0;
            while (chr!='\n'&&chr!=0)
            {
              k=1-k;
              while (chr==' ')
              {
                i++;
              }
              j=0;
              while (((char*)("0123456789abcdef"))[j]!=0)
              {
                if (((char*)("0123456789abcdef"))[j]==chr)
                {
                  break;
                }
                j+=1;
              }
              if (((char*)("0123456789abcdef"))[j]==0)
              {
                sprintf(buf2,"Failed to understand hex character '%c'\r\n",chr);
                log(buf2,strlen(buf2));
                return;
              }
              if (k==0)
              {
                l|=j;
                rom(j,1);
                if (IsVerbose)
                {
                  sprintf(buf2," %02X",l);
                  log(buf2,strlen(buf2));
                }
              }
              else
              {
                l=j<<4;
              }
              i++;
            }
            vlog("\r\n");
          }
          else
          {
            sprintf(buf2,"Warning: Unknown command \"%s\"\r\n",buf);
            log(buf2,strlen(buf2));
            while (chr!='\n'&&chr!=0)
            {
              i++;
            }
          }
          break;
        }
        i++;
      }
    }
    else if (mode==CRYSTAL)
    {
      while (i<strlen(Script))
      {
        switch (chr) //Behave differently according to char
        {
        case 0:  //E O Script
          return;
        case '\'':
          while (chr!='\n'&&chr!=0)
          {
            i++;
          }i--;
          break;
        case ' ':  //Ignore spaces
        case '\n':
          break;
        default:
          j=0;
          while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
          {
            buf[j]=chr;
            i++;
            j++;
          }
          buf[j]=0;
          if (!strcmp(buf,"#define")||!strcmp(buf,"#d"))
          {
            vlog("#DEFINE\r\n");
            while (chr==' ') {
              i++;
            }
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            j=0;
            while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0; //Append null
            sprintf(buf2,"   -> %s\r\n",buf);
            vlog(buf2);
            while (chr==' ') {
              i++;    //The same old same old.
            }
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            k=GetNum("#DEFINE");
            if (!gffs) {
              return;
            }
            Define(buf,k);
            ec();
          }
          aa("#dyn")
          {
            vlog("#DYN\r\n");
            start=GetNum("#DYN");
            if (!gffs)return;
            dyntype=0;
            ec();
          }
          aa("#dyn2")
          {
            vlog("#DYN2\r\n");
            start=GetNum("#DYN2");
            if (!gffs)return;
            dyntype=1;
            ec();
          }
          aa("#dynamic")
          {
            vlog("#DYNAMIC\r\n");
            start=GetNum("#DYNAMIC");
            if (!gffs)return;
            dyntype=0;
            ec();
          }
          aa("#org")
          {
            eorg=0;
            vlog("#ORG\r\n");
            while (chr==' ')i++;
            buf[0]=0;
            if (chr=='@')
            {
              dynu=1;
              j=0;
              while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
              {
                buf[j]=chr;
                i++;
                j++;
              }
              buf[j]=0;
              sprintf(buf2,"   -> %s\r\n",buf);
              log(buf2,strlen(buf2));
            }
            else
            {
              k=GetNum("#ORG");
              if (!gffs) {
                return;
              }
            }
            d=malloc(sizeof(codeblock));
            if (*buf==0)
              init_codeblock(d,NULL);
            else
              init_codeblock(d,buf);
            if (*buf==0)d->org=k;
            d->prev=c;
            if (c!=NULL)c->next=d;
            c=d;
            ec();
          }
          aa("#eorg")
          {
            eorg=1;
            vlog("#EORG\r\n");
            while (chr==' ')i++;
            buf[0]=0;
            if (chr=='@')
            {
              j=0;
              while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
              {
                buf[j]=chr;
                i++;
                j++;
              }
              buf[j]=0;
              sprintf(buf2,"   -> %s\r\n",buf);
              log(buf2,strlen(buf2));
            }
            else
            {
              k=GetNum("#EORG");
              if (!gffs) {
                return;
              }
            }
            d=malloc(sizeof(codeblock));
            if (*buf==0)
              init_codeblock(d,NULL);
            else
              init_codeblock(d,buf);
            if (*buf==0)d->org=k;
            d->prev=c;
            if (c!=NULL)c->next=d;
            c=d;
            ec();
          }
          aa("#quiet")
          {
            IsVerbose=0;
            ec();
          }
          aa("#loud")
          {
            IsVerbose=1;
            ec();
          }
          aa("#gs")
          {
            mode=GOLD;
            goto gsc;
          }
          aa("#c")
          {
            mode=CRYSTAL;
cry:
            ec();
          }
          aa("#frlg")
          {
            mode=FIRE_RED;
            goto frlg;
            ec();
          }
          aa("#rse")
          {
            mode=RUBY;
            goto rse;
            ec();
          }
          aa("#dp")
          {
            mode=DIAMOND;
            goto dp;
            ec();
          }
          aa("#raw")
          {
            vlog("#RAW\r\n");
            k=GetNum("#RAW");
            if (!gffs) {
              return;
            }
            BASIC(k);
            if (k>255)
            {
              BASIC(k>>8);
              if (k>65535)
              {
                BASIC(k>>16);
                if (k>16777215)
                {
                  BASIC(k>>24);
                }
              }
            }
            ec();
          }
          aa("end")                 {
            BASIC(CRY_END);
            ec();
          }
          aa("return")              {
            BASIC(CRY_RETURN);
            ec();
          }
          aa("reloadandreturn")     {
            BASIC(CRY_RELOADANDRETURN);
            ec();
          }
          aa("checktriggers")       {
            BASIC(CRY_CHECKTRIGGERS);
            ec();
          }
          aa("checkver")            {
            BASIC(CRY_CHECKVER);
            ec();
          }
          aa("wildoff")             {
            BASIC(CRY_WILDOFF);
            ec();
          }
          aa("wildon")              {
            BASIC(CRY_WILDON);
            ec();
          }
          aa("itemnotify")          {
            BASIC(CRY_ITEMNOTIFY);
            ec();
          }
          aa("pocketisfull")        {
            BASIC(CRY_POCKETISFULL);
            ec();
          }
          aa("loadfont")            {
            BASIC(CRY_LOADFONT);
            ec();
          }
          aa("loadmovesprites")     {
            BASIC(CRY_LOADMOVESPRITES);
            ec();
          }
          aa("release")             {
            BASIC(CRY_LOADMOVESPRITES);
            ec();
          }
          aa("yesorno")             {
            BASIC(CRY_YESORNO);
            ec();
          }
          aa("writebackup")         {
            BASIC(CRY_WRITEBACKUP);
            ec();
          }
          aa("closetext")           {
            BASIC(CRY_CLOSETEXT);
            ec();
          }
          aa("keeptextopen")        {
            BASIC(CRY_KEEPTEXTOPEN);
            ec();
          }
          aa("pokepicyesorno")      {
            BASIC(CRY_POKEPICYESORNO);
            ec();
          }
          aa("interpretmenu")       {
            BASIC(CRY_INTERPRETMENU);
            ec();
          }
          aa("interpretmenu2")      {
            BASIC(CRY_INTERPRETMENU2);
            ec();
          }
          aa("loadpikadata")        {
            BASIC(CRY_LOADPIKADATA);
            ec();
          }
          aa("clearfight")          {
            BASIC(CRY_CLEARFIGHT);
            ec();
          }
          aa("loadtrainerseen")     {
            BASIC(CRY_LOADTRAINERSEEN);
            ec();
          }
          aa("startbattle")         {
            BASIC(CRY_STARTBATTLE);
            ec();
          }
          aa("returnafterbattle")   {
            BASIC(CRY_RETURNAFTERBATTLE);
            ec();
          }
          aa("talkafter")           {
            BASIC(CRY_TALKAFTER);
            ec();
          }
          aa("talkaftercancel")     {
            BASIC(CRY_TALKAFTERCANCEL);
            ec();
          }
          aa("talkaftercheck")      {
            BASIC(CRY_TALKAFTERCHECK);
            ec();
          }
          aa("faceplayer")          {
            BASIC(CRY_FACEPLAYER);
            ec();
          }
          aa("stopfollow")          {
            BASIC(CRY_STOPFOLLOW);
            ec();
          }
          aa("reloadmap")           {
            BASIC(CRY_RELOADMAP);
            ec();
          }
          aa("reloadmappart")       {
            BASIC(CRY_RELOADMAPPART);
            ec();
          }
          aa("playrammusic")        {
            BASIC(CRY_PLAYRAMMUSIC);
            ec();
          }
          aa("playmapmusic")        {
            BASIC(CRY_PLAYMAPMUSIC);
            ec();
          }
          aa("reloadmapmusic")      {
            BASIC(CRY_RELOADMAPMUSIC);
            ec();
          }
          aa("waitbutton")          {
            BASIC(CRY_WAITBUTTON);
            ec();
          }
          aa("warpsound")           {
            BASIC(CRY_WARPSOUND);
            ec();
          }
          aa("specialsound")        {
            BASIC(CRY_SPECIALSOUND);
            ec();
          }
          aa("warpcheck")           {
            BASIC(CRY_WARPCHECK);
            ec();
          }
          aa("resetfuncs")          {
            BASIC(CRY_RESETFUNCS);
            ec();
          }
          aa("hangup")              {
            BASIC(CRY_HANGUP);
            ec();
          }
          aa("checkphonecall")      {
            BASIC(CRY_CHECKPHONECALL);
            ec();
          }
          aa("halloffame")          {
            BASIC(CRY_HALLOFFAME);
            ec();
          }
          aa("credits")             {
            BASIC(CRY_CREDITS);
            ec();
          }
          aa("text-newline")        {
            BASIC(5);
            ec();
          }
          aa("text-waitbutton")     {
            BASIC(6);
            ec();
          }
          aa("text-newlinewitharrow") {
            BASIC(7);
            ec();
          }
          aa("text-switchtoasm")    {
            BASIC(8);
            ec();
          }
          aa("text-interpretdata")  {
            BASIC(10);
            ec();
          }
          aa("text-playsound0")     {
            BASIC(11);
            ec();
          }
          aa("text-waitbutton2")    {
            BASIC(13);
            ec();
          }
          aa("text-playsound9")     {
            BASIC(14);
            ec();
          }
          aa("text-playsound1")     {
            BASIC(15);
            ec();
          }
          aa("text-playsound2")     {
            BASIC(16);
            ec();
          }
          aa("text-playsounda")     {
            BASIC(17);
            ec();
          }
          aa("text-playsoundd")     {
            BASIC(18);
            ec();
          }
          aa("text-playsoundc")     {
            BASIC(19);
            ec();
          }
          aa("text-day")            {
            BASIC(21);
            ec();
          }
          aa("2call")
          {
            vlog("2CALL\r\n");
            arg1=GetNum("2CALL");
            if (!gffs) {
              return;
            }
            BASIC(CRY_2CALL);
            rom(arg1,2);
            ec();
          }
          aa("3call")
          {
            vlog("3CALL\r\n");
            GoldDyn=1;
            arg1=GetNum("3CALL");
            GoldDyn=0;
            if (!gffs) {
              return;
            }
            BASIC(CRY_3CALL);
            rom(arg1,3);
            ec();
          }
          aa("2ptcall")
          {
            vlog("2PTCALL\r\n");
            arg1=GetNum("2PTCALL");
            if (!gffs) {
              return;
            }
            BASIC(CRY_2PTCALL);
            rom(arg1,2);
            ec();
          }
          aa("2jump")
          {
            vlog("2JUMP\r\n");
            arg1=GetNum("2JUMP");
            if (!gffs) {
              return;
            }
            BASIC(CRY_2JUMP);
            rom(arg1,2);
            ec();
          }
          aa("3jump")
          {
            vlog("3JUMP\r\n");
            GoldDyn=1;
            arg1=GetNum("3JUMP");
            GoldDyn=0;
            if (!gffs) {
              return;
            }
            BASIC(CRY_3JUMP);
            rom(arg1,3);
            ec();
          }
          aa("2ptjump")
          {
            vlog("2PTJUMP\r\n");
            arg1=GetNum("2PTJUMP");
            if (!gffs) {
              return;
            }
            BASIC(CRY_2PTJUMP);
            rom(arg1,2);
            ec();
          }
          aa("text-ram")
          {
            vlog("TEXT-RAM\r\n");
            arg1=GetNum("TEXT-RAM");
            if (!gffs) {
              return;
            }
            BASIC(1);
            rom(arg1,2);
            ec();
          }
          aa("text-reloc")
          {
            vlog("TEXT-RELOC\r\n");
            arg1=GetNum("TEXT-RELOC");
            if (!gffs) {
              return;
            }
            BASIC(3);
            rom(arg1,2);
            ec();
          }
          aa("text-box")
          {
            vlog("TEXT-BOX\r\n");
            arg1=GetNum("TEXT-BOX");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TEXT-BOX");
            if (!gffs) {
              return;
            }
            arg3=GetNum("TEXT-BOX");
            if (!gffs) {
              return;
            }
            BASIC(4);
            rom(arg1,2);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("text-interpretxdata")
          {
            vlog("TEXT-INTERPRETXDATA\r\n");
            arg1=GetNum("TEXT-INTERPRETXDATA");
            if (!gffs) {
              return;
            }
            BASIC(12);
            rom(arg1,1);
            ec();
          }
          aa("text-buffer")
          {
            vlog("TEXT-BUFFER\r\n");
            arg1=GetNum("TEXT-BUFFER");
            if (!gffs) {
              return;
            }
            BASIC(20);
            rom(arg1,1);
            ec();
          }
          aa("text-newtxt")
          {
            vlog("TEXT-NEWTXT\r\n");
            arg1=GetNum("TEXT-NEWTXT");
            if (!gffs) {
              return;
            }
            BASIC(22);
            rom(arg1>>8,2);
            rom(arg1,1);
            ec();
          }
          aa("text-number")
          {
            vlog("TEXT-NUMBER\r\n");
            arg1=GetNum("TEXT-NUMBER");
            if (!gffs) {
              return;
            }
            arg2=(0xF&GetNum("TEXT-NUMBER"))<<4;
            if (!gffs) {
              return;
            }
            arg2|=0xF&GetNum("TEXT-NUMBER");
            if (!gffs) {
              return;
            }
            BASIC(9);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("text-hex")
          {
            vlog("TEXT-HEX\r\n");
            arg1=GetNum("TEXT-HEX");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TEXT-HEX");
            if (!gffs) {
              return;
            }
            arg2&=0x1F;
            while (chr==' ')i++;
            j=0;
            while (chr!='\n'&&chr!=' '&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0;
            if (!strcmp(buf,"money-no-zeros")) {
              arg2|=(7<<5);
            }
            else if (!strcmp(buf,"no-zeros")) {
              arg2|=(6<<5);
            }
            else if (!strcmp(buf,"money-spaces")) {
              arg2|=(5<<5);
            }
            else if (!strcmp(buf,"spaces")) {
              arg2|=(4<<5);
            }
            else if (!strcmp(buf,"money2")) {
              arg2|=(3<<5);
            }
            else if (!strcmp(buf,"normal2")) {
              arg2|=(2<<5);
            }
            else if (!strcmp(buf,"money")) {
              arg2|=(1<<5);
            }
            else if (!strcmp(buf,"normal")) {
              arg2|=(0<<5);
            }
            else
            {
              log("Incorrect arguments to text-hex\r\n",33);
              return;
            }
            BASIC(2);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("if")
          {
            vlog("IF\r\n");
            while (chr==' ')i++;
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Incorrect arguments to IF.\r\n",28);
            }
            j=0;
            while (chr!='\n'&&chr!=' '&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0;
            arg1=0;
            if (!strcmp(buf,"=")||!strcmp(buf,"=="))
            {
              BASIC(CRY_EQBYTE);
            }
            else if (!strcmp(buf,"!=")||!strcmp(buf,"<>"))
            {
              BASIC(CRY_NEQBYTE);
            }
            else if (!strcmp(buf,"<")||!strcmp(buf,"<<"))
            {
              BASIC(CRY_LTBYTE);
            }
            else if (!strcmp(buf,">")||!strcmp(buf,">>"))
            {
              BASIC(CRY_GTBYTE);
            }
            else if (!strcmp(buf,"==0")||!strcmp(buf,"=0")||!strcmp(buf,"false"))
            {
              BASIC(CRY_EQZERO);
              arg1=-1;
            }
            else if (!strcmp(buf,"!=0")||!strcmp(buf,"<>0")||!strcmp(buf,"true"))
            {
              BASIC(CRY_NEQZERO);
              arg1=-1;
            }
            else
            {
              log("Incorrect arguments to IF\r\n",29);
              return;
            }
            if (arg1!=-1)
            {
              arg1=GetNum("IF");
              if (!gffs) {
                return;
              }
            }
            arg2=GetNum("IF");
            if (!gffs) {
              return;
            }
            if (arg1!=-1)
              rom(arg1,1);
            rom(arg2,2);
            ec();
          }
          aa("jumpstd")
          {
            vlog("JUMPSTD\r\n");
            arg1=GetNum("JUMPSTD");
            if (!gffs) {
              return;
            }
            BASIC(CRY_JUMPSTD);
            rom(arg1,2);
            ec();
          }
          aa("callstd")
          {
            vlog("CALLSTD\r\n");
            arg1=GetNum("CALLSTD");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CALLSTD);
            rom(arg1,2);
            ec();
          }
          aa("3callasm")
          {
            vlog("3CALLASM\r\n");
            arg1=GetNum("3CALLASM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_3CALLASM);
            rom(arg1,3);
            ec();
          }
          aa("special")
          {
            vlog("SPECIAL\r\n");
            arg1=GetNum("SPECIAL");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SPECIAL);
            rom(arg1,2);
            ec();
          }
          aa("2ptcallasm")
          {
            vlog("2PTCALLASM\r\n");
            arg1=GetNum("2PTCALLASM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_2PTCALLASM);
            rom(arg1,2);
            ec();
          }
          aa("checkmaptriggers")
          {
            vlog("CHECKMAPTRIGGERS\r\n");
            arg1=GetNum("CHECKMAPTRIGGERS");
            if (!gffs) {
              return;
            }
            arg2=GetNum("CHECKMAPTRIGGERS");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKMAPTRIGGERS);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("domaptrigger")
          {
            vlog("DOMAPTRIGGER\r\n");
            arg1=GetNum("DOMAPTRIGGER");
            if (!gffs) {
              return;
            }
            arg2=GetNum("DOMAPTRIGGER");
            if (!gffs) {
              return;
            }
            arg3=GetNum("DOMAPTRIGGER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DOMAPTRIGGER);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("dotrigger")
          {
            vlog("DOTRIGGER\r\n");
            arg1=GetNum("DOTRIGGER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DOTRIGGER);
            rom(arg1,1);
            ec();
          }
          aa("loadvar")
          {
            vlog("LOADVAR\r\n");
            arg1=GetNum("LOADVAR");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOADVAR);
            rom(arg1,1);
            ec();
          }
          aa("addvar")
          {
            vlog("ADDVAR\r\n");
            arg1=GetNum("ADDVAR");
            if (!gffs) {
              return;
            }
            BASIC(CRY_ADDVAR);
            rom(arg1,1);
            ec();
          }
          aa("random")
          {
            vlog("RANDOM\r\n");
            arg1=GetNum("RANDOM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_RANDOM);
            rom(arg1,1);
            ec();
          }
          aa("copybytetovar")
          {
            vlog("COPYBYTETOVAR\r\n");
            arg1=GetNum("COPYBYTETOVAR");
            if (!gffs) {
              return;
            }
            BASIC(CRY_COPYBYTETOVAR);
            rom(arg1,2);
            ec();
          }
          aa("copyvartobyte")
          {
            vlog("COPYVARTOBYTE\r\n");
            arg1=GetNum("COPYVARTOBYTE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_COPYVARTOBYTE);
            rom(arg1,2);
            ec();
          }
          aa("checkcode")
          {
            vlog("CHECKCODE\r\n");
            arg1=GetNum("CHECKCODE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKCODE);
            rom(arg1,1);
            ec();
          }
          aa("writevarcode")
          {
            vlog("WRITEVARCODE\r\n");
            arg1=GetNum("WRITEVARCODE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WRITEVARCODE);
            rom(arg1,1);
            ec();
          }
          aa("writecode")
          {
            vlog("WRITECODE\r\n");
            arg1=GetNum("WRITECODE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WRITECODE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WRITECODE);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("giveitem")
          {
            vlog("GIVEITEM\r\n");
            arg1=GetNum("GIVEITEM");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEITEM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_GIVEITEM);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("takeitem")
          {
            vlog("TAKEITEM\r\n");
            arg1=GetNum("TAKEITEM");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TAKEITEM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TAKEITEM);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("givemoney")
          {
            vlog("GIVEMONEY\r\n");
            arg1=GetNum("GIVEMONEY");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEMONEY");
            if (!gffs) {
              return;
            }
            BASIC(CRY_GIVEMONEY);
            rom(arg1,1);
            rom(arg2,3);
            ec();
          }
          aa("takemoney")
          {
            vlog("TAKEMONEY\r\n");
            arg1=GetNum("TAKEMONEY");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TAKEMONEY");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TAKEMONEY);
            rom(arg1,1);
            rom(arg2,3);
            ec();
          }
          aa("checkmoney")
          {
            vlog("CHECKMONEY\r\n");
            arg1=GetNum("CHECKMONEY");
            if (!gffs) {
              return;
            }
            arg2=GetNum("CHECKMONEY");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKMONEY);
            rom(arg1,1);
            rom(arg2,3);
            ec();
          }
          aa("checkitem")
          {
            vlog("CHECKITEM\r\n");
            arg1=GetNum("CHECKITEM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKITEM);
            rom(arg1,1);
            ec();
          }
          aa("givephonenumber")
          {
            vlog("GIVEPHONENUMBER\r\n");
            arg1=GetNum("GIVEPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_GIVEPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("takephonenumber")
          {
            vlog("TAKEPHONENUMBER\r\n");
            arg1=GetNum("TAKEPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TAKEPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("checkphonenumber")
          {
            vlog("CHECKPHONENUMBER\r\n");
            arg1=GetNum("CHECKPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("checktime")
          {
            vlog("CHECKTIME\r\n");
            arg1=GetNum("CHECKTIME");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKTIME);
            rom(arg1,1);
            ec();
          }
          aa("checkpoke")
          {
            vlog("CHECKPOKE\r\n");
            arg1=GetNum("CHECKPOKE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKPOKE);
            rom(arg1,1);
            ec();
          }
          aa("givepoke")
          {
            vlog("GIVEPOKE\r\n");
            arg1=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            arg3=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            arg4=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            if (arg4==1)
            {
              arg5=GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              arg6=GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
            }
            BASIC(CRY_GIVEPOKE);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            rom(arg4,1);
            if (arg4==1)
            {
              rom(arg5,1);
              rom(arg6,1);
            }
            ec();
          }
          aa("giveegg")
          {
            vlog("GIVEEGG\r\n");
            arg1=GetNum("GIVEEGG");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEEGG");
            if (!gffs) {
              return;
            }
            BASIC(CRY_GIVEEGG);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("givepokeitem")
          {
            vlog("GIVEPOKEITEM\r\n");
            arg1=GetNum("GIVEPOKEITEM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_GIVEPOKEITEM);
            rom(arg1,2);
            ec();
          }
          aa("takeifletter")
          {
            vlog("TAKEIFLETTER\r\n");
            arg1=GetNum("TAKEIFLETTER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TAKEIFLETTER);
            rom(arg1,2);
            ec();
          }
          aa("checkbit1")
          {
            vlog("CHECKBIT1\r\n");
            arg1=GetNum("CHECKBIT1");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKBIT1);
            rom(arg1,2);
            ec();
          }
          aa("clearbit1")
          {
            vlog("CLEARBIT1\r\n");
            arg1=GetNum("CLEARBIT1");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CLEARBIT1);
            rom(arg1,2);
            ec();
          }
          aa("setbit1")
          {
            vlog("SETBIT1\r\n");
            arg1=GetNum("SETBIT1");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SETBIT1);
            rom(arg1,2);
            ec();
          }
          aa("checkbit2")
          {
            vlog("CHECKBIT2\r\n");
            arg1=GetNum("CHECKBIT2");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKBIT2);
            rom(arg1,2);
            ec();
          }
          aa("clearbit2")
          {
            vlog("CLEARBIT2\r\n");
            arg1=GetNum("CLEARBIT2");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CLEARBIT2);
            rom(arg1,2);
            ec();
          }
          aa("setbit2")
          {
            vlog("SETBIT2\r\n");
            arg1=GetNum("SETBIT2");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SETBIT2);
            rom(arg1,2);
            ec();
          }
          aa("givecoins")
          {
            vlog("GIVECOINS\r\n");
            arg1=GetNum("GIVECOINS");
            if (!gffs) {
              return;
            }
            BASIC(CRY_GIVECOINS);
            rom(arg1,2);
            ec();
          }
          aa("takecoins")
          {
            vlog("TAKECOINS\r\n");
            arg1=GetNum("TAKECOINS");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TAKECOINS);
            rom(arg1,2);
            ec();
          }
          aa("checkcoins")
          {
            vlog("CHECKCOINS\r\n");
            arg1=GetNum("CHECKCOINS");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHECKCOINS);
            rom(arg1,2);
            ec();
          }
          aa("xycompare")
          {
            vlog("XYCOMPARE\r\n");
            arg1=GetNum("XYCOMPARE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_XYCOMPARE);
            rom(arg1,2);
            ec();
          }
          aa("warpmod")
          {
            vlog("WARPMOD\r\n");
            arg1=GetNum("WARPMOD");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WARPMOD");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WARPMOD");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WARPMOD);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("warp")
          {
            vlog("WARP\r\n");
            arg1=GetNum("WARP");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WARP");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WARP");
            if (!gffs) {
              return;
            }
            arg4=GetNum("WARP");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WARP);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            rom(arg4,1);
            ec();
          }
          aa("warpfacing")
          {
            vlog("WARPFACING\r\n");
            arg1=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg4=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg5=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WARPFACING);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            rom(arg4,1);
            rom(arg5,1);
            ec();
          }
          aa("blackoutmod")
          {
            vlog("BLACKOUTMOD\r\n");
            arg1=GetNum("BLACKOUTMOD");
            if (!gffs) {
              return;
            }
            arg2=GetNum("BLACKOUTMOD");
            if (!gffs) {
              return;
            }
            BASIC(CRY_BLACKOUTMOD);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("moneytotext")
          {
            vlog("MONEYTOTEXT\r\n");
            arg1=GetNum("MONEYTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("MONEYTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_MONEYTOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("coinstotext")
          {
            vlog("COINSTOTEXT\r\n");
            arg1=GetNum("COINSTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_COINSTOTEXT);
            rom(arg1,1);
            ec();
          }
          aa("vartotext")
          {
            vlog("VARTOTEXT\r\n");
            arg1=GetNum("VARTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_VARTOTEXT);
            rom(arg1,1);
            ec();
          }
          aa("poketotext")
          {
            vlog("POKETOTEXT\r\n");
            arg1=GetNum("POKETOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("POKETOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_POKETOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("itemtotext")
          {
            vlog("ITEMTOTEXT\r\n");
            arg1=GetNum("ITEMTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("ITEMTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_ITEMTOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("locationtotext")
          {
            vlog("LOCATIONTOTEXT\r\n");
            arg1=GetNum("LOCATIONTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOCATIONTOTEXT);
            rom(arg1,1);
            ec();
          }
          aa("trainertotext")
          {
            vlog("TRAINERTOTEXT\r\n");
            arg1=GetNum("TRAINERTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TRAINERTOTEXT");
            if (!gffs) {
              return;
            }
            arg3=GetNum("TRAINERTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TRAINERTOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("stringtotext")
          {
            vlog("STRINGTOTEXT\r\n");
            arg1=GetNum("STRINGTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("STRINGTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_STRINGTOTEXT);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("refreshscreen")
          {
            vlog("REFRESHSCREEN\r\n");
            arg1=GetNum("REFRESHSCREEN");
            if (!gffs) {
              return;
            }
            BASIC(CRY_REFRESHSCREEN);
            rom(arg1,1);
            ec();
          }
          aa("c1celoadbyte")
          {
            vlog("C1CELOADBYTE\r\n");
            arg1=GetNum("C1CELOADBYTE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_C1CELOADBYTE);
            rom(arg1,1);
            ec();
          }
          aa("3writetext")
          {
            vlog("3WRITETEXT\r\n");
            arg1=GetNum("3WRITETEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_3WRITETEXT);
            rom(arg1,3);
            ec();
          }
          aa("2writetext")
          {
            vlog("2WRITETEXT\r\n");
            arg1=GetNum("2WRITETEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_2WRITETEXT);
            rom(arg1,2);
            ec();
          }
          aa("repeattext")
          {
            vlog("REPEATTEXT\r\n");
            arg1=GetNum("REPEATTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_REPEATTEXT);
            rom(arg1,2);
            ec();
          }
          aa("loadmenudata")
          {
            vlog("LOADMENUDATA\r\n");
            arg1=GetNum("LOADMENUDATA");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOADMENUDATA);
            rom(arg1,2);
            ec();
          }
          aa("jumptextfaceplayer")
          {
            vlog("JUMPTEXTFACEPLAYER\r\n");
            arg1=GetNum("JUMPTEXTFACEPLAYER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_JUMPTEXTFACEPLAYER);
            rom(arg1,2);
            ec();
          }
          aa("jumptext")
          {
            vlog("JUMPTEXT\r\n");
            arg1=GetNum("JUMPTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_JUMPTEXT);
            rom(arg1,2);
            ec();
          }
          aa("pokepic")
          {
            vlog("POKEPIC\r\n");
            arg1=GetNum("POKEPIC");
            if (!gffs) {
              return;
            }
            BASIC(CRY_POKEPIC);
            rom(arg1,1);
            ec();
          }
          aa("loadpokedata")
          {
            vlog("LOADPOKEDATA\r\n");
            arg1=GetNum("LOADPOKEDATA");
            if (!gffs) {
              return;
            }
            arg2=GetNum("LOADPOKEDATA");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOADPOKEDATA);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("loadtrainer")
          {
            vlog("LOADTRAINER\r\n");
            arg1=GetNum("LOADTRAINER");
            if (!gffs) {
              return;
            }
            arg2=GetNum("LOADTRAINER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOADTRAINER);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("catchtutorial")
          {
            vlog("CATCHTUTORIAL\r\n");
            arg1=GetNum("CATCHTUTORIAL");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CATCHTUTORIAL);
            rom(arg1,1);
            ec();
          }
          aa("trainertext")
          {
            vlog("TRAINERTEXT\r\n");
            arg1=GetNum("TRAINERTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TRAINERTEXT);
            rom(arg1,1);
            ec();
          }
          aa("trainerstatus")
          {
            vlog("TRAINERSTATUS\r\n");
            arg1=GetNum("TRAINERSTATUS");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TRAINERSTATUS);
            rom(arg1,1);
            ec();
          }
          aa("winlosstext")
          {
            vlog("WINLOSSTEXT\r\n");
            arg1=GetNum("WINLOSSTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WINLOSSTEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WINLOSSTEXT);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("setlasttalked")
          {
            vlog("SETLASTTALKED\r\n");
            arg1=GetNum("SETLASTTALKED");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SETLASTTALKED);
            rom(arg1,1);
            ec();
          }
          aa("applymovement")
          {
            vlog("APPLYMOVEMENT\r\n");
            arg1=GetNum("APPLYMOVEMENT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("APPLYMOVEMENT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_APPLYMOVEMENT);
            rom(arg1,1);
            rom(arg2,2);
            ec();
          }
          aa("applymoveother")
          {
            vlog("APPLYMOVEOTHER\r\n");
            arg1=GetNum("APPLYMOVEOTHER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_APPLYMOVEOTHER);
            rom(arg1,2);
            ec();
          }
          aa("faceperson")
          {
            vlog("FACEPERSON\r\n");
            arg1=GetNum("FACEPERSON");
            if (!gffs) {
              return;
            }
            arg2=GetNum("FACEPERSON");
            if (!gffs) {
              return;
            }
            BASIC(CRY_FACEPERSON);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("variablesprite")
          {
            vlog("VARIABLESPRITE\r\n");
            arg1=GetNum("VARIABLESPRITE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("VARIABLESPRITE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_VARIABLESPRITE);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("disappear")
          {
            vlog("DISAPPEAR\r\n");
            arg1=GetNum("DISAPPEAR");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DISAPPEAR);
            rom(arg1,1);
            ec();
          }
          aa("appear")
          {
            vlog("APPEAR\r\n");
            arg1=GetNum("APPEAR");
            if (!gffs) {
              return;
            }
            BASIC(CRY_APPEAR);
            rom(arg1,1);
            ec();
          }
          aa("follow")
          {
            vlog("FOLLOW\r\n");
            arg1=GetNum("FOLLOW");
            if (!gffs) {
              return;
            }
            arg2=GetNum("FOLLOW");
            if (!gffs) {
              return;
            }
            BASIC(CRY_FOLLOW);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("moveperson")
          {
            vlog("MOVEPERSON\r\n");
            arg1=GetNum("MOVEPERSON");
            if (!gffs) {
              return;
            }
            arg2=GetNum("MOVEPERSON");
            if (!gffs) {
              return;
            }
            arg3=GetNum("MOVEPERSON");
            if (!gffs) {
              return;
            }
            BASIC(CRY_MOVEPERSON);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("writepersonloc")
          {
            vlog("WRITEPERSONLOC\r\n");
            arg1=GetNum("WRITEPERSONLOC");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WRITEPERSONLOC);
            rom(arg1,1);
            ec();
          }
          aa("loademote")
          {
            vlog("LOADEMOTE\r\n");
            arg1=GetNum("LOADEMOTE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOADEMOTE);
            rom(arg1,1);
            ec();
          }
          aa("showemote")
          {
            vlog("SHOWEMOTE\r\n");
            arg1=GetNum("SHOWEMOTE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("SHOWEMOTE");
            if (!gffs) {
              return;
            }
            arg3=GetNum("SHOWEMOTE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SHOWEMOTE);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("spriteface")
          {
            vlog("SPRITEFACE\r\n");
            arg1=GetNum("SPRITEFACE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("SPRITEFACE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SPRITEFACE);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("follownotexact")
          {
            vlog("FOLLOWNOTEXACT\r\n");
            arg1=GetNum("FOLLOWNOTEXACT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("FOLLOWNOTEXACT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_FOLLOWNOTEXACT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("earthquake")
          {
            vlog("EARTHQUAKE\r\n");
            arg1=GetNum("EARTHQUAKE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_EARTHQUAKE);
            rom(arg1,1);
            ec();
          }
          aa("swapmaps")
          {
            vlog("SWAPMAPS\r\n");
            arg1=GetNum("SWAPMAPS");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SWAPMAPS);
            rom(arg1,3);
            ec();
          }
          aa("changeblock")
          {
            vlog("CHANGEBLOCK\r\n");
            arg1=GetNum("CHANGEBLOCK");
            if (!gffs) {
              return;
            }
            arg2=GetNum("CHANGEBLOCK");
            if (!gffs) {
              return;
            }
            arg3=GetNum("CHANGEBLOCK");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CHANGEBLOCK);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("writecmdqueue")
          {
            vlog("WRITECMDQUEUE\r\n");
            arg1=GetNum("WRITECMDQUEUE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WRITECMDQUEUE);
            rom(arg1,2);
            ec();
          }
          aa("delcmdqueue")
          {
            vlog("DELCMDQUEUE\r\n");
            arg1=GetNum("DELCMDQUEUE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DELCMDQUEUE);
            rom(arg1,1);
            ec();
          }
          aa("playmusic")
          {
            vlog("PLAYMUSIC\r\n");
            arg1=GetNum("PLAYMUSIC");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PLAYMUSIC);
            rom(arg1,2);
            ec();
          }
          aa("musicfadeout")
          {
            vlog("MUSICFADEOUT\r\n");
            arg1=GetNum("MUSICFADEOUT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("MUSICFADEOUT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_MUSICFADEOUT);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("cry")
          {
            vlog("CRY\r\n");
            arg1=GetNum("CRY");
            if (!gffs) {
              return;
            }
            BASIC(CRY_CRY);
            rom(arg1,2);
            ec();
          }
          aa("playsound")
          {
            vlog("PLAYSOUND\r\n");
            arg1=GetNum("PLAYSOUND");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PLAYSOUND);
            rom(arg1,2);
            ec();
          }
          aa("passtoengine")
          {
            vlog("PASSTOENGINE\r\n");
            arg1=GetNum("PASSTOENGINE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PASSTOENGINE);
            rom(arg1,3);
            ec();
          }
          aa("newloadmap")
          {
            vlog("NEWLOADMAP\r\n");
            arg1=GetNum("NEWLOADMAP");
            if (!gffs) {
              return;
            }
            BASIC(CRY_NEWLOADMAP);
            rom(arg1,1);
            ec();
          }
          aa("pause")
          {
            vlog("PAUSE\r\n");
            arg1=GetNum("PAUSE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PAUSE);
            rom(arg1,1);
            ec();
          }
          aa("deactivatefacing")
          {
            vlog("DEACTIVATEFACING\r\n");
            arg1=GetNum("DEACTIVATEFACING");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DEACTIVATEFACING);
            rom(arg1,1);
            ec();
          }
          aa("priorityjump")
          {
            vlog("PRIORITYJUMP\r\n");
            arg1=GetNum("PRIORITYJUMP");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PRIORITYJUMP);
            rom(arg1,2);
            ec();
          }
          aa("ptpriorityjump")
          {
            vlog("PTPRIORITYJUMP\r\n");
            arg1=GetNum("PTPRIORITYJUMP");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PTPRIORITYJUMP);
            rom(arg1,2);
            ec();
          }
          aa("pokemart")
          {
            vlog("POKEMART\r\n");
            arg1=GetNum("POKEMART");
            if (!gffs) {
              return;
            }
            arg2=GetNum("POKEMART");
            if (!gffs) {
              return;
            }
            BASIC(CRY_POKEMART);
            rom(arg1,1);
            rom(arg2,2);
            ec();
          }
          aa("elevator")
          {
            vlog("ELEVATOR\r\n");
            arg1=GetNum("ELEVATOR");
            if (!gffs) {
              return;
            }
            BASIC(CRY_ELEVATOR);
            rom(arg1,2);
            ec();
          }
          aa("trade")
          {
            vlog("TRADE\r\n");
            arg1=GetNum("TRADE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_TRADE);
            rom(arg1,1);
            ec();
          }
          aa("askforphonenumber")
          {
            vlog("ASKFORPHONENUMBER\r\n");
            arg1=GetNum("ASKFORPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(CRY_ASKFORPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("phonecall")
          {
            vlog("PHONECALL\r\n");
            arg1=GetNum("PHONECALL");
            if (!gffs) {
              return;
            }
            BASIC(CRY_PHONECALL);
            rom(arg1,2);
            ec();
          }
          aa("describedecoration")
          {
            vlog("DESCRIBEDECORATION\r\n");
            arg1=GetNum("DESCRIBEDECORATION");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DESCRIBEDECORATION);
            rom(arg1,1);
            ec();
          }
          aa("fruittree")
          {
            vlog("FRUITTREE\r\n");
            arg1=GetNum("FRUITTREE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_FRUITTREE);
            rom(arg1,1);
            ec();
          }
          aa("specialphonecall")
          {
            vlog("SPECIALPHONECALL\r\n");
            arg1=GetNum("SPECIALPHONECALL");
            if (!gffs) {
              return;
            }
            BASIC(CRY_SPECIALPHONECALL);
            rom(arg1,1);
            ec();
          }
          aa("verbosegiveitem")
          {
            vlog("VERBOSEGIVEITEM\r\n");
            arg1=GetNum("VERBOSEGIVEITEM");
            if (!gffs) {
              return;
            }
            arg2=GetNum("VERBOSEGIVEITEM");
            if (!gffs) {
              return;
            }
            BASIC(CRY_VERBOSEGIVEITEM);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("loadwilddata")
          {
            vlog("LOADWILDDATA\r\n");
            arg1=GetNum("LOADWILDDATA");
            if (!gffs) {
              return;
            }
            arg2=GetNum("LOADWILDDATA");
            if (!gffs) {
              return;
            }
            BASIC(CRY_LOADWILDDATA);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("storetext")
          {
            vlog("STORETEXT\r\n");
            arg1=GetNum("STORETEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("STORETEXT");
            if (!gffs) {
              return;
            }
            arg3=GetNum("STORETEXT");
            if (!gffs) {
              return;
            }
            BASIC(CRY_STORETEXT);
            rom(arg1,2);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("displaylocation")
          {
            vlog("DISPLAYLOCATION\r\n");
            arg1=GetNum("DISPLAYLOCATION");
            if (!gffs) {
              return;
            }
            BASIC(CRY_DISPLAYLOCATION);
            rom(arg1,1);
            ec();
          }
          aa("writebyte")
          {
            vlog("WRITEBYTE\r\n");
            arg1=GetNum("WRITEBYTE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WRITEBYTE");
            if (!gffs) {
              return;
            }
            BASIC(CRY_WRITEBYTE);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("m")
          {
            vlog("Movement data...\r\n");
            add_data(c,trans,transbackmove(Script,&i));
            ec();
          }
          aa("=")
          {
            vlog("[STRING]\r\n");
            if (chr==' ') {
              i++;
            }
            else {
              log("Should have a space after the =\r\n",33);
            }
            if (IncFile)
            {
              fseek(IncFile,0,SEEK_END);
              arg6=ftell(IncFile);
            } else
              arg6=0;
            temp_ptr=transbackstr(script,i-arg6-2,c);
            while (chr!='\n'&&chr!=0) {
              i++;
            }
            sprintf(buf2,"   -> %s\r\n",(char*)temp_ptr);
            free(temp_ptr);
            vlog(buf2);
          }
          aa(".")
          {
            vlog("[BINARY]\r\n   ->");
            while (chr==' ') {
              i++;
            }
            k=0;
            while (chr!='\n'&&chr!=0)
            {
              k=1-k;
              while (chr==' ')
              {
                i++;
              }
              j=0;
              while (((char*)("0123456789abcdef"))[j]!=0)
              {
                if (((char*)("0123456789abcdef"))[j]==chr)
                {
                  break;
                }
                j+=1;
              }
              if (((char*)("0123456789abcdef"))[j]==0)
              {
                sprintf(buf2,"Failed to understand hex character '%c'\r\n",chr);
                log(buf2,strlen(buf2));
                return;
              }
              if (k==0)
              {
                l|=j;
                rom(j,1);
                if (IsVerbose)
                {
                  sprintf(buf2," %02X",l);
                  log(buf2,strlen(buf2));
                }
              }
              else
              {
                l=j<<4;
              }
              i++;
            }
            vlog("\r\n");
          }
          else
          {
            sprintf(buf2,"Warning: Unknown command \"%s\"\r\n",buf);
            log(buf2,strlen(buf2));
            while (chr!='\n'&&chr!=0)
            {
              i++;
            }
          }
          break;
        }
        i++;
      }
    }
    else if (mode==GOLD)
    {
      while (i<strlen(Script))
      {
        switch (chr) //Behave differently according to char
        {
        case 0:  //E O Script
          return;
        case '\'':
          while (chr!='\n'&&chr!=0)
          {
            i++;
          }i--;
          break;
        case ' ':  //Ignore spaces
        case '\n':
          break;
        default:
          j=0;
          while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
          {
            buf[j]=chr;
            i++;
            j++;
          }
          buf[j]=0;
          if (!strcmp(buf,"#define")||!strcmp(buf,"#d"))
          {
            vlog("#DEFINE\r\n");
            while (chr==' ') {
              i++;
            }
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            j=0;
            while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0; //Append null
            sprintf(buf2,"   -> %s\r\n",buf);
            vlog(buf2);
            while (chr==' ') {
              i++;    //The same old same old.
            }
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Premature end to #DEFINE!\r\n",27);
              return;
            }
            k=GetNum("#DEFINE");
            if (!gffs) {
              return;
            }
            Define(buf,k);
            ec();
          }
          aa("#dyn")
          {
            vlog("#DYN\r\n");
            start=GetNum("#DYN");
            if (!gffs)return;
            dyntype=0;
            ec();
          }
          aa("#dyn2")
          {
            vlog("#DYN2\r\n");
            start=GetNum("#DYN2");
            if (!gffs)return;
            dyntype=1;
            ec();
          }
          aa("#dynamic")
          {
            vlog("#DYNAMIC\r\n");
            start=GetNum("#DYNAMIC");
            if (!gffs)return;
            dyntype=0;
            ec();
          }
          aa("#org")
          {
            eorg=0;
            vlog("#ORG\r\n");
            while (chr==' ')i++;
            buf[0]=0;
            if (chr=='@')
            {
              dynu=1;
              j=0;
              while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
              {
                buf[j]=chr;
                i++;
                j++;
              }
              buf[j]=0;
              sprintf(buf2,"   -> %s\r\n",buf);
              log(buf2,strlen(buf2));
            }
            else
            {
              k=GetNum("#ORG");
              if (!gffs) {
                return;
              }
            }
            d=malloc(sizeof(codeblock));
            if (*buf==0)
              init_codeblock(d,NULL);
            else
              init_codeblock(d,buf);
            if (*buf==0)d->org=k;
            d->prev=c;
            if (c!=NULL)c->next=d;
            c=d;
            ec();
          }
          aa("#eorg")
          {
            eorg=1;
            vlog("#EORG\r\n");
            while (chr==' ')i++;
            buf[0]=0;
            if (chr=='@')
            {
              j=0;
              while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
              {
                buf[j]=chr;
                i++;
                j++;
              }
              buf[j]=0;
              sprintf(buf2,"   -> %s\r\n",buf);
              log(buf2,strlen(buf2));
            }
            else
            {
              k=GetNum("#EORG");
              if (!gffs) {
                return;
              }
            }
            d=malloc(sizeof(codeblock));
            if (*buf==0)
              init_codeblock(d,NULL);
            else
              init_codeblock(d,buf);
            if (*buf==0)d->org=k;
            d->prev=c;
            if (c!=NULL)c->next=d;
            c=d;
            ec();
          }
          aa("#quiet")
          {
            IsVerbose=0;
            ec();
          }
          aa("#loud")
          {
            IsVerbose=1;
            ec();
          }
          aa("#gs")
          {
            mode=GOLD;
gsc:
            ec();
          }
          aa("#c")
          {
            mode=CRYSTAL;
            goto cry;
          }
          aa("#frlg")
          {
            mode=FIRE_RED;
            goto frlg;
          }
          aa("#rse")
          {
            mode=RUBY;
            goto rse;
          }
          aa("#dp")
          {
            mode=DIAMOND;
            goto dp;
          }
          aa("#raw")
          {
            vlog("#RAW\r\n");
            k=GetNum("#RAW");
            if (!gffs) {
              return;
            }
            BASIC(k);
            if (k>255)
            {
              BASIC(k>>8);
              if (k>65535)
              {
                BASIC(k>>16);
                if (k>16777215)
                {
                  BASIC(k>>24);
                }
              }
            }
            ec();
          }
          aa("end")                 {
            BASIC(GLD_END);
            ec();
          }
          aa("return")              {
            BASIC(GLD_RETURN);
            ec();
          }
          aa("reloadandreturn")     {
            BASIC(GLD_RELOADANDRETURN);
            ec();
          }
          aa("checktriggers")       {
            BASIC(GLD_CHECKTRIGGERS);
            ec();
          }
          aa("checkver")            {
            BASIC(GLD_CHECKVER);
            ec();
          }
          aa("wildoff")             {
            BASIC(GLD_WILDOFF);
            ec();
          }
          aa("wildon")              {
            BASIC(GLD_WILDON);
            ec();
          }
          aa("itemnotify")          {
            BASIC(GLD_ITEMNOTIFY);
            ec();
          }
          aa("pocketisfull")        {
            BASIC(GLD_POCKETISFULL);
            ec();
          }
          aa("loadfont")            {
            BASIC(GLD_LOADFONT);
            ec();
          }
          aa("loadmovesprites")     {
            BASIC(GLD_LOADMOVESPRITES);
            ec();
          }
          aa("release")             {
            BASIC(GLD_LOADMOVESPRITES);
            ec();
          }
          aa("yesorno")             {
            BASIC(GLD_YESORNO);
            ec();
          }
          aa("writebackup")         {
            BASIC(GLD_WRITEBACKUP);
            ec();
          }
          aa("closetext")           {
            BASIC(GLD_CLOSETEXT);
            ec();
          }
          aa("keeptextopen")        {
            BASIC(GLD_KEEPTEXTOPEN);
            ec();
          }
          aa("pokepicyesorno")      {
            BASIC(GLD_POKEPICYESORNO);
            ec();
          }
          aa("interpretmenu")       {
            BASIC(GLD_INTERPRETMENU);
            ec();
          }
          aa("interpretmenu2")      {
            BASIC(GLD_INTERPRETMENU2);
            ec();
          }
          aa("loadpikadata")        {
            BASIC(GLD_LOADPIKADATA);
            ec();
          }
          aa("clearfight")          {
            BASIC(GLD_CLEARFIGHT);
            ec();
          }
          aa("loadtrainerseen")     {
            BASIC(GLD_LOADTRAINERSEEN);
            ec();
          }
          aa("startbattle")         {
            BASIC(GLD_STARTBATTLE);
            ec();
          }
          aa("returnafterbattle")   {
            BASIC(GLD_RETURNAFTERBATTLE);
            ec();
          }
          aa("talkafter")           {
            BASIC(GLD_TALKAFTER);
            ec();
          }
          aa("talkaftercancel")     {
            BASIC(GLD_TALKAFTERCANCEL);
            ec();
          }
          aa("talkaftercheck")      {
            BASIC(GLD_TALKAFTERCHECK);
            ec();
          }
          aa("faceplayer")          {
            BASIC(GLD_FACEPLAYER);
            ec();
          }
          aa("stopfollow")          {
            BASIC(GLD_STOPFOLLOW);
            ec();
          }
          aa("reloadmap")           {
            BASIC(GLD_RELOADMAP);
            ec();
          }
          aa("reloadmappart")       {
            BASIC(GLD_RELOADMAPPART);
            ec();
          }
          aa("playrammusic")        {
            BASIC(GLD_PLAYRAMMUSIC);
            ec();
          }
          aa("playmapmusic")        {
            BASIC(GLD_PLAYMAPMUSIC);
            ec();
          }
          aa("reloadmapmusic")      {
            BASIC(GLD_RELOADMAPMUSIC);
            ec();
          }
          aa("waitbutton")          {
            BASIC(GLD_WAITBUTTON);
            ec();
          }
          aa("warpsound")           {
            BASIC(GLD_WARPSOUND);
            ec();
          }
          aa("specialsound")        {
            BASIC(GLD_SPECIALSOUND);
            ec();
          }
          aa("warpcheck")           {
            BASIC(GLD_WARPCHECK);
            ec();
          }
          aa("resetfuncs")          {
            BASIC(GLD_RESETFUNCS);
            ec();
          }
          aa("hangup")              {
            BASIC(GLD_HANGUP);
            ec();
          }
          aa("checkphonecall")      {
            BASIC(GLD_CHECKPHONECALL);
            ec();
          }
          aa("halloffame")          {
            BASIC(GLD_HALLOFFAME);
            ec();
          }
          aa("credits")             {
            BASIC(GLD_CREDITS);
            ec();
          }
          aa("text-newline")        {
            BASIC(5);
            ec();
          }
          aa("text-waitbutton")     {
            BASIC(6);
            ec();
          }
          aa("text-newlinewitharrow") {
            BASIC(7);
            ec();
          }
          aa("text-switchtoasm")    {
            BASIC(8);
            ec();
          }
          aa("text-interpretdata")  {
            BASIC(10);
            ec();
          }
          aa("text-playsound0")     {
            BASIC(11);
            ec();
          }
          aa("text-waitbutton2")    {
            BASIC(13);
            ec();
          }
          aa("text-playsound9")     {
            BASIC(14);
            ec();
          }
          aa("text-playsound1")     {
            BASIC(15);
            ec();
          }
          aa("text-playsound2")     {
            BASIC(16);
            ec();
          }
          aa("text-playsounda")     {
            BASIC(17);
            ec();
          }
          aa("text-playsoundd")     {
            BASIC(18);
            ec();
          }
          aa("text-playsoundc")     {
            BASIC(19);
            ec();
          }
          aa("text-day")            {
            BASIC(21);
            ec();
          }
          aa("2call")
          {
            vlog("2CALL\r\n");
            arg1=GetNum("2CALL");
            if (!gffs) {
              return;
            }
            BASIC(GLD_2CALL);
            rom(arg1,2);
            ec();
          }
          aa("3call")
          {
            vlog("3CALL\r\n");
            GoldDyn=1;
            arg1=GetNum("3CALL");
            GoldDyn=0;
            if (!gffs) {
              return;
            }
            BASIC(GLD_3CALL);
            rom(arg1,3);
            ec();
          }
          aa("2ptcall")
          {
            vlog("2PTCALL\r\n");
            arg1=GetNum("2PTCALL");
            if (!gffs) {
              return;
            }
            BASIC(GLD_2PTCALL);
            rom(arg1,2);
            ec();
          }
          aa("2jump")
          {
            vlog("2JUMP\r\n");
            arg1=GetNum("2JUMP");
            if (!gffs) {
              return;
            }
            BASIC(GLD_2JUMP);
            rom(arg1,2);
            ec();
          }
          aa("3jump")
          {
            vlog("3JUMP\r\n");
            GoldDyn=1;
            arg1=GetNum("3JUMP");
            GoldDyn=0;
            if (!gffs) {
              return;
            }
            BASIC(GLD_3JUMP);
            rom(arg1,3);
            ec();
          }
          aa("2ptjump")
          {
            vlog("2PTJUMP\r\n");
            arg1=GetNum("2PTJUMP");
            if (!gffs) {
              return;
            }
            BASIC(GLD_2PTJUMP);
            rom(arg1,2);
            ec();
          }
          aa("text-ram")
          {
            vlog("TEXT-RAM\r\n");
            arg1=GetNum("TEXT-RAM");
            if (!gffs) {
              return;
            }
            BASIC(1);
            rom(arg1,2);
            ec();
          }
          aa("text-reloc")
          {
            vlog("TEXT-RELOC\r\n");
            arg1=GetNum("TEXT-RELOC");
            if (!gffs) {
              return;
            }
            BASIC(3);
            rom(arg1,2);
            ec();
          }
          aa("text-box")
          {
            vlog("TEXT-BOX\r\n");
            arg1=GetNum("TEXT-BOX");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TEXT-BOX");
            if (!gffs) {
              return;
            }
            arg3=GetNum("TEXT-BOX");
            if (!gffs) {
              return;
            }
            BASIC(4);
            rom(arg1,2);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("text-interpretxdata")
          {
            vlog("TEXT-INTERPRETXDATA\r\n");
            arg1=GetNum("TEXT-INTERPRETXDATA");
            if (!gffs) {
              return;
            }
            BASIC(12);
            rom(arg1,1);
            ec();
          }
          aa("text-buffer")
          {
            vlog("TEXT-BUFFER\r\n");
            arg1=GetNum("TEXT-BUFFER");
            if (!gffs) {
              return;
            }
            BASIC(20);
            rom(arg1,1);
            ec();
          }
          aa("text-newtxt")
          {
            vlog("TEXT-NEWTXT\r\n");
            arg1=GetNum("TEXT-NEWTXT");
            if (!gffs) {
              return;
            }
            BASIC(22);
            rom(arg1>>8,2);
            rom(arg1,1);
            ec();
          }
          aa("text-number")
          {
            vlog("TEXT-NUMBER\r\n");
            arg1=GetNum("TEXT-NUMBER");
            if (!gffs) {
              return;
            }
            arg2=(0xF&GetNum("TEXT-NUMBER"))<<4;
            if (!gffs) {
              return;
            }
            arg2|=0xF&GetNum("TEXT-NUMBER");
            if (!gffs) {
              return;
            }
            BASIC(9);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("text-hex")
          {
            vlog("TEXT-HEX\r\n");
            arg1=GetNum("TEXT-HEX");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TEXT-HEX");
            if (!gffs) {
              return;
            }
            arg2&=0x1F;
            while (chr==' ')i++;
            j=0;
            while (chr!='\n'&&chr!=' '&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0;
            if (!strcmp(buf,"money-no-zeros")) {
              arg2|=(7<<5);
            }
            else if (!strcmp(buf,"no-zeros")) {
              arg2|=(6<<5);
            }
            else if (!strcmp(buf,"money-spaces")) {
              arg2|=(5<<5);
            }
            else if (!strcmp(buf,"spaces")) {
              arg2|=(4<<5);
            }
            else if (!strcmp(buf,"money2")) {
              arg2|=(3<<5);
            }
            else if (!strcmp(buf,"normal2")) {
              arg2|=(2<<5);
            }
            else if (!strcmp(buf,"money")) {
              arg2|=(1<<5);
            }
            else if (!strcmp(buf,"normal")) {
              arg2|=(0<<5);
            }
            else
            {
              log("Incorrect arguments to text-hex\r\n",33);
              return;
            }
            BASIC(2);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("if")
          {
            vlog("IF\r\n");
            while (chr==' ')i++;
            if (chr=='\n'||chr==0||chr=='\'')
            {
              log("Incorrect arguments to IF.\r\n",28);
            }
            j=0;
            while (chr!='\n'&&chr!=' '&&chr!=0&&chr!='\'')
            {
              buf[j]=chr;
              i++;
              j++;
            }
            buf[j]=0;
            arg1=0;
            if (!strcmp(buf,"=")||!strcmp(buf,"=="))
            {
              BASIC(GLD_EQBYTE);
            }
            else if (!strcmp(buf,"!=")||!strcmp(buf,"<>"))
            {
              BASIC(GLD_NEQBYTE);
            }
            else if (!strcmp(buf,"<")||!strcmp(buf,"<<"))
            {
              BASIC(GLD_LTBYTE);
            }
            else if (!strcmp(buf,">")||!strcmp(buf,">>"))
            {
              BASIC(GLD_GTBYTE);
            }
            else if (!strcmp(buf,"==0")||!strcmp(buf,"=0")||!strcmp(buf,"false"))
            {
              BASIC(GLD_EQZERO);
              arg1=-1;
            }
            else if (!strcmp(buf,"!=0")||!strcmp(buf,"<>0")||!strcmp(buf,"true"))
            {
              BASIC(GLD_NEQZERO);
              arg1=-1;
            }
            else
            {
              log("Incorrect arguments to IF\r\n",29);
              return;
            }
            if (arg1!=-1)
            {
              arg1=GetNum("IF");
              if (!gffs) {
                return;
              }
            }
            arg2=GetNum("IF");
            if (!gffs) {
              return;
            }
            if (arg1!=-1)
              rom(arg1,1);
            rom(arg2,2);
            ec();
          }
          aa("jumpstd")
          {
            vlog("JUMPSTD\r\n");
            arg1=GetNum("JUMPSTD");
            if (!gffs) {
              return;
            }
            BASIC(GLD_JUMPSTD);
            rom(arg1,2);
            ec();
          }
          aa("callstd")
          {
            vlog("CALLSTD\r\n");
            arg1=GetNum("CALLSTD");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CALLSTD);
            rom(arg1,2);
            ec();
          }
          aa("3callasm")
          {
            vlog("3CALLASM\r\n");
            arg1=GetNum("3CALLASM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_3CALLASM);
            rom(arg1,3);
            ec();
          }
          aa("special")
          {
            vlog("SPECIAL\r\n");
            arg1=GetNum("SPECIAL");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SPECIAL);
            rom(arg1,2);
            ec();
          }
          aa("2ptcallasm")
          {
            vlog("2PTCALLASM\r\n");
            arg1=GetNum("2PTCALLASM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_2PTCALLASM);
            rom(arg1,2);
            ec();
          }
          aa("checkmaptriggers")
          {
            vlog("CHECKMAPTRIGGERS\r\n");
            arg1=GetNum("CHECKMAPTRIGGERS");
            if (!gffs) {
              return;
            }
            arg2=GetNum("CHECKMAPTRIGGERS");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKMAPTRIGGERS);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("domaptrigger")
          {
            vlog("DOMAPTRIGGER\r\n");
            arg1=GetNum("DOMAPTRIGGER");
            if (!gffs) {
              return;
            }
            arg2=GetNum("DOMAPTRIGGER");
            if (!gffs) {
              return;
            }
            arg3=GetNum("DOMAPTRIGGER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DOMAPTRIGGER);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("dotrigger")
          {
            vlog("DOTRIGGER\r\n");
            arg1=GetNum("DOTRIGGER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DOTRIGGER);
            rom(arg1,1);
            ec();
          }
          aa("loadvar")
          {
            vlog("LOADVAR\r\n");
            arg1=GetNum("LOADVAR");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOADVAR);
            rom(arg1,1);
            ec();
          }
          aa("addvar")
          {
            vlog("ADDVAR\r\n");
            arg1=GetNum("ADDVAR");
            if (!gffs) {
              return;
            }
            BASIC(GLD_ADDVAR);
            rom(arg1,1);
            ec();
          }
          aa("random")
          {
            vlog("RANDOM\r\n");
            arg1=GetNum("RANDOM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_RANDOM);
            rom(arg1,1);
            ec();
          }
          aa("copybytetovar")
          {
            vlog("COPYBYTETOVAR\r\n");
            arg1=GetNum("COPYBYTETOVAR");
            if (!gffs) {
              return;
            }
            BASIC(GLD_COPYBYTETOVAR);
            rom(arg1,2);
            ec();
          }
          aa("copyvartobyte")
          {
            vlog("COPYVARTOBYTE\r\n");
            arg1=GetNum("COPYVARTOBYTE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_COPYVARTOBYTE);
            rom(arg1,2);
            ec();
          }
          aa("checkcode")
          {
            vlog("CHECKCODE\r\n");
            arg1=GetNum("CHECKCODE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKCODE);
            rom(arg1,1);
            ec();
          }
          aa("writevarcode")
          {
            vlog("WRITEVARCODE\r\n");
            arg1=GetNum("WRITEVARCODE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WRITEVARCODE);
            rom(arg1,1);
            ec();
          }
          aa("writecode")
          {
            vlog("WRITECODE\r\n");
            arg1=GetNum("WRITECODE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WRITECODE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WRITECODE);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("giveitem")
          {
            vlog("GIVEITEM\r\n");
            arg1=GetNum("GIVEITEM");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEITEM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_GIVEITEM);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("takeitem")
          {
            vlog("TAKEITEM\r\n");
            arg1=GetNum("TAKEITEM");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TAKEITEM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TAKEITEM);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("givemoney")
          {
            vlog("GIVEMONEY\r\n");
            arg1=GetNum("GIVEMONEY");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEMONEY");
            if (!gffs) {
              return;
            }
            BASIC(GLD_GIVEMONEY);
            rom(arg1,1);
            rom(arg2,3);
            ec();
          }
          aa("takemoney")
          {
            vlog("TAKEMONEY\r\n");
            arg1=GetNum("TAKEMONEY");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TAKEMONEY");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TAKEMONEY);
            rom(arg1,1);
            rom(arg2,3);
            ec();
          }
          aa("checkmoney")
          {
            vlog("CHECKMONEY\r\n");
            arg1=GetNum("CHECKMONEY");
            if (!gffs) {
              return;
            }
            arg2=GetNum("CHECKMONEY");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKMONEY);
            rom(arg1,1);
            rom(arg2,3);
            ec();
          }
          aa("checkitem")
          {
            vlog("CHECKITEM\r\n");
            arg1=GetNum("CHECKITEM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKITEM);
            rom(arg1,1);
            ec();
          }
          aa("givephonenumber")
          {
            vlog("GIVEPHONENUMBER\r\n");
            arg1=GetNum("GIVEPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_GIVEPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("takephonenumber")
          {
            vlog("TAKEPHONENUMBER\r\n");
            arg1=GetNum("TAKEPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TAKEPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("checkphonenumber")
          {
            vlog("CHECKPHONENUMBER\r\n");
            arg1=GetNum("CHECKPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("checktime")
          {
            vlog("CHECKTIME\r\n");
            arg1=GetNum("CHECKTIME");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKTIME);
            rom(arg1,1);
            ec();
          }
          aa("checkpoke")
          {
            vlog("CHECKPOKE\r\n");
            arg1=GetNum("CHECKPOKE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKPOKE);
            rom(arg1,1);
            ec();
          }
          aa("givepoke")
          {
            vlog("GIVEPOKE\r\n");
            arg1=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            arg3=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            arg4=GetNum("GIVEPOKE");
            if (!gffs) {
              return;
            }
            if (arg4==1)
            {
              arg5=GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              arg6=GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
            }
            BASIC(GLD_GIVEPOKE);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            rom(arg4,1);
            if (arg4==1)
            {
              rom(arg5,1);
              rom(arg6,1);
            }
            ec();
          }
          aa("giveegg")
          {
            vlog("GIVEEGG\r\n");
            arg1=GetNum("GIVEEGG");
            if (!gffs) {
              return;
            }
            arg2=GetNum("GIVEEGG");
            if (!gffs) {
              return;
            }
            BASIC(GLD_GIVEEGG);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("givepokeitem")
          {
            vlog("GIVEPOKEITEM\r\n");
            arg1=GetNum("GIVEPOKEITEM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_GIVEPOKEITEM);
            rom(arg1,2);
            ec();
          }
          aa("takeifletter")
          {
            vlog("TAKEIFLETTER\r\n");
            arg1=GetNum("TAKEIFLETTER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TAKEIFLETTER);
            rom(arg1,2);
            ec();
          }
          aa("checkbit1")
          {
            vlog("CHECKBIT1\r\n");
            arg1=GetNum("CHECKBIT1");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKBIT1);
            rom(arg1,2);
            ec();
          }
          aa("clearbit1")
          {
            vlog("CLEARBIT1\r\n");
            arg1=GetNum("CLEARBIT1");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CLEARBIT1);
            rom(arg1,2);
            ec();
          }
          aa("setbit1")
          {
            vlog("SETBIT1\r\n");
            arg1=GetNum("SETBIT1");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SETBIT1);
            rom(arg1,2);
            ec();
          }
          aa("checkbit2")
          {
            vlog("CHECKBIT2\r\n");
            arg1=GetNum("CHECKBIT2");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKBIT2);
            rom(arg1,2);
            ec();
          }
          aa("clearbit2")
          {
            vlog("CLEARBIT2\r\n");
            arg1=GetNum("CLEARBIT2");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CLEARBIT2);
            rom(arg1,2);
            ec();
          }
          aa("setbit2")
          {
            vlog("SETBIT2\r\n");
            arg1=GetNum("SETBIT2");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SETBIT2);
            rom(arg1,2);
            ec();
          }
          aa("givecoins")
          {
            vlog("GIVECOINS\r\n");
            arg1=GetNum("GIVECOINS");
            if (!gffs) {
              return;
            }
            BASIC(GLD_GIVECOINS);
            rom(arg1,2);
            ec();
          }
          aa("takecoins")
          {
            vlog("TAKECOINS\r\n");
            arg1=GetNum("TAKECOINS");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TAKECOINS);
            rom(arg1,2);
            ec();
          }
          aa("checkcoins")
          {
            vlog("CHECKCOINS\r\n");
            arg1=GetNum("CHECKCOINS");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHECKCOINS);
            rom(arg1,2);
            ec();
          }
          aa("xycompare")
          {
            vlog("XYCOMPARE\r\n");
            arg1=GetNum("XYCOMPARE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_XYCOMPARE);
            rom(arg1,2);
            ec();
          }
          aa("warpmod")
          {
            vlog("WARPMOD\r\n");
            arg1=GetNum("WARPMOD");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WARPMOD");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WARPMOD");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WARPMOD);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("warp")
          {
            vlog("WARP\r\n");
            arg1=GetNum("WARP");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WARP");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WARP");
            if (!gffs) {
              return;
            }
            arg4=GetNum("WARP");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WARP);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            rom(arg4,1);
            ec();
          }
          aa("warpfacing")
          {
            vlog("WARPFACING\r\n");
            arg1=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg3=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg4=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            arg5=GetNum("WARPFACING");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WARPFACING);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            rom(arg4,1);
            rom(arg5,1);
            ec();
          }
          aa("blackoutmod")
          {
            vlog("BLACKOUTMOD\r\n");
            arg1=GetNum("BLACKOUTMOD");
            if (!gffs) {
              return;
            }
            arg2=GetNum("BLACKOUTMOD");
            if (!gffs) {
              return;
            }
            BASIC(GLD_BLACKOUTMOD);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("moneytotext")
          {
            vlog("MONEYTOTEXT\r\n");
            arg1=GetNum("MONEYTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("MONEYTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_MONEYTOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("coinstotext")
          {
            vlog("COINSTOTEXT\r\n");
            arg1=GetNum("COINSTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_COINSTOTEXT);
            rom(arg1,1);
            ec();
          }
          aa("vartotext")
          {
            vlog("VARTOTEXT\r\n");
            arg1=GetNum("VARTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_VARTOTEXT);
            rom(arg1,1);
            ec();
          }
          aa("poketotext")
          {
            vlog("POKETOTEXT\r\n");
            arg1=GetNum("POKETOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("POKETOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_POKETOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("itemtotext")
          {
            vlog("ITEMTOTEXT\r\n");
            arg1=GetNum("ITEMTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("ITEMTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_ITEMTOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("locationtotext")
          {
            vlog("LOCATIONTOTEXT\r\n");
            arg1=GetNum("LOCATIONTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOCATIONTOTEXT);
            rom(arg1,1);
            ec();
          }
          aa("trainertotext")
          {
            vlog("TRAINERTOTEXT\r\n");
            arg1=GetNum("TRAINERTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("TRAINERTOTEXT");
            if (!gffs) {
              return;
            }
            arg3=GetNum("TRAINERTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TRAINERTOTEXT);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("stringtotext")
          {
            vlog("STRINGTOTEXT\r\n");
            arg1=GetNum("STRINGTOTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("STRINGTOTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_STRINGTOTEXT);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("refreshscreen")
          {
            vlog("REFRESHSCREEN\r\n");
            arg1=GetNum("REFRESHSCREEN");
            if (!gffs) {
              return;
            }
            BASIC(GLD_REFRESHSCREEN);
            rom(arg1,1);
            ec();
          }
          aa("c1celoadbyte")
          {
            vlog("C1CELOADBYTE\r\n");
            arg1=GetNum("C1CELOADBYTE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_C1CELOADBYTE);
            rom(arg1,1);
            ec();
          }
          aa("3writetext")
          {
            vlog("3WRITETEXT\r\n");
            arg1=GetNum("3WRITETEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_3WRITETEXT);
            rom(arg1,3);
            ec();
          }
          aa("2writetext")
          {
            vlog("2WRITETEXT\r\n");
            arg1=GetNum("2WRITETEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_2WRITETEXT);
            rom(arg1,2);
            ec();
          }
          aa("repeattext")
          {
            vlog("REPEATTEXT\r\n");
            arg1=GetNum("REPEATTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_REPEATTEXT);
            rom(arg1,2);
            ec();
          }
          aa("loadmenudata")
          {
            vlog("LOADMENUDATA\r\n");
            arg1=GetNum("LOADMENUDATA");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOADMENUDATA);
            rom(arg1,2);
            ec();
          }
          aa("jumptextfaceplayer")
          {
            vlog("JUMPTEXTFACEPLAYER\r\n");
            arg1=GetNum("JUMPTEXTFACEPLAYER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_JUMPTEXTFACEPLAYER);
            rom(arg1,2);
            ec();
          }
          aa("jumptext")
          {
            vlog("JUMPTEXT\r\n");
            arg1=GetNum("JUMPTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_JUMPTEXT);
            rom(arg1,2);
            ec();
          }
          aa("pokepic")
          {
            vlog("POKEPIC\r\n");
            arg1=GetNum("POKEPIC");
            if (!gffs) {
              return;
            }
            BASIC(GLD_POKEPIC);
            rom(arg1,1);
            ec();
          }
          aa("loadpokedata")
          {
            vlog("LOADPOKEDATA\r\n");
            arg1=GetNum("LOADPOKEDATA");
            if (!gffs) {
              return;
            }
            arg2=GetNum("LOADPOKEDATA");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOADPOKEDATA);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("loadtrainer")
          {
            vlog("LOADTRAINER\r\n");
            arg1=GetNum("LOADTRAINER");
            if (!gffs) {
              return;
            }
            arg2=GetNum("LOADTRAINER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOADTRAINER);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("catchtutorial")
          {
            vlog("CATCHTUTORIAL\r\n");
            arg1=GetNum("CATCHTUTORIAL");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CATCHTUTORIAL);
            rom(arg1,1);
            ec();
          }
          aa("trainertext")
          {
            vlog("TRAINERTEXT\r\n");
            arg1=GetNum("TRAINERTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TRAINERTEXT);
            rom(arg1,1);
            ec();
          }
          aa("trainerstatus")
          {
            vlog("TRAINERSTATUS\r\n");
            arg1=GetNum("TRAINERSTATUS");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TRAINERSTATUS);
            rom(arg1,1);
            ec();
          }
          aa("winlosstext")
          {
            vlog("WINLOSSTEXT\r\n");
            arg1=GetNum("WINLOSSTEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WINLOSSTEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WINLOSSTEXT);
            rom(arg1,2);
            rom(arg2,2);
            ec();
          }
          aa("setlasttalked")
          {
            vlog("SETLASTTALKED\r\n");
            arg1=GetNum("SETLASTTALKED");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SETLASTTALKED);
            rom(arg1,1);
            ec();
          }
          aa("applymovement")
          {
            vlog("APPLYMOVEMENT\r\n");
            arg1=GetNum("APPLYMOVEMENT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("APPLYMOVEMENT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_APPLYMOVEMENT);
            rom(arg1,1);
            rom(arg2,2);
            ec();
          }
          aa("applymoveother")
          {
            vlog("APPLYMOVEOTHER\r\n");
            arg1=GetNum("APPLYMOVEOTHER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_APPLYMOVEOTHER);
            rom(arg1,2);
            ec();
          }
          aa("faceperson")
          {
            vlog("FACEPERSON\r\n");
            arg1=GetNum("FACEPERSON");
            if (!gffs) {
              return;
            }
            arg2=GetNum("FACEPERSON");
            if (!gffs) {
              return;
            }
            BASIC(GLD_FACEPERSON);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("variablesprite")
          {
            vlog("VARIABLESPRITE\r\n");
            arg1=GetNum("VARIABLESPRITE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("VARIABLESPRITE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_VARIABLESPRITE);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("disappear")
          {
            vlog("DISAPPEAR\r\n");
            arg1=GetNum("DISAPPEAR");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DISAPPEAR);
            rom(arg1,1);
            ec();
          }
          aa("appear")
          {
            vlog("APPEAR\r\n");
            arg1=GetNum("APPEAR");
            if (!gffs) {
              return;
            }
            BASIC(GLD_APPEAR);
            rom(arg1,1);
            ec();
          }
          aa("follow")
          {
            vlog("FOLLOW\r\n");
            arg1=GetNum("FOLLOW");
            if (!gffs) {
              return;
            }
            arg2=GetNum("FOLLOW");
            if (!gffs) {
              return;
            }
            BASIC(GLD_FOLLOW);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("moveperson")
          {
            vlog("MOVEPERSON\r\n");
            arg1=GetNum("MOVEPERSON");
            if (!gffs) {
              return;
            }
            arg2=GetNum("MOVEPERSON");
            if (!gffs) {
              return;
            }
            arg3=GetNum("MOVEPERSON");
            if (!gffs) {
              return;
            }
            BASIC(GLD_MOVEPERSON);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("writepersonloc")
          {
            vlog("WRITEPERSONLOC\r\n");
            arg1=GetNum("WRITEPERSONLOC");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WRITEPERSONLOC);
            rom(arg1,1);
            ec();
          }
          aa("loademote")
          {
            vlog("LOADEMOTE\r\n");
            arg1=GetNum("LOADEMOTE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOADEMOTE);
            rom(arg1,1);
            ec();
          }
          aa("showemote")
          {
            vlog("SHOWEMOTE\r\n");
            arg1=GetNum("SHOWEMOTE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("SHOWEMOTE");
            if (!gffs) {
              return;
            }
            arg3=GetNum("SHOWEMOTE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SHOWEMOTE);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("spriteface")
          {
            vlog("SPRITEFACE\r\n");
            arg1=GetNum("SPRITEFACE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("SPRITEFACE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SPRITEFACE);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("follownotexact")
          {
            vlog("FOLLOWNOTEXACT\r\n");
            arg1=GetNum("FOLLOWNOTEXACT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("FOLLOWNOTEXACT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_FOLLOWNOTEXACT);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("earthquake")
          {
            vlog("EARTHQUAKE\r\n");
            arg1=GetNum("EARTHQUAKE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_EARTHQUAKE);
            rom(arg1,1);
            ec();
          }
          aa("swapmaps")
          {
            vlog("SWAPMAPS\r\n");
            arg1=GetNum("SWAPMAPS");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SWAPMAPS);
            rom(arg1,3);
            ec();
          }
          aa("changeblock")
          {
            vlog("CHANGEBLOCK\r\n");
            arg1=GetNum("CHANGEBLOCK");
            if (!gffs) {
              return;
            }
            arg2=GetNum("CHANGEBLOCK");
            if (!gffs) {
              return;
            }
            arg3=GetNum("CHANGEBLOCK");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CHANGEBLOCK);
            rom(arg1,1);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("writecmdqueue")
          {
            vlog("WRITECMDQUEUE\r\n");
            arg1=GetNum("WRITECMDQUEUE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WRITECMDQUEUE);
            rom(arg1,2);
            ec();
          }
          aa("delcmdqueue")
          {
            vlog("DELCMDQUEUE\r\n");
            arg1=GetNum("DELCMDQUEUE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DELCMDQUEUE);
            rom(arg1,1);
            ec();
          }
          aa("playmusic")
          {
            vlog("PLAYMUSIC\r\n");
            arg1=GetNum("PLAYMUSIC");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PLAYMUSIC);
            rom(arg1,2);
            ec();
          }
          aa("musicfadeout")
          {
            vlog("MUSICFADEOUT\r\n");
            arg1=GetNum("MUSICFADEOUT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("MUSICFADEOUT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_MUSICFADEOUT);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("cry")
          {
            vlog("CRY\r\n");
            arg1=GetNum("CRY");
            if (!gffs) {
              return;
            }
            BASIC(GLD_CRY);
            rom(arg1,2);
            ec();
          }
          aa("playsound")
          {
            vlog("PLAYSOUND\r\n");
            arg1=GetNum("PLAYSOUND");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PLAYSOUND);
            rom(arg1,2);
            ec();
          }
          aa("passtoengine")
          {
            vlog("PASSTOENGINE\r\n");
            arg1=GetNum("PASSTOENGINE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PASSTOENGINE);
            rom(arg1,3);
            ec();
          }
          aa("newloadmap")
          {
            vlog("NEWLOADMAP\r\n");
            arg1=GetNum("NEWLOADMAP");
            if (!gffs) {
              return;
            }
            BASIC(GLD_NEWLOADMAP);
            rom(arg1,1);
            ec();
          }
          aa("pause")
          {
            vlog("PAUSE\r\n");
            arg1=GetNum("PAUSE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PAUSE);
            rom(arg1,1);
            ec();
          }
          aa("deactivatefacing")
          {
            vlog("DEACTIVATEFACING\r\n");
            arg1=GetNum("DEACTIVATEFACING");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DEACTIVATEFACING);
            rom(arg1,1);
            ec();
          }
          aa("priorityjump")
          {
            vlog("PRIORITYJUMP\r\n");
            arg1=GetNum("PRIORITYJUMP");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PRIORITYJUMP);
            rom(arg1,2);
            ec();
          }
          aa("ptpriorityjump")
          {
            vlog("PTPRIORITYJUMP\r\n");
            arg1=GetNum("PTPRIORITYJUMP");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PTPRIORITYJUMP);
            rom(arg1,2);
            ec();
          }
          aa("pokemart")
          {
            vlog("POKEMART\r\n");
            arg1=GetNum("POKEMART");
            if (!gffs) {
              return;
            }
            arg2=GetNum("POKEMART");
            if (!gffs) {
              return;
            }
            BASIC(GLD_POKEMART);
            rom(arg1,1);
            rom(arg2,2);
            ec();
          }
          aa("elevator")
          {
            vlog("ELEVATOR\r\n");
            arg1=GetNum("ELEVATOR");
            if (!gffs) {
              return;
            }
            BASIC(GLD_ELEVATOR);
            rom(arg1,2);
            ec();
          }
          aa("trade")
          {
            vlog("TRADE\r\n");
            arg1=GetNum("TRADE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_TRADE);
            rom(arg1,1);
            ec();
          }
          aa("askforphonenumber")
          {
            vlog("ASKFORPHONENUMBER\r\n");
            arg1=GetNum("ASKFORPHONENUMBER");
            if (!gffs) {
              return;
            }
            BASIC(GLD_ASKFORPHONENUMBER);
            rom(arg1,1);
            ec();
          }
          aa("phonecall")
          {
            vlog("PHONECALL\r\n");
            arg1=GetNum("PHONECALL");
            if (!gffs) {
              return;
            }
            BASIC(GLD_PHONECALL);
            rom(arg1,2);
            ec();
          }
          aa("describedecoration")
          {
            vlog("DESCRIBEDECORATION\r\n");
            arg1=GetNum("DESCRIBEDECORATION");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DESCRIBEDECORATION);
            rom(arg1,1);
            ec();
          }
          aa("fruittree")
          {
            vlog("FRUITTREE\r\n");
            arg1=GetNum("FRUITTREE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_FRUITTREE);
            rom(arg1,1);
            ec();
          }
          aa("specialphonecall")
          {
            vlog("SPECIALPHONECALL\r\n");
            arg1=GetNum("SPECIALPHONECALL");
            if (!gffs) {
              return;
            }
            BASIC(GLD_SPECIALPHONECALL);
            rom(arg1,1);
            ec();
          }
          aa("verbosegiveitem")
          {
            vlog("VERBOSEGIVEITEM\r\n");
            arg1=GetNum("VERBOSEGIVEITEM");
            if (!gffs) {
              return;
            }
            arg2=GetNum("VERBOSEGIVEITEM");
            if (!gffs) {
              return;
            }
            BASIC(GLD_VERBOSEGIVEITEM);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("loadwilddata")
          {
            vlog("LOADWILDDATA\r\n");
            arg1=GetNum("LOADWILDDATA");
            if (!gffs) {
              return;
            }
            arg2=GetNum("LOADWILDDATA");
            if (!gffs) {
              return;
            }
            BASIC(GLD_LOADWILDDATA);
            rom(arg1,1);
            rom(arg2,1);
            ec();
          }
          aa("storetext")
          {
            vlog("STORETEXT\r\n");
            arg1=GetNum("STORETEXT");
            if (!gffs) {
              return;
            }
            arg2=GetNum("STORETEXT");
            if (!gffs) {
              return;
            }
            arg3=GetNum("STORETEXT");
            if (!gffs) {
              return;
            }
            BASIC(GLD_STORETEXT);
            rom(arg1,2);
            rom(arg2,1);
            rom(arg3,1);
            ec();
          }
          aa("displaylocation")
          {
            vlog("DISPLAYLOCATION\r\n");
            arg1=GetNum("DISPLAYLOCATION");
            if (!gffs) {
              return;
            }
            BASIC(GLD_DISPLAYLOCATION);
            rom(arg1,1);
            ec();
          }
          aa("writebyte")
          {
            vlog("WRITEBYTE\r\n");
            arg1=GetNum("WRITEBYTE");
            if (!gffs) {
              return;
            }
            arg2=GetNum("WRITEBYTE");
            if (!gffs) {
              return;
            }
            BASIC(GLD_WRITEBYTE);
            rom(arg1,2);
            rom(arg2,1);
            ec();
          }
          aa("m")
          {
            vlog("Movement data...\r\n");
            add_data(c,trans,transbackmove(Script,&i));
            ec();
          }
          aa("=")
          {
            vlog("[STRING]\r\n");
            if (chr==' ') {
              i++;
            }
            else {
              log("Should have a space after the =\r\n",33);
            }
            if (IncFile)
            {
              fseek(IncFile,0,SEEK_END);
              arg6=ftell(IncFile);
            } else
              arg6=0;
            temp_ptr=transbackstr(script,i-arg6-2,c);
            while (chr!='\n'&&chr!=0) {
              i++;
            }
            sprintf(buf2,"   -> %s\r\n",(char*)temp_ptr);
            free(temp_ptr);
            vlog(buf2);
          }
          aa(".")
          {
            vlog("[BINARY]\r\n   ->");
            while (chr==' ') {
              i++;
            }
            k=0;
            while (chr!='\n'&&chr!=0)
            {
              k=1-k;
              while (chr==' ')
              {
                i++;
              }
              j=0;
              while (((char*)("0123456789abcdef"))[j]!=0)
              {
                if (((char*)("0123456789abcdef"))[j]==chr)
                {
                  break;
                }
                j+=1;
              }
              if (((char*)("0123456789abcdef"))[j]==0)
              {
                sprintf(buf2,"Failed to understand hex character '%c'\r\n",chr);
                log(buf2,strlen(buf2));
                return;
              }
              if (k==0)
              {
                l|=j;
                rom(j,1);
                if (IsVerbose)
                {
                  sprintf(buf2," %02X",l);
                  log(buf2,strlen(buf2));
                }
              }
              else
              {
                l=j<<4;
              }
              i++;
            }
            vlog("\r\n");
          }
          else
          {
            sprintf(buf2,"Warning: Unknown command \"%s\"\r\n",buf);
            log(buf2,strlen(buf2));
            while (chr!='\n'&&chr!=0)
            {
              i++;
            }
          }
          break;
        }
        i++;
      }
    }
    else //Fire Red
    {
frlg:
rse:
      while (i<strlen(Script))
      {
        switch (chr) //Behave differently according to char
        {
        case 0:  //E O Script
          return;
        case '\'':
          while (chr!='\n'&&chr!=0)
          {
            i++;
          }i--;
          break;
        case ' ':  //Ignore spaces
        case '\n':
          break;
        default:
          j=0;
          while (chr!=' '&&chr!='\n'&&chr!=0)
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
          if (*buf=='#')
          {
            if (!strcmp(buf,"#define")||!strcmp(buf,"#d"))
            {
              vlog("#DEFINE\r\n");
              while (chr==' ') {
                i++;
              }
              if (chr=='\n'||chr==0||chr=='\'')
              {
                log("Premature end to #DEFINE!\r\n",27);
                return;
              }
              j=0;
              while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
              {
                buf[j]=chr;
                i++;
                j++;
              }
              buf[j]=0; //Append null
              sprintf(buf2,"   -> %s\r\n",buf);
              vlog(buf2);
              while (chr==' ') {
                i++;    //The same old same old.
              }
              if (chr=='\n'||chr==0||chr=='\'')
              {
                log("Premature end to #DEFINE!\r\n",27);
                return;
              }
              k=GetNum("#DEFINE");
              if (!gffs)
                return;
              Define(buf,k);
            }
            aa("#dyn")
            {
              vlog("#DYN\r\n");
              start=GetNum("#DYN");
              if (!gffs)return;
              dyntype=0;
            }
            aa("#dyn2")
            {
              vlog("#DYN2\r\n");
              start=GetNum("#DYN2");
              if (!gffs)return;
              dyntype=1;
            }
            aa("#dynamic")
            {
              vlog("#DYNAMIC\r\n");
              start=GetNum("#DYNAMIC");
              if (!gffs)return;
              dyntype=0;
            }
            aa("#org")
            {
              eorg=0;
              vlog("#ORG\r\n");
              while (chr==' ')i++;
              buf[0]=0;
              if (chr=='@')
              {
                dynu=1;
                j=0;
                while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
                {
                  buf[j]=chr;
                  i++;
                  j++;
                }
                buf[j]=0;
                sprintf(buf2,"   -> %s\r\n",buf);
                log(buf2,strlen(buf2));
              }
              else
              {
                k=GetNum("#ORG");
                if (!gffs)
                  return;
              }
              d=malloc(sizeof(codeblock));
              if (*buf==0)
                init_codeblock(d,NULL);
              else
                init_codeblock(d,buf);
              if (*buf==0)
                d->org=k;
              d->prev=c;
              if (c!=NULL)
                c->next=d;
              c=d;
            }
            aa("#eorg")
            {
              eorg=1;
              vlog("#EORG\r\n");
              while (chr==' ')i++;
              buf[0]=0;
              if (chr=='@')
              {
                j=0;
                while (chr!=' '&&chr!='\n'&&chr!=0&&chr!='\'')
                {
                  buf[j]=chr;
                  i++;
                  j++;
                }
                buf[j]=0;
                sprintf(buf2,"   -> %s\r\n",buf);
                log(buf2,strlen(buf2));
              }
              else
              {
                k=GetNum("#EORG");
                if (!gffs) {
                  return;
                }
              }
              d=malloc(sizeof(codeblock));
              if (*buf==0)
                init_codeblock(d,NULL);
              else
                init_codeblock(d,buf);
              if (*buf==0)d->org=k;
              d->prev=c;
              if (c!=NULL)c->next=d;
              c=d;
            }
            aa("#quiet")
            {
              IsVerbose=0;
            }
            aa("#loud")
            {
              IsVerbose=1;
            }
            aa("#gs")
            {
              mode=GOLD;
              goto gsc;
            }
            aa("#c")
            {
              mode=CRYSTAL;
              goto cry;
            }
            aa("#frlg")
            {
              mode=FIRE_RED;
            }
            aa("#rse")
            {
              mode=RUBY;
            }
            aa("#dp")
            {
              mode=DIAMOND;
              goto dp;
            }
            aa("#raw")
            {
              vlog("#RAW\r\n");
              k=GetNum("#RAW");
              if (!gffs) {
                return;
              }
              BASIC(k);
              if (k>255)
              {
                BASIC(k>>8);
                if (k>65535)
                {
                  BASIC(k>>16);
                  if (k>16777215)
                  {
                    BASIC(k>>24);
                  }
                }
              }
            }
            aa("#ptr")
            {
              vlog("#PTR\r\n");
              k=GetNum("#PTR");
              if (!gffs) {
                return;
              }
              rom(k|0x08000000,4);
            }
            aa("#dword")
            {
              vlog("#DWORD\r\n");
              k=GetNum("#DWORD");
              if (!gffs) {
                return;
              }
              rom(k,4);
            }
            aa("#word")
            {
              vlog("#WORD\r\n");
              k=GetNum("#WORD");
              if (!gffs) {
                return;
              }
              rom(k,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='a')
          {
            if (!strcmp(buf,"applymovement"))
            {
              vlog("APPLYMOVEMENT\r\n");
              BASIC(CMD_APPLYMOVEMENT);
              arg1=GetNum("APPLYMOVEMENT");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
              arg2=GetNum("APPLYMOVEMENT");
              if (!gffs) {
                return;
              }
              if ((arg2&0xff000000)==0)
              {
                arg2|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                vlog(buf3);
              }
              rom(arg2,4);
            }
            aa("applymovementfinishat")
            {
              vlog("APPLYMOVEMENTFINISHAT\r\n");
              BASIC(CMD_APPLYMOVEMENTFINISHAT);
              arg1=GetNum("APPLYMOVEMENTFINISHAT");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
              arg2=GetNum("APPLYMOVEMENTFINISHAT");
              if (!gffs) {
                return;
              }
              if ((arg2&0xff000000)==0)
              {
                arg2|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                vlog(buf3);
              }
              rom(arg2,4);
              arg3=GetNum("APPLYMOVEMENTFINISHAT");
              if (!gffs) {
                return;
              }
              rom(arg3,1);
              arg4=GetNum("APPLYMOVEMENTFINISHAT");
              if (!gffs) {
                return;
              }
              rom(arg4,1);
            }
            aa("addfurniture")
            {
              vlog("ADDFURNITURE\r\n");
              arg1=GetNum("ADDFURNITURE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_ADDFURNITURE);
              rom(arg1,2);
            }
            aa("addpokemon")
            {
              vlog("ADDPOKEMON\r\n");
              BASIC(CMD_ADDPOKEMON);
              arg1=GetNum("ADDPOKEMON");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
              arg2=GetNum("ADDPOKEMON");
              if (!gffs) {
                return;
              }
              rom(arg2,1);
              arg3=GetNum("ADDPOKEMON");
              if (!gffs) {
                return;
              }
              rom(arg3,2);
              arg4=GetNum("ADDPOKEMON");
              if (!gffs) {
                return;
              }
              rom(arg4,1);
              arg5=GetNum("ADDPOKEMON");
              if (!gffs) {
                return;
              }
              rom(arg5,4);
              arg6=GetNum("ADDPOKEMON");
              if (!gffs) {
                return;
              }
              rom(arg6,4);
            }
            aa("addvar")
            {
              vlog("ADDVAR\r\n");
              arg1=GetNum("ADDVAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("ADDVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_ADDVAR);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("additem")
            {
              vlog("ADDITEM\r\n");
              arg1=GetNum("ADDITEM");
              if (!gffs) {
                return;
              }
              arg2=GetNum("ADDITEM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_ADDITEM);
              rom(arg1,2);
              rom(arg2,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='b')
          {
            if (!strcmp(buf,"boxset"))
            {
              vlog("BOXSET\r\n");
              arg1=GetNum("BOXSET");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CALLSTD);
              BASIC(arg1);
            }
            aa("braille")
            {
              vlog("BRAILLE\r\n");
              BASIC(CMD_BRAILLE);
              arg1=GetNum("BRAILLE");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("battle")
            {
              vlog("BATTLE\r\n");
              arg1=GetNum("BATTLE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("BATTLE");
              if (!gffs) {
                return;
              }
              arg3=GetNum("BATTLE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_BATTLE);
              rom(arg1,2);
              rom(arg2,1);
              rom(arg3,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='c')
          {
            if (!strcmp(buf,"checkgender"))
            {
              BASIC(CMD_CHECKGENDER);
            }
            aa("closemsg")
            {
              BASIC(CMD_CLOSEMSG);
            }
            aa("countpokemon")
            {
              BASIC(CMD_COUNTPOKEMON);
            }
            aa("cmd_b2")
            {
              BASIC(CMD_B2);
            }
            aa("choosecontestpokemon")
            {
              BASIC(CMD_CHOOSECONTESTPOKEMON);
            }
            aa("cmd_2d")
            {
              BASIC(CMD_2D);
            }
            aa("cmd_8a")
            {
              vlog("CMD_8A\r\n");
              arg1=GetNum("CMD_8A");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CMD_8A");
              if (!gffs) {
                return;
              }
              arg3=GetNum("CMD_8A");
              if (!gffs) {
                return;
              }
              BASIC(CMD_8A);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
            }
            aa("comparehiddenvar")
            {
              vlog("COMPAREHIDDENVAR\r\n");
              arg1=GetNum("COMPAREHIDDENVAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COMPAREHIDDENVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COMPAREHIDDENVAR);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("checkobedience")
            {
              vlog("CHECKOBEDIENCE\r\n");
              arg1=GetNum("CHECKOBEDIENCE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKOBEDIENCE);
              rom(arg1,2);
            }
            aa("checkanimation")
            {
              vlog("CHECKANIMATION\r\n");
              arg1=GetNum("CHECKANIMATION");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKANIMATION);
              rom(arg1,2);
            }
            aa("call")
            {
              BASIC(CMD_CALL);
              vlog("CALL\r\n");
              arg1=GetNum("CALL");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("callstd")
            {
              vlog("CALLSTD\r\n");
              arg1=GetNum("CALLSTD");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CALLSTD);
              BASIC(arg1);
            }
            aa("compare")
            {
              vlog("COMPARE\r\n");
              arg1=GetNum("COMPARE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COMPARE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COMPARE);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("copyvar")
            {
              vlog("COPYVAR\r\n");
              arg1=GetNum("COPYVAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COPYVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COPYVAR);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("copyvarifnotzero")
            {
              vlog("COPYVARIFNOTZERO\r\n");
              arg1=GetNum("COPYVARIFNOTZERO");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COPYVARIFNOTZERO");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COPYVARIFNOTZERO);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("copyscriptbanks")
            {
              vlog("COPYSCRIPTBANKS\r\n");
              arg1=GetNum("COPYSCRIPTBANKS");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COPYSCRIPTBANKS");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COPYSCRIPTBANKS);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("copybyte")
            {
              vlog("COPYBYTE\r\n");
              BASIC(CMD_COPYBYTE);
              arg1=GetNum("COPYBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1,4);
              arg2=GetNum("COPYBYTE");
              if (!gffs) {
                return;
              }
              rom(arg2,4);
            }
            aa("comparevartobyte")
            {
              vlog("COMPAREVARTOBYTE\r\n");
              arg1=GetNum("COMPAREVARTOBYTE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COMPAREVARTOBYTE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COMPAREVARTOBYTE);
              rom(arg1,2);
              rom(arg2,1);
            }
            aa("comparevartofarbyte")
            {
              vlog("COMPAREVARTOFARBYTE\r\n");
              BASIC(CMD_COMPAREVARTOFARBYTE);
              arg1=GetNum("COMPAREVARTOFARBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
              arg2=GetNum("COMPAREVARTOFARBYTE");
              if (!gffs) {
                return;
              }
              rom(arg2,4);
            }
            aa("comparefarbytetovar")
            {
              vlog("COMPAREFARBYTETOVAR\r\n");
              BASIC(CMD_COMPAREFARBYTETOVAR);
              arg1=GetNum("COMPAREFARBYTETOVAR");
              if (!gffs) {
                return;
              }
              rom(arg1,4);
              arg2=GetNum("COMPAREFARBYTETOVAR");
              if (!gffs) {
                return;
              }
              rom(arg2,2);
            }
            aa("comparefarbytetobyte")
            {
              vlog("COMPAREFARBYTETOBYTE\r\n");
              BASIC(CMD_COMPAREFARBYTETOBYTE);
              arg1=GetNum("COMPAREFARBYTETOBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1,4);
              arg2=GetNum("COMPAREFARBYTETOBYTE");
              if (!gffs) {
                return;
              }
              rom(arg2,1);
            }
            aa("comparefarbytetofarbyte")
            {
              vlog("COMPAREFARBYTETOFARBYTE\r\n");
              BASIC(CMD_COMPAREFARBYTETOFARBYTE);
              arg1=GetNum("COMPAREFARBYTETOFARBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1,4);
              arg2=GetNum("COMPAREFARBYTETOFARBYTE");
              if (!gffs) {
                return;
              }
              rom(arg2,4);
            }
            aa("callasm")
            {
              vlog("CALLASM\r\n");
              BASIC(CMD_CALLASM);
              arg1=GetNum("CALLASM");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("callasm2")
            {
              vlog("CALLASM2\r\n");
              BASIC(CMD_CALLASM2);
              arg1=GetNum("CALLASM2");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("clearflag")
            {
              vlog("CLEARFLAG\r\n");
              arg1=GetNum("CLEARFLAG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CLEARFLAG);
              rom(arg1,2);
            }
            aa("checkflag")
            {
              vlog("CHECKFLAG\r\n");
              arg1=GetNum("CHECKFLAG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKFLAG);
              rom(arg1,2);
            }
            aa("cry")
            {
              vlog("CRY\r\n");
              arg1=GetNum("CRY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CRY");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED)
              {
                BASIC(CMD_CRYFR);
                rom(arg1,2);
              }
              else
              {
                BASIC(CMD_CRY);
                rom(arg1,1);
              }
              rom(arg2,2);
            }
            aa("checksound")
            {
              vlog("CHECKSOUND\r\n");
              BASIC(CMD_CRY);
            }
            aa("cmd_c3")
            {
              vlog("CMD_C3\r\n");
              arg1=GetNum("CMD_C3");
              if (!gffs) {
                return;
              }
              BASIC(CMD_C3);
              rom(arg1,1);
            }
            aa("checkitem")
            {
              vlog("CHECKITEM\r\n");
              arg1=GetNum("CHECKITEM");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CHECKITEM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKITEM);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("checkitemamount")
            {
              vlog("CHECKITEMAMOUNT\r\n");
              arg1=GetNum("CHECKITEMAMOUNT");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CHECKITEMAMOUNT");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKITEMAMOUNT);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("cmd_2c")
            {
              vlog("CMD_2C\r\n");
              arg1=GetNum("CMD_2C");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CMD_2C");
              if (!gffs) {
                return;
              }
              BASIC(CMD_2C);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("checkattack")
            {
              vlog("CHECKATTACK\r\n");
              arg1=GetNum("CHECKATTACK");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKATTACK);
              rom(arg1,2);
            }
            aa("checkfurniture")
            {
              vlog("CHECKFURNITURE\r\n");
              BASIC(CMD_CHECKFURNITURE);
              arg1=GetNum("CHECKFURNITURE");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
            }
            aa("checkifroomforfurniture")
            {
              vlog("CHECKIFROOMFORFURNITURE\r\n");
              BASIC(CMD_CHECKIFROOMFORFURNITURE);
              arg1=GetNum("CHECKIFROOMFORFURNITURE");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
            }
            aa("checkitemtype")
            {
              vlog("CHECKITEMTYPE\r\n");
              arg1=GetNum("CHECKITEMTYPE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKITEMTYPE);
              rom(arg1,2);
            }
            aa("checkiteminpc")
            {
              vlog("CHECKITEMINPC\r\n");
              arg1=GetNum("CHECKITEMINPC");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CHECKITEMINPC");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKITEMINPC);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("createtempsprite")
            {
              vlog("CREATETEMPSPRITE\r\n");
              arg1=GetNum("CREATETEMPSPRITE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CREATETEMPSPRITE");
              if (!gffs) {
                return;
              }
              arg3=GetNum("CREATETEMPSPRITE");
              if (!gffs) {
                return;
              }
              arg4=GetNum("CREATETEMPSPRITE");
              if (!gffs) {
                return;
              }
              arg5=GetNum("CREATETEMPSPRITE");
              if (!gffs) {
                return;
              }
              arg6=GetNum("CREATETEMPSPRITE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CREATETEMPSPRITE);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,2);
              rom(arg4,2);
              rom(arg5,1);
              rom(arg6,1);
            }
            aa("cmd_a6")
            {
              vlog("CMD_A6\r\n");
              arg1=GetNum("CMD_A6");
              if (!gffs) {
                return;
              }
              BASIC(CMD_A6);
              rom(arg1,1);
            }
            aa("cmd_b1")
            {
              vlog("CMD_B1\r\n");
              arg1=GetNum("CMD_B1");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CMD_B1");
              if (!gffs) {
                return;
              }
              arg3=GetNum("CMD_B1");
              if (!gffs) {
                return;
              }
              arg4=GetNum("CMD_B1");
              if (!gffs) {
                return;
              }
              BASIC(CMD_B1);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,2);
              rom(arg4,2);
            }
            aa("cmd_d3")
            {
              vlog("CMD_D3\r\n");
              BASIC(CMD_D3);
              arg1=GetNum("CMD_D3");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("cmd_fb")
            {
              vlog("CMD_FB\r\n");
              BASIC(CMD_FB);
              arg1=GetNum("CMD_FB");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("checkattack")
            {
              vlog("CHECKATTACK\r\n");
              arg1=GetNum("CHECKATTACK");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CHECKATTACK");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKATTACK);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("coincasetovar")
            {
              vlog("CoinCaseToVar\r\n");
              arg1=GetNum("COINCASETOVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COINCASETOVAR);
              rom(arg1,2);
            }
            aa("callstdif")
            {
              vlog("callstdif\r\n");
              arg1=GetNum("callstdif");
              if (!gffs) {
                return;
              }
              arg2=GetNum("callstdif");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CALLSTDIF);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("checkmoney")
            {
              vlog("CHECKMONEY\r\n");
              arg1=GetNum("CHECKMONEY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("CHECKMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKMONEY);
              rom(arg1,2);
              rom(arg2,1);
            }
            aa("cmd_96")
            {
              vlog("CMD_96\r\n");
              arg1=GetNum("CMD_96");
              if (!gffs) {
                return;
              }
              BASIC(CMD_96);
              rom(arg1,2);
            }
            aa("cmd_fe")
            {
              vlog("CMD_FE\r\n");
              arg1=GetNum("CMD_FE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FE);
              rom(arg1,1);
            }
            aa("cleartrainerflag")
            {
              vlog("CLEARTRAINERFLAG\r\n");
              arg1=GetNum("CLEARTRAINERFLAG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CLEARTRAINERFLAG);
              rom(arg1,2);
            }
            aa("comparevars")
            {
              vlog("COMPAREVARS\r\n");
              arg1=GetNum("COMPAREVARS");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COMPAREVARS");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COMPAREVARS);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("comparevars2")
            {
              vlog("COMPAREVARS2\r\n");
              arg1=GetNum("COMPAREVARS2");
              if (!gffs) {
                return;
              }
              arg2=GetNum("COMPAREVARS2");
              if (!gffs) {
                return;
              }
              BASIC(CMD_COMPAREVARS2);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("checktrainerflag")
            {
              vlog("CHECKTRAINERFLAG\r\n");
              arg1=GetNum("CHECKTRAINERFLAG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_CHECKTRAINERFLAG);
              rom(arg1,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='d')
          {
            if (!strcmp(buf,"doweather"))
            {
              BASIC(CMD_DOWEATHER);
            }
            aa("doorchange")
            {
              BASIC(CMD_DOORCHANGE);
            }
            aa("doanimation")
            {
              vlog("DOANIMATION\r\n");
              arg1=GetNum("DOANIMATION");
              if (!gffs) {
                return;
              }
              BASIC(CMD_DOANIMATION);
              rom(arg1,2);
            }
            aa("disappear")
            {
              vlog("DISAPPEAR\r\n");
              arg1=GetNum("DISAPPEAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_DISAPPEAR);
              rom(arg1,2);
            }
            aa("darkenroom")
            {
              vlog("DARKENROOM\r\n");
              arg1=GetNum("DARKENROOM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_DARKENROOM);
              rom(arg1,2);
            }
            aa("disappearat")
            {
              vlog("DISAPPEARAT\r\n");
              arg1=GetNum("DISAPPEARAT");
              if (!gffs) {
                return;
              }
              arg2=GetNum("DISAPPEARAT");
              if (!gffs) {
                return;
              }
              arg3=GetNum("DISAPPEARAT");
              if (!gffs) {
                return;
              }
              BASIC(CMD_DISAPPEARAT);
              rom(arg1,2);
              rom(arg2,1);
              rom(arg3,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='e')
          {
            if (!strcmp(buf,"end"))
            {
              BASIC(CMD_END);                    //BASIC() writes 1 byte to ROM
            }
            aa("executeram")
            {
              BASIC(CMD_EXECUTERAM);
            }
            aa("endtrainerbattle")
            {
              BASIC(CMD_ENDTRAINERBATTLE);
            }
            aa("endtrainerbattle2")
            {
              BASIC(CMD_ENDTRAINERBATTLE2);
            }
            aa("endmart")
            {
              vlog("ENDMART\r\n");
              rom(0,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='f')
          {
            if (!strcmp(buf,"faceplayer"))
            {
              BASIC(CMD_FACEPLAYER);
            }
            aa("fadedefault")
            {
              BASIC(CMD_FADEDEFAULT);
            }
            aa("fadeout")
            {
              BASIC(CMD_FADEOUT);
            }
            aa("fadein")
            {
              BASIC(CMD_FADEIN);
            }
            aa("fadedefault")
            {
              BASIC(CMD_FADEDEFAULT);
            }
            aa("falldownhole")
            {
              vlog("FALLDOWNHOLE\r\n");
              arg1=GetNum("FALLDOWNHOLE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("FALLDOWNHOLE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FALLDOWNHOLE);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("fadescreen")
            {
              vlog("FADESCREEN\r\n");
              arg1=GetNum("FADESCREEN");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FADESCREEN);
              rom(arg1,1);
            }
            aa("fadescreendelay")
            {
              vlog("FADESCREENDELAY\r\n");
              arg1=GetNum("FADESCREENDELAY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("FADESCREENDELAY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FADESCREENDELAY);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("fanfare")
            {
              vlog("FANFARE\r\n");
              arg1=GetNum("FANFARE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FANFARE);
              rom(arg1,2);
            }
            aa("fadesound")
            {
              vlog("FADESOUND\r\n");
              arg1=GetNum("FADESOUND");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FADEDEFAULT);
              rom(arg1,2);
            }
            aa("fadesong")
            {
              vlog("FADESONG\r\n");
              arg1=GetNum("FADESONG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FADESONG);
              rom(arg1,2);
            }
            aa("fadedefault")
            {
              vlog("FADEDEFAULT\r\n");
              arg1=GetNum("FADEDEFAULT");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FADEDEFAULT);
            }
            aa("fakejumpstd")
            {
              vlog("FAKEJUMPSTD\r\n");
              arg1=GetNum("FAKEJUMPSTD");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FAKEJUMPSTD);
              rom(arg1,1);
            }
            aa("fakecallstd")
            {
              vlog("FAKECALLSTD\r\n");
              arg1=GetNum("FAKECALLSTD");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FAKECALLSTD);
              rom(arg1,1);
            }
            aa("farreappear")
            {
              vlog("FARREAPPEAR\r\n");
              arg1=GetNum("FARREAPPEAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("FARREAPPEAR");
              if (!gffs) {
                return;
              }
              arg3=GetNum("FARREAPPEAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FARREAPPEAR);
              rom(arg1,2);
              rom(arg2,1);
              rom(arg3,1);
            }
            aa("fardisappear")
            {
              vlog("FARDISAPPEAR\r\n");
              arg1=GetNum("FARDISAPPEAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("FARDISAPPEAR");
              if (!gffs) {
                return;
              }
              arg3=GetNum("FARDISAPPEAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_FARDISAPPEAR);
              rom(arg1,2);
              rom(arg2,1);
              rom(arg3,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='g')
          {
            if (!strcmp(buf,"getplayerxy"))
            {
              vlog("GETPLAYERXY\r\n");
              arg1=GetNum("GETPLAYERXY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("GETPLAYERXY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_GETPLAYERXY);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("giveitemtopc")
            {
              vlog("GIVEITEMTOPC\r\n");
              arg1=GetNum("GIVEITEMTOPC");
              if (!gffs) {
                return;
              }
              arg2=GetNum("GIVEITEMTOPC");
              if (!gffs) {
                return;
              }
              BASIC(CMD_GIVEITEMTOPC);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("giveegg")
            {
              vlog("GIVEEGG\r\n");
              arg1=GetNum("GIVEEGG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_GIVEEGG);
              rom(arg1,2);
            }
            aa("givetocoincase")
            {
              vlog("GIVETOCOINCASE\r\n");
              arg1=GetNum("GIVETOCOINCASE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_GIVETOCOINCASE);
              rom(arg1,2);
            }
            aa("givemoney")
            {
              vlog("GIVEMONEY\r\n");
              BASIC(CMD_GIVEMONEY);
              arg1=GetNum("GIVEMONEY");
              if (!gffs) {
                return;
              }
              rom(arg1,4);
              arg2=GetNum("GIVEMONEY");
              if (!gffs) {
                return;
              }
              rom(arg2,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='h')
          {
            if (!strcmp(buf,"hidepokepic"))
            {
              BASIC(CMD_HIDEPOKEPIC);
            }
            aa("hidebox")
            {
              vlog("HIDEBOX\r\n");
              arg1=GetNum("HIDEBOX");
              if (!gffs) {
                return;
              }
              arg2=GetNum("HIDEBOX");
              if (!gffs) {
                return;
              }
              arg3=GetNum("HIDEBOX");
              if (!gffs) {
                return;
              }
              arg4=GetNum("HIDEBOX");
              if (!gffs) {
                return;
              }
              BASIC(CMD_HIDEBOX);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              rom(arg4,1);
            }
            aa("hidecoins")
            {
              vlog("HIDECOINS\r\n");
              arg1=GetNum("HIDECOINS");
              if (!gffs) {
                return;
              }
              arg2=GetNum("HIDECOINS");
              if (!gffs) {
                return;
              }
              BASIC(CMD_HIDECOINS);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("hidemoney")
            {
              vlog("HIDEMONEY\r\n");
              arg1=GetNum("HIDEMONEY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("HIDEMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_HIDEMONEY);
              rom(arg1,1);
              rom(arg2,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='i')
          {
            if (!strcmp(buf,"if"))
            {
              vlog("IF\r\n");
              arg1=GetNum("IF");
              if (!gffs) {
                return;
              }
              while (chr==' ')
              {
                i++;
              }
              j=0;
              while (chr!=' ')
              {
                buf[j]=chr;
                i++;
                j++;
              }
              buf[j]=0;
              if (!strcmp(buf,"jump")||!strcmp(buf,"goto"))
              {
                BASIC(CMD_JUMPIF);
                BASIC(arg1);
                vlog("   -> JUMP\r\n");
                arg2=GetNum("IF (JUMP)");
                if (!gffs) {
                  return;
                }
                if ((arg2&0xff000000)==0)
                {
                  arg2|=0x08000000;
                  sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                  vlog(buf3);
                }
                rom(arg2,4);
              }
              else if (!strcmp(buf,"call"))
              {
                BASIC(CMD_CALLIF);
                BASIC(arg1);
                vlog("   -> CALL\r\n");
                arg2=GetNum("IF (CALL)");
                if (!gffs) {
                  return;
                }
                if ((arg2&0xff000000)==0)
                {
                  arg2|=0x08000000;
                  sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                  vlog(buf3);
                }
                rom(arg2,4);
              }
              else if (!strcmp(buf,"callstd"))
              {
                BASIC(CMD_CALLSTDIF);
                BASIC(arg1);
                vlog("   -> CALLSTD\r\n");
                arg2=GetNum("IF (CALLSTD)");
                if (!gffs) {
                  return;
                }
                rom(arg2,1);
              }
              else if (!strcmp(buf,"jumpstd"))
              {
                BASIC(CMD_JUMPSTDIF);
                BASIC(arg1);
                vlog("   -> JUMPSTD\r\n");
                arg2=GetNum("IF (JUMPSTD)");
                if (!gffs) {
                  return;
                }
                rom(arg2,1);
              }
              else
              {
                log("IF was not correctly used. Use with jump, call, callstd and jumpstd.\r\n",58);
                return;
              }
            }
            aa("item")
            {
              vlog("ITEM\r\n");
              arg1=GetNum("ITEM");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='j')
          {
            if (!strcmp(buf,"jumpram"))
            {
              BASIC(CMD_JUMPRAM);
            }
            aa("jump")
            {
              BASIC(CMD_JUMP);
              vlog("JUMP\r\n");
              arg1=GetNum("JUMP");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("jumpstd")
            {
              vlog("JUMPSTD\r\n");
              arg1=GetNum("JUMPSTD");
              if (!gffs) {
                return;
              }
              BASIC(CMD_JUMPSTD);
              BASIC(arg1);
            }
            aa("jumpstdif")
            {
              vlog("jumpstdif\r\n");
              arg1=GetNum("jumpstdif");
              if (!gffs) {
                return;
              }
              arg2=GetNum("jumpstdif");
              if (!gffs) {
                return;
              }
              BASIC(CMD_JUMPSTDIF);
              rom(arg1,1);
              rom(arg2,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='k')
          {
            if (!strcmp(buf,"killscript"))
            {
              BASIC(CMD_KILLSCRIPT);
              ec();
            } else goto unk_cmd_fr;
          }
          else if (*buf=='l')
          {
            if (!strcmp(buf,"lockall"))
            {
              BASIC(CMD_LOCKALL);
            }
            aa("lock")
            {
              BASIC(CMD_LOCK);
            }
            aa("lastbattle")
            {
              BASIC(CMD_LASTBATTLE);
            }
            aa("lasttrainerbattle")
            {
              BASIC(CMD_LASTTRAINERBATTLE);
            }
            aa("loadpointer")
            {
              BASIC(CMD_MSGBOX);
              vlog("LOADPOINTER\r\n");
              arg1=GetNum("LOADPOINTER");
              if (!gffs) {
                return;
              }
              BASIC(arg1);
              arg2=GetNum("LOADPOINTER");
              if (!gffs) {
                return;
              }
              if ((arg2&0xff000000)==0)
              {
                arg2|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                vlog(buf3);
              }
              rom(arg2,4);
            }
            aa("loadbytefrompointer")
            {
              vlog("LOADBYTEFROMPOINTER\r\n");
              arg1=GetNum("LOADBYTEFROMPOINTER");
              if (!gffs) {
                return;
              }
              BASIC(CMD_LOADBYTEFROMPOINTER);
              rom(arg1,1);
              arg2=GetNum("LOADBYTEFROMPOINTER");
              if (!gffs) {
                return;
              }
              rom(arg2,4);
            }
            aa("lightroom")
            {
              vlog("LIGHTROOM\r\n");
              arg1=GetNum("LIGHTROOM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_LIGHTROOM);
              rom(arg1,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='m')
          {
            if (!strcmp(buf,"msgboxsign"))
            {
              BASIC(CMD_MSGBOXSIGN);
            }
            aa("msgboxnormal")
            {
              BASIC(CMD_MSGBOXNORMAL);
            }
            aa("msgbox")
            {
              BASIC(CMD_MSGBOX);
              BASIC(0);
              vlog("MSGBOX\r\n");
              arg1=GetNum("MSGBOX");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("msgbox2")
            {
              BASIC(CMD_MSGBOX2);
              BASIC(0);
              vlog("MSGBOX2\r\n");
              arg1=GetNum("MSGBOX2");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("martend")
            {
              vlog("MARTEND\r\n");
              rom(0,2);
            }
            aa("message")
            {
              vlog("MESSAGE\r\n");
              BASIC(CMD_MESSAGE);
              arg1=GetNum("MESSAGE");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("multichoice")
            {
              vlog("MULTICHOICE\r\n");
              arg1=GetNum("MULTICHOICE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("MULTICHOICE");
              if (!gffs) {
                return;
              }
              arg3=GetNum("MULTICHOICE");
              if (!gffs) {
                return;
              }
              arg4=GetNum("MULTICHOICE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_MULTICHOICE);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              rom(arg4,1);
            }
            aa("multichoice2")
            {
              vlog("MULTICHOICE2\r\n");
              arg1=GetNum("MULTICHOICE2");
              if (!gffs) {
                return;
              }
              arg2=GetNum("MULTICHOICE2");
              if (!gffs) {
                return;
              }
              arg3=GetNum("MULTICHOICE2");
              if (!gffs) {
                return;
              }
              arg4=GetNum("MULTICHOICE2");
              if (!gffs) {
                return;
              }
              BASIC(CMD_MULTICHOICE2);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              rom(arg4,2);
            }
            aa("multichoice3")
            {
              vlog("MULTICHOICE3\r\n");
              arg1=GetNum("MULTICHOICE3");
              if (!gffs) {
                return;
              }
              arg2=GetNum("MULTICHOICE3");
              if (!gffs) {
                return;
              }
              arg3=GetNum("MULTICHOICE3");
              if (!gffs) {
                return;
              }
              arg4=GetNum("MULTICHOICE3");
              if (!gffs) {
                return;
              }
              arg5=GetNum("MULTICHOICE3");
              if (!gffs) {
                return;
              }
              BASIC(CMD_MULTICHOICE3);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              rom(arg4,1);
              rom(arg5,1);
            }
            aa("movesprite2")
            {
              vlog("movesprite2\r\n");
              arg1=GetNum("movesprite2");
              if (!gffs) {
                return;
              }
              arg2=GetNum("movesprite2");
              if (!gffs) {
                return;
              }
              arg3=GetNum("movesprite2");
              if (!gffs) {
                return;
              }
              BASIC(CMD_MOVESPRITE2);
              rom(arg1,2);
              rom(arg2,2);
              rom(arg3,2);
            }
            aa("movesprite")
            {
              vlog("MOVESPRITE\r\n");
              arg1=GetNum("MOVESPRITE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("MOVESPRITE");
              if (!gffs) {
                return;
              }
              arg3=GetNum("MOVESPRITE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_MOVESPRITE);
              rom(arg1,2);
              rom(arg2,2);
              rom(arg3,2);
            }
            aa("moveoffscreen")
            {
              vlog("MOVEOFFSCREEN\r\n");
              arg1=GetNum("MOVEOFFSCREEN");
              if (!gffs) {
                return;
              }
              BASIC(CMD_MOVEOFFSCREEN);
              rom(arg1,2);
            }
            aa("m")
            {
              vlog("Movement data...\r\n");
              add_data(c,trans,transbackmove(Script,&i));
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='n')
          {
            if (!strcmp(buf,"nop")||!strcmp(buf,"nop0"))
            {
              BASIC(CMD_NOP0);
            }
            aa("nop1")
            {
              BASIC(CMD_NOP1);
            }
            aa("namepokemon")
            {
              BASIC(CMD_FADESCREEN);
              BASIC(1);
              BASIC(CMD_SPECIAL);
              rom(0x9E,2);
              BASIC(CMD_WAITSPECIAL);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='p')
          {
            if (!strcmp(buf,"pause"))
            {
              vlog("PAUSE\r\n");
              arg1=GetNum("PAUSE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_PAUSE);
              rom(arg1,2);
            }
            aa("playsound")
            {
              vlog("PLAYSOUND\r\n");
              arg1=GetNum("PLAYSOUND");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED)
              {
                arg1=GetNum("PLAYSOUND");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_PLAYSOUND);
              rom(arg1,2);
              if (mode==FIRE_RED)
              {
                rom(arg2,1);
              }
            }
            aa("playsong")
            {
              vlog("PLAYSONG\r\n");
              arg1=GetNum("PLAYSONG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_PLAYSONG);
              rom(arg1,2);
            }
            aa("pauseevent")
            {
              BASIC(CMD_PAUSEEVENT);
              vlog("PAUSEEVENT\r\n");
              arg1=GetNum("PAUSEEVENT");
              if (!gffs) {
                return;
              }
              rom(arg1,2);
            }
            aa("picture")
            {
              vlog("PICTURE\r\n");
              arg1=GetNum("PICTURE");
              if (!gffs) {
                return;    //GFFS stands for Get-For-Function success. GetNum sets this to 0 on failure.
              }
              BASIC(CMD_PICTURE);
              rom(arg1,1);
            }
            aa("pokemart")
            {
              vlog("POKEMART\r\n");
              BASIC(CMD_POKEMART);
              arg1=GetNum("POKEMART");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("pokemart2")
            {
              vlog("POKEMART2\r\n");
              BASIC(CMD_POKEMART2);
              arg1=GetNum("POKEMART2");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("pokemart3")
            {
              vlog("POKEMART3\r\n");
              BASIC(CMD_POKEMART3);
              arg1=GetNum("POKEMART3");
              if (!gffs) {
                return;
              }
              if ((arg1&0xff000000)==0)
              {
                arg1|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg1);
                vlog(buf3);
              }
              rom(arg1,4);
            }
            aa("paymoney")
            {
              vlog("PAYMONEY\r\n");
              arg1=GetNum("PAYMONEY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("PAYMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_PAYMONEY);
              rom(arg1,2);
              rom(arg2,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='r')
          {
            if (!strcmp(buf,"return"))
            {
              BASIC(CMD_RETURN);
            }
            aa("release")
            {
              BASIC(CMD_RELEASE);
            }
            aa("releaseall")
            {
              BASIC(CMD_RELEASEALL);
            }
            aa("resetweather")
            {
              BASIC(CMD_RESETWEATHER);
            }
            aa("resetvars")
            {
              BASIC(CMD_RESETVARS);
            }
            aa("random")
            {
              vlog("RANDOM\r\n");
              arg1=GetNum("RANDOM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_RANDOM);
              rom(arg1,2);
            }
            aa("removeitem")
            {
              vlog("REMOVEITEM\r\n");
              arg1=GetNum("REMOVEITEM");
              if (!gffs) {
                return;
              }
              arg2=GetNum("REMOVEITEM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_REMOVEITEM);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("resetspritelevel")
            {
              vlog("RESETSPRITELEVEL\r\n");
              arg1=GetNum("RESETSPRITELEVEL");
              if (!gffs) {
                return;
              }
              arg2=GetNum("RESETSPRITELEVEL");
              if (!gffs) {
                return;
              }
              arg3=GetNum("RESETSPRITELEVEL");
              if (!gffs) {
                return;
              }
              BASIC(CMD_RESETSPRITELEVEL);
              rom(arg1,2);
              rom(arg2,2);
              rom(arg3,2);
            }
            aa("reappear")
            {
              vlog("REAPPEAR\r\n");
              arg1=GetNum("REAPPEAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_REAPPEAR);
              rom(arg1,2);
            }
            aa("reappearat")
            {
              vlog("REAPPEARAT\r\n");
              arg1=GetNum("REAPPEARAT");
              if (!gffs) {
                return;
              }
              arg2=GetNum("REAPPEARAT");
              if (!gffs) {
                return;
              }
              arg3=GetNum("REAPPEARAT");
              if (!gffs) {
                return;
              }
              BASIC(CMD_REAPPEARAT);
              rom(arg1,2);
              rom(arg2,1);
              rom(arg3,1);
            }
          }
          else if (*buf=='s')
          {
            if (!strcmp(buf,"showmsg"))
            {
              BASIC(CMD_SHOWMSG);
            }
            aa("startcontest")        {
              BASIC(CMD_STARTCONTEST);
            }
            aa("startwireless")       {
              BASIC(CMD_STARTWIRELESS);
            }
            aa("showpokepic")
            {
              vlog("SHOWPOKEPIC\r\n");
              arg1=GetNum("SHOWPOKEPIC");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SHOWPOKEPIC");
              if (!gffs) {
                return;
              }
              arg3=GetNum("SHOWPOKEPIC");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SHOWPOKEPIC);
              rom(arg1,2);
              rom(arg2,1);
              rom(arg3,1);
            }
            aa("storeitems")
            {
              vlog("STOREITEMS\r\n");
              arg1=GetNum("STOREITEMS");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREITEMS");
              if (!gffs) {
                return;
              }
              arg3=GetNum("STOREITEMS");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREITEMS);
              rom(arg1,1);
              rom(arg2,2);
              rom(arg3,2);
            }
            aa("setpokemonpp")
            {
              vlog("SETPOKEMONPP\r\n");
              arg1=GetNum("SETPOKEMONPP");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETPOKEMONPP");
              if (!gffs) {
                return;
              }
              arg3=GetNum("SETPOKEMONPP");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETPOKEMONPP);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,2);
            }
            aa("storepartypokemon")
            {
              vlog("STOREPARTYPOKEMON\r\n");
              arg1=GetNum("STOREPARTYPOKEMON");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREPARTYPOKEMON");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREPARTYPOKEMON);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storefirstpokemon")
            {
              vlog("STOREFIRSTPOKEMON\r\n");
              arg1=GetNum("STOREFIRSTPOKEMON");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREFIRSTPOKEMON);
              rom(arg1,1);
            }
            aa("setcatchlocation")
            {
              vlog("SETCATCHLOCATION\r\n");
              arg1=GetNum("SETCATCHLOCATION");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETCATCHLOCATION");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETCATCHLOCATION);
              rom(arg1,2);
              rom(arg2,1);
            }
            aa("setdoorclosed")
            {
              vlog("SETDOORCLOSED\r\n");
              arg1=GetNum("SETDOORCLOSED");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETDOORCLOSED");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETDOORCLOSED);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setdoorclosedstatic")
            {
              vlog("SETDOORCLOSEDSTATIC\r\n");
              arg1=GetNum("SETDOORCLOSEDSTATIC");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETDOORCLOSEDSTATIC");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETDOORCLOSEDSTATIC);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setdooropened")
            {
              vlog("SETDOOROPENED\r\n");
              arg1=GetNum("SETDOOROPENED");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETDOOROPENED");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETDOOROPENED);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setdooropenedstatic")
            {
              vlog("SETDOOROPENEDSTATIC\r\n");
              arg1=GetNum("SETDOOROPENEDSTATIC");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETDOOROPENEDSTATIC");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETDOOROPENEDSTATIC);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setanimation")
            {
              vlog("SETANIMATION\r\n");
              arg1=GetNum("SETANIMATION");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETANIMATION");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETANIMATION);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("setobedience")
            {
              vlog("SETOBEDIENCE\r\n");
              arg1=GetNum("SETOBEDIENCE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETOBEDIENCE);
              rom(arg1,2);
            }
            aa("setbyte")
            {
              vlog("SETBYTE\r\n");
              arg1=GetNum("SETBYTE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETBYTE);
              BASIC(arg1);
            }
            aa("spriteface")
            {
              vlog("SPRITEFACE\r\n");
              arg1=GetNum("SPRITEFACE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SPRITEFACE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SPRITEFACE);
              rom(arg1,2);
              rom(arg2,1);
            }
            aa("sethealingplace")
            {
              vlog("SETHEALINGPLACE\r\n");
              arg1=GetNum("SETHEALINGPLACE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETHEALINGPLACE);
              rom(arg1,2);
            }
            aa("special")
            {
              vlog("SPECIAL\r\n");
              arg1=GetNum("SPECIAL");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SPECIAL);
              rom(arg1,2);
            }
            aa("setworldmapflag")
            {
              vlog("SETWORLDMAPFLAG\r\n");
              arg1=GetNum("setworldmapflag");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETWORLDMAPFLAG);
              rom(arg1,2);
            }
            aa("special2")
            {
              vlog("SPECIAL2\r\n");
              arg1=GetNum("SPECIAL2");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SPECIAL2");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SPECIAL2);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setvar")
            {
              vlog("SETVAR\r\n");
              arg1=GetNum("SETVAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETVAR);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setflag")
            {
              vlog("SETFLAG\r\n");
              arg1=GetNum("SETFLAG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETFLAG);
              rom(arg1,2);
            }
            aa("sound")
            {
              vlog("SOUND\r\n");
              arg1=GetNum("SOUND");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SOUND);
              rom(arg1,2);
            }
            aa("storepokemon")
            {
              vlog("STOREPOKEMON\r\n");
              arg1=GetNum("STOREPOKEMON");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREPOKEMON");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREPOKEMON);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storeboxname")
            {
              vlog("STOREBOXNAME\r\n");
              arg1=GetNum("STOREBOXNAME");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREBOXNAME");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREBOXNAME);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storevar")
            {
              vlog("STOREVAR\r\n");
              arg1=GetNum("STOREVAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREVAR);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storetext")
            {
              vlog("STORETEXT\r\n");
              BASIC(CMD_STORETEXT);
              arg1=GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              rom(arg1,1);
              arg2=GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              if ((arg2&0xff000000)==0)
              {
                arg2|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg2);
                vlog(buf3);
              }
              rom(arg2,4);
            }
            aa("setmaptile")
            {
              vlog("SETMAPTILE\r\n");
              arg1=GetNum("SETMAPTILE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETMAPTILE");
              if (!gffs) {
                return;
              }
              arg3=GetNum("SETMAPTILE");
              if (!gffs) {
                return;
              }
              arg4=GetNum("SETMAPTILE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETMAPTILE);
              rom(arg1,2);
              rom(arg2,2);
              rom(arg3,2);
              rom(arg4,2);
            }
            aa("setmapfooter")
            {
              vlog("SETMAPFOOTER\r\n");
              arg1=GetNum("SETMAPFOOTER");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETMAPFOOTER);
              rom(arg1,2);
            }
            aa("setweather")
            {
              vlog("SETWEATHER\r\n");
              arg1=GetNum("SETWEATHER");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETWEATHER);
              arg2=1;
              if (mode==FIRE_RED)arg2=2;
              rom(arg1,arg2);
            }
            aa("storeitem")
            {
              vlog("STOREITEM\r\n");
              arg1=GetNum("STOREITEM");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREITEM");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREITEM);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storeatk")
            {
              vlog("STOREATK\r\n");
              arg1=GetNum("STOREATK");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREATK");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREATK);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storefurniture")
            {
              vlog("STOREFURNITURE\r\n");
              arg1=GetNum("STOREFURNITURE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STOREFURNITURE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STOREFURNITURE);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("storecomp")
            {
              vlog("STORECOMP\r\n");
              arg1=GetNum("STORECOMP");
              if (!gffs) {
                return;
              }
              arg2=GetNum("STORECOMP");
              if (!gffs) {
                return;
              }
              BASIC(CMD_STORECOMP);
              rom(arg1,1);
              rom(arg2,2);
            }
            aa("slotmachine")
            {
              vlog("SLOTMACHINE\r\n");
              arg1=GetNum("SLOTMACHINE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SLOTMACHINE);
              rom(arg1,2);
            }
            aa("showmoney")
            {
              vlog("SHOWMONEY\r\n");
              arg1=GetNum("SHOWMONEY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SHOWMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SHOWMONEY);
              rom(arg1,2);
              rom(arg2,1);
            }
            aa("showyesno")
            {
              vlog("SHOWYESNO\r\n");
              arg1=GetNum("SHOWYESNO");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SHOWYESNO");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SHOWYESNO);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("settrainerflag")
            {
              vlog("SETTRAINERFLAG\r\n");
              arg1=GetNum("SETTRAINERFLAG");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETTRAINERFLAG);
              rom(arg1,2);
            }
            aa("showcoins")
            {
              vlog("SHOWCOINS\r\n");
              arg1=GetNum("SHOWCOINS");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SHOWCOINS");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SHOWCOINS);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("subtractvar")
            {
              vlog("SUBTRACTVAR\r\n");
              arg1=GetNum("SUBTRACTVAR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SUBTRACTVAR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SUBTRACTVAR);
              rom(arg1,2);
              rom(arg2,2);
            }
            aa("setfarbyte")
            {
              vlog("SETFARBYTE\r\n");
              BASIC(CMD_SETFARBYTE);
              arg1=GetNum("SETFARBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1,1);
              arg2=GetNum("SETFARBYTE");
              if (!gffs) {
                return;
              }
              rom(arg2,4);
            }
            aa("setbyte2")
            {
              vlog("SETBYTE2\r\n");
              arg1=GetNum("SETBYTE2");
              if (!gffs) {
                return;
              }
              arg2=GetNum("SETBYTE2");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SETBYTE2);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("spritebehave")
            {
              vlog("spritebehave\r\n");
              arg1=GetNum("spritebehave");
              if (!gffs) {
                return;
              }
              arg2=GetNum("spritebehave");
              if (!gffs) {
                return;
              }
              BASIC(CMD_SPRITEBEHAVE);
              rom(arg1,2);
              rom(arg2,2);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='t')
          {
            if (!strcmp(buf,"takefurniture"))
            {
              vlog("TAKEFURNITURE\r\n");
              arg1=GetNum("TAKEFURNITURE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_TAKEFURNITURE);
              rom(arg1,2);
            }
            aa("tempspriteface")
            {
              vlog("TEMPSPRITEFACE\r\n");
              arg1=GetNum("TEMPSPRITEFACE");
              if (!gffs) {
                return;
              }
              arg2=GetNum("TEMPSPRITEFACE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_TEMPSPRITEFACE);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("trainerbattle")
            {
              BASIC(CMD_TRAINERBATTLE);
              vlog("TRAINERBATTLE\r\n");
              arg1=GetNum("TRAINERBATTLE");
              if (!gffs) {
                return;
              }
              rom(arg1,1);
              arg2=GetNum("TRAINERBATTLE");
              if (!gffs) {
                return;
              }
              rom(arg2,2);
              arg3=GetNum("TRAINERBATTLE");
              if (!gffs) {
                return;
              }
              rom(arg3,2);
              arg4=GetNum("TRAINERBATTLE");
              if (!gffs) {
                return;
              }
              if ((arg4&0xff000000)==0)
              {
                arg4|=0x08000000;
                sprintf(buf3,"   -> Converted to 0x%x\r\n",arg4);
                vlog(buf3);
              }
              rom(arg4,4);
              if (arg1!=3)
              {
                arg5=GetNum("TRAINERBATTLE");
                if (!gffs) {
                  return;
                }
                if ((arg5&0xff000000)==0)
                {
                  arg5|=0x08000000;
                  sprintf(buf3,"   -> Converted to 0x%x\r\n",arg4);
                  vlog(buf3);
                }
                rom(arg5,4);
              }
              if (arg1==1||arg1==2)
              {
                arg6=GetNum("TRAINERBATTLE");
                if (!gffs) {
                  return;
                }
                if ((arg6&0xff000000)==0)
                {
                  arg6|=0x08000000;
                  sprintf(buf3,"   -> Converted to 0x%x\r\n",arg4);
                  vlog(buf3);
                }
                rom(arg6,4);
              }
            }
            aa("takefromcoincase")
            {
              vlog("TAKEFROMCOINCASE\r\n");
              arg1=GetNum("TAKEFROMCOINCASE");
              if (!gffs) {
                return;
              }
              BASIC(CMD_TAKEFROMCOINCASE);
              rom(arg1,2);
            }
            else if (!strcmp(buf,"textcolor")||!strcmp(buf,"textcolour"))
            {
              vlog("TEXTCOLOUR\r\n");
              arg1=GetNum("TEXTCOLOUR");
              if (!gffs) {
                return;
              }
              BASIC(CMD_TEXTCOLOR);
              rom(arg1,1);
            } else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='u')
          {
            if (!strcmp(buf,"updatecoins"))
            {
              vlog("UPDATECOINS\r\n");
              arg1=GetNum("UPDATECOINS");
              if (!gffs) {
                return;
              }
              arg2=GetNum("UPDATECOINS");
              if (!gffs) {
                return;
              }
              BASIC(CMD_UPDATECOINS);
              rom(arg1,1);
              rom(arg2,1);
            }
            aa("updatemoney")
            {
              vlog("UPDATEMONEY\r\n");
              arg1=GetNum("UPDATEMONEY");
              if (!gffs) {
                return;
              }
              arg2=GetNum("UPDATEMONEY");
              if (!gffs) {
                return;
              }
              arg3=GetNum("UPDATEMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CMD_UPDATEMONEY);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
            }else goto unk_cmd_fr;
            ec();
          }
          else if (*buf=='w')
          {
            if (!strcmp(buf,"waitbutton"))
            {
              BASIC(CMD_WAITBUTTON);
            }
            aa("waitfanfare")
            {
              BASIC(CMD_WAITFANFARE);
            }
            aa("waitspecial")
            {
              BASIC(CMD_WAITSPECIAL);
            }
            aa("warpteleport2")
            {
              vlog("WARPTELEPORT2\r\n");
              arg1=GetNum("WARPTELEPORT2");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARPTELEPORT2");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARPTELEPORT2");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARPTELEPORT2");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARPTELEPORT2");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARPTELEPORT2);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warp6")
            {
              vlog("WARP6\r\n");
              arg1=GetNum("WARP6");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARP6");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARP6");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARP6");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARP6");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARP6);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warpmuted")
            {
              vlog("WARPMUTED\r\n");
              arg1=GetNum("WARPMUTED");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARPMUTED");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARPMUTED");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARPMUTED");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARPMUTED");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARPMUTED);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warpwalking")
            {
              vlog("WARPWALKING\r\n");
              arg1=GetNum("WARPWALKING");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARPWALKING");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARPWALKING");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARPWALKING");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARPWALKING");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARPWALKING);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warpteleport")
            {
              vlog("WARPTELEPORT\r\n");
              arg1=GetNum("WARPTELEPORT");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARPTELEPORT");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARPTELEPORT");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARPTELEPORT");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARPTELEPORT");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARPTELEPORT);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warp3")
            {
              vlog("WARP3\r\n");
              arg1=GetNum("WARP3");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARP3");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARP3");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARP3");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARP3");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARP3);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warp4")
            {
              vlog("WARP4\r\n");
              arg1=GetNum("WARP4");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARP4");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARP4");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARP4");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARP4");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARP4);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warp5")
            {
              vlog("WARP5\r\n");
              arg1=GetNum("WARP5");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARP5");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARP5");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARP5");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARP5");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARP4);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("warpelevator")
            {
              vlog("WARPELEVATOR\r\n");
              arg1=GetNum("WARPELEVATOR");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARPELEVATOR");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARPELEVATOR");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARPELEVATOR");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARPELEVATOR");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARPELEVATOR);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }
            aa("writebytetooffset")
            {
              vlog("WRITEBYTETOOFFSET\r\n");
              arg1=GetNum("WRITEBYTETOOFFSET");
              if (!gffs) {
                return;
              }
              BASIC(CMD_WRITEBYTETOOFFSET);
              rom(arg1,1);
              arg2=GetNum("WRITEBYTETOOFFSET");
              if (!gffs) {
                return;
              }
              rom(arg2,4);
            }
            aa("waitcry")
            {
              vlog("WAITCRY\r\n");
              BASIC(CMD_WAITCRY);
            }
            aa("warp")
            {
              vlog("WARP\r\n");
              arg1=GetNum("WARP");
              if (!gffs) {
                return;
              }
              arg2=GetNum("WARP");
              if (!gffs) {
                return;
              }
              arg3=GetNum("WARP");
              if (!gffs) {
                return;
              }
              if (mode==FIRE_RED||arg3==0xFF)
              {
                arg4=GetNum("WARP");
                if (!gffs) {
                  return;
                }
                arg5=GetNum("WARP");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CMD_WARP);
              rom(arg1,1);
              rom(arg2,1);
              rom(arg3,1);
              if (mode==FIRE_RED||arg3==0xFF)
              {
                rom(arg4,2);
                rom(arg5,2);
              }
            }else goto unk_cmd_fr;
            ec();
          }
          aa("=")
          {
            vlog("[STRING]\r\n");
            if (chr==' ') {
              i++;
            }
            else {
              log("Should have a space after the =\r\n",33);
            }
            if (IncFile)
            {
              fseek(IncFile,0,SEEK_END);
              arg6=ftell(IncFile);
            } else
              arg6=0;
            temp_ptr=transbackstr(script,i-arg6-2,c);
            while (chr!='\n'&&chr!=0) {
              i++;
            }
            sprintf(buf2,"   -> %s\r\n",(char*)temp_ptr);
            free(temp_ptr);
            vlog(buf2);
          }
          aa(".")
          {
            vlog("[BINARY]\r\n   ->");
            while (chr==' ') {
              i++;
            }
            k=0;
            while (chr!='\n'&&chr!=0)
            {
              k=1-k;
              while (chr==' ')
              {
                i++;
              }
              j=0;
              while (((char*)("0123456789abcdef"))[j]!=0)
              {
                if (((char*)("0123456789abcdef"))[j]==chr)
                {
                  break;
                }
                j+=1;
              }
              if (((char*)("0123456789abcdef"))[j]==0)
              {
                sprintf(buf2,"Failed to understand hex character '%c'\r\n",chr);
                log(buf2,strlen(buf2));
                return;
              }
              if (k==0)
              {
                l|=j;
                rom(j,1);
                if (IsVerbose)
                {
                  sprintf(buf2," %02X",l);
                  log(buf2,strlen(buf2));
                }
              }
              else
              {
                l=j<<4;
              }
              i++;
            }
            vlog("\r\n");
          }
          else
          {
unk_cmd_fr:
            sprintf(buf2,"Warning: Unknown command \"%s\"\r\n",buf);
            log(buf2,strlen(buf2));
            while (chr!='\n'&&chr!=0)
            {
              i++;
            }
          }
          break;
        }
        i++;
      }
    }
    free(Script);
    if (IncFile)
      fclose(IncFile);
#ifndef DLL
    if (strcmp(script,""))fclose(CurrFile);
  }
  else
  {
    printf("Error opening script.");
    return;
  }
#endif
#ifdef WIN32
#ifndef DLL
  HW_DLG=CreateDialog(inst,MAKEINTRESOURCE(10),NULL,(DLGPROC)&OffsetDlg);
  UI_WIN=FindWindow("Score_Under_PKSVUI",NULL);
  if (UI_WIN!=NULL)
  {
    GetWindowRect(UI_WIN,&rect);
    rect.top+=40;
    rect.left+=40;
    SetWindowPos(HW_DLG,HWND_TOPMOST,rect.left,rect.top,0,0,SWP_NOSIZE);
  }
  else
  {
    SetWindowPos(HW_DLG,HWND_NOTOPMOST,200,200,0,0,SWP_NOSIZE);
  }
#endif
  OutputDebugString("Finished compiler loop, processing #ORGs");
  SendMessage(GetDlgItem(HW_DLG,3),LB_RESETCONTENT,0,0);
#endif
  if (dynu&&start==0)
  {
    log("Error: No #dyn used with dynamic offsets!\r\n",43);
  }
  else
  {
    vlog("\r\n#ORG: data\r\n");
    if (dynu)
    {
      calc_org(c,start,romfn);
      process_inserts(c);
    }
#ifdef WIN32
    OutputDebugString("Calculated ORGs, processed inserts");
#endif
    if (c!=NULL)
      c=rewind_codeblock(c);
    d=c;
    while (c)
    {
      c->org&=0x07FFFFFF;
      if (c->name)
      {
        sprintf(buf,"   -> %s <-> 0x%X (0x%X bytes)\r\n",c->name,c->org,c->size);
        log(buf,strlen(buf));
#ifdef WIN32
        strings=LocalAlloc(LPTR,strlen(c->name)+50);
        sprintf(strings,"%s <-> 0x%X (0x%X bytes)",c->name,c->org,c->size);
        SendMessage(GetDlgItem(HW_DLG,3),LB_ADDSTRING,0,(LPARAM)strings);
        LocalFree(strings);
        needdlg=1;
#endif
      }
      else
      {
        sprintf(buf,"   -> 0x%X (0x%X bytes)\r\n",c->org,c->size);
        log(buf,strlen(buf));
      }
      if (!testing)
      {
        fseek(RomFile,c->org,SEEK_SET);
        fwrite(c->data,1,c->size,RomFile);
      }
      c=c->next;
    }
  }
  fclose(RomFile);
  c=d;
  while (c)
  {
    d=c->next;
    delete_inserts(c);
    if (c->data)
      free(c->data);
    if (c->name)
      free(c->name);
    free(c);
    c=d;
  }
#ifdef WIN32
  if (needdlg)
  {
    ShowWindow(HW_DLG,SW_SHOW);
    SetWindowPos(HW_DLG,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
  }
  else
    ShowWindow(HW_DLG,SW_HIDE);
#endif
#ifndef DLL
  fclose(LogFile);
#endif
  return;
}
