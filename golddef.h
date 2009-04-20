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
//      Text   (#org xx\n= asdfg\e)         ,D
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
#define GLD_DOMAPTRIGGER            0x12 //C,D bank(1) map#(1) trig(1)
#define GLD_CHECKTRIGGERS           0x13 //C,D
#define GLD_DOTRIGGER               0x14 //C,D trig(1)
#define GLD_LOADVAR                 0x15 //C,D valu(1)
#define GLD_ADDVAR                  0x16 //C,D valu(1)
#define GLD_RANDOM                  0x17 //C,D max#(1)
#define GLD_CHECKVER                0x18 //C,D
#define GLD_COPYBYTETOVAR           0x19 //C,D addr(2)
#define GLD_COPYVARTOBYTE           0x1A //C,D addr(2)
#define GLD_WRITEBYTE               0x1B //C,D addr(2) valu(1)
#define GLD_CHECKCODE               0x1C //C,D code(1)
#define GLD_WRITEVARCODE            0x1D //C,D code(1)  Writes var to code
#define GLD_WRITECODE               0x1E //C,D code(1) valu(1)
#define GLD_GIVEITEM                0x1F //C,D item(1) amnt(1)
#define GLD_TAKEITEM                0x20 //C,D item(1) amnt(1)
#define GLD_CHECKITEM               0x21 //C,D item(1)
#define GLD_GIVEMONEY               0x22 //C,D what(1) amnt(3)
#define GLD_TAKEMONEY               0x23 //C,D what(1) amnt(3)
#define GLD_CHECKMONEY              0x24 //C,D what(1) amnt(3)
#define GLD_GIVECOINS               0x25 //C,D coin(2)
#define GLD_TAKECOINS               0x26 //C,D coin(2)
#define GLD_CHECKCOINS              0x27 //C,D coin(2) 0=[>] 1=[=] 2=[<]
#define GLD_GIVEPHONENUMBER         0x28 //C,D person(1)
#define GLD_TAKEPHONENUMBER         0x29 //C,D person(1)
#define GLD_CHECKPHONENUMBER        0x2A //C,D person(1)
#define GLD_CHECKTIME               0x2B //C,D time(1) 1=morn ... 4=nite
#define GLD_CHECKPOKE               0x2C //C,D poke(1)
#define GLD_GIVEPOKE                0x2D //C,D poke(1) levl(1) item(1) trnr(1) if(trnr==1){trname(2) pknick(2)}
#define GLD_GIVEEGG                 0x2E //C,D poke(1) levl(1)
#define GLD_GIVEPOKEITEM            0x2F //C,D itm*(2) ptr= itm#(1) + "THIS IS MESSAGE TEXT" (used in letters)
#define GLD_TAKEIFLETTER            0x30 //C,D ltr*(2) ptr= itm#(1) + "THE MSG"
#define GLD_CHECKBIT1               0x31 //C,D bit#(2)
#define GLD_CLEARBIT1               0x32 //C,D bit#(2)
#define GLD_SETBIT1                 0x33 //C,D bit#(2)
#define GLD_CHECKBIT2               0x34 //C,D bit#(2)
#define GLD_CLEARBIT2               0x35 //C,D bit#(2)
#define GLD_SETBIT2                 0x36 //C,D bit#(2)
#define GLD_WILDOFF                 0x37 //C,D
#define GLD_WILDON                  0x38 //C,D
#define GLD_XYCOMPARE               0x39 //C,D tbl*(2)
#define GLD_WARPMOD                 0x3A //C,D towp(1) tobk(1) tomp(1)
#define GLD_BLACKOUTMOD             0x3B //C,D bank(1) map#(1)
#define GLD_WARP                    0x3C //C,D bank(1) map#(1) xval(1) yval(1)
#define GLD_MONEYTOTEXT             0x3D //C,D what(1) tvar(1)
#define GLD_COINSTOTEXT             0x3E //C,D tvar(1)
#define GLD_VARTOTEXT               0x3F //C,D tvar(1)
#define GLD_POKETOTEXT              0x40 //C,D poke(1) tvar(1)
#define GLD_ITEMTOTEXT              0x41 //C,D item(1) tvar(1)
#define GLD_LOCATIONTOTEXT          0x42 //C,D tvar(1)
#define GLD_TRAINERTOTEXT           0x43 //C,D trnr(1) grp#(1) textvar(1)
#define GLD_STRINGTOTEXT            0x44 //C,D str*(2) textvar(1)
#define GLD_ITEMNOTIFY              0x45 //C,D
#define GLD_POCKETISFULL            0x46 //C,D
#define GLD_LOADFONT                0x47 //C,D
#define GLD_REFRESHSCREEN           0x48 //C,D dummy(1)
#define GLD_LOADMOVESPRITES         0x49 //C,D
#define GLD_C1CELOADBYTE            0x4A //C,D byte(1)
#define GLD_3WRITETEXT              0x4B //C,D txt*(3)
#define GLD_2WRITETEXT              0x4C //C,D txt*(2)
#define GLD_REPEATTEXT              0x4D //C,D 0xFFFF
#define GLD_YESORNO                 0x4E //C,D
#define GLD_LOADMENUDATA            0x4F //C,D menu*(2)
#define GLD_WRITEBACKUP             0x50 //C,D
#define GLD_JUMPTEXTFACEPLAYER      0x51 //C,D txt*(2)
#define GLD_JUMPTEXT                0x52 //C,D txt*(2)
#define GLD_CLOSETEXT               0x53 //C,D
#define GLD_KEEPTEXTOPEN            0x54 //C,D
#define GLD_POKEPIC                 0x55 //C,D 0x00 if from var, else poke(1)
#define GLD_POKEPICYESORNO          0x56 //C,D
#define GLD_INTERPRETMENU           0x57 //C,D
#define GLD_INTERPRETMENU2          0x58 //C,D
#define GLD_LOADPIKADATA            0x59 //C,D
#define GLD_CLEARFIGHT              0x5A //C,D
#define GLD_LOADTRAINERSEEN         0x5B //C,D
#define GLD_LOADPOKEDATA            0x5C //C,D poke(1) level(1)
#define GLD_LOADTRAINER             0x5D //C,D group#(1) trainer(1)
#define GLD_STARTBATTLE             0x5E //C,D
#define GLD_RETURNAFTERBATTLE       0x5F //C,D
#define GLD_CATCHTUTORIAL           0x60 //C,D spr#(1)
#define GLD_TRAINERTEXT             0x61 //C,D text(1)
#define GLD_TRAINERSTATUS           0x62 //C,D stat(1) 0=deact 1=act 2=check
#define GLD_WINLOSSTEXT             0x63 //C,D win*(2) lose*(2)
#define GLD_TALKAFTER               0x64 //C,D
#define GLD_TALKAFTERCANCEL         0x65 //C,D
#define GLD_TALKAFTERCHECK          0x66 //C,D 
#define GLD_SETLASTTALKED           0x67 //C,D person(1)
#define GLD_APPLYMOVEMENT           0x68 //C,D person(1) moves*(2)
#define GLD_APPLYMOVEOTHER          0x69 //C,D moves*(2)
#define GLD_FACEPLAYER              0x6A //C,D
#define GLD_FACEPERSON              0x6B //C,D targetperson(1) facingperson(1) if[tprs>fd]tprs=talkedto, if[fprs=fe]tprs=talkedto
#define GLD_VARIABLESPRITE          0x6C //C,D xxxx(1) sprite(1)
#define GLD_DISAPPEAR               0x6D //C,D person(1)
#define GLD_APPEAR                  0x6E //C,D person(1)
#define GLD_FOLLOW                  0x6F //C,D leader(1) follower(1)
#define GLD_STOPFOLLOW              0x70 //C,D
#define GLD_MOVEPERSON              0x71 //C,D person(1) xval(1) yval(1)
#define GLD_WRITEPERSONLOC          0x72 //C,D person(1)
#define GLD_LOADEMOTE               0x73 //C,D emot(1)
#define GLD_SHOWEMOTE               0x74 //C,D emot(1) person(1) time(1)
#define GLD_SPRITEFACE              0x75 //C,D person(1) dirc(1)
#define GLD_FOLLOWNOTEXACT          0x76 //C,D lead(1) folo(1)
#define GLD_EARTHQUAKE              0x77 //C,D time(1)
#define GLD_SWAPMAPS                0x78 //C,D map*(3)
#define GLD_CHANGEBLOCK             0x79 //C,D xval(1) yval(1) blok(1)
#define GLD_RELOADMAP               0x7A //C,D
#define GLD_RELOADMAPPART           0x7B //C,D
#define GLD_WRITECMDQUEUE           0x7C //C,D queue*(2)->[[queue=XXXXX (5byte)]]
#define GLD_DELCMDQUEUE             0x7D //C,D first cmd of resp queue(1)
#define GLD_PLAYMUSIC               0x7E //C,D music(2)
#define GLD_PLAYRAMMUSIC            0x7F //C,D
#define GLD_MUSICFADEOUT            0x80 //C,D nmusic(2) time(1)
#define GLD_PLAYMAPMUSIC            0x81 //C,D
#define GLD_RELOADMAPMUSIC          0x82 //C,D
#define GLD_CRY                     0x83 //C,D cry(2)
#define GLD_PLAYSOUND               0x84 //C,D sound(2)
#define GLD_WAITBUTTON              0x85 //C,D
#define GLD_WARPSOUND               0x86 //C,D
#define GLD_SPECIALSOUND            0x87 //C,D
#define GLD_PASSTOENGINE            0x88 //C,D ptr(3)
#define GLD_NEWLOADMAP              0x89 //C,D type(1)
#define GLD_PAUSE                   0x8A //C,D time(1)
#define GLD_DEACTIVATEFACING        0x8B //C,D time(1)
#define GLD_PRIORITYJUMP            0x8C //C,D place(2)
#define GLD_WARPCHECK               0x8D //C,D
#define GLD_PTPRIORITYJUMP          0x8E //C,D *place(2)
#define GLD_RETURN                  0x8F //C,D
#define GLD_END                     0x90 //C,D
#define GLD_RELOADANDRETURN         0x91 //C,D
#define GLD_RESETFUNCS              0x92 //C,D
#define GLD_POKEMART                0x93 //C,D dialog#(1) mart#(2)
#define GLD_ELEVATOR                0x94 //C,D floorlist(2)
#define GLD_TRADE                   0x95 //C,D trade#(1)
#define GLD_ASKFORPHONENUMBER       0x96 //C,D number(1)
#define GLD_PHONECALL               0x97 //C,D caller(2)
#define GLD_HANGUP                  0x98 //C,D
#define GLD_DESCRIBEDECORATION      0x99 //C,D type(1)
#define GLD_FRUITTREE               0x9A //C,D type(1)
#define GLD_SPECIALPHONECALL        0x9B //C,D type(1)
#define GLD_CHECKPHONECALL          0x9C //C,D
#define GLD_VERBOSEGIVEITEM         0x9D //C,D item(1)amt(1)
#define GLD_LOADWILDDATA            0x9E //C,D bank(1)map(1)
#define GLD_HALLOFFAME              0x9F //C,D
#define GLD_CREDITS                 0xA0 //C,D
#define GLD_WARPFACING              0xA1 //C,D facing(1)bank(1)map(1)x(1)y(1)
#define GLD_STORETEXT               0xA2 //C,D text(2)bank(1)txtvar(1)
#define GLD_DISPLAYLOCATION         0xA3 //C,D location(1)

