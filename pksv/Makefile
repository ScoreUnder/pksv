.POSIX:
# ... But don't try to run under pdpmake, it doesn't like substitution on non-straightforward macros

TOOLCHAIN_PREFIX =
EXE_EXT =
SHLIB_EXT = .so
CC = $(TOOLCHAIN_PREFIX)gcc

CPPFLAGS =
CFLAGS = -Os -ggdb -Wall -Wextra -Wpedantic -pedantic
CFLAGS_SH = -shared -fpic
LDFLAGS =

SRC_COMMON = codeproc.c decompiler.c gba_asm.c isdone.c recompiler.c sulib.c textproc.c
SRC_MAIN = pksv2.c $(SRC_COMMON)
SRC_SHLIB = pksv_dll.c $(SRC_COMMON)

OBJ_MAIN = $(SRC_MAIN:.c=.o)
OBJ_SHLIB = $(SRC_SHLIB:.c=.sh_o)

DEPS = $(OBJ_MAIN:.o=.d) $(OBJ_SHLIB:.sh_o=.sh_d)

MAIN = pksv$(EXE_EXT)
SHLIB = pksv$(SHLIB_EXT)

all: $(MAIN) $(SHLIB)

check: $(MAIN)
	bunzip2 -fkq tests/fakerom.gba.bz2
	./$(MAIN) -r tests/test.pks tests/fakerom.gba
	./$(MAIN) tests/fakerom.gba 6B09F8

clean: mostlyclean
	rm -f -- $(MAIN) $(SHLIB)

mostlyclean:
	rm -f -- $(OBJ_MAIN) $(OBJ_SHLIB) PokeScrE.log

$(MAIN): $(OBJ_MAIN)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ_MAIN) -o $@

$(SHLIB): $(OBJ_SHLIB)
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(LDFLAGS) $(OBJ_SHLIB) -o $@

.SUFFIXES: .sh_o .o .c
.c.sh_o:
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.PHONY: all check clean

-include $(DEPS)
