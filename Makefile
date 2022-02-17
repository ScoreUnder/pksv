.POSIX:
# ... But don't try to run under pdpmake, it doesn't like substitution on non-straightforward macros
# https://github.com/rmyorston/pdpmake/issues/1

PLATFORM = any   # set to "win" to enable more windows specific stuff
TOOLCHAIN_PREFIX =
EXE_EXT =
SHLIB_EXT = .so
CC = $(TOOLCHAIN_PREFIX)gcc
WINDRES = $(TOOLCHAIN_PREFIX)windres
GPERF = gperf
CMAKE = cmake
TOOL_WRAPPER =  # Might be wine

LIB_FMEM_BASE = lib/fmem
LIB_FMEM = $(LIB_FMEM_BASE)/build
LIB_FMEM_A = $(LIB_FMEM)/libfmem.a

LIBS_PKSVUI_P_win = -lcomdlg32 -lgdi32 -lwsock32 -lcomctl32
LIBS_PKSVUI = $(LIBS_PKSVUI_P_$(PLATFORM))

CPPFLAGS = -I$(LIB_FMEM)/gen -Isrc_common
CFLAGS = -Os -ggdb -Wall -Wextra -Wpedantic -pedantic
CFLAGS_SH = -shared -fpic -DDLL
LDFLAGS_CONSOLE =
LDFLAGS =
LDFLAGS_SH = -L$(LIB_FMEM)
LDLIBS_SH = -lfmem

LINK.c = $(CC) $(CFLAGS) $(LDFLAGS)

SRC_PKSV_COMMON = \
	src_pksv/codeproc.c src_pksv/decompiler.c src_pksv/gba_asm.c src_pksv/isdone.c \
	src_pksv/recompiler.c src_pksv/sulib.c src_pksv/textproc.c src_common/binarysearch.c \
	src_pksv/romutil.c \
	src_pksv/sublang/gsc_moves.c src_pksv/sublang/gsc_moves_reverse.c
SRC_PKSV_MAIN = src_pksv/pksv2.c $(SRC_PKSV_COMMON)
SRC_PKSV_SHLIB = src_pksv/pksv_dll.c $(SRC_PKSV_COMMON)
RES_PKSV_SHLIB = src_pksv/pksv.rc

SRC_PKSVUI = src_pksvui/pksvui.c
RES_PKSVUI = src_pksvui/vcpksv2.rc

SRC_PROCESS_DEFINES = tools/process-defines.c src_common/binarysearch.c
SRC_GPERF_REVERSE = tools/gperf-but-in-reverse.c
SRC_PROCESS_DEFINES_REVERSE = tools/process-defines-reverse.c

GENERATED_SOURCES = pksv/sublang/gsc_moves.c pksv/sublang/gsc_moves_reverse.c

OBJ_PKSVUI_P_win = $(RES_PKSVUI:.rc=.o)
OBJ_PKSVUI = $(SRC_PKSVUI:.c=.o) $(OBJ_PKSVUI_P_$(PLATFORM))

OBJ_PKSV_MAIN = $(SRC_PKSV_MAIN:.c=.o)
OBJ_PKSV_SHLIB = $(SRC_PKSV_SHLIB:.c=.sh_o) $(RES_PKSV_SHLIB:.rc=.o)
OBJ_PROCESS_DEFINES = $(SRC_PROCESS_DEFINES:.c=.o)
OBJ_GPERF_REVERSE = $(SRC_GPERF_REVERSE:.c=.o)
OBJ_PROCESS_DEFINES_REVERSE = $(SRC_PROCESS_DEFINES_REVERSE:.c=.o)

BIN_PROCESS_DEFINES = tools/process-defines$(EXE_EXT)
BIN_GPERF_REVERSE = tools/gperf-but-in-reverse$(EXE_EXT)
BIN_PROCESS_DEFINES_REVERSE = tools/process-defines-reverse$(EXE_EXT)

DEPS = $(OBJ_PKSV_MAIN:.o=.d) $(OBJ_PKSV_SHLIB:o=d) $(OBJ_PROCESS_DEFINES:.o=.d) $(OBJ_GPERF_REVERSE:.o=.d)

PKSV = pksv$(EXE_EXT)
PKSV_SHLIB = pksv$(SHLIB_EXT)
PKSVUI = pksvui$(EXE_EXT)

all: $(PKSV) $(PKSV_SHLIB) $(PKSVUI) defines.dat Scintilla.dll

