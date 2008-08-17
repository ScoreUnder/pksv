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
//GoldDef.h by Charles Daffern
//Here come the defs!              Compile,Decompile, Args
//      Text   (#org xx\n= asdfg)           , 
//      Moves  (#org xx\nM x)               , 
#define GLD_2CALL                   0x00 //C,D addr(2)
#define GLD_3CALL                   0x01 //C,D addr(3)
#define GLD_2PTCALL                 0x02 //C,D addr(2)
#define GLD_2JUMP                   0x03 //C,D addr(2)
#define GLD_3JUMP                   0x04 //C,D addr(3)
#define GLD_2PTJUMP                 0x05 //C,D addr(2)
#define GLD_EQBYTE                  0x06 //C,D addr(2)
#define GLD_NEQBYTE                 0x07 //C,D addr(2)
#define GLD_EQZERO                  0x08 //C,D addr(2)
#define GLD_NEQZERO                 0x09 //C,D addr(2)
#define GLD_LTBYTE                  0x0A //C,D addr(2)
#define GLD_GTBYTE                  0x0B //C,D addr(2)
#define GLD_JUMPSTD                 0x0C //C,D num#(2)
#define GLD_CALLSTD                 0x0D //C,D num#(2)
#define GLD_3CALLASM                0x0E //C,D addr(3)
#define GLD_SPECIAL                 0x0F //C,D num#(2)
#define GLD_2PTCALLASM              0x10 //C,D addr(2)
#define GLD_CHECKMAPTRIGGERS        0x11 //C,D bank(1) map#(1)
#define GLD_DOMAPTRIGGER            0x12 // ,D bank(1) map#(1) trig(1)
#define GLD_CHECKTRIGGERS           0x13 //C,D
#define GLD_DOTRIGGER               0x14 // ,D trig(1)
#define GLD_LOADVAR                 0x15 // ,D valu(1)
#define GLD_ADDVAR                  0x16 // ,D valu(1)
#define GLD_RANDOM                  0x17 // ,D max#(1)
#define GLD_CHECKVER                0x18 //C,D
#define GLD_COPYBYTETOVAR           0x19 // ,D addr(2)
#define GLD_COPYVARTOBYTE           0x1A // ,D addr(2)
#define GLD_WRITEBYTE               0x1B // ,D addr(2) valu(1)
#define GLD_CHECKCODE               0x1C // ,D code(1) rchk(1)
#define GLD_WRITEVARCODE            0x1D // ,D code(1)  Writes var to code
#define GLD_WRITECODE               0x1E // ,D code(1) valu(1)
#define GLD_GIVEITEM                0x1F // ,D item(1) amnt(1)
#define GLD_TAKEITEM                0x20 // ,D item(1) amnt(1)
#define GLD_CHECKITEM               0x21 // ,D item(1)
#define GLD_GIVEMONEY               0x22 // ,D what(1) amnt(3)
#define GLD_TAKEMONEY               0x23 // ,D what(1) amnt(3)
#define GLD_CHECKMONEY              0x24 // ,D what(1) amnt(3)
#define GLD_GIVECOINS               0x25 // ,D coin(2)
#define GLD_TAKECOINS               0x26 // ,D coin(2)
#define GLD_CHECKCOINS              0x27 // ,D coin(2) 0=[>] 1=[=] 2=[<]
#define GLD_GIVEPHONENUMBER         0x28 // ,D prsn(1)
#define GLD_TAKEPHONENUMBER         0x29 // ,D prsn(1)
#define GLD_CHECKPHONENUMBER        0x2A // ,D prsn(1)
#define GLD_CHECKTIME               0x2B // ,D time(1) 1=morn ... 4=nite
#define GLD_CHECKPOKE               0x2C // ,D poke(1)
#define GLD_GIVEPOKE                0x2D // ,D poke(1) levl(1) item(1) trnr(1) if(trnr==1){trname(2) pknick(2)}
#define GLD_GIVEEGG                 0x2E // ,D poke(1) levl(1)
#define GLD_GIVEPOKEITEM            0x2F // ,D itm*(2) ptr= itm#(1) + "THIS IS MESSAGE TEXT" (used in letters)
#define GLD_TAKEIFLETTER            0x30 // ,D ltr*(2) ptr= itm#(1) + "THE MSG"
#define GLD_CHECKBIT1               0x31 // ,D bit#(2)
#define GLD_CLEARBIT1               0x32 // ,D bit#(2)
#define GLD_SETBIT1                 0x33 // ,D bit#(2)
#define GLD_CHECKBIT2               0x34 // ,D bit#(2)
#define GLD_CLEARBIT2               0x35 // ,D bit#(2)
#define GLD_SETBIT2                 0x36 // ,D bit#(2)
#define GLD_WILDOFF                 0x37 //C,D
#define GLD_WILDON                  0x38 //C,D
#define GLD_XYCOMPARE               0x39 // ,D tbl*(2)
#define GLD_WARPMOD                 0x3A // ,D towp(1) tobk(1) tomp(1)
#define GLD_BLACKOUTMOD             0x3B // ,D bank(1) map#(1)
#define GLD_WARP                    0x3C // ,D bank(1) map#(1) xval(1) yval(1)
#define GLD_MONEYTOTEXT             0x3D // ,D what(1) tvar(1)
#define GLD_COINSTOTEXT             0x3E // ,D tvar(1)
#define GLD_VARTOTEXT               0x3F // ,D tvar(1)
#define GLD_POKETOTEXT              0x40 // ,D poke(1) tvar(1)
#define GLD_ITEMTOTEXT              0x41 // ,D item(1) tvar(1)
#define GLD_LOCATIONTOTEXT          0x42 // ,D tvar(1)
#define GLD_TRAINERTOTEXT           0x43 // ,D trnr(1) grp#(1) tvar(1)
#define GLD_STRINGTOTEXT            0x44 // ,D str*(2) tvar(1)
#define GLD_ITEMNOTIFY              0x45 //C,D
#define GLD_POCKETISFULL            0x46 //C,D
#define GLD_OPENTEXTBOX             0x47 //C,D
#define GLD_REFRESHSCREEN           0x48 // ,D dumy(1)
#define GLD_LOADMOVESPRITES         0x49 //C,D
#define GLD_C1CELOADBYTE            0x4A // ,D byte(1)
#define GLD_WRITETEXTBANK           0x4B // ,D txt*(3)
#define GLD_WRITETEXT               0x4C // ,D txt*(2)
#define GLD_REPEATTEXT              0x4D // ,D 0xFFFF
#define GLD_YESORNO                 0x4E //C,D
#define GLD_LOADMENUDATA            0x4F // ,D mnu*(2)
#define GLD_WRITEBACKUP             0x50 //C,D
#define GLD_JUMPTEXTFACEPLAYER      0x51 // ,D txt*(2)
#define GLD_JUMPTEXT                0x52 // ,D txt*(2)
#define GLD_CLOSETEXT               0x53 //C,D
#define GLD_KEEPTEXTOPEN            0x54 //C,D
#define GLD_POKEPIC                 0x55 // ,D 0x00 if from var, else poke(1)
#define GLD_POKEPICYESORNO          0x56 //C,D
#define GLD_INTERPRETMENU           0x57 //C,D
#define GLD_INTERPRETMENU2          0x58 //C,D
#define GLD_LOADPIKADATA            0x59 //C,D
#define GLD_CLEARFIGHT              0x5A //C,D
#define GLD_LOADTRAINERSEEN         0x5B //C,D
#define GLD_LOADPOKEDATA            0x5C // ,D poke(1) levl(1)
#define GLD_LOADTRAINER             0x5D // ,D grp#(1) trnr(1)
#define GLD_STARTBATTLE             0x5E //C,D
#define GLD_RETURNAFTERBATTLE       0x5F //C,D
#define GLD_CATCHTUTORIAL           0x60 // ,D spr#(1)
#define GLD_TRAINERTEXT             0x61 // ,D text(1)
#define GLD_TRAINERSTATUS           0x62 // ,D stat(1) 0=deact 1=act 2=chk
#define GLD_WINLOSSTEXT             0x63 // ,D win*(2) los*(2)
#define GLD_TALKAFTER               0x64 //C,D
#define GLD_TALKAFTERCANCEL         0x65 //C,D
#define GLD_TALKAFTERCHECK          0x66 //C,D 
#define GLD_SETLASTTALKED           0x67 // ,D prsn(1)
#define GLD_APPLYMOVEMENT           0x68 // ,D prsn(1) mvs*(2)
#define GLD_APPLYMOVEOTHER          0x69 // ,D mvs*(2)
#define GLD_FACEPLAYER              0x6A //C,D
#define GLD_FACEPERSON              0x6B // ,D tprs(1) fprs(1) if[tprs>fd]tprs=talkedto, if[fprs=fe]tprs=talkedto
#define GLD_VARIABLESPRITE          0x6C // ,D xxxx(1) sprt(1)
#define GLD_DISAPPEAR               0x6D // ,D prsn(1)
#define GLD_APPEAR                  0x6E // ,D prsn(1)
#define GLD_FOLLOW                  0x6F // ,D lead(1) folo(1)
#define GLD_STOPFOLLOW              0x70 //C,D
#define GLD_MOVEPERSON              0x71 // ,D prsn(1) xval(1) yval(1)
#define GLD_WRITEPERSONLOC          0x72 // ,D prsn(1)
#define GLD_LOADEMOTE               0x73 // ,D emot(1)
#define GLD_SHOWEMOTE               0x74 // ,D emot(1) prsn(1) time(1)
#define GLD_CHANGEPERSONDIR         0x75 // ,D prsn(1) dirc(1)
#define GLD_FOLLOWNOTEXACT          0x76 // ,D lead(1) folo(1)
#define GLD_EARTHQUAKE              0x77 // ,D time(1)
#define GLD_SWAPMAPS                0x78 // ,D map*(3)
#define GLD_CHANGEBLOCK             0x79 // ,D xval(1) yval(1) blok(1)
#define GLD_RELOADMAP               0x7A //C,D
#define GLD_RELOADMAPPART           0x7B //C,D
#define GLD_WRITECMDQUEUE           0x7C // ,D quu*(2)->[[queue=XXXXX (5byte)]]
#define GLD_DELCMDQUEUE             0x7D // ,D first cmd of resp queue(1)
#define GLD_PLAYMUSIC               0x7E // ,D msic(2)
#define GLD_PLAYRAMMUSIC            0x7F //C,D
#define GLD_MUSICFADEOUT            0x80 // ,D nmsc(2) time(1)
#define GLD_PLAYMAPMUSIC            0x81 //C,D
#define GLD_RELOADMAPMUSIC          0x82 //C,D
#define GLD_CRY                     0x83 // ,D cry(2)
#define GLD_PLAYSOUND               0x84 // ,D sound(2)
#define GLD_WAITBUTTON              0x85 //C,D
#define GLD_WARPSOUND               0x86 //C,D
#define GLD_SPECIALSOUND            0x87 //C,D
#define GLD_PASSTOENGINE            0x88 // ,D ptr(3)
#define GLD_NEWLOADMAP              0x89 // ,D type(1)
#define GLD_PAUSE                   0x8A // ,D time(1)
#define GLD_DEACTIVATEFACING        0x8B // ,D time(1)
#define GLD_PRIORITYJUMP            0x8C // ,D place(2)
#define GLD_WARPCHECK               0x8D //C,D
#define GLD_PTPRIORITYJUMP          0x8E // ,D *place(2)
#define GLD_RETURN                  0x8F //C,D
#define GLD_END                     0x90 //C,D
#define GLD_RELOADANDRETURN         0x91 //C,D
#define GLD_RESETFUNCS              0x92 //C,D
#define GLD_POKEMART                0x93 // ,D dialog#(1) mart#(2)
#define GLD_ELEVATOR                0x94 // ,D floorlist(2)
#define GLD_TRADE                   0x95 // ,D trade#(1)
#define GLD_ASKFORPHONENUMBER       0x96 // ,D number(1)
#define GLD_PHONECALL               0x97 // ,D caller(2)
#define GLD_HANGUP                  0x98 //C,D
#define GLD_DESCRIBEDECORATION      0x99 // ,D type(1)
#define GLD_FRUITTREE               0x9A // ,D type(1)
#define GLD_SPECIALPHONECALL        0x9B // ,D type(1)
#define GLD_CHECKPHONECALL          0x9C //C,D
#define GLD_VERBOSEGIVEITEM         0x9D // ,D item(1)amt(1)
#define GLD_LOADWILDDATA            0x9E // ,D bank(1)map(1)
#define GLD_HALLOFFAME              0x9F //C,D
#define GLD_CREDITS                 0xA0 //C,D
#define GLD_WARPFACING              0xA1 // ,D facing(1)bank(1)map(1)x(1)y(1)
#define GLD_STORETEXT               0xA2 // ,D text(2)bank(1)txtvar(1)
#define GLD_DISPLAYLOCATION         0xA3 // ,D location(1)

