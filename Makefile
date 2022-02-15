.POSIX:
# ... But don't try to run under pdpmake, it doesn't like substitution on non-straightforward macros

TOOLCHAIN_PREFIX =
EXE_EXT =
SHLIB_EXT = .so
CC = $(TOOLCHAIN_PREFIX)gcc
GPERF = gperf
CMAKE = cmake
TOOL_WRAPPER =  # Might be wine

LIB_FMEM = lib/fmem/build
LIB_FMEM_A = $(LIB_FMEM)/libfmem.a

CPPFLAGS = -I$(LIB_FMEM)/gen
CFLAGS = -Os -ggdb -Wall -Wextra -Wpedantic -pedantic
CFLAGS_SH = -shared -fpic -DDLL
LDFLAGS =
LDFLAGS_SH =

LINK.c = $(CC) $(CFLAGS) $(LDFLAGS)

SRC_PKSV_COMMON = \
	pksv/codeproc.c pksv/decompiler.c pksv/gba_asm.c pksv/isdone.c \
	pksv/recompiler.c pksv/sulib.c pksv/textproc.c pksv/binarysearch.c \
	pksv/sublang/gsc_moves.c pksv/sublang/gsc_moves_reverse.c
SRC_PKSV_MAIN = pksv/pksv2.c $(SRC_PKSV_COMMON)
SRC_PKSV_SHLIB = pksv/pksv_dll.c $(SRC_PKSV_COMMON)
SRC_PROCESS_DEFINES = tools/process-defines.c pksv/binarysearch.c
SRC_GPERF_REVERSE = tools/gperf-but-in-reverse.c

OBJ_PKSV_MAIN = $(SRC_PKSV_MAIN:.c=.o)
OBJ_PKSV_SHLIB = $(SRC_PKSV_SHLIB:.c=.sh_o)
OBJ_PROCESS_DEFINES = $(SRC_PROCESS_DEFINES:.c=.o)
OBJ_GPERF_REVERSE = $(SRC_GPERF_REVERSE:.c=.o)

DEPS = $(OBJ_PKSV_MAIN:.o=.d) $(OBJ_PKSV_SHLIB:.sh_o=.sh_d) $(OBJ_PROCESS_DEFINES:.o=.d) $(OBJ_GPERF_REVERSE:.o=.d)

PKSV = pksv$(EXE_EXT)
PKSV_SHLIB = pksv$(SHLIB_EXT)

all: $(PKSV) $(PKSV_SHLIB) defines.dat

check: $(PKSV) defines.dat
	bunzip2 -fkq pksv/tests/fakerom.gba.bz2
	./$(PKSV) -r pksv/tests/test.pks pksv/tests/fakerom.gba
	./$(PKSV) -d pksv/tests/fakerom.gba 6B09F8 pksv/tests/compare.pks
	./$(PKSV) -d pksv/tests/fakerom.gba 6B0A01 pksv/tests/compare2.pks
	./$(PKSV) -d pksv/tests/fakerom.gba 6B0A13 pksv/tests/compare3.pks
	echo 'f183692e033935c95d085039069a5f64  pksv/tests/fakerom.gba' | md5sum -c
	dd if=/dev/zero of=pksv/tests/fakegold.gbc bs=1024 count=256
	./$(PKSV) -r pksv/tests/test_gsc_moves.pks pksv/tests/fakegold.gbc
	echo '6146a2f980bcaacc6ae89ef89813b115  pksv/tests/fakegold.gbc' | md5sum -c

clean: mostlyclean
	rm -f -- $(PKSV) $(PKSV_SHLIB) tools/process-defines$(EXE_EXT) tools/gperf-but-in-reverse$(EXE_EXT) defines.dat

mostlyclean: clean-fmem
	rm -f -- $(OBJ_PKSV_MAIN) $(OBJ_PKSV_SHLIB) $(OBJ_PROCESS_DEFINES) $(OBJ_GPERF_REVERSE) $(DEPS) pksv/tests/fakerom.gba pksv/tests/fakegold.gbc PokeScrE.log

clean-fmem:
	rm -rf -- $(LIB_FMEM) $(LIB_FMEM_A)

$(LIB_FMEM)/gen/fmem.h $(LIB_FMEM_A): $(LIB_FMEM)/Makefile
	$(MAKE) -C $(LIB_FMEM) VERBOSE=1

$(LIB_FMEM)/Makefile:
	mkdir -p $(LIB_FMEM)
	cd $(LIB_FMEM) && $(CMAKE) -DBUILD_TESTING=false ..

defines.dat: pksv/pokeinc_default.txt tools/process-defines$(EXE_EXT)
	$(TOOL_WRAPPER) tools/process-defines$(EXE_EXT) pksv/pokeinc_default.txt

tools/process-defines$(EXE_EXT): $(OBJ_PROCESS_DEFINES)
	$(LINK.c) $(OBJ_PROCESS_DEFINES) -o $@

tools/gperf-but-in-reverse$(EXE_EXT): $(OBJ_GPERF_REVERSE)
	$(LINK.c) $(OBJ_GPERF_REVERSE) -o $@

$(PKSV): $(OBJ_PKSV_MAIN)
	$(LINK.c) $(OBJ_PKSV_MAIN) -o $@

$(PKSV_SHLIB): $(OBJ_PKSV_SHLIB) $(LIB_FMEM_A)
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(LDFLAGS) $(LDFLAGS_SH) $(OBJ_PKSV_SHLIB) -L$(LIB_FMEM) -lfmem -o $@

pksv/sublang/gsc_moves_reverse.c: pksv/sublang/gsc_moves.gperf tools/gperf-but-in-reverse$(EXE_EXT)
	$(TOOL_WRAPPER) tools/gperf-but-in-reverse < pksv/sublang/gsc_moves.gperf > $@ || { rm -f -- $@; false; }

.SUFFIXES: .sh_o .o .c .gperf
.c.sh_o:
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.gperf.c:
	$(GPERF) -C -l -L ANSI-C --output-file=$@ $<

.PHONY: all check clean clean-fmem

pksv/pksv_dll.c: $(LIB_FMEM)/gen/fmem.h

-include $(DEPS)
