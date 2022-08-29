/*
    Sophie Daffern (Score_Under)'s PKSV - Pokemon ROM script viewer/editor
    Copyright (C) 2007  Sophie Daffern

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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>

#include "lang_load.h"
#include "lang_parsers.h"
#include "lang_decompiler.h"
#include "lang_recompiler.h"
#include "pksv.h"
#include "textutil.h"
#include "version.h"
#include "romutil.h"

bool dyndec = false;
int dynplace = 0;
bool VersionOverride = false;
char mode = FIRE_RED;
bool IsVerbose = true;
unsigned char search = 0xFF;  // Free Space character
bool eorg = false;
bool testing = false;
FILE *LogFile;

#define MAX_POSITIONAL_ARGUMENTS 3

void show_help(FILE *where) {
  fprintf(where, "PKSV V" PKSV_VERSION
                 " - Command line tool to compile/decompile PokeScript.\n\
pksv -e ScriptFile.txt RomFile.gba          -- Debug compile\n\
pksv -r ScriptFile.txt RomFile.gba          -- Compile\n\
pksv [-d] RomFile.gba HexOffset [OutputFile.txt] -- Decompile\n\
\n\
Flags:\n\
    --lang LANGUAGE\n\
          Select initial language for decompilation or recompilation\n\
    -d    Decompile\n\
    -r    (Re)compile\n\
    -e    Debug compile (dry-run without writing to ROM)\n\
    -v    Verbose (also affects script decompilation)\n\
");
}

#define LANGUAGE_DIR_NAME "sublang"

int main(int argc, char **argv) {
  char *rom_file_name;
  char *script_file_name;
  FILE *romfile;

  char command_line = DECOMPILE;
  bool autodetect_mode = true;
  uint32_t decompile_at;
  char *decompile_lang;
  bool verbose = false;

  char *positional_arguments[MAX_POSITIONAL_ARGUMENTS];
  int positional_argument_count = 0;
  bool opts_ended = false;

  // n.b. dirname doesn't make a copy of the input, so we do it ourselves
  // because we are not supposed to modify argv
  char *exename = strdup(argv[0]);
  char *exe_dirname = dirname(exename);
  size_t exe_dirlen = strlen(exe_dirname);
  char *language_dir = malloc(exe_dirlen + strlen(LANGUAGE_DIR_NAME) + 2);
  strcpy(language_dir, exe_dirname);
  language_dir[exe_dirlen] = '/';
  strcpy(language_dir + exe_dirlen + 1, LANGUAGE_DIR_NAME);

  free(exename);

  // Argument Processing
  for (int i = 1; i < argc; i++) {
    if (opts_ended || argv[i][0] != '-') {
      if (positional_argument_count < MAX_POSITIONAL_ARGUMENTS) {
        positional_arguments[positional_argument_count++] = argv[i];
      } else {
        fprintf(stderr, "Too many positional arguments.\n");
        show_help(stderr);
        return 1;
      }
    } else if (!strcmp(argv[i], "-r")) {
      command_line = RECOMPILE;
    } else if (!strcmp(argv[i], "-e")) {
      command_line = RECOMPILE;
      testing = 1;
    } else if (!strcmp(argv[i], "-d")) {
      command_line = DECOMPILE;
    } else if (!strcmp(argv[i], "--lang")) {
      if (i + 1 < argc) {
        decompile_lang = argv[++i];
        autodetect_mode = false;
      } else {
        fprintf(stderr, "--lang requires an argument\n");
        show_help(stderr);
        return 1;
      }
    } else if (!strcmp(argv[i], "-v")) {
      verbose = true;
    } else if (!strcmp(argv[i], "--help")) {
      show_help(stdout);
      return 0;
    } else if (!strcmp(argv[i], "--")) {
      opts_ended = true;
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      show_help(stderr);
      return 1;
    }
  }

  if (positional_argument_count < 2) {
    printf("Not enough arguments...\n");
    show_help(stderr);
    return 1;
  }

  // Filename, etc processing
  if (command_line == DECOMPILE) {
    rom_file_name = positional_arguments[0];

    if (*hex_to_uint32(positional_arguments[1], SIZE_MAX, &decompile_at)) {
      fprintf(stderr, "Please specify a valid offset to decompile at.\n");
      show_help(stderr);
      return 1;
    }

    if (positional_argument_count >= 3) {
      script_file_name = positional_arguments[2];
    } else {
      script_file_name = NULL;
    }
  } else /* if recompile */ {
    script_file_name = positional_arguments[0];
    rom_file_name = positional_arguments[1];
  }

  romfile = fopen(rom_file_name, "rb");
  if (!romfile) {
    printf("Unable to open file %s.", rom_file_name);
    return 1;
  }
  if (autodetect_mode) {
    struct rom_mode rom_mode = determine_mode(romfile);
    if (rom_mode.type != ROM_UNKNOWN) {
      mode = rom_mode.type;
      search = rom_mode.search;
    }
    switch (rom_mode.type) {
      case GOLD:
        decompile_lang = "gs";
        break;
      case CRYSTAL:
        decompile_lang = "cry";
        break;
      case RUBY:
      case EMERALD:
      default:
        decompile_lang = "rse";
        break;
      case FIRE_RED:
        decompile_lang = "frlg";
        break;
    }
  }

  struct language_cache *lang_cache = create_language_cache(language_dir);
  struct parser_cache *parser_cache = create_parser_cache(language_dir);

  const struct language_def *language =
      get_language(lang_cache, decompile_lang);
  if (language == NULL) {
    fprintf(stderr, "Cannot find a language definition for \"%s\".\n",
            decompile_lang);
    exit(1);
  }

  if (command_line == DECOMPILE) {
    FILE *script_file =
        script_file_name == NULL ? stdout : fopen(script_file_name, "wt");
    decompile_all(romfile, decompile_at, language, lang_cache, parser_cache,
                  script_file, verbose);

    if (script_file != stdout) {
      fclose(script_file);
    }
  } else if (command_line == RECOMPILE) {
    FILE *script_file =
        script_file_name == NULL ? stdin : fopen(script_file_name, "rt");
    compile_all(script_file, romfile, language, lang_cache, parser_cache);

    if (script_file != stdin) {
      fclose(script_file);
    }
  }
  fclose(romfile);

#ifndef NDEBUG
  // This is to keep valgrind happy.
  // The program is exiting, so we don't really have to free everything.
  // But, it's also nice to know if we have any real leaks.
  destroy_language_cache(lang_cache);
  destroy_parser_cache(parser_cache);

  free(language_dir);
#endif
  return 0;
}