check: $(PKSV) defines.dat
	bunzip2 -fkq src_pksv/tests/fakerom.gba.bz2
	./$(PKSV) -r src_pksv/tests/test.pks src_pksv/tests/fakerom.gba
	./$(PKSV) -d src_pksv/tests/fakerom.gba 6B09F8 src_pksv/tests/compare.pks
	./$(PKSV) -d src_pksv/tests/fakerom.gba 6B0A01 src_pksv/tests/compare2.pks
	./$(PKSV) -d src_pksv/tests/fakerom.gba 6B0A13 src_pksv/tests/compare3.pks
	echo 'f183692e033935c95d085039069a5f64  src_pksv/tests/fakerom.gba' | md5sum -c
	dd if=/dev/zero of=src_pksv/tests/fakegold.gbc bs=1024 count=256
	./$(PKSV) -r src_pksv/tests/test_gsc_moves.pks src_pksv/tests/fakegold.gbc
	echo '6146a2f980bcaacc6ae89ef89813b115  src_pksv/tests/fakegold.gbc' | md5sum -c

clean: mostlyclean
	rm -f -- $(PKSV) $(PKSV_SHLIB) $(BIN_PROCESS_DEFINES) $(BIN_GPERF_REVERSE) $(BIN_PROCESS_DEFINES_REVERSE) defines.dat Scintilla.dll

mostlyclean: clean-fmem
	rm -f -- $(OBJ_PKSV_MAIN) $(OBJ_PKSV_SHLIB) $(OBJ_PROCESS_DEFINES) $(OBJ_GPERF_REVERSE) $(OBJ_PROCESS_DEFINES_REVERSE) $(DEPS) $(GENERATED_SOURCES) $(OBJ_PKSVUI) src_pksv/tests/fakerom.gba src_pksv/tests/fakegold.gbc PokeScrE.log

clean-fmem:
	rm -rf -- $(LIB_FMEM) $(LIB_FMEM_A)

# Lets us run pksvui from the root of the source tree
Scintilla.dll: src_pksvui/Scintilla.dll
	ln -s src_pksvui/Scintilla.dll Scintilla.dll

$(LIB_FMEM)/gen/fmem.h $(LIB_FMEM_A): $(LIB_FMEM)/Makefile
	$(MAKE) -C $(LIB_FMEM) VERBOSE=1

$(LIB_FMEM)/Makefile: $(LIB_FMEM_BASE)/CMakeLists.txt
	mkdir -p $(LIB_FMEM)
	cd $(LIB_FMEM) && $(CMAKE) -DBUILD_TESTING=false ..

$(LIB_FMEM_BASE)/CMakeLists.txt:
	# Git repo not checked out properly
	git submodule update --init --force --recursive

defines.dat: src_pksv/pokeinc_default.txt $(BIN_PROCESS_DEFINES)
	$(TOOL_WRAPPER) $(BIN_PROCESS_DEFINES) src_pksv/pokeinc_default.txt

$(BIN_PROCESS_DEFINES): $(OBJ_PROCESS_DEFINES)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_PROCESS_DEFINES) -o $@

$(BIN_GPERF_REVERSE): $(OBJ_GPERF_REVERSE)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_GPERF_REVERSE) -o $@

$(BIN_PROCESS_DEFINES_REVERSE): $(OBJ_PROCESS_DEFINES_REVERSE)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_PROCESS_DEFINES_REVERSE) -o $@

$(PKSV): $(OBJ_PKSV_MAIN)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(CFLAGS) $(OBJ_PKSV_MAIN) -o $@

$(PKSV_SHLIB): $(OBJ_PKSV_SHLIB) $(LIB_FMEM_A)
	$(LINK.c) $(CFLAGS_SH) $(LDFLAGS_SH) $(OBJ_PKSV_SHLIB) $(LDLIBS_SH) -o $@

$(PKSVUI): $(OBJ_PKSVUI)
	$(LINK.c) $(OBJ_PKSVUI) $(LIBS_PKSVUI) -o $@

src_pksv/sublang/gsc_moves_reverse.c: src_pksv/sublang/gsc_moves.gperf $(BIN_GPERF_REVERSE)
	$(TOOL_WRAPPER) $(BIN_GPERF_REVERSE) < src_pksv/sublang/gsc_moves.gperf > $@ || { rm -f -- $@; false; }

.SUFFIXES: .sh_o .o .c .gperf .rc
.c.sh_o:
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.gperf.c:
	$(GPERF) -C -l -L ANSI-C --output-file=$@ $<

.rc.o:
	$(WINDRES) -o $@ $<

.PHONY: all check clean clean-fmem

src_pksv/pksv_dll.c: $(LIB_FMEM)/gen/fmem.h

-include $(DEPS)
