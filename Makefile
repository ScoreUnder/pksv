# set to "release" to enable updating, show version, etc.
PROFILE = debug
# set to "win" to enable more windows specific stuff
PLATFORM = any
TOOLCHAIN_PREFIX =

EXE_EXT_P_win = .exe
SHLIB_EXT_P_win = .dll
SHLIB_EXT_P_mac = .dylib
SHLIB_EXT_P_lin = .so
SHLIB_EXT_P_any = .so

EXE_EXT = $(EXE_EXT_P_$(PLATFORM))
SHLIB_EXT = $(SHLIB_EXT_P_$(PLATFORM))

CC = $(TOOLCHAIN_PREFIX)gcc
WINDRES = $(TOOLCHAIN_PREFIX)windres
GPERF = gperf
CMAKE = cmake
LEX = lex
YACC = yacc
TOOL_WRAPPER =  # Might be wine
ZIP = 7z a -tzip

LIB_FMEM_BASE = lib/fmem
LIB_FMEM = $(LIB_FMEM_BASE)/build
LIB_FMEM_A = $(LIB_FMEM)/libfmem.a

LIBS_PKSVUI_P_win_PR_release = -lwsock32
LIBS_PKSVUI_P_win = -lcomdlg32 -lgdi32 -lcomctl32
LIBS_PKSVUI = $(LIBS_PKSVUI_P_$(PLATFORM)) $(LIBS_PKSVUI_P_$(PLATFORM)_PR_$(PROFILE))

CPPFLAGS_PR_debug = -DDOES_NOT_UPDATE=1
CPPFLAGS_PR_release = -DNDEBUG=1
CPPFLAGS = -I$(LIB_FMEM)/gen -Isrc_common -Isrc_lang -D_FILE_OFFSET_BITS=64 $(CPPFLAGS_PR_$(PROFILE))
CFLAGS_PR_debug = -ggdb3 -Og
CFLAGS_PR_release = -ggdb1 -flto -O2
CFLAGS_PGO_1=-fprofile-generate=pgo_data
CFLAGS_PGO_2=-fprofile-use=pgo_data
CFLAGS = -ggdb -Wall -Wextra -Wpedantic -pedantic -std=c17 $(CFLAGS_PR_$(PROFILE)) $(CFLAGS_PGO_$(PGO))
CFLAGS_SH = -shared -fpic -DDLL

LDFLAGS_CONSOLE_P_win = -mconsole
LDFLAGS_P_win = -mwindows

LDFLAGS_CONSOLE = $(LDFLAGS_CONSOLE_P_$(PLATFORM))
LDFLAGS = $(LDFLAGS_P_$(PLATFORM))
LDFLAGS_SH = -L$(LIB_FMEM)
LDLIBS_SH = -lfmem

SRC_PKSV_COMMON = \
	src_pksv/codeproc.c src_pksv/gba_asm.c src_pksv/isdone.c \
	src_pksv/recompiler.c src_pksv/sulib.c src_pksv/textproc.c src_common/binarysearch.c \
	src_pksv/romutil.c src_common/textutil.c src_pksv/uint32_interval.c \
	src_pksv/sublang/gsc_moves.c src_pksv/sublang/gsc_moves_reverse.c \
	src_pksv/sublang/frlg_moves.c src_pksv/sublang/frlg_moves_reverse.c \
	src_pksv/sublang/rse_moves.c src_pksv/sublang/rse_moves_reverse.c \
	src_pksv/lang_parsers.c src_common/stdio_ext.c src_pksv/lang_default_parsers.c \
	src_pksv/lang_decompiler.c src_lang/lang_load.c src_common/binarysearch_u32.c
SRC_PKSV_MAIN = src_pksv/pksv2.c $(SRC_PKSV_COMMON)
SRC_PKSV_SHLIB = src_pksv/pksv_dll.c $(SRC_PKSV_COMMON)
RES_PKSV_SHLIB = src_pksv/pksv.rc

SRC_PKSVUI = src_pksvui/pksvui.c src_pksvui/windows_portability.c
RES_PKSVUI = src_pksvui/vcpksv2.rc

SRC_PROCESS_DEFINES = \
	tools/process-defines.c src_common/binarysearch.c src_common/stdio_ext.c \
	src_common/textutil.c src_common/binarysearch_u32.c
SRC_GPERF_REVERSE = tools/gperf-but-in-reverse.c
SRC_PROCESS_DEFINES_REVERSE = tools/process-defines-reverse.c src_common/stdio_ext.c
SRC_LANGUAGE_PARSER = \
	tools/language_parser/language-def-lex.c tools/language_parser/language-def-yacc.tab.c \
	src_common/binarysearch.c src_common/stdio_ext.c src_common/textutil.c