#define CRY_2CALL                   0x00 //C,D addr(2)
#define CRY_3CALL                   0x01 //C,D addr(3)
#define CRY_2PTCALL                 0x02 //C,D addr(2)
#define CRY_2JUMP                   0x03 //C,D addr(2)
#define CRY_3JUMP                   0x04 //C,D addr(3)
#define CRY_2PTJUMP                 0x05 //C,D addr(2)
#define CRY_EQBYTE                  0x06 //C,D addr(2)
#define CRY_NEQBYTE                 0x07 //C,D addr(2)
#define CRY_EQZERO                  0x08 //C,D addr(2)
#define CRY_NEQZERO                 0x09 //C,D addr(2)
#define CRY_LTBYTE                  0x0A //C,D addr(2)
#define CRY_GTBYTE                  0x0B //C,D addr(2)
#define CRY_JUMPSTD                 0x0C //C,D num#(2)
#define CRY_CALLSTD                 0x0D //C,D num#(2)
#define CRY_3CALLASM                0x0E //C,D addr(3)
#define CRY_SPECIAL                 0x0F //C,D num#(2)
#define CRY_2PTCALLASM              0x10 //C,D addr(2)
#define CRY_CHECKMAPTRIGGERS        0x11 //C,D bank(1) map#(1)
#define CRY_DOMAPTRIGGER            0x12 //C,D bank(1) map#(1) trig(1)
#define CRY_CHECKTRIGGERS           0x13 //C,D
#define CRY_DOTRIGGER               0x14 //C,D trig(1)
#define CRY_LOADVAR                 0x15 //C,D valu(1)
#define CRY_ADDVAR                  0x16 //C,D valu(1)
#define CRY_RANDOM                  0x17 //C,D max#(1)
#define CRY_CHECKVER                0x18 //C,D
#define CRY_COPYBYTETOVAR           0x19 //C,D addr(2)
#define CRY_COPYVARTOBYTE           0x1A //C,D addr(2)
#define CRY_WRITEBYTE               0x1B //C,D addr(2) valu(1)
#define CRY_CHECKCODE               0x1C //C,D code(1)
#define CRY_WRITEVARCODE            0x1D //C,D code(1)  Writes var to code
#define CRY_WRITECODE               0x1E //C,D code(1) valu(1)
#define CRY_GIVEITEM                0x1F //C,D item(1) amnt(1)
#define CRY_TAKEITEM                0x20 //C,D item(1) amnt(1)
#define CRY_CHECKITEM               0x21 //C,D item(1)
#define CRY_GIVEMONEY               0x22 //C,D what(1) amnt(3)
#define CRY_TAKEMONEY               0x23 //C,D what(1) amnt(3)
#define CRY_CHECKMONEY              0x24 //C,D what(1) amnt(3)
#define CRY_GIVECOINS               0x25 //C,D coin(2)
#define CRY_TAKECOINS               0x26 //C,D coin(2)
#define CRY_CHECKCOINS              0x27 //C,D coin(2) 0=[>] 1=[=] 2=[<]
#define CRY_GIVEPHONENUMBER         0x28 //C,D person(1)
#define CRY_TAKEPHONENUMBER         0x29 //C,D person(1)
#define CRY_CHECKPHONENUMBER        0x2A //C,D person(1)
#define CRY_CHECKTIME               0x2B //C,D time(1) 1=morn ... 4=nite
#define CRY_CHECKPOKE               0x2C //C,D poke(1)
#define CRY_GIVEPOKE                0x2D //C,D poke(1) levl(1) item(1) trnr(1) if(trnr==1){trname(2) pknick(2)}
#define CRY_GIVEEGG                 0x2E //C,D poke(1) levl(1)
#define CRY_GIVEPOKEITEM            0x2F //C,D itm*(2) ptr= itm#(1) + "THIS IS MESSAGE TEXT" (used in letters)
#define CRY_TAKEIFLETTER            0x30 //C,D ltr*(2) ptr= itm#(1) + "THE MSG"
#define CRY_CHECKBIT1               0x31 //C,D bit#(2)
#define CRY_CLEARBIT1               0x32 //C,D bit#(2)
#define CRY_SETBIT1                 0x33 //C,D bit#(2)
#define CRY_CHECKBIT2               0x34 //C,D bit#(2)
#define CRY_CLEARBIT2               0x35 //C,D bit#(2)
#define CRY_SETBIT2                 0x36 //C,D bit#(2)
#define CRY_WILDOFF                 0x37 //C,D
#define CRY_WILDON                  0x38 //C,D
#define CRY_XYCOMPARE               0x39 //C,D tbl*(2)
#define CRY_WARPMOD                 0x3A //C,D towp(1) tobk(1) tomp(1)
#define CRY_BLACKOUTMOD             0x3B //C,D bank(1) map#(1)
#define CRY_WARP                    0x3C //C,D bank(1) map#(1) xval(1) yval(1)
#define CRY_MONEYTOTEXT             0x3D //C,D what(1) tvar(1)
#define CRY_COINSTOTEXT             0x3E //C,D tvar(1)
#define CRY_VARTOTEXT               0x3F //C,D tvar(1)
#define CRY_POKETOTEXT              0x40 //C,D poke(1) tvar(1)
#define CRY_ITEMTOTEXT              0x41 //C,D item(1) tvar(1)
#define CRY_LOCATIONTOTEXT          0x42 //C,D tvar(1)
#define CRY_TRAINERTOTEXT           0x43 //C,D trnr(1) grp#(1) textvar(1)
#define CRY_STRINGTOTEXT            0x44 //C,D str*(2) textvar(1)
#define CRY_ITEMNOTIFY              0x45 //C,D
#define CRY_POCKETISFULL            0x46 //C,D
#define CRY_LOADFONT                0x47 //C,D
#define CRY_REFRESHSCREEN           0x48 //C,D dummy(1)
#define CRY_LOADMOVESPRITES         0x49 //C,D
#define CRY_C1CELOADBYTE            0x4A //C,D byte(1)
#define CRY_3WRITETEXT              0x4B //C,D txt*(3)
#define CRY_2WRITETEXT              0x4C //C,D txt*(2)
#define CRY_REPEATTEXT              0x4D //C,D 0xFFFF
#define CRY_YESORNO                 0x4E //C,D
#define CRY_LOADMENUDATA            0x4F //C,D menu*(2)
#define CRY_WRITEBACKUP             0x50 //C,D
#define CRY_JUMPTEXTFACEPLAYER      0x51 //C,D txt*(2)
#define CRY_JUMPTEXT                0x53 //C,D txt*(2)
#define CRY_CLOSETEXT               0x54 //C,D
#define CRY_KEEPTEXTOPEN            0x55 //C,D
#define CRY_POKEPIC                 0x56 //C,D 0x00 if from var, else poke(1)
#define CRY_POKEPICYESORNO          0x57 //C,D
#define CRY_INTERPRETMENU           0x58 //C,D
#define CRY_INTERPRETMENU2          0x59 //C,D
#define CRY_LOADPIKADATA            0x5A //C,D
#define CRY_CLEARFIGHT              0x5B //C,D
#define CRY_LOADTRAINERSEEN         0x5C //C,D
#define CRY_LOADPOKEDATA            0x5D //C,D poke(1) level(1)
#define CRY_LOADTRAINER             0x5E //C,D group#(1) trainer(1)
#define CRY_STARTBATTLE             0x5F //C,D
#define CRY_RETURNAFTERBATTLE       0x60 //C,D
#define CRY_CATCHTUTORIAL           0x61 //C,D spr#(1)
#define CRY_TRAINERTEXT             0x62 //C,D text(1)
#define CRY_TRAINERSTATUS           0x63 //C,D stat(1) 0=deact 1=act 2=check
#define CRY_WINLOSSTEXT             0x64 //C,D win*(2) lose*(2)
#define CRY_TALKAFTER               0x65 //C,D
#define CRY_TALKAFTERCANCEL         0x66 //C,D
#define CRY_TALKAFTERCHECK          0x67 //C,D 
#define CRY_SETLASTTALKED           0x68 //C,D person(1)
#define CRY_APPLYMOVEMENT           0x69 //C,D person(1) moves*(2)
#define CRY_APPLYMOVEOTHER          0x6A //C,D moves*(2)
#define CRY_FACEPLAYER              0x6B //C,D
#define CRY_FACEPERSON              0x6C //C,D targetperson(1) facingperson(1) if[tprs>fd]tprs=talkedto, if[fprs=fe]tprs=talkedto
#define CRY_VARIABLESPRITE          0x6D //C,D xxxx(1) sprite(1)
#define CRY_DISAPPEAR               0x6E //C,D person(1)
#define CRY_APPEAR                  0x6F //C,D person(1)
#define CRY_FOLLOW                  0x70 //C,D leader(1) follower(1)
#define CRY_STOPFOLLOW              0x71 //C,D
#define CRY_MOVEPERSON              0x72 //C,D person(1) xval(1) yval(1)
#define CRY_WRITEPERSONLOC          0x73 //C,D person(1)
#define CRY_LOADEMOTE               0x74 //C,D emot(1)
#define CRY_SHOWEMOTE               0x75 //C,D emot(1) person(1) time(1)
#define CRY_SPRITEFACE              0x76 //C,D person(1) dirc(1)
#define CRY_FOLLOWNOTEXACT          0x77 //C,D lead(1) folo(1)
#define CRY_EARTHQUAKE              0x78 //C,D time(1)
#define CRY_SWAPMAPS                0x79 //C,D map*(3)
#define CRY_CHANGEBLOCK             0x7A //C,D xval(1) yval(1) blok(1)
#define CRY_RELOADMAP               0x7B //C,D
#define CRY_RELOADMAPPART           0x7C //C,D
#define CRY_WRITECMDQUEUE           0x7D //C,D queue*(2)->[[queue=XXXXX (5byte)]]
#define CRY_DELCMDQUEUE             0x7E //C,D first cmd of resp queue(1)
#define CRY_PLAYMUSIC               0x7F //C,D music(2)
#define CRY_PLAYRAMMUSIC            0x80 //C,D
#define CRY_MUSICFADEOUT            0x81 //C,D nmusic(2) time(1)
#define CRY_PLAYMAPMUSIC            0x82 //C,D
#define CRY_RELOADMAPMUSIC          0x83 //C,D
#define CRY_CRY                     0x84 //C,D cry(2)
#define CRY_PLAYSOUND               0x85 //C,D sound(2)
#define CRY_WAITBUTTON              0x86 //C,D
#define CRY_WARPSOUND               0x87 //C,D
#define CRY_SPECIALSOUND            0x88 //C,D
#define CRY_PASSTOENGINE            0x89 //C,D ptr(3)
#define CRY_NEWLOADMAP              0x8A //C,D type(1)
#define CRY_PAUSE                   0x8B //C,D time(1)
#define CRY_DEACTIVATEFACING        0x8C //C,D time(1)
#define CRY_PRIORITYJUMP            0x8D //C,D place(2)
#define CRY_WARPCHECK               0x8E //C,D
#define CRY_PTPRIORITYJUMP          0x8F //C,D *place(2)
#define CRY_RETURN                  0x90 //C,D
#define CRY_END                     0x91 //C,D
#define CRY_RELOADANDRETURN         0x92 //C,D
#define CRY_RESETFUNCS              0x93 //C,D
#define CRY_POKEMART                0x94 //C,D dialog#(1) mart#(2)
#define CRY_ELEVATOR                0x95 //C,D floorlist(2)
#define CRY_TRADE                   0x96 //C,D trade#(1)
#define CRY_ASKFORPHONENUMBER       0x97 //C,D number(1)
#define CRY_PHONECALL               0x98 //C,D caller(2)
#define CRY_HANGUP                  0x99 //C,D
#define CRY_DESCRIBEDECORATION      0x9A //C,D type(1)
#define CRY_FRUITTREE               0x9B //C,D type(1)
#define CRY_SPECIALPHONECALL        0x9C //C,D type(1)
#define CRY_CHECKPHONECALL          0x9D //C,D
#define CRY_VERBOSEGIVEITEM         0x9E //C,D item(1)amt(1)
#define CRY_LOADWILDDATA            0xA0 //C,D bank(1)map(1)
#define CRY_HALLOFFAME              0xA1 //C,D
#define CRY_CREDITS                 0xA2 //C,D
#define CRY_WARPFACING              0xA3 //C,D facing(1)bank(1)map(1)x(1)y(1)
#define CRY_STORETEXT               0xA4 //C,D text(2)bank(1)txtvar(1)
#define CRY_DISPLAYLOCATION         0xA5 //C,D location(1)

