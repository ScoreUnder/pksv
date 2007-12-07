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
//PokeDef.h by Charles Daffern
//Here come the defs!              Compile,Decompile, Args
//      Text   (#org xx\n= asdfg)          C,D
//      Moves  (#org xx\nM x)               ,D
//      Braille (Used as #raw in compiler) C,D
#define CMD_NOP0                    0x00 //C,D
#define CMD_NOP1                    0x01 //C,D
#define CMD_END                     0x02 //C,D
#define CMD_RETURN                  0x03 //C,D
#define CMD_CALL                    0x04 //C,D addr(4)
#define CMD_JUMP                    0x05 //C,D DITTO
#define CMD_JUMPIF                  0x06 //C,D ops(1) addr(4)
#define CMD_CALLIF                  0x07 //C,D ops(1) addr(4)
#define CMD_JUMPSTD                 0x08 //C,D type(1)
#define CMD_CALLSTD                 0x09 //C,D type(1)
#define CMD_KILLSCRIPT              0x0D //C,D
#define CMD_SETBYTE                 0x0E //C,D byte(1)
#define CMD_MSGBOX                  0x0F //C,D 00 addr(4)
/////// CMD_MSGBOX used as LOADPOINTER     C,D bank(1) addr(4)
#define CMD_WRITEBYTETOOFFSET       0x11 //C,D byte(1) addr(4)
#define CMD_LOADBYTEFROMPOINTER     0x12 //C,D byte(1) addr(4)
#define CMD_COPYSCRIPTBANKS         0x14 //C,D bank(1) bank(1)
#define CMD_COPYBYTE                0x15 //C,D addr(4) addr(4)
#define CMD_SETVAR                  0x16 //C,D var(2) val(2)
#define CMD_COPYVAR                 0x19 //C,D var(2) val(2)
#define CMD_COPYVARIFNOTZERO        0x1A //C,D var(2) val(2)
#define CMD_COMPAREVARTOBYTE        0x1C //C,D var(2) byte(1)
#define CMD_COMPAREVARTOFARBYTE     0x1D //C,D var(2) addr(4)
#define CMD_COMPAREFARBYTETOVAR     0x1E //C,D addr(4) var(2)
#define CMD_COMPAREFARBYTETOBYTE    0x1F //C,D addr(4) byte(1)
#define CMD_COMPAREFARBYTETOFARBYTE 0x20 //C,D fb(4) fb(4)
#define CMD_COMPARE                 0x21 //C,D var(2) val(2)
#define CMD_CALLASM                 0x23 //C,D addr(4)
#define CMD_SPECIAL                 0x25 //C,D type(2)
#define CMD_SPECIAL2                0x26 //C,D var(2) type(2)
#define CMD_WAITSPECIAL             0x27 //C,D
// NamePokemon fadescreen 1 special 0x9E waitspecial
// Don't decompile this. Put it in code    C,X
//   cleaner program. 0x8004 is number in party. (eg. 0 is the 1st poke, 1 is 2nd etc)
#define CMD_PAUSE                   0x28 //C,D time(2)
#define CMD_SETFLAG                 0x29 //C,D flag(2)
#define CMD_CLEARFLAG               0x2A //C,D flag(2)
#define CMD_CHECKFLAG               0x2B //C,D flag(2)
#define CMD_2F                      0x2F //?,? ??(2)
#define CMD_CRY                     0x30 //C,D bank(1) poke(2)
#define CMD_FANFARE                 0x31 //C,D snd(2)
#define CMD_WAITFANFARE             0x32 //C,D
#define CMD_PLAYSOUND               0x33 //C,D snd(2)
#define CMD_FADEDEFAULT             0x34 //C,D
#define CMD_FADESOUND               0x35 //C,D snd(2)
#define CMD_FADEOUT                 0x37 //C,D
#define CMD_FADEIN                  0x38 //C,D
#define CMD_WARP                    0x39 //C,D bank map exit - ALL 1 BYTE
#define CMD_COUNTPOKEMON            0x43 //C,D
#define CMD_REMOVEITEM              0x45 //C,D item(2) num(2)
#define CMD_CHECKITEM               0x47 //C,D item(2)
#define CMD_APPLYMOVEMENT           0x4F //C,D spr(2) addr(4)
#define CMD_PAUSEEVENT              0x51 //C,D event(2)
#define CMD_DISAPPEAR               0x53 //C,D sprite(2)
#define CMD_FACEPLAYER              0x5A //C,D
#define CMD_TRAINERBATTLE           0x5C //C,D kind[0x0==normal,0x4==double](1) num(2) ???(2) start(4) defeat(4)
#define CMD_SHOWMSG                 0x66 //C,D
#define CMD_MESSAGE                 0x67 //C,D addr(4)
#define CMD_CLOSEMSG2               0x68 //C,D
#define CMD_JINGLE                  0x69 //C,D
#define CMD_LOCK                    0x6A //C,D
#define CMD_CLOSEMSG                0x6B //C,D
#define CMD_RELEASE                 0x6C //C,D
#define CMD_WAITBUTTON              0x6D //C,D
#define CMD_MULTICHOICE             0x6F //C,D x(1) y(1) list(1) AbleToCancel(1)
#define CMD_73                      0x73 //?,? ?(2) ?(2)
#define CMD_PICTURE                 0x77 //C,D num(1)
#define CMD_BRAILLE                 0x78 //C,D addr(4)
#define CMD_ADDPOKEMON              0x79 //C,D poke(2) lvl(1) item(2)
// GivePokemon copyvar 0x8004 0x800d addpokemon X Y Z hashpoke 0x1901 0x0
// SelectPokemon special 0x73 copyvar 0x800a 0x8004
// WildBattle Battle X Y 0x0000 special 0x(143|137|138|139)
// GiveItem copyvarifnotzero 0x8000 X copyvarifnotzero 0x8001 Y
// ... callstd 0x(00|01)
#define CMD_GIVEEGG                 0x7A //C,D poke(2)
#define CMD_HASHPOKE                0x7D //C,D (2)(1)
#define CMD_STOREITEM               0x80 //C,D TXT_VAR(1) ITM(2)
#define CMD_STOREFURNITURE          0x81 //C,D TXT_VAR(1) ITM(2)
#define CMD_STOREATK                0x82 //C,D TXT_VAR(1) ATK(2)
#define CMD_STOREVAR                0x83 //C,D TXT_VAR(1) VAR(2)
#define CMD_STORECOMP               0x84 //C,D TXT_VAR(1) COMP(2)
#define CMD_STORETEXT               0x85 //C,D TXT_VAR(1) TXT(4)
#define CMD_POKEMART                0x86 //C,D mart(4)
#define CMD_SLOTMACHINE             0x89 //C,D ??(2)
#define CMD_FADESCREEN              0x97 //C,D blk(1)
#define CMD_9D                      0x9D //?,? (2)(1) or (1)(2)??
#define CMD_CHECKGENDER             0xA0 //C,D
#define CMD_SETMAPTILE              0xA2 //C,D X(2) Y(2) tile(2) attr(2)
#define CMD_SETWEATHER              0xA4 //C,D weather(1)
#define CMD_DOWEATHER               0xA5 //C,D
#define CMD_BATTLE                  0xB6 //C,D poke(2) lvl(1) item(2)
#define CMD_C3                      0xC3 //?,? ??????(1)
#define CMD_D3                      0xD3 //?,? Braille(4)
#define CMD_FB                      0xFB //?,? ADDR4?

/*
storetext Stores text (argument 2) into text-var (\v\hXX+2) where XX is argument 1.
storevar  Converts the var (argument 2) into text-var (\v\hXX+2) where XX is argument 1.
storecomp Stores a competition name (eg.  0x4 == TOUGH)
*/