SRC_LANGUAGE_DUMPER = \
	tools/language-dat-dumper.c src_common/binarysearch.c src_lang/lang_load.c \
	src_common/stdio_ext.c src_common/textutil.c

GENERATED_SOURCES = \
	src_pksv/sublang/gsc_moves.c src_pksv/sublang/gsc_moves_reverse.c \
	src_pksv/sublang/frlg_moves.c src_pksv/sublang/frlg_moves_reverse.c \
	src_pksv/sublang/rse_moves.c src_pksv/sublang/rse_moves_reverse.c \
	tools/language_parser/language-def-lex.c tools/language_parser/language-def-yacc.tab.c \
	tools/language_parser/language-def-yacc.tab.h

OBJ_PKSVUI_P_win = $(RES_PKSVUI:.rc=.o)
OBJ_PKSVUI = $(SRC_PKSVUI:.c=.o) $(OBJ_PKSVUI_P_$(PLATFORM))

OBJ_PKSV_MAIN = $(SRC_PKSV_MAIN:.c=.o)
OBJ_PKSV_SHLIB = $(SRC_PKSV_SHLIB:.c=.sh_o) $(RES_PKSV_SHLIB:.rc=.o)
OBJ_PROCESS_DEFINES = $(SRC_PROCESS_DEFINES:.c=.o)
OBJ_GPERF_REVERSE = $(SRC_GPERF_REVERSE:.c=.o)
OBJ_PROCESS_DEFINES_REVERSE = $(SRC_PROCESS_DEFINES_REVERSE:.c=.o)
OBJ_LANGUAGE_PARSER = $(SRC_LANGUAGE_PARSER:.c=.o)
OBJ_LANGUAGE_DUMPER = $(SRC_LANGUAGE_DUMPER:.c=.o)

BIN_PROCESS_DEFINES = tools/process-defines$(EXE_EXT)
BIN_GPERF_REVERSE = tools/gperf-but-in-reverse$(EXE_EXT)
BIN_PROCESS_DEFINES_REVERSE = tools/process-defines-reverse$(EXE_EXT)
BIN_LANGUAGE_PARSER = tools/language_parser/language-parser$(EXE_EXT)
BIN_LANGUAGE_DUMPER = tools/language-dat-dumper$(EXE_EXT)
ALL_TOOLS = $(BIN_PROCESS_DEFINES) $(BIN_GPERF_REVERSE) $(BIN_PROCESS_DEFINES_REVERSE) $(BIN_LANGUAGE_PARSER) $(BIN_LANGUAGE_DUMPER)

SUBLANGS_SRC = $(wildcard src_pksv/sublang/lang_*.lang.txt)
SUBLANGS = $(subst src_pksv/,,$(SUBLANGS_SRC:.lang.txt=.dat))

SUBLANG_DEFS_SRC = $(wildcard src_pksv/sublang/defs_*.defs.txt)
SUBLANG_DEFS = $(subst src_pksv/,,$(SUBLANG_DEFS_SRC:.defs.txt=.dat))

OBJS = $(OBJ_PKSV_MAIN) $(OBJ_PKSV_SHLIB) $(OBJ_PKSVUI) $(OBJ_PROCESS_DEFINES) $(OBJ_PROCESS_DEFINES_REVERSE) $(OBJ_GPERF_REVERSE) $(OBJ_LANGUAGE_PARSER)
DEPS = $(OBJS:o=d)

PKSV = pksv$(EXE_EXT)
PKSV_SHLIB = pksv$(SHLIB_EXT)
PKSVUI = pksvui$(EXE_EXT)

DIST_FILES_P_win = $(PKSV_SHLIB) $(PKSVUI) Scintilla.dll
DIST_FILES = $(PKSV) $(SUBLANGS) $(SUBLANG_DEFS) license.txt $(ALL_TOOLS) $(DIST_FILES_P_$(PLATFORM))
DIST_OUT = pksv-$(PLATFORM)-"$$(git describe --long --dirty --always)".zip
DIST_OUT_WC = pksv-*.zip

all: $(PKSV) $(PKSV_SHLIB) $(PKSVUI) Scintilla.dll $(BIN_LANGUAGE_PARSER) $(SUBLANGS) $(SUBLANG_DEFS)
compat: $(PKSV) $(BIN_LANGUAGE_PARSER) $(SUBLANGS) $(SUBLANG_DEFS)
tools: $(BIN_LANGUAGE_PARSER) $(BIN_LANGUAGE_DUMPER) $(BIN_PROCESS_DEFINES) $(BIN_PROCESS_DEFINES_REVERSE)

