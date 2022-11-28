#define test 0x86B0D88 'FREESPACEA0
#org 0x6B09F8
cmd_c7 1
msgbox test
callstd msg_normal
end
''''
setflag 0x807
setflag 0x808
'call 0x81a01c0
givetocoincase 0x0100 'Stock up!
setvar 0x8004 0x33
special 0xA2
end
lock
faceplayer
copyvarifnotzero 0x8000 0x1
copyvarifnotzero 0x8001 0x1
callstd MSG_OBTAIN
battle 0x195 0x1 0x1
special 0x143
waitspecial
storevar 0x0 2
storevar 0x1 3
storevar 0x2 4
storevar 0x3 5
storevar 0x4 6
storevar 0x5 7
storevar 0x6 8
storevar 0x7 9
checkflag 0xE000
if 0x1 jump 0x6b0C50
jump 0x6b0C30

#org 0x6B0B00
= Dude, where's my \v\h02?\p\v\h03\p\v\h04\p\v\h05\p\v\h06\p\v\h07\p\v\h08\p\v\h09...

#org 0x6b0C00
= car

#org 0x6b0C10
= burrito of doom?!

#org 0x6b0c30
setflag 0xE000
storetext 0x0 0x86b0C00 'storetext 0x0 ADDR means store ADDR into \v\h02.
                        'likewise, storetext 0x1 ADDR means store ADDR into \v\h03.
                        'storetext 0x50 ADDR means store ADDR into \v\h52.
                        'Get it?
jump 0x6b0d00

#org 0x6b0c50
clearflag 0xE000
#raw 0x9D
nop
nop
'cmd_84 0x0 0x1 Dude wheres my beauty
'0x90 returned 1...... 5 arg
'storetext 0x0 0x86b0c10
storevar 0x1 LASTRESULT
jump 0x6b0d00

#org 0x6b0d00
message 0x6B0B00
showmsg
waitbutton
closemsg
release
end

#org 0x6b0d50
= I Score Under.

#org test
= Testing

#erase 0x6b0d90 0x6c0000
#org @deterministic-erased-area
setvar 0x8000 0
special NAME_POKEMON
return

#org @deterministic-2
msgbox test
callstd MSG_LOCK
call @deterministic-erased-area
end
#raw 0x101 ' Should warn about an oversized byte
#raw -1 ' Should not warn
