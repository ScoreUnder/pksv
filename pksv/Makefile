.POSIX:

TOOLCHAIN_PREFIX =
EXE_EXT =
SHLIB_EXT = .so
CC = $(TOOLCHAIN_PREFIX)gcc

CFLAGS = -Os -ggdb
LDFLAGS =

SRC_MAIN = pksv2.c
SRC_SHLIB = pksv_dll.c

OBJ_MAIN = $(SRC_MAIN:.c=.o)
OBJ_SHLIB = $(SRC_SHLIB:.c=.o)

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
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ_SHLIB) -shared -o $@

.PHONY: all check clean
