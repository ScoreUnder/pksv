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
#define CMD_17                      0x17 //?,? var(2) val(2)
#define CMD_COPYVAR                 0x19 //C,D var(2) val(2)
#define CMD_COPYVARIFNOTZERO        0x1A //C,D var(2) val(2)
#define CMD_COMPAREVARTOBYTE        0x1C //C,D var(2) byte(1)
#define CMD_COMPAREVARTOFARBYTE     0x1D //C,D var(2) addr(4)
#define CMD_COMPAREFARBYTETOVAR     0x1E //C,D addr(4) var(2)
#define CMD_COMPAREFARBYTETOBYTE    0x1F //C,D addr(4) byte(1)
#define CMD_COMPAREFARBYTETOFARBYTE 0x20 //C,D fb(4) fb(4)
#define CMD_COMPARE                 0x21 //C,D var(2) val(2)
#define CMD_22                      0x22 //?,? var(2) val(2)
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
#define CMD_44                      0x44 //?,? (2) (2)
#define CMD_REMOVEITEM              0x45 //C,D item(2) num(2)
#define CMD_CHECKITEM               0x47 //C,D item(2) num(2)
#define CMD_APPLYMOVEMENT           0x4F //C,D spr(2) addr(4)
#define CMD_PAUSEEVENT              0x51 //C,D event(2)
#define CMD_DISAPPEAR               0x53 //C,D sprite(2)
#define CMD_FACEPLAYER              0x5A //C,D
#define CMD_TRAINERBATTLE           0x5C //C,D kind[0x0==normal,0x4==double](1) num(2) ???(2) start(4) defeat(4)
#define CMD_60                      0x60 //?,? (2)
#define CMD_61                      0x61 //?,? (2)
#define CMD_63                      0x63 //?,? (2)(2)(2)
#define CMD_65                      0x65 //?,? (2)(2)
#define CMD_SHOWMSG                 0x66 //C,D
#define CMD_MESSAGE                 0x67 //C,D addr(4)
#define CMD_CLOSEMSG2               0x68 //C,D
#define CMD_JINGLE                  0x69 //C,D
#define CMD_LOCK                    0x6A //C,D
#define CMD_CLOSEMSG                0x6B //C,D
#define CMD_RELEASE                 0x6C //C,D
#define CMD_WAITBUTTON              0x6D //C,D
#define CMD_MULTICHOICE             0x6F //C,D x(1) y(1) list(1) AbleToCancel(1)
#define CMD_MULTICHOICE2            0x70 //C,D x(1) y(1) list(1) ??(2)
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
#define CMD_STOREPOKEMON            0x7D //C,D TXT_VAR(1) POKE(2)
#define CMD_STOREITEM               0x80 //C,D TXT_VAR(1) ITM(2)
#define CMD_STOREFURNITURE          0x81 //C,D TXT_VAR(1) ITM(2)
#define CMD_STOREATK                0x82 //C,D TXT_VAR(1) ATK(2)
#define CMD_STOREVAR                0x83 //C,D TXT_VAR(1) VAR(2)
#define CMD_STORECOMP               0x84 //C,D TXT_VAR(1) COMP(2)
#define CMD_STORETEXT               0x85 //C,D TXT_VAR(1) TXT(4)
#define CMD_POKEMART                0x86 //C,D mart(4)
#define CMD_FAKEJUMPSTD             0x88 //C,D FR type(1)Does not affect actual gameplay
#define CMD_FAKECALLSTD             0x89 //C,D FR type(1)Ditto
#define CMD_SLOTMACHINE             0x89 //C,D RS ??(2)
#define CMD_91                      0x91 //?,? (1)(2)(2)
#define CMD_92                      0x92 //?,? (1)(2)(2)
#define CMD_93                      0x93 //?,? (2)(1)
#define CMD_94                      0x94 //?,? (2)
#define CMD_95                      0x95 //?,? (1)
#define CMD_96                      0x96 //?,? (2)
#define CMD_FADESCREEN              0x97 //C,D blk(1)
#define CMD_99                      0x99 //?,? (2)
#define CMD_9A                      0x9A //?,? (1)
#define CMD_9D                      0x9D //?,? (2)(1) or (1)(2)??
#define CMD_CHECKGENDER             0xA0 //C,D
#define CMD_SETMAPTILE              0xA2 //C,D X(2) Y(2) tile(2) attr(2)
#define CMD_SETWEATHER              0xA4 //C,D weather(1)
#define CMD_DOWEATHER               0xA5 //C,D
#define CMD_COINCASETOVAR           0xB3 //C,D var(2)
#define CMD_GIVETOCOINCASE          0xB4 //C,D coins(2)
#define CMD_BATTLE                  0xB6 //C,D poke(2) lvl(1) item(2)
#define CMD_C0                      0xC0 //?,? (2)
#define CMD_C1                      0xC1 //?,? (2)
#define CMD_C2                      0xC2 //?,? (2)
#define CMD_C3                      0xC3 //?,? ??????(1)
#define CMD_TEXTCOLOR               0xC7 //?,? colour(1)
#define CMD_D3                      0xD3 //?,? Braille(4)
#define CMD_FB                      0xFB //?,? ADDR4?
#define CMD_FE                      0xFE //?,? (1)

