.POSIX:
# ... But don't try to run under pdpmake, it doesn't like substitution on non-straightforward macros

TOOLCHAIN_PREFIX =
EXE_EXT =
SHLIB_EXT = .so
CC = $(TOOLCHAIN_PREFIX)gcc
GPERF = gperf

CPPFLAGS =
CFLAGS = -Os -ggdb -Wall -Wextra -Wpedantic -pedantic
CFLAGS_SH = -shared -fpic
LDFLAGS =

LINK.c = $(CC) $(CFLAGS) $(LDFLAGS)

SRC_COMMON = codeproc.c decompiler.c gba_asm.c isdone.c recompiler.c sulib.c textproc.c binarysearch.c sublang/gsc_moves.c
SRC_MAIN = pksv2.c $(SRC_COMMON)
SRC_SHLIB = pksv_dll.c $(SRC_COMMON)
SRC_PROCESS_DEFINES = tools/process-defines.c binarysearch.c

OBJ_MAIN = $(SRC_MAIN:.c=.o)
OBJ_SHLIB = $(SRC_SHLIB:.c=.sh_o)
OBJ_PROCESS_DEFINES = $(SRC_PROCESS_DEFINES:.c=.o)

DEPS = $(OBJ_MAIN:.o=.d) $(OBJ_SHLIB:.sh_o=.sh_d) $(OBJ_PROCESS_DEFINES:.o=.d)

MAIN = pksv$(EXE_EXT)
SHLIB = pksv$(SHLIB_EXT)

all: $(MAIN) $(SHLIB) defines.dat

check: $(MAIN) defines.dat
	bunzip2 -fkq tests/fakerom.gba.bz2
	./$(MAIN) -r tests/test.pks tests/fakerom.gba
	./$(MAIN) -d tests/fakerom.gba 6B09F8 tests/compare.pks
	./$(MAIN) -d tests/fakerom.gba 6B0A01 tests/compare2.pks
	./$(MAIN) -d tests/fakerom.gba 6B0A13 tests/compare3.pks
	echo 'f183692e033935c95d085039069a5f64  tests/fakerom.gba' | md5sum -c
	dd if=/dev/zero of=tests/fakegold.gbc bs=1024 count=256
	./$(MAIN) -r tests/test_gsc_moves.pks tests/fakegold.gbc
	echo '6146a2f980bcaacc6ae89ef89813b115  tests/fakegold.gbc' | md5sum -c

clean: mostlyclean
	rm -f -- $(MAIN) $(SHLIB) tools/process-defines defines.dat

mostlyclean:
	rm -f -- $(OBJ_MAIN) $(OBJ_SHLIB) $(OBJ_PROCESS_DEFINES) $(DEPS) tests/fakerom.gba tests/fakegold.gbc PokeScrE.log

defines.dat: pokeinc_default.txt tools/process-defines
	tools/process-defines

tools/process-defines: $(OBJ_PROCESS_DEFINES)
	$(LINK.c) $(OBJ_PROCESS_DEFINES) -o $@

$(MAIN): $(OBJ_MAIN)
	$(LINK.c) $(OBJ_MAIN) -o $@

$(SHLIB): $(OBJ_SHLIB)
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(LDFLAGS) $(OBJ_SHLIB) -o $@

.SUFFIXES: .sh_o .o .c .gperf
.c.sh_o:
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.gperf.c:
	$(GPERF) -C -l -L ANSI-C --output-file=$@ $<

.PHONY: all check clean

-include $(DEPS)