check: $(PKSV) $(SUBLANGS) $(SUBLANG_DEFS)
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
	rm -f -- $(PKSV) $(PKSV_SHLIB) $(PKSVUI) $(BIN_PROCESS_DEFINES) $(BIN_GPERF_REVERSE) $(BIN_PROCESS_DEFINES_REVERSE) $(BIN_LANGUAGE_PARSER) $(BIN_LANGUAGE_DUMPER) $(DIST_OUT_WC) Scintilla.dll $(SUBLANGS) $(SUBLANG_DEFS) pgo_data

mostlyclean: clean-fmem
	rm -f -- $(OBJS) $(DEPS) $(GENERATED_SOURCES) src_pksv/tests/fakerom.gba src_pksv/tests/fakegold.gbc PokeScrE.log
	rm -f -- $(OBJS:o=gcno) $(OBJS:o=gcda)

clean-fmem:
	rm -rf -- $(LIB_FMEM) $(LIB_FMEM_A)

dist: $(DIST_FILES)
	rm -f -- $(DIST_OUT_WC)
	$(ZIP) $(DIST_OUT) $(DIST_FILES)
	torrentzip $(DIST_OUT)
	rm -v \[*\]_\[*\].log  # Torrentzip output

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

$(BIN_PROCESS_DEFINES): $(OBJ_PROCESS_DEFINES)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_PROCESS_DEFINES) -o $@

$(BIN_GPERF_REVERSE): $(OBJ_GPERF_REVERSE)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_GPERF_REVERSE) -o $@

$(BIN_PROCESS_DEFINES_REVERSE): $(OBJ_PROCESS_DEFINES_REVERSE)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_PROCESS_DEFINES_REVERSE) -o $@

$(BIN_LANGUAGE_PARSER): $(OBJ_LANGUAGE_PARSER)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_LANGUAGE_PARSER) -o $@

$(BIN_LANGUAGE_DUMPER): $(OBJ_LANGUAGE_DUMPER)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_LANGUAGE_DUMPER) -o $@

$(PKSV): $(OBJ_PKSV_MAIN)
	$(LINK.c) $(LDFLAGS_CONSOLE) $(OBJ_PKSV_MAIN) -o $@

$(PKSV_SHLIB): $(OBJ_PKSV_SHLIB) $(LIB_FMEM_A)
	$(LINK.c) $(CFLAGS_SH) $(LDFLAGS_SH) $(OBJ_PKSV_SHLIB) $(LDLIBS_SH) -o $@

$(PKSVUI): $(OBJ_PKSVUI)
	$(LINK.c) $(OBJ_PKSVUI) $(LIBS_PKSVUI) -o $@

%_reverse.c: %.gperf $(BIN_GPERF_REVERSE)
	$(TOOL_WRAPPER) $(BIN_GPERF_REVERSE) < $< > $@ || { rm -f -- $@; false; }

sublang:
	mkdir -p sublang

sublang/lang_%.dat: src_pksv/sublang/lang_%.lang.txt $(BIN_LANGUAGE_PARSER) | sublang
	$(TOOL_WRAPPER) $(BIN_LANGUAGE_PARSER) $< $@

sublang/defs_%.dat: src_pksv/sublang/defs_%.defs.txt $(BIN_PROCESS_DEFINES) | sublang
	$(TOOL_WRAPPER) $(BIN_PROCESS_DEFINES) $< $@

.SUFFIXES: .sh_o .o .c .gperf .rc .y .l .c .tab.c
.c.sh_o:
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MD -MF $(@:o=d) -c $< -o $@

.gperf.c:
	$(GPERF) -C -l -L ANSI-C --output-file=$@ $<

.rc.o:
	$(WINDRES) $(CPPFLAGS) -o $@ $<

.l.c:
	$(LEX) -t $< > $@ || { rm -f -- $@; false; }

.y.tab.c:
	$(YACC) -d -b $(@D)/$(*F) $<

.PHONY: all check clean clean-fmem compat dist tools

tools/language_parser/language-def-yacc.tab.h: tools/language_parser/language-def-yacc.tab.c
	@:

src_pksv/pksv_dll.c: $(LIB_FMEM)/gen/fmem.h

tools/language_parser/language-def-lex.o: tools/language_parser/language-def-lex.c tools/language_parser/language-def-yacc.tab.h

-include $(DEPS)