/*
nop0 Does Nothing
nop1 Does Nothing
end Stops the script
return Goes back to last "call" statement
call Goes to another portion of the script, which can be returned to
jump Goes to another portion of the script.
if ?? jump ???????? Checks previously compared values, and might jump, depending on what they are.
(That was /so/ hard to explain)
if ?? call ???????? same as the jumping if, except it can be returned to
jumpstd is like "boxset", except it ends the script too.
callstd is "boxset", and is returned to when it finishes.
killscript I'm not sure what this does, but it clears some stuff to do with the script.
setbyte I'm not sure what this is used for, but it sets a byte [somewhere in RAM-find out] to a value.
(You could use it for callasm)
msgbox Loads a message into memory, ready to be displayed by callstd.
loadpointer Loads anything into memory, for various uses.
writebytetooffset writes a byte to an address in memory.
loadbytefrompointer ...er - not sure
copyscriptbanks Copies a bank of scripts from one bank to another. God knows what you could use this for.
copybyte copies one place in RAM to another.
setvar Sets a variable to some value.
cmd_17 I really have no idea.
copyvar Copies one variable to another.
Copyvarifnotzero Copies one variable to another, if the value is not zero, else it does nothing. ( if(b>0)a=b; )
comparevartobyte Compares a variable to a byte. ( var(a)==b )
comparevartofarbyte Compares a variable to a byte at a certain offset.( var(a)==*b )
comparefarbytetovar Exactly the same, the other way round. ( *a==var(b) )
comparefarbytetobyte Compares a byte at an offset to another byte. ( *a==b )
comparefarbytetofarbyte Compares the value of 2 offsets ( *a==*b )
compare Compares a variable to another, or to a number.
cmd_22 No idea
callasm Calls a code in ARM or THUMB format for direct access to the game. The most powerful command.
special Calls a predefined routine
special2 Calls a predefined routine and stores the result in a variable.
waitspecial Waits for either of these commands to complete
(does not decompile correctly in pokescript)
pause Waits a certain amount of time
Setflag Sets a flag to 1 (a flag is a variable that can be only 1 or 0)
clearflag Sets a flag to 0
checkflag Checks if a flag is 1
cmd_2f Seriously, why do nintendo make such cryptic commands. WHAT DOES IT MEAN? What's the significance of MR. Bean?
cry Makes the same sound a pokemon (that you decide) makes when it comes into battle.
fanfare Plays any pre-defined sound
waitfanfare Waits for that command to complete
playsound Plays a pre-defined sound.
fadedefault Fades into the normal bg music.
fadesound Fades into a pre-defined sound.
fadeout Fades out of music
fadein Fades into music
warp Teleports the player
countpokemon Counts the player's party. (0-6) Stored in LASTRESULT
removeitem Ebil-ly steals an item from the player!
checkitem Checks if a player has an item
applymovement Applies movement commands to any sprite.
pauseevent Pauses and waits for an event (usually the end of applymovement.)
disappear Causes a sprite to magically disappear.
faceplayer Causes the sprite that called the script to face the protagonist.
trainerbattle Initiates a ferocious battle with powerful trainers and their tank-like superpokes!
cmd_61 \
cmd_63  |- Don't know
cmd_65 /
showmsg Shows a message (usually stored by MESSAGE)
message Gets ready to show a message
closemsg2 One of the commands to close a message on-screen
(don't you hate it when "you obtained ultra ball" stays on-screen?)
jingle Makes that tinny "clink" sound that happens when you press A while someone is talking.
lock Stops the calling sprite from wandering off.
closemsg Another one of those message-closing commands
release Gives the calling sprite free will again (opposite of lock)
waitbutton Waits for a button to be pressed before continuing.
multichoice Brings up a multiple-choice menu.
multichoice2 The same.
cmd_73 I still don't know
picture Shows a picture.
braille Shows a message in binary-encoded-braille.
addpokemon Adds a pokemon to the player's party.
giveegg Exactly the same, but with an egg.
storepokemon Stores a pokemon's name into a text-var
storeitem The same, but with an item.
storefurniture The same, but with furniture (for secret base)
storeatk The same, but with an attack name (0=="POUND")
storevar The same, but with the value of a variable (eg. 103 or 59, etc)
storecomp Stores a competition name (eg.  0x4 == TOUGH)
storetext Stores text (argument 2) into text-var (\v\hXX+2) where XX is argument 1.
pokemart Opens up a pokemon-mart screen.
slotmachine Whee, fun! It's the slots!
fadescreen Fades in or out.
cmd_91 ¯\
cmd_92  |
cmd_93  |
cmd_94  >- I have no clue. PS, to do overscore, do ALT+0175. Yes, I remember that off the top of my head
cmd_95  |
cmd_96  |
cmd_9D _/
checkgender Checks whether you're a lad or a lass.
setmaptile Sets a tile on the map to something different. May be used for making a tile walkable-across.
setweather Sets a new weather (snow :oD )
doweather Applies the weather change
coincasetovar Copies the amount of coins in the coin case (even if you don't have one) to a variable.
givetocoincase Gives money to a coin-case, even if you don't have one.
battle Stores the variables necessary for having a wild pokemon battle.
cmd_XX Don't know
*/
