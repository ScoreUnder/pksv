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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "binarysearch.h"
#include "recompiler.h"
#include "textproc.h"
#include "codeproc.h"
#include "textutil.h"
#include "pokedef.h"
#include "golddef.h"
#include "sulib.h"
#include "pksv.h"

#define chr Script[i]

#define aa(x) else if (!strcmp(buf, x))
#define ec() end_command(Script, &i)

// TODO: when is c==NULL? is that acceptable?
#define rom(x, s)                                                            \
  do {                                                                       \
    uint32_t rom_m_data;                                                     \
    if (eorg)                                                                \
      rom_m_data = search | (search << 8) | (search << 16) | (search << 24); \
    else                                                                     \
      rom_m_data = (x);                                                      \
    if (c != NULL) add_data(c, (char *)&rom_m_data, (s));                    \
  } while (0)

#define BASIC(x) rom((x), 1)

char *defines_dat_location = NULL;

int thumb = 1;
void end_command(const char *in, pos_int *ppos) {
  pos_int pos = skip_whitespace(in, *ppos);

  if (in[pos] == '\'') {
    // Skip line comment
    while (in[pos] != '\0' && in[pos] != '\n') {
      pos++;
    }
  } else {
    // Make sure the line has ended
    if (in[pos] != '\0' && in[pos] != '\n') {
      // TODO: make this an error
      log_txt("Extra characters on line. Ignoring.\n", 37 - 1);
      while (in[pos] != '\0' && in[pos] != '\n') {
        pos++;
      }
    }
  }

  *ppos = pos;
  return;
}

void vlog_txt(const char *x) {
  if (IsVerbose) {
    log_txt(x, strlen(x));
  }
}
#ifdef WIN32
unsigned int needdlg = 0;
#endif
int GetReg(const char *in, pos_int *ppos) {
  pos_int pos = skip_whitespace(in, *ppos);

  if (in[pos] != 'r') return -1;
  pos++;

  uint32_t result;
  const char *end = dec_to_uint32(&in[pos], SIZE_MAX, &result);
  if (strchr(ARG_END_CHARS, *end) == NULL) {
    pos--;
    char *token = extract_text_interval(&in[pos], end);
    char *errmsg = malloc(end - &in[pos] + 64);
    sprintf(errmsg, "Bad register number: %s\n", token);
    free(token);
    free(errmsg);
    log_txt(errmsg, strlen(errmsg));
  }

  *ppos = skip_whitespace_and_comma(in, pos);
  return result;
}
#define REG() GetReg(Script, &i)

uint32_t GetHex(const char *in, pos_int *ppos) {
  pos_int pos = skip_whitespace(in, *ppos);
  uint32_t result;

  const char *end = hex_to_uint32(&in[pos], SIZE_MAX, &result);
  if (strchr(ARG_END_CHARS, *end) == NULL) {
    char *token = extract_text_interval(&in[pos], end);
    char *errmsg = malloc(end - &in[pos] + 64);
    sprintf(errmsg, "Bad hex constant: %s\n", token);
    free(token);
    free(errmsg);
    log_txt(errmsg, strlen(errmsg));
  }

  *ppos = skip_whitespace_and_comma(in, pos);
  return result;
}
#define HEX() GetHex(Script, &i)

void try_asm_x(const char *Script, pos_int *ppos, char *buf, codeblock *c) {
  unsigned int i = *ppos;
  int j;  // used in rom macro
  register int arg1, arg2, arg3;
  ///////////////////ASM////////////////////
  if (thumb) {
    if (!strcmp(buf, "-lsl")) {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 15) {
        log_txt("Bad arguments to THUMB LSL\n", 28 - 1);
        return;
      }
      i = skip_whitespace(Script, i);
      if ((chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f')) {
        arg3 = HEX() & 0x1F;
        rom(arg1 | (arg2 << 3) | (arg3 << 6), 2);
        // 0000 0TTT TTSS SFFF
      } else {
        rom(arg1 | (arg2 << 3) | 0x4080, 2);
        // 0100 0000 10SS SFFF
      }
      ec();
    }
    aa("-lsr") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 15) {
        log_txt("Bad arguments to THUMB LSR\n", 28 - 1);
        return;
      }
      i = skip_whitespace(Script, i);
      if ((chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f')) {
        arg3 = HEX() & 0x1F;
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x0800, 2);
        // 0000 1TTT TTSS SFFF
      } else {
        rom(arg1 | (arg2 << 3) | 0x40C0, 2);
        // 0100 0000 11SS SFFF
      }
      ec();
    }
    aa("-asr") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 15) {
        log_txt("Bad arguments to THUMB ASR\n", 28 - 1);
        return;
      }
      i = skip_whitespace(Script, i);
      if ((chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f')) {
        arg3 = HEX() & 0x1F;
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x1000, 2);
        // 0001 0TTT TTSS SFFF
      } else {
        rom(arg1 | (arg2 << 3) | 0x4100, 2);
        // 0100 0001 00SS SFFF
      }
      ec();
    }
    aa("-add") {
      i = skip_whitespace(Script, i);
      if (chr == 's') {
        i++;
        if (chr != 'p') {
          log_txt("Bad arguments to THUMB ADD\n", 28 - 1);
          return;
        }
        i++;
        i = skip_whitespace_and_comma(Script, i);
        if (chr == '-') {
          arg2 = 0x80;
        } else
          arg2 = 0;
        i++;
        arg1 = HEX();
        arg1 /= 4;
        arg1 &= 0x7F;
        rom(0xB000 | arg1 | arg2, 2);
        // 1011 0000 XXXX XXXX
      } else {
        arg1 = REG();
        if (chr == 'r') {
          arg2 = REG();
          if ((unsigned)(arg1 | arg2) > 15) {
            log_txt("Bad arguments to THUMB ADD\n", 28 - 1);
            return;
          }
          i = skip_whitespace(Script, i);
          if (chr == 'r') {
            arg3 = REG();
            if ((unsigned)arg3 > 15) {
              log_txt("Bad arguments to THUMB ADD\n", 28 - 1);
              return;
            }
            rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x1800, 2);
            // 0001 100T TTSS SFFF
          } else if ((chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f')) {
            arg3 = HEX() & 0x7;
            rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x1C00, 2);
            // 0001 110T TTSS SFFF
          } else {
            rom((arg1 & 0x7) | ((arg1 & 0x8) << 4) | (arg2 << 3) | 0x4400, 2);
            // 0100 0100 FSSS SFFF
          }
        } else if (chr == 's') {
          i++;
          if (chr != 'p') {
            log_txt("Bad arguments to THUMB ADD\n", 28 - 1);
            return;
          }
          i++;
          i = skip_whitespace_and_comma(Script, i);
          arg2 = HEX();
          arg2 /= 4;
          arg2 &= 0xFF;
          rom((arg1 << 8) | arg2 | 0xA800, 2);
          // 1010 1FFF SSSS SSSS
        } else if (chr == 'p') {
          i++;
          if (chr != 'c') {
            log_txt("Bad arguments to THUMB ADD\n", 28 - 1);
            return;
          }
          i++;
          i = skip_whitespace_and_comma(Script, i);
          arg2 = HEX();
          arg2 /= 4;
          arg2 &= 0xFF;
          rom((arg1 << 8) | arg2 | 0xA000, 2);
          // 1010 0FFF SSSS SSSS
        } else {
          arg2 = HEX();
          rom(arg2 | (arg1 << 8) | 0x3000, 2);
          // 0011 0RRR IIII IIII
        }
      }
      ec();
    }
    aa("-sub") {
      i = skip_whitespace(Script, i);
      if (chr == 's') {
        i++;
        if (chr != 'p') {
          log_txt("Bad arguments to THUMB SUB\n", 28 - 1);
          return;
        }
        i++;
        i = skip_whitespace_and_comma(Script, i);
        if (chr == '-') {
          arg2 = 0x80;
        } else
          arg2 = 0;
        i++;
        arg1 = HEX();
        arg1 /= 4;
        arg1 &= 0x7F;
        rom(0xB000 | arg1 | arg2, 2);
        // 1011 0000 XXXX XXXX
      } else {
        arg1 = REG();
        if (chr == 'r') {
          arg2 = REG();
          if ((unsigned)(arg1 | arg2) > 15) {
            log_txt("Bad arguments to THUMB SUB\n", 28 - 1);
            return;
          }
          i = skip_whitespace(Script, i);
          if (chr == 'r') {
            arg3 = REG();
            if ((unsigned)arg3 > 15) {
              log_txt("Bad arguments to THUMB SUB\n", 28 - 1);
              return;
            }
            rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x1A00, 2);
            // 0001 101T TTSS SFFF
          } else {
            arg3 = HEX() & 0x7;
            rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x1E00, 2);
            // 0001 111T TTSS SFFF
          }
        } else {
          arg2 = HEX();
          rom(arg2 | (arg1 << 8) | 0x3800, 2);
          // 0011 1RRR IIII IIII
        }
      }
      ec();
    }
    aa("-mov") {
      arg1 = REG();
      if ((unsigned)arg1 > 15) {
        log_txt("Bad arguments to THUMB MOV\n", 28 - 1);
        return;
      }
      i = skip_whitespace_and_comma(Script, i);
      if (chr == 'r') {
        arg2 = REG();
        if ((unsigned)arg2 > 15) {
          log_txt("Bad arguments to THUMB MOV\n", 28 - 1);
          return;
        }
        rom(0x4600 | (arg1 & 0x7) | ((arg1 & 0x8) << 4) | (arg2 << 3), 2);
        // 0100 0110 FSSS SFFF
      } else {
        arg2 = HEX();
        rom(0x2000 | arg2 | (arg1 << 8), 2);
        // 0010 0RRR IIII IIII
      }
      ec();
    }
    aa("-cmp") {
      arg1 = REG();
      if ((unsigned)arg1 > 15) {
        log_txt("Bad arguments to THUMB CMP\n", 28 - 1);
        return;
      }
      i = skip_whitespace_and_comma(Script, i);
      if (chr == 'r') {
        arg2 = REG();
        if ((unsigned)arg2 > 15) {
          log_txt("Bad arguments to THUMB CMP\n", 28 - 1);
          return;
        }
        rom(0x4500 | (arg1 & 0x7) | ((arg1 & 0x8) << 4) | (arg2 << 3), 2);
        // 0100 0101 FSSS SFFF
      } else {
        arg2 = HEX();
        rom(0x2800 | arg2 | (arg1 << 8), 2);
        // 0010 1RRR IIII IIII
      }
      ec();
    }
    aa("-and") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB AND\n", 28 - 1);
        return;
      }
      rom(0x4000 | arg1 | (arg2 << 3), 2);
      // 0100 0000 00SS SFFF
      ec();
    }
    aa("-eor") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB EOR\n", 28 - 1);
        return;
      }
      rom(0x4040 | arg1 | (arg2 << 3), 2);
      // 0100 0000 01SS SFFF
      ec();
    }
    aa("-adc") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB ADC\n", 28 - 1);
        return;
      }
      rom(0x4140 | arg1 | (arg2 << 3), 2);
      // 0100 0001 01SS SFFF
      ec();
    }
    aa("-sbc") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB SBC\n", 28 - 1);
        return;
      }
      rom(0x4180 | arg1 | (arg2 << 3), 2);
      // 0100 0001 10SS SFFF
      ec();
    }
    aa("-ror") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB ROR\n", 28 - 1);
        return;
      }
      rom(0x41C0 | arg1 | (arg2 << 3), 2);
      // 0100 0001 11SS SFFF
      ec();
    }
    aa("-tst") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB TST\n", 28 - 1);
        return;
      }
      rom(0x4200 | arg1 | (arg2 << 3), 2);
      // 0100 0010 00SS SFFF
      ec();
    }
    aa("-neg") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB NEG\n", 28 - 1);
        return;
      }
      rom(0x4240 | arg1 | (arg2 << 3), 2);
      // 0100 0010 01SS SFFF
      ec();
    }
    aa("-cmn") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB CMN\n", 28 - 1);
        return;
      }
      rom(0x42C0 | arg1 | (arg2 << 3), 2);
      // 0100 0010 11SS SFFF
      ec();
    }
    aa("-orr") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB ORR\n", 28 - 1);
        return;
      }
      rom(0x4300 | arg1 | (arg2 << 3), 2);
      // 0100 0011 00SS SFFF
      ec();
    }
    aa("-mul") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB MUL\n", 28 - 1);
        return;
      }
      rom(0x4340 | arg1 | (arg2 << 3), 2);
      // 0100 0011 01SS SFFF
      ec();
    }
    aa("-bic") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB BIC\n", 28 - 1);
        return;
      }
      rom(0x4380 | arg1 | (arg2 << 3), 2);
      // 0100 0011 10SS SFFF
      ec();
    }
    aa("-mvn") {
      arg1 = REG();
      arg2 = REG();
      if ((unsigned)(arg2 | arg1) > 7) {
        log_txt("Bad arguments to THUMB MVN\n", 28 - 1);
        return;
      }
      rom(0x43C0 | arg1 | (arg2 << 3), 2);
      // 0100 0011 11SS SFFF
      ec();
    }
    aa("-nop") {
      rom(0x46C0, 2);
      ec();
    }
    aa("-blx") {
      arg1 = REG();
      if ((unsigned)arg1 > 15) {
        log_txt("Bad arguments to THUMB BLX\n", 28 - 1);
        return;
      }
      rom(0x4780 | (arg1 << 3), 2);
      // 0100 0111 1RRR Rxxx
      ec();
    }
    aa("-bx") {
      arg1 = REG();
      if ((unsigned)arg1 > 15) {
        log_txt("Bad arguments to THUMB BX\n", 27 - 1);
        return;
      }
      rom(0x4700 | (arg1 << 3), 2);
      // 0100 0111 0RRR Rxxx
      ec();
    }
    aa("-strh") {
      arg1 = REG();
      while (chr == '[' || chr == ' ' || chr == ',') i++;
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 7) {
        log_txt("Bad arguments to THUMB STRH\n", 29 - 1);
        return;
      }
      if (chr == 'r') {
        arg3 = REG();
        if ((unsigned)arg3 > 7) {
          log_txt("Bad arguments to THUMB STRH\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5200, 2);
        // 0101 001T TTSS SFFF
      } else {
        arg3 = HEX();
        if ((unsigned)arg3 > 0x1F) {
          log_txt("Bad arguments to THUMB STRH\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x8000, 2);
        // 1000 0TTT TTSS SFFF
      }
      while (chr == ' ' || chr == ']') i++;
      ec();
    }
    aa("-ldsb") {
      arg1 = REG();
      while (chr == '[' || chr == ' ' || chr == ',') i++;
      arg2 = REG();
      arg3 = REG();
      if ((unsigned)(arg1 | arg2 | arg3) > 7) {
        log_txt("Bad arguments to THUMB LDSB\n", 29 - 1);
        return;
      }
      rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5600, 2);
      // 0101 011T TTSS SFFF
      while (chr == ' ' || chr == ']') i++;
      ec();
    }
    aa("-ldrh") {
      arg1 = REG();
      while (chr == '[' || chr == ' ' || chr == ',') i++;
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 7) {
        log_txt("Bad arguments to THUMB LDRH\n", 29 - 1);
        return;
      }
      if (chr == 'r') {
        arg3 = REG();
        if ((unsigned)arg3 > 7) {
          log_txt("Bad arguments to THUMB LDRH\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5A00, 2);
        // 0101 101T TTSS SFFF
      } else {
        arg3 = HEX();
        if ((unsigned)arg3 > 0x1F) {
          log_txt("Bad arguments to THUMB LDRH\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x8800, 2);
        // 1000 1TTT TTSS SFFF
      }
      while (chr == ' ' || chr == ']') i++;
      ec();
    }
    aa("-ldsh") {
      arg1 = REG();
      while (chr == '[' || chr == ' ' || chr == ',') i++;
      arg2 = REG();
      arg3 = REG();
      if ((unsigned)(arg1 | arg2 | arg3) > 7) {
        log_txt("Bad arguments to THUMB LDSH\n", 29 - 1);
        return;
      }
      rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5E00, 2);
      // 0101 111T TTSS SFFF
      while (chr == ' ' || chr == ']') i++;
      ec();
    }
    // forms of str: R,[R,R] / R,[R,X] / R,[SP,X]
    aa("-str") {
      arg1 = REG();
      if ((unsigned)arg1 > 7) {
        log_txt("Bad arguments to THUMB STR\n", 28 - 1);
        return;
      }
      while (chr == ',' || chr == ' ' || chr == '[') i++;
      if (chr == 'r') {  // R,[R,R] / R,[R,X]
        arg2 = REG();
        if ((unsigned)arg2 > 7) {
          log_txt("Bad arguments to THUMB STR\n", 28 - 1);
          return;
        }
        if (chr == 'r') {  // R,[R,R]
          arg3 = REG();
          if ((unsigned)arg3 > 7) {
            log_txt("Bad arguments to THUMB STR\n", 28 - 1);
            return;
          }
          rom(arg1 | (arg2 < 3) | (arg3 < 6) | 0x5000, 2);
          // 0101 000T TTSS SFFF
        } else {  // R,[R,X]
          arg3 = HEX();
          if ((unsigned)arg3 > 0x1F) {
            log_txt("Bad arguments to THUMB STR\n", 28 - 1);
            return;
          }
          rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x6000, 2);
          // 0110 0TTT TTSS SFFF
        }
      } else {  // R,[SP,X]
        if (chr != 's') {
          log_txt("Bad arguments to THUMB STR\n", 28 - 1);
          return;
        }
        i++;
        if (chr != 'p') {
          log_txt("Bad arguments to THUMB STR\n", 28 - 1);
          return;
        }
        i++;
        while (chr == ' ' || chr == ',') i++;
        arg2 = HEX();
        if ((arg2 & 3) || (unsigned)arg2 > 1020) {
          log_txt("Bad arguments to THUMB STR\n", 28 - 1);
          return;
        }
        arg2 /= 4;
        rom((arg1 << 8) | arg2 | 0x9000, 2);  // it's over 0x9000!
        // 1001 0RRR OOOO OOOO
      }
      ec();
    }
    // forms of LDR: R,[R,R] / R,[R,X] / R,[PC,X] / R,[SP,X]
    aa("-ldr") {
      arg1 = REG();
      while (chr == ' ' || chr == '[') i++;
      if (chr == 'r') {
        arg2 = REG();
        if (chr == 'r') {
          arg3 = REG();
          if ((unsigned)(arg1 | arg2 | arg3) > 7) {
            log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
            return;
          }
          rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5800, 2);
          // 0101 100T TTSS SFFF
        } else {
          arg3 = HEX();
          if ((unsigned)(arg1 | arg2) > 7 || (unsigned)arg3 > 0x1F) {
            log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
            return;
          }
          rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x6800, 2);
          // 0110 1III IISS SFFF
        }
      } else if (chr == 's') {
        i++;
        if (chr != 'p') {
          log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
          return;
        }
        i++;
        while (chr == ' ' || chr == ',') i++;
        arg2 = HEX();
        if (arg2 > 1020 || (arg2 & 3)) {
          log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
          return;
        }
        rom(arg2 | (arg1 << 8) | 0x9800, 2);
        // 1001 1RRR IIII IIII
      } else {  // R, [PC,X]
        if (chr != 'p') {
          log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
          return;
        }
        i++;
        if (chr != 'c') {
          log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
          return;
        }
        i++;
        while (chr == ',' || chr == ' ') i++;
        arg2 = HEX();
        if (arg2 > 1020 || (arg2 & 3)) {
          log_txt("Bad arguments to THUMB LDR\n", 28 - 1);
          return;
        }
        rom((arg2 >> 2) | (arg1 << 8) | 0x4800, 2);
        // 0100 1RRR IIII IIII
      }
      ec();
    }
    aa("-strb") {  // forms: r,[r,r] / r,[r,x]
      arg1 = REG();
      while (chr == ' ' || chr == '[') i++;
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 7) {
        log_txt("Bad arguments to THUMB STRB\n", 29 - 1);
        return;
      }
      if (chr == 'r') {
        arg3 = REG();
        if ((unsigned)arg3 > 7) {
          log_txt("Bad arguments to THUMB STRB\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5400, 2);
        // 0101 010T TTSS SFFF
      } else {
        arg3 = HEX();
        if ((unsigned)arg3 > 0x1F) {
          log_txt("Bad arguments to THUMB STRB\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x7000, 2);
        // 0111 0III IISS SFFF
      }
      ec();
    }
    aa("-ldrb") {  // forms: r,[r,r] / r,[r,x]
      arg1 = REG();
      while (chr == ' ' || chr == '[') i++;
      arg2 = REG();
      if ((unsigned)(arg1 | arg2) > 7) {
        log_txt("Bad arguments to THUMB LDRB\n", 29 - 1);
        return;
      }
      if (chr == 'r') {
        arg3 = REG();
        if ((unsigned)arg3 > 7) {
          log_txt("Bad arguments to THUMB LDRB\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x5C00, 2);
        // 0101 110T TTSS SFFF
      } else {
        arg3 = HEX();
        if ((unsigned)arg3 > 0x1F) {
          log_txt("Bad arguments to THUMB LDRB\n", 29 - 1);
          return;
        }
        rom(arg1 | (arg2 << 3) | (arg3 << 6) | 0x7800, 2);
        // 0111 1III IISS SFFF
      }
      ec();
    }
    aa("-pop") {
      while (chr == ' ' || chr == '{') i++;
      arg1 = 0;
      while (chr != '}' && chr != '\n' && chr != 0 && chr != '\'') {
        if (chr == '-') {
          i++;
          arg3 = REG();
          if ((unsigned)arg3 <= 7 && (unsigned)arg2 <= 7) {
            if (arg2 < arg3) {
              while (arg2 < arg3) {
                arg1 |= 1 << arg2;
                arg2++;
              }
            } else {
              while (arg3 < arg2) {
                arg1 |= 1 << arg3;
                arg3++;
              }
            }
          }

        } else if (chr == 'p') {
          i++;
          i++;
          arg1 |= 0x100;
        } else {
          arg2 = REG();
          if ((unsigned)arg2 <= 7) {
            arg1 |= 1 << arg2;
          }
        }
        while (chr == ',' || chr == ' ') i++;
      }
      rom(arg1 | 0xBC00, 2);
      // 1011 110P RRRR RRRR
      ec();
    }
    aa("-push") {
      while (chr == ' ' || chr == '{') i++;
      arg1 = 0;
      while (chr != '}' && chr != '\n' && chr != 0 && chr != '\'') {
        if (chr == '-') {
          i++;
          arg3 = REG();
          if ((unsigned)arg3 <= 7 && (unsigned)arg2 <= 7) {
            if (arg2 < arg3) {
              while (arg2 < arg3) {
                arg1 |= 1 << arg2;
                arg2++;
              }
            } else {
              while (arg3 < arg2) {
                arg1 |= 1 << arg3;
                arg3++;
              }
            }
          }

        } else if (chr == 'l') {
          i++;
          i++;
          arg1 |= 0x100;
        } else {
          arg2 = REG();
          if ((unsigned)arg2 <= 7) {
            arg1 |= 1 << arg2;
          }
        }
        while (chr == ',' || chr == ' ') i++;
      }
      rom(arg1 | 0xB400, 2);
      // 1011 010P RRRR RRRR
      ec();
    }
    aa("-swi") {
      arg1 = HEX();
      rom((arg1 & 0xFF) | 0xDF00, 2);
      ec();
    }
    aa("-bkpt") {
      arg1 = HEX();
      rom((arg1 & 0xFF) | 0xBE00, 2);
      ec();
    }
    aa("-beq") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD000 | arg2, 2);
      // 1101 0000 IIII IIII
      ec();
    }
    aa("-bne") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD100 | arg2, 2);
      // 1101 0001 IIII IIII
      ec();
    }
    aa("-bcs") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD200 | arg2, 2);
      // 1101 0010 IIII IIII
      ec();
    }
    aa("-bcc") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD300 | arg2, 2);
      // 1101 0011 IIII IIII
      ec();
    }
    aa("-bmi") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD400 | arg2, 2);
      // 1101 0100 IIII IIII
      ec();
    }
    aa("-bpl") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD500 | arg2, 2);
      // 1101 0101 IIII IIII
      ec();
    }
    aa("-bvs") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD600 | arg2, 2);
      // 1101 0110 IIII IIII
      ec();
    }
    aa("-bvc") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD700 | arg2, 2);
      // 1101 0111 IIII IIII
      ec();
    }
    aa("-bhi") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD800 | arg2, 2);
      // 1101 1000 IIII IIII
      ec();
    }
    aa("-bls") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xD900 | arg2, 2);
      // 1101 1001 IIII IIII
      ec();
    }
    aa("-bge") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xDA00 | arg2, 2);
      // 1101 1010 IIII IIII
      ec();
    }
    aa("-blt") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xDB00 | arg2, 2);
      // 1101 1011 IIII IIII
      ec();
    }
    aa("-bgt") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xDC00 | arg2, 2);
      // 1101 1100 IIII IIII
      ec();
    }
    aa("-ble") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0xFF;
      rom(0xDD00 | arg2, 2);
      // 1101 1101 IIII IIII
      ec();
    }
    aa("-ldmia") {
      arg3 = REG();
      while (chr == ',' || chr == ' ' || chr == '{') i++;
      arg1 = 0;
      while (chr != '}') {
        arg2 = REG();
        if ((unsigned)arg2 <= 7) {
          arg1 |= 1 << arg2;
        }
        while (chr == ',' || chr == ' ') i++;
      }
      rom(arg1 | (arg3 << 8) | 0xC800, 2);
      // 1100 1RRR IIII IIII
      ec();
    }
    aa("-stmia") {
      arg3 = REG();
      while (chr == ',' || chr == ' ' || chr == '{') i++;
      arg1 = 0;
      while (chr != '}' && chr != '\0' && chr != '\n') {
        arg2 = REG();
        if ((unsigned)arg2 <= 7) {
          arg1 |= 1 << arg2;
        }
        while (chr == ',' || chr == ' ') i++;
      }
      rom(arg1 | (arg3 << 8) | 0xC000, 2);
      // 1100 0RRR IIII IIII
      ec();
    }
    aa("-b") {
      i = skip_whitespace(Script, i);
      if (chr == '-')
        arg1 = 1;
      else
        arg1 = 0;
      i++;
      arg2 = HEX();
      if (arg1) arg2 = -arg2;
      arg2 -= 4;
      arg2 = (signed)arg2 / 2;
      arg2 &= 0x7FF;
      rom(0xE000 | arg2, 2);
      // 1110 0III IIII IIII
      ec();
    }
    aa("-bl_part1") {
      arg1 = HEX();
      rom(0xF000 | ((arg1 >> 12) & 0x7FF), 2);
      // 1111 0III IIII IIII
      ec();
    }
    aa("-blh") {
      arg1 = HEX();
      rom(0xF800 | (arg1 & 0x7FF), 2);
      // 1111 1III IIII IIII
      ec();
    }
    aa("-blxh") {
      arg1 = HEX();
      rom(0xE800 | (arg1 & 0x7FF), 2);
      // 1110 1III IIII IIII
      ec();
    }
  } else {
    // do nothing at all :) not implemented yet
  }
  ///////////////////END////////////////////
  *ppos = i;
}

static int bsearch_key_strcmp(const void *a, const void *b) {
  return strcmp((const char *)a, (const char *)b);
}

static void *bsearch_key_strdup(const void *a) {
  return strdup((const char *)a);
}

#define try_asm() try_asm_x(Script, &i, buf, c)
struct bsearch_root *DoDefines() {
  char buf[500];
  struct bsearch_root *defines =
      bsearch_create_root(bsearch_key_strcmp, bsearch_key_strdup, free, NULL);

#ifdef WIN32
  OutputDebugString("Started Defines");
#endif

  FILE *f = NULL;
  if (defines_dat_location) {
    f = fopen(defines_dat_location, "rb");
  }
  if (!f) {
    f = fopen("defines.dat", "rb");
  }
  if (!f) {
    log_txt("Cannot open defines.dat!\n", 26 - 1);
    return defines;
  }
  bool ok = true;
  while (!feof(f)) {
    uint8_t l;
    if (fread(&l, 1, 1, f) == 0) {
      if (!feof(f)) ok = false;
      break;
    }
    if (fread(buf, 1, l, f) < l) {
      ok = false;
      break;
    }
    uint32_t value;
    if (fread(&value, 1, 4, f) < 4) {
      ok = false;
      break;
    }
    bsearch_ensure_capacity(defines, defines->size + 1);
    buf[l] = '\0';
    // Note: safe to insert directly because defines.dat is already sorted
    defines->pairs[defines->size++] = (struct bsearch_kv){
        .key = strdup(buf),
        .value = (void *)(intptr_t)value,
    };
  }
  if (!ok) {
    char *s;
    if (feof(f)) {
      s = "defines.dat is truncated and not fully valid\n";
    } else if (ferror(f)) {
      s = "Error reading defines.dat\n";
    }
    log_txt(s, strlen(s));
    bsearch_deinit_root(defines);  // Might not have been a valid defines.dat
    bsearch_init_root(defines, bsearch_key_strcmp, bsearch_key_strdup, free,
                      NULL);  // Still need a struct to return
  }
  fclose(f);
#ifdef WIN32
  OutputDebugString("Finished Defines, starting main compiler loop");
#endif
  return defines;
}
void RecodeProc(char *script, char *romfn) {
#ifndef DLL
  FILE *CurrFile;
#endif
  FILE *IncFile, *RomFile;
#ifdef WIN32
  char *strings;
#endif
  char *Script;  // Whoops, used the same name for the filename.
  // Use caps-lock carefully.
  char buf[1024], buf2[1024], buf3[1024];
  void *temp_ptr;
  int buf_loc;
  unsigned int start = 0, dynu = 0,
#ifndef DLL
               fs,
#endif
               fst, i, j, k, l, arg1, arg2, arg3, arg4, arg5, arg6,
               scriptlen;  //,arg7;
  codeblock *c = NULL;
  codeblock *d;
  codelabel *cl = NULL;
  codelabel *cl2;

  RomFile = fopen(romfn, "r+b");
  if (RomFile == NULL) {
    strcat(romfn, ".gba");
    RomFile = fopen(romfn, "r+b");
  }
  struct bsearch_root *defines = DoDefines();
#ifndef DLL
  strcat(GlobBuf, "pokeinc.txt");
  IncFile = fopen(GlobBuf, "rt");
#else
  IncFile = fopen("pokeinc.txt", "rt");
#endif

  if (RomFile == NULL) {
    printf("Cannot open ROM\n");
#ifdef WIN32
    MessageBox(NULL, "The ROM file could not be opened", "Error", 0x10);
#endif
    if (IncFile) fclose(IncFile);
    bsearch_destroy_root(defines);
    return;
  }
#ifndef DLL
  CurrFile = fopen(script, "rt");
  if (CurrFile != NULL) {
    fseek(CurrFile, 0, SEEK_END);
    fs = ftell(CurrFile);
    fseek(CurrFile, 0, SEEK_SET);
#endif

    if (IncFile) {
      fseek(IncFile, 0, SEEK_END);
      fst = ftell(IncFile);
      fseek(IncFile, 0, SEEK_SET);
    } else {
      fst = 0;
    }
#ifndef DLL
    Script = malloc(fs + fst + 6);
#else
  Script = malloc(strlen(script) + fst + 6);
#endif
    if (Script == NULL) {
#ifdef WIN32
      MessageBox(NULL, "malloc() Failed to allocate memory for the script.",
                 "Error", 0x10);
#endif
#ifndef DLL
      if (CurrFile) fclose(CurrFile);
#endif
      if (IncFile) fclose(IncFile);
      bsearch_destroy_root(defines);
      return;
    }
    if (IncFile) {
      fread(Script, 1, fst, IncFile);
      strcpy(&Script[fst], "\n\n");
      fst += 2;
    }
#ifndef DLL
    if (fread((char *)(Script + fst), 1, fs, CurrFile) != fs) {
      fprintf(stderr, "Error reading script file\n");
      if (CurrFile) fclose(CurrFile);
      if (IncFile) fclose(IncFile);
      free(Script);
      bsearch_destroy_root(defines);
      return;
    }
    strcpy(&Script[fst + fs], "\n\n");
#else
  strcpy(Script + fst, script);
  strcat(Script + fst, "\n\n");
#endif

#ifndef DLL
    LogFile = fopen("PokeScrE.log", "wt");
    if (LogFile == NULL) {
      printf("Failed to open a logfile\n");
#ifdef WIN32
      MessageBox(NULL, "Failed to open a log.", "Error", 0x10);
#endif
      free(Script);
      if (CurrFile) fclose(CurrFile);
      if (IncFile) fclose(IncFile);
      bsearch_destroy_root(defines);
      return;
    }
    fprintf(LogFile, "Opened file.\n");
#else
  log_txt("Initialized.\n", 14 - 1);
#endif

    i = 0;
    LowerCaseAndRemAll0D(Script);
    scriptlen = strlen(Script);
    if (mode == DIAMOND) {
      while (i < scriptlen) {
        switch (chr)  // Behave differently according to char
        {
          case 0:  // E O Script
            return;
          case '\'':
            while (chr != '\n' && chr != 0) {
              i++;
            }
            i--;
            break;
          case ' ':  // Ignore spaces
          case '\n':
            break;
          default:
            j = 0;
            while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
              buf[j] = chr;
              i++;
              j++;
            }
            buf[j] = 0;
            if (!strcmp(buf, "#define") || !strcmp(buf, "#d")) {
              vlog_txt("#DEFINE\n");
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Premature end to #DEFINE!\n", 27 - 1);
                return;
              }
              j = 0;
              while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;  // Append null
              sprintf(buf2, "   -> %s\n", buf);
              vlog_txt(buf2);
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Premature end to #DEFINE!\n", 27 - 1);
                return;
              }
              k = GetNum("#DEFINE");
              if (!gffs) {
                return;
              }
              bsearch_upsert(defines, buf, (void *)(intptr_t)k);
              ec();
            }
            aa("#narc") {
              vlog_txt("#NARC\n");
              arg1 = GetNum("#NARC");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("#NARC");
              if (!gffs) {
                return;
              }
              fseek(RomFile, 0xE, SEEK_SET);
              arg3 = 0;
              fread(&arg3, 1, 2, RomFile);
              fseek(RomFile, 0x10, SEEK_SET);
              fread(&buf, 1, 4, RomFile);
              buf[4] = 0;
              k = 0;
              while (k < arg3 && strcmp(buf, "BTAF")) {
                arg4 = 0;
                fread(&arg4, 1, 4, RomFile);
                fseek(RomFile, arg4 - 0x8, SEEK_CUR);
                fread(&buf, 1, 4, RomFile);
                buf[4] = 0;
                k++;
              }
              if (!strcmp(buf, "BTAF")) {
                fseek(RomFile, 8 * (arg1 + 1), SEEK_CUR);
                fread(&arg1, 1, 4, RomFile);
                fseek(RomFile, 0x10, SEEK_SET);
                fread(&buf, 1, 4, RomFile);
                buf[4] = 0;
                k = 0;
                while (k < arg3 && strcmp(buf, "GMIF")) {
                  arg4 = 0;
                  fread(&arg4, 1, 4, RomFile);
                  fseek(RomFile, arg4 - 0x8, SEEK_CUR);
                  fread(&buf, 1, 4, RomFile);
                  buf[4] = 0;
                  k++;
                }
                if (!strcmp(buf, "GMIF")) {
                  fseek(RomFile, 4 + arg1 + arg2, SEEK_CUR);
                } else {
                  log_txt("Incomplete NARC.\n", 18 - 1);
                  return;
                }
              } else {
                log_txt("Incomplete NARC.\n", 18 - 1);
                return;
              }
              ec();
            }
            aa("#quiet") {
              IsVerbose = 0;
              ec();
            }
            aa("#loud") {
              IsVerbose = 1;
              ec();
            }
            aa("#thumb") {
              thumb = 1;
              ec();
            }
            aa("#arm") {
              thumb = 0;
              ec();
            }
            aa("#gs") {
              mode = GOLD;
              goto gsc;
            }
            aa("#c") {
              mode = CRYSTAL;
              goto cry;
            }
            aa("#frlg") {
              mode = FIRE_RED;
              goto frlg;
            }
            aa("#rse") {
              mode = RUBY;
              goto rse;
            }
            aa("#dp") {
              mode = DIAMOND;
            dp:
              ec();
            }
            aa("#raw") {
              vlog_txt("#RAW\n");
              arg1 = 1;
              while (arg1) {
                k = GetNum("#RAW");
                if (!gffs) {
                  return;
                }
                BASIC(k);
                i = skip_whitespace(Script, i);
                arg1 = (chr >= '0' && chr <= '9') || chr == '$';
              }
              ec();
            }
            aa("#ptr") {
              vlog_txt("#PTR\n");
              k = GetNum("#PTR");
              if (!gffs) {
                return;
              }
              k |= 0x08000000;
              rom(k, 4);
              ec();
            }
            aa("wildbattle") {
              vlog_txt("WILDBATTLE\n");
              arg1 = GetNum("WILDBATTLE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WILDBATTLE");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("WILDBATTLE");
              if (!gffs) {
                return;
              }
              BASIC(0xBC);
              BASIC(0x00);
              BASIC(0x06);
              BASIC(0x00);
              BASIC(0x01);
              BASIC(0x00);
              BASIC(0x01);
              BASIC(0x00);
              BASIC(0x00);
              BASIC(0x00);
              BASIC(0xBD);
              BASIC(0x00);
              BASIC(0xDE);
              BASIC(0x00);
              BASIC(0x00);
              BASIC(0x80);
              BASIC(0x96);
              BASIC(0x00);
              BASIC(0x00);
              BASIC(0x80);
              BASIC(0x05);
              BASIC(0x00);
              BASIC(0x00);
              BASIC(0x00);
              BASIC(0x0C);
              BASIC(0x80);
              BASIC(0x1E);
              BASIC(0x00);
              BASIC(arg1);
              BASIC(0x01);
              rom(arg2, 2);
              BASIC(arg3);
              ec();
            }
            try_asm();
            if (*buf == ':') {
              if (c) add_label(buf, c, c->size, &cl);
              ec();
            }
            aa("m") {
              vlog_txt("Movement data...\n");
              add_data(c, trans, transbackmove(Script, &i));
              ec();
            }
            aa("=") {
              vlog_txt("[STRING]\n");
              if (chr == ' ') {
                i++;
              } else {
                log_txt("Should have a space after the =\n", 33 - 1);
              }
              temp_ptr = transbackstr(script, i - fst, c);
              while (chr != '\n' && chr != 0) {
                i++;
              }
              sprintf(buf2, "   -> %s\n", (char *)temp_ptr);
              free(temp_ptr);
              vlog_txt(buf2);
            }
            aa(".") {
              vlog_txt("[BINARY]\n   ->");
              i = skip_whitespace(Script, i);
              k = 0;
              while (chr != '\n' && chr != 0) {
                k = 1 - k;
                i = skip_whitespace(Script, i);
                if (chr >= '0' && chr <= '9')
                  j = chr - '0';
                else if (chr >= 'A' && chr <= 'F')
                  j = 10 + (chr - 'A');
                if (((char *)("0123456789abcdef"))[j] == 0) {
                  sprintf(buf2, "Failed to understand hex character '%c'\n",
                          chr);
                  log_txt(buf2, strlen(buf2));
                  return;
                }
                if (k == 0) {
                  l |= j;
                  rom(j, 1);
                  if (IsVerbose) {
                    sprintf(buf2, " %02X", l);
                    log_txt(buf2, strlen(buf2));
                  }
                } else {
                  l = j << 4;
                }
                i++;
              }
              vlog_txt("\n");
            }
            else {
              sprintf(buf2, "Warning: Unknown command \"%s\"\n", buf);
              log_txt(buf2, strlen(buf2));
              while (chr != '\n' && chr != 0) {
                i++;
              }
            }
            break;
        }
        i++;
      }
    } else if (mode == CRYSTAL) {
      while (i < scriptlen) {
        switch (chr)  // Behave differently according to char
        {
          case 0:  // E O Script
            return;
          case '\'':
            while (chr != '\n' && chr != 0) {
              i++;
            }
            i--;
            break;
          case ' ':  // Ignore spaces
          case '\n':
            break;
          default:
            j = 0;
            while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
              buf[j] = chr;
              i++;
              j++;
            }
            buf[j] = 0;
            if (!strcmp(buf, "#define") || !strcmp(buf, "#d")) {
              vlog_txt("#DEFINE\n");
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Premature end to #DEFINE!\n", 27 - 1);
                return;
              }
              j = 0;
              while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;  // Append null
              sprintf(buf2, "   -> %s\n", buf);
              vlog_txt(buf2);
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Premature end to #DEFINE!\n", 27 - 1);
                return;
              }
              k = GetNum("#DEFINE");
              if (!gffs) {
                return;
              }
              bsearch_upsert(defines, buf, (void *)(intptr_t)k);
              ec();
            }
            aa("#dyn") {
              vlog_txt("#DYN\n");
              start = GetNum("#DYN");
              if (!gffs) return;
              dyntype = 0;
              bsearch_upsert(defines, "findfromgold", (void *)(intptr_t)start);
              ec();
            }
            aa("#dyn2") {
              vlog_txt("#DYN2\n");
              start = GetNum("#DYN2");
              if (!gffs) return;
              dyntype = 1;
              ec();
            }
            aa("#dynamic") {
              vlog_txt("#DYNAMIC\n");
              start = GetNum("#DYNAMIC");
              if (!gffs) return;
              dyntype = 0;
              bsearch_upsert(defines, "findfromgold", (void *)(intptr_t)start);
              ec();
            }
            aa("#org") {
              eorg = 0;
              vlog_txt("#ORG\n");
              i = skip_whitespace(Script, i);
              buf[0] = 0;
              if (chr == '@') {
                if (start == 0) {
                  log_txt("Need #dyn with dynamic offsets\n", 32 - 1);
                  return;
                }
                dynu = 1;
                j = 0;
                while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                  buf[j] = chr;
                  i++;
                  j++;
                }
                buf[j] = 0;
                sprintf(buf2, "   -> %s\n", buf);
                vlog_txt(buf2);
              } else {
                k = GetNum("#ORG");
                if (!gffs) {
                  return;
                }
              }
              d = malloc(sizeof(codeblock));
              if (*buf == 0)
                init_codeblock(d, NULL, k);
              else
                init_codeblock(d, buf, (start & 0x0FFFFFFF) | (dyntype << 24));
              d->prev = c;
              if (c != NULL) c->next = d;
              c = d;
              ec();
            }
            aa("#eorg") {
              eorg = 1;
              vlog_txt("#EORG\n");
              i = skip_whitespace(Script, i);
              buf[0] = 0;
              if (chr == '@') {
                j = 0;
                while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                  buf[j] = chr;
                  i++;
                  j++;
                }
                buf[j] = 0;
                sprintf(buf2, "   -> %s\n", buf);
                vlog_txt(buf2);
              } else {
                k = GetNum("#EORG");
                if (!gffs) {
                  return;
                }
              }
              d = malloc(sizeof(codeblock));
              if (*buf == 0)
                init_codeblock(d, NULL, k);
              else
                init_codeblock(d, buf, (start & 0x0FFFFFFF) | (dyntype << 24));
              d->prev = c;
              if (c != NULL) c->next = d;
              c = d;
              ec();
            }
            aa("#quiet") {
              IsVerbose = 0;
              ec();
            }
            aa("#loud") {
              IsVerbose = 1;
              ec();
            }
            aa("#thumb") {
              thumb = 1;
              ec();
            }
            aa("#arm") {
              thumb = 0;
              ec();
            }
            aa("#gs") {
              mode = GOLD;
              goto gsc;
            }
            aa("#c") {
              mode = CRYSTAL;
            cry:
              ec();
            }
            aa("#frlg") {
              mode = FIRE_RED;
              goto frlg;
              ec();
            }
            aa("#rse") {
              mode = RUBY;
              goto rse;
              ec();
            }
            aa("#dp") {
              mode = DIAMOND;
              goto dp;
              ec();
            }
            aa("#raw") {
              vlog_txt("#RAW\n");
              arg1 = 1;
              while (arg1) {
                k = GetNum("#RAW");
                if (!gffs) {
                  return;
                }
                BASIC(k);
                i = skip_whitespace(Script, i);
                arg1 = (chr >= '0' && chr <= '9') || chr == '$';
              }
              ec();
            }
            aa("end") {
              BASIC(CRY_END);
              ec();
            }
            aa("return") {
              BASIC(CRY_RETURN);
              ec();
            }
            aa("reloadandreturn") {
              BASIC(CRY_RELOADANDRETURN);
              ec();
            }
            aa("checktriggers") {
              BASIC(CRY_CHECKTRIGGERS);
              ec();
            }
            aa("checkver") {
              BASIC(CRY_CHECKVER);
              ec();
            }
            aa("wildoff") {
              BASIC(CRY_WILDOFF);
              ec();
            }
            aa("wildon") {
              BASIC(CRY_WILDON);
              ec();
            }
            aa("itemnotify") {
              BASIC(CRY_ITEMNOTIFY);
              ec();
            }
            aa("pocketisfull") {
              BASIC(CRY_POCKETISFULL);
              ec();
            }
            aa("loadfont") {
              BASIC(CRY_LOADFONT);
              ec();
            }
            aa("loadmovesprites") {
              BASIC(CRY_LOADMOVESPRITES);
              ec();
            }
            aa("release") {
              BASIC(CRY_LOADMOVESPRITES);
              ec();
            }
            aa("yesorno") {
              BASIC(CRY_YESORNO);
              ec();
            }
            aa("writebackup") {
              BASIC(CRY_WRITEBACKUP);
              ec();
            }
            aa("closetext") {
              BASIC(CRY_CLOSETEXT);
              ec();
            }
            aa("keeptextopen") {
              BASIC(CRY_KEEPTEXTOPEN);
              ec();
            }
            aa("pokepicyesorno") {
              BASIC(CRY_POKEPICYESORNO);
              ec();
            }
            aa("interpretmenu") {
              BASIC(CRY_INTERPRETMENU);
              ec();
            }
            aa("interpretmenu2") {
              BASIC(CRY_INTERPRETMENU2);
              ec();
            }
            aa("loadpikadata") {
              BASIC(CRY_LOADPIKADATA);
              ec();
            }
            aa("clearfight") {
              BASIC(CRY_CLEARFIGHT);
              ec();
            }
            aa("loadtrainerseen") {
              BASIC(CRY_LOADTRAINERSEEN);
              ec();
            }
            aa("startbattle") {
              BASIC(CRY_STARTBATTLE);
              ec();
            }
            aa("returnafterbattle") {
              BASIC(CRY_RETURNAFTERBATTLE);
              ec();
            }
            aa("talkafter") {
              BASIC(CRY_TALKAFTER);
              ec();
            }
            aa("talkaftercancel") {
              BASIC(CRY_TALKAFTERCANCEL);
              ec();
            }
            aa("talkaftercheck") {
              BASIC(CRY_TALKAFTERCHECK);
              ec();
            }
            aa("faceplayer") {
              BASIC(CRY_FACEPLAYER);
              ec();
            }
            aa("stopfollow") {
              BASIC(CRY_STOPFOLLOW);
              ec();
            }
            aa("reloadmap") {
              BASIC(CRY_RELOADMAP);
              ec();
            }
            aa("reloadmappart") {
              BASIC(CRY_RELOADMAPPART);
              ec();
            }
            aa("playrammusic") {
              BASIC(CRY_PLAYRAMMUSIC);
              ec();
            }
            aa("playmapmusic") {
              BASIC(CRY_PLAYMAPMUSIC);
              ec();
            }
            aa("reloadmapmusic") {
              BASIC(CRY_RELOADMAPMUSIC);
              ec();
            }
            aa("waitbutton") {
              BASIC(CRY_WAITBUTTON);
              ec();
            }
            aa("warpsound") {
              BASIC(CRY_WARPSOUND);
              ec();
            }
            aa("specialsound") {
              BASIC(CRY_SPECIALSOUND);
              ec();
            }
            aa("warpcheck") {
              BASIC(CRY_WARPCHECK);
              ec();
            }
            aa("resetfuncs") {
              BASIC(CRY_RESETFUNCS);
              ec();
            }
            aa("hangup") {
              BASIC(CRY_HANGUP);
              ec();
            }
            aa("checkphonecall") {
              BASIC(CRY_CHECKPHONECALL);
              ec();
            }
            aa("halloffame") {
              BASIC(CRY_HALLOFFAME);
              ec();
            }
            aa("credits") {
              BASIC(CRY_CREDITS);
              ec();
            }
            aa("text-newline") {
              BASIC(5);
              ec();
            }
            aa("text-waitbutton") {
              BASIC(6);
              ec();
            }
            aa("text-newlinewitharrow") {
              BASIC(7);
              ec();
            }
            aa("text-switchtoasm") {
              BASIC(8);
              ec();
            }
            aa("text-interpretdata") {
              BASIC(10);
              ec();
            }
            aa("text-playsound0") {
              BASIC(11);
              ec();
            }
            aa("text-waitbutton2") {
              BASIC(13);
              ec();
            }
            aa("text-playsound9") {
              BASIC(14);
              ec();
            }
            aa("text-playsound1") {
              BASIC(15);
              ec();
            }
            aa("text-playsound2") {
              BASIC(16);
              ec();
            }
            aa("text-playsounda") {
              BASIC(17);
              ec();
            }
            aa("text-playsoundd") {
              BASIC(18);
              ec();
            }
            aa("text-playsoundc") {
              BASIC(19);
              ec();
            }
            aa("text-day") {
              BASIC(21);
              ec();
            }
            aa("2call") {
              vlog_txt("2CALL\n");
              arg1 = GetNum("2CALL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_2CALL);
              rom(arg1, 2);
              ec();
            }
            aa("3call") {
              vlog_txt("3CALL\n");
              arg1 = GetNum("3CALL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_3CALL);
              rom(arg1, 3);
              ec();
            }
            aa("2ptcall") {
              vlog_txt("2PTCALL\n");
              arg1 = GetNum("2PTCALL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_2PTCALL);
              rom(arg1, 2);
              ec();
            }
            aa("2jump") {
              vlog_txt("2JUMP\n");
              arg1 = GetNum("2JUMP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_2JUMP);
              rom(arg1, 2);
              ec();
            }
            aa("3jump") {
              vlog_txt("3JUMP\n");
              arg1 = GetNum("3JUMP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_3JUMP);
              rom(arg1, 3);
              ec();
            }
            aa("2ptjump") {
              vlog_txt("2PTJUMP\n");
              arg1 = GetNum("2PTJUMP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_2PTJUMP);
              rom(arg1, 2);
              ec();
            }
            aa("text-ram") {
              vlog_txt("TEXT-RAM\n");
              arg1 = GetNum("TEXT-RAM");
              if (!gffs) {
                return;
              }
              BASIC(1);
              rom(arg1, 2);
              ec();
            }
            aa("text-reloc") {
              vlog_txt("TEXT-RELOC\n");
              arg1 = GetNum("TEXT-RELOC");
              if (!gffs) {
                return;
              }
              BASIC(3);
              rom(arg1, 2);
              ec();
            }
            aa("text-box") {
              vlog_txt("TEXT-BOX\n");
              arg1 = GetNum("TEXT-BOX");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("TEXT-BOX");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("TEXT-BOX");
              if (!gffs) {
                return;
              }
              BASIC(4);
              rom(arg1, 2);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("text-interpretxdata") {
              vlog_txt("TEXT-INTERPRETXDATA\n");
              arg1 = GetNum("TEXT-INTERPRETXDATA");
              if (!gffs) {
                return;
              }
              BASIC(12);
              rom(arg1, 1);
              ec();
            }
            aa("text-buffer") {
              vlog_txt("TEXT-BUFFER\n");
              arg1 = GetNum("TEXT-BUFFER");
              if (!gffs) {
                return;
              }
              BASIC(20);
              rom(arg1, 1);
              ec();
            }
            aa("text-newtxt") {
              vlog_txt("TEXT-NEWTXT\n");
              arg1 = GetNum("TEXT-NEWTXT");
              if (!gffs) {
                return;
              }
              BASIC(22);
              rom(arg1 >> 8, 2);
              rom(arg1, 1);
              ec();
            }
            aa("text-number") {
              vlog_txt("TEXT-NUMBER\n");
              arg1 = GetNum("TEXT-NUMBER");
              if (!gffs) {
                return;
              }
              arg2 = (0xF & GetNum("TEXT-NUMBER")) << 4;
              if (!gffs) {
                return;
              }
              arg2 |= 0xF & GetNum("TEXT-NUMBER");
              if (!gffs) {
                return;
              }
              BASIC(9);
              rom(arg1, 2);
              rom(arg2, 1);
              ec();
            }
            aa("text-hex") {
              vlog_txt("TEXT-HEX\n");
              arg1 = GetNum("TEXT-HEX");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("TEXT-HEX");
              if (!gffs) {
                return;
              }
              arg2 &= 0x1F;
              i = skip_whitespace(Script, i);
              j = 0;
              while (chr != '\n' && chr != ' ' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;
              if (!strcmp(buf, "money-no-zeros")) {
                arg2 |= (7 << 5);
              } else if (!strcmp(buf, "no-zeros")) {
                arg2 |= (6 << 5);
              } else if (!strcmp(buf, "money-spaces")) {
                arg2 |= (5 << 5);
              } else if (!strcmp(buf, "spaces")) {
                arg2 |= (4 << 5);
              } else if (!strcmp(buf, "money2")) {
                arg2 |= (3 << 5);
              } else if (!strcmp(buf, "normal2")) {
                arg2 |= (2 << 5);
              } else if (!strcmp(buf, "money")) {
                arg2 |= (1 << 5);
              } else if (!strcmp(buf, "normal")) {
                arg2 |= (0 << 5);
              } else {
                log_txt("Incorrect arguments to text-hex\n", 33 - 1);
                return;
              }
              BASIC(2);
              rom(arg1, 2);
              rom(arg2, 1);
              ec();
            }
            aa("if") {
              vlog_txt("IF\n");
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Incorrect arguments to IF.\n", 28 - 1);
              }
              j = 0;
              while (chr != '\n' && chr != ' ' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;
              arg1 = 0;
              if (!strcmp(buf, "=") || !strcmp(buf, "==")) {
                BASIC(CRY_EQBYTE);
              } else if (!strcmp(buf, "!=") || !strcmp(buf, "<>")) {
                BASIC(CRY_NEQBYTE);
              } else if (!strcmp(buf, "<") || !strcmp(buf, "<<")) {
                BASIC(CRY_LTBYTE);
              } else if (!strcmp(buf, ">") || !strcmp(buf, ">>")) {
                BASIC(CRY_GTBYTE);
              } else if (!strcmp(buf, "==0") || !strcmp(buf, "=0") ||
                         !strcmp(buf, "false")) {
                BASIC(CRY_EQZERO);
                arg1 = -1;
              } else if (!strcmp(buf, "!=0") || !strcmp(buf, "<>0") ||
                         !strcmp(buf, "true")) {
                BASIC(CRY_NEQZERO);
                arg1 = -1;
              } else {
                log_txt("Incorrect arguments to IF\n", 29 - 1);
                return;
              }
              if (arg1 != -1) {
                arg1 = GetNum("IF");
                if (!gffs) {
                  return;
                }
              }
              arg2 = GetNum("IF");
              if (!gffs) {
                return;
              }
              if (arg1 != -1) rom(arg1, 1);
              rom(arg2, 2);
              ec();
            }
            aa("jumpstd") {
              vlog_txt("JUMPSTD\n");
              arg1 = GetNum("JUMPSTD");
              if (!gffs) {
                return;
              }
              BASIC(CRY_JUMPSTD);
              rom(arg1, 2);
              ec();
            }
            aa("callstd") {
              vlog_txt("CALLSTD\n");
              arg1 = GetNum("CALLSTD");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CALLSTD);
              rom(arg1, 2);
              ec();
            }
            aa("3callasm") {
              vlog_txt("3CALLASM\n");
              arg1 = GetNum("3CALLASM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_3CALLASM);
              rom(arg1, 3);
              ec();
            }
            aa("special") {
              vlog_txt("SPECIAL\n");
              arg1 = GetNum("SPECIAL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SPECIAL);
              rom(arg1, 2);
              ec();
            }
            aa("2ptcallasm") {
              vlog_txt("2PTCALLASM\n");
              arg1 = GetNum("2PTCALLASM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_2PTCALLASM);
              rom(arg1, 2);
              ec();
            }
            aa("checkmaptriggers") {
              vlog_txt("CHECKMAPTRIGGERS\n");
              arg1 = GetNum("CHECKMAPTRIGGERS");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("CHECKMAPTRIGGERS");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKMAPTRIGGERS);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("domaptrigger") {
              vlog_txt("DOMAPTRIGGER\n");
              arg1 = GetNum("DOMAPTRIGGER");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("DOMAPTRIGGER");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("DOMAPTRIGGER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DOMAPTRIGGER);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("dotrigger") {
              vlog_txt("DOTRIGGER\n");
              arg1 = GetNum("DOTRIGGER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DOTRIGGER);
              rom(arg1, 1);
              ec();
            }
            aa("loadvar") {
              vlog_txt("LOADVAR\n");
              arg1 = GetNum("LOADVAR");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOADVAR);
              rom(arg1, 1);
              ec();
            }
            aa("addvar") {
              vlog_txt("ADDVAR\n");
              arg1 = GetNum("ADDVAR");
              if (!gffs) {
                return;
              }
              BASIC(CRY_ADDVAR);
              rom(arg1, 1);
              ec();
            }
            aa("random") {
              vlog_txt("RANDOM\n");
              arg1 = GetNum("RANDOM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_RANDOM);
              rom(arg1, 1);
              ec();
            }
            aa("copybytetovar") {
              vlog_txt("COPYBYTETOVAR\n");
              arg1 = GetNum("COPYBYTETOVAR");
              if (!gffs) {
                return;
              }
              BASIC(CRY_COPYBYTETOVAR);
              rom(arg1, 2);
              ec();
            }
            aa("copyvartobyte") {
              vlog_txt("COPYVARTOBYTE\n");
              arg1 = GetNum("COPYVARTOBYTE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_COPYVARTOBYTE);
              rom(arg1, 2);
              ec();
            }
            aa("checkcode") {
              vlog_txt("CHECKCODE\n");
              arg1 = GetNum("CHECKCODE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKCODE);
              rom(arg1, 1);
              ec();
            }
            aa("writevarcode") {
              vlog_txt("WRITEVARCODE\n");
              arg1 = GetNum("WRITEVARCODE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WRITEVARCODE);
              rom(arg1, 1);
              ec();
            }
            aa("writecode") {
              vlog_txt("WRITECODE\n");
              arg1 = GetNum("WRITECODE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WRITECODE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WRITECODE);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("giveitem") {
              vlog_txt("GIVEITEM\n");
              arg1 = GetNum("GIVEITEM");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("GIVEITEM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_GIVEITEM);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("takeitem") {
              vlog_txt("TAKEITEM\n");
              arg1 = GetNum("TAKEITEM");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("TAKEITEM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TAKEITEM);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("givemoney") {
              vlog_txt("GIVEMONEY\n");
              arg1 = GetNum("GIVEMONEY");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("GIVEMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CRY_GIVEMONEY);
              rom(arg1, 1);
              rom(arg2, 3);
              ec();
            }
            aa("takemoney") {
              vlog_txt("TAKEMONEY\n");
              arg1 = GetNum("TAKEMONEY");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("TAKEMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TAKEMONEY);
              rom(arg1, 1);
              rom(arg2, 3);
              ec();
            }
            aa("checkmoney") {
              vlog_txt("CHECKMONEY\n");
              arg1 = GetNum("CHECKMONEY");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("CHECKMONEY");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKMONEY);
              rom(arg1, 1);
              rom(arg2, 3);
              ec();
            }
            aa("checkitem") {
              vlog_txt("CHECKITEM\n");
              arg1 = GetNum("CHECKITEM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKITEM);
              rom(arg1, 1);
              ec();
            }
            aa("givephonenumber") {
              vlog_txt("GIVEPHONENUMBER\n");
              arg1 = GetNum("GIVEPHONENUMBER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_GIVEPHONENUMBER);
              rom(arg1, 1);
              ec();
            }
            aa("takephonenumber") {
              vlog_txt("TAKEPHONENUMBER\n");
              arg1 = GetNum("TAKEPHONENUMBER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TAKEPHONENUMBER);
              rom(arg1, 1);
              ec();
            }
            aa("checkphonenumber") {
              vlog_txt("CHECKPHONENUMBER\n");
              arg1 = GetNum("CHECKPHONENUMBER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKPHONENUMBER);
              rom(arg1, 1);
              ec();
            }
            aa("checktime") {
              vlog_txt("CHECKTIME\n");
              arg1 = GetNum("CHECKTIME");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKTIME);
              rom(arg1, 1);
              ec();
            }
            aa("checkpoke") {
              vlog_txt("CHECKPOKE\n");
              arg1 = GetNum("CHECKPOKE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKPOKE);
              rom(arg1, 1);
              ec();
            }
            aa("givepoke") {
              vlog_txt("GIVEPOKE\n");
              arg1 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              arg4 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              if (arg4 == 1) {
                arg5 = GetNum("GIVEPOKE");
                if (!gffs) {
                  return;
                }
                arg6 = GetNum("GIVEPOKE");
                if (!gffs) {
                  return;
                }
              }
              BASIC(CRY_GIVEPOKE);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              rom(arg4, 1);
              if (arg4 == 1) {
                rom(arg5, 1);
                rom(arg6, 1);
              }
              ec();
            }
            aa("giveegg") {
              vlog_txt("GIVEEGG\n");
              arg1 = GetNum("GIVEEGG");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("GIVEEGG");
              if (!gffs) {
                return;
              }
              BASIC(CRY_GIVEEGG);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("givepokeitem") {
              vlog_txt("GIVEPOKEITEM\n");
              arg1 = GetNum("GIVEPOKEITEM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_GIVEPOKEITEM);
              rom(arg1, 2);
              ec();
            }
            aa("takeifletter") {
              vlog_txt("TAKEIFLETTER\n");
              arg1 = GetNum("TAKEIFLETTER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TAKEIFLETTER);
              rom(arg1, 2);
              ec();
            }
            aa("checkbit1") {
              vlog_txt("CHECKBIT1\n");
              arg1 = GetNum("CHECKBIT1");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKBIT1);
              rom(arg1, 2);
              ec();
            }
            aa("clearbit1") {
              vlog_txt("CLEARBIT1\n");
              arg1 = GetNum("CLEARBIT1");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CLEARBIT1);
              rom(arg1, 2);
              ec();
            }
            aa("setbit1") {
              vlog_txt("SETBIT1\n");
              arg1 = GetNum("SETBIT1");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SETBIT1);
              rom(arg1, 2);
              ec();
            }
            aa("checkbit2") {
              vlog_txt("CHECKBIT2\n");
              arg1 = GetNum("CHECKBIT2");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKBIT2);
              rom(arg1, 2);
              ec();
            }
            aa("clearbit2") {
              vlog_txt("CLEARBIT2\n");
              arg1 = GetNum("CLEARBIT2");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CLEARBIT2);
              rom(arg1, 2);
              ec();
            }
            aa("setbit2") {
              vlog_txt("SETBIT2\n");
              arg1 = GetNum("SETBIT2");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SETBIT2);
              rom(arg1, 2);
              ec();
            }
            aa("givecoins") {
              vlog_txt("GIVECOINS\n");
              arg1 = GetNum("GIVECOINS");
              if (!gffs) {
                return;
              }
              BASIC(CRY_GIVECOINS);
              rom(arg1, 2);
              ec();
            }
            aa("takecoins") {
              vlog_txt("TAKECOINS\n");
              arg1 = GetNum("TAKECOINS");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TAKECOINS);
              rom(arg1, 2);
              ec();
            }
            aa("checkcoins") {
              vlog_txt("CHECKCOINS\n");
              arg1 = GetNum("CHECKCOINS");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHECKCOINS);
              rom(arg1, 2);
              ec();
            }
            aa("xycompare") {
              vlog_txt("XYCOMPARE\n");
              arg1 = GetNum("XYCOMPARE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_XYCOMPARE);
              rom(arg1, 2);
              ec();
            }
            aa("warpmod") {
              vlog_txt("WARPMOD\n");
              arg1 = GetNum("WARPMOD");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WARPMOD");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("WARPMOD");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WARPMOD);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("warp") {
              vlog_txt("WARP\n");
              arg1 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              arg4 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WARP);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              rom(arg4, 1);
              ec();
            }
            aa("warpfacing") {
              vlog_txt("WARPFACING\n");
              arg1 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              arg4 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              arg5 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WARPFACING);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              rom(arg4, 1);
              rom(arg5, 1);
              ec();
            }
            aa("blackoutmod") {
              vlog_txt("BLACKOUTMOD\n");
              arg1 = GetNum("BLACKOUTMOD");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("BLACKOUTMOD");
              if (!gffs) {
                return;
              }
              BASIC(CRY_BLACKOUTMOD);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("moneytotext") {
              vlog_txt("MONEYTOTEXT\n");
              arg1 = GetNum("MONEYTOTEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("MONEYTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_MONEYTOTEXT);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("coinstotext") {
              vlog_txt("COINSTOTEXT\n");
              arg1 = GetNum("COINSTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_COINSTOTEXT);
              rom(arg1, 1);
              ec();
            }
            aa("vartotext") {
              vlog_txt("VARTOTEXT\n");
              arg1 = GetNum("VARTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_VARTOTEXT);
              rom(arg1, 1);
              ec();
            }
            aa("poketotext") {
              vlog_txt("POKETOTEXT\n");
              arg1 = GetNum("POKETOTEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("POKETOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_POKETOTEXT);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("itemtotext") {
              vlog_txt("ITEMTOTEXT\n");
              arg1 = GetNum("ITEMTOTEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("ITEMTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_ITEMTOTEXT);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("locationtotext") {
              vlog_txt("LOCATIONTOTEXT\n");
              arg1 = GetNum("LOCATIONTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOCATIONTOTEXT);
              rom(arg1, 1);
              ec();
            }
            aa("trainertotext") {
              vlog_txt("TRAINERTOTEXT\n");
              arg1 = GetNum("TRAINERTOTEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("TRAINERTOTEXT");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("TRAINERTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TRAINERTOTEXT);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("stringtotext") {
              vlog_txt("STRINGTOTEXT\n");
              arg1 = GetNum("STRINGTOTEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("STRINGTOTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_STRINGTOTEXT);
              rom(arg1, 2);
              rom(arg2, 1);
              ec();
            }
            aa("refreshscreen") {
              vlog_txt("REFRESHSCREEN\n");
              arg1 = GetNum("REFRESHSCREEN");
              if (!gffs) {
                return;
              }
              BASIC(CRY_REFRESHSCREEN);
              rom(arg1, 1);
              ec();
            }
            aa("c1celoadbyte") {
              vlog_txt("C1CELOADBYTE\n");
              arg1 = GetNum("C1CELOADBYTE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_C1CELOADBYTE);
              rom(arg1, 1);
              ec();
            }
            aa("3writetext") {
              vlog_txt("3WRITETEXT\n");
              arg1 = GetNum("3WRITETEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_3WRITETEXT);
              rom(arg1, 3);
              ec();
            }
            aa("2writetext") {
              vlog_txt("2WRITETEXT\n");
              arg1 = GetNum("2WRITETEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_2WRITETEXT);
              rom(arg1, 2);
              ec();
            }
            aa("repeattext") {
              vlog_txt("REPEATTEXT\n");
              arg1 = GetNum("REPEATTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_REPEATTEXT);
              rom(arg1, 2);
              ec();
            }
            aa("loadmenudata") {
              vlog_txt("LOADMENUDATA\n");
              arg1 = GetNum("LOADMENUDATA");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOADMENUDATA);
              rom(arg1, 2);
              ec();
            }
            aa("jumptextfaceplayer") {
              vlog_txt("JUMPTEXTFACEPLAYER\n");
              arg1 = GetNum("JUMPTEXTFACEPLAYER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_JUMPTEXTFACEPLAYER);
              rom(arg1, 2);
              ec();
            }
            aa("jumptext") {
              vlog_txt("JUMPTEXT\n");
              arg1 = GetNum("JUMPTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_JUMPTEXT);
              rom(arg1, 2);
              ec();
            }
            aa("pokepic") {
              vlog_txt("POKEPIC\n");
              arg1 = GetNum("POKEPIC");
              if (!gffs) {
                return;
              }
              BASIC(CRY_POKEPIC);
              rom(arg1, 1);
              ec();
            }
            aa("loadpokedata") {
              vlog_txt("LOADPOKEDATA\n");
              arg1 = GetNum("LOADPOKEDATA");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("LOADPOKEDATA");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOADPOKEDATA);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("loadtrainer") {
              vlog_txt("LOADTRAINER\n");
              arg1 = GetNum("LOADTRAINER");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("LOADTRAINER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOADTRAINER);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("catchtutorial") {
              vlog_txt("CATCHTUTORIAL\n");
              arg1 = GetNum("CATCHTUTORIAL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CATCHTUTORIAL);
              rom(arg1, 1);
              ec();
            }
            aa("trainertext") {
              vlog_txt("TRAINERTEXT\n");
              arg1 = GetNum("TRAINERTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TRAINERTEXT);
              rom(arg1, 1);
              ec();
            }
            aa("trainerstatus") {
              vlog_txt("TRAINERSTATUS\n");
              arg1 = GetNum("TRAINERSTATUS");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TRAINERSTATUS);
              rom(arg1, 1);
              ec();
            }
            aa("winlosstext") {
              vlog_txt("WINLOSSTEXT\n");
              arg1 = GetNum("WINLOSSTEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WINLOSSTEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WINLOSSTEXT);
              rom(arg1, 2);
              rom(arg2, 2);
              ec();
            }
            aa("setlasttalked") {
              vlog_txt("SETLASTTALKED\n");
              arg1 = GetNum("SETLASTTALKED");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SETLASTTALKED);
              rom(arg1, 1);
              ec();
            }
            aa("applymovement") {
              vlog_txt("APPLYMOVEMENT\n");
              arg1 = GetNum("APPLYMOVEMENT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("APPLYMOVEMENT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_APPLYMOVEMENT);
              rom(arg1, 1);
              rom(arg2, 2);
              ec();
            }
            aa("applymoveother") {
              vlog_txt("APPLYMOVEOTHER\n");
              arg1 = GetNum("APPLYMOVEOTHER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_APPLYMOVEOTHER);
              rom(arg1, 2);
              ec();
            }
            aa("faceperson") {
              vlog_txt("FACEPERSON\n");
              arg1 = GetNum("FACEPERSON");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("FACEPERSON");
              if (!gffs) {
                return;
              }
              BASIC(CRY_FACEPERSON);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("variablesprite") {
              vlog_txt("VARIABLESPRITE\n");
              arg1 = GetNum("VARIABLESPRITE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("VARIABLESPRITE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_VARIABLESPRITE);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("disappear") {
              vlog_txt("DISAPPEAR\n");
              arg1 = GetNum("DISAPPEAR");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DISAPPEAR);
              rom(arg1, 1);
              ec();
            }
            aa("appear") {
              vlog_txt("APPEAR\n");
              arg1 = GetNum("APPEAR");
              if (!gffs) {
                return;
              }
              BASIC(CRY_APPEAR);
              rom(arg1, 1);
              ec();
            }
            aa("follow") {
              vlog_txt("FOLLOW\n");
              arg1 = GetNum("FOLLOW");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("FOLLOW");
              if (!gffs) {
                return;
              }
              BASIC(CRY_FOLLOW);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("moveperson") {
              vlog_txt("MOVEPERSON\n");
              arg1 = GetNum("MOVEPERSON");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("MOVEPERSON");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("MOVEPERSON");
              if (!gffs) {
                return;
              }
              BASIC(CRY_MOVEPERSON);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("writepersonloc") {
              vlog_txt("WRITEPERSONLOC\n");
              arg1 = GetNum("WRITEPERSONLOC");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WRITEPERSONLOC);
              rom(arg1, 1);
              ec();
            }
            aa("loademote") {
              vlog_txt("LOADEMOTE\n");
              arg1 = GetNum("LOADEMOTE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOADEMOTE);
              rom(arg1, 1);
              ec();
            }
            aa("showemote") {
              vlog_txt("SHOWEMOTE\n");
              arg1 = GetNum("SHOWEMOTE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("SHOWEMOTE");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("SHOWEMOTE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SHOWEMOTE);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("spriteface") {
              vlog_txt("SPRITEFACE\n");
              arg1 = GetNum("SPRITEFACE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("SPRITEFACE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SPRITEFACE);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("follownotexact") {
              vlog_txt("FOLLOWNOTEXACT\n");
              arg1 = GetNum("FOLLOWNOTEXACT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("FOLLOWNOTEXACT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_FOLLOWNOTEXACT);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("earthquake") {
              vlog_txt("EARTHQUAKE\n");
              arg1 = GetNum("EARTHQUAKE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_EARTHQUAKE);
              rom(arg1, 1);
              ec();
            }
            aa("swapmaps") {
              vlog_txt("SWAPMAPS\n");
              arg1 = GetNum("SWAPMAPS");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SWAPMAPS);
              rom(arg1, 3);
              ec();
            }
            aa("changeblock") {
              vlog_txt("CHANGEBLOCK\n");
              arg1 = GetNum("CHANGEBLOCK");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("CHANGEBLOCK");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("CHANGEBLOCK");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CHANGEBLOCK);
              rom(arg1, 1);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("writecmdqueue") {
              vlog_txt("WRITECMDQUEUE\n");
              arg1 = GetNum("WRITECMDQUEUE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WRITECMDQUEUE);
              rom(arg1, 2);
              ec();
            }
            aa("delcmdqueue") {
              vlog_txt("DELCMDQUEUE\n");
              arg1 = GetNum("DELCMDQUEUE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DELCMDQUEUE);
              rom(arg1, 1);
              ec();
            }
            aa("playmusic") {
              vlog_txt("PLAYMUSIC\n");
              arg1 = GetNum("PLAYMUSIC");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PLAYMUSIC);
              rom(arg1, 2);
              ec();
            }
            aa("musicfadeout") {
              vlog_txt("MUSICFADEOUT\n");
              arg1 = GetNum("MUSICFADEOUT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("MUSICFADEOUT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_MUSICFADEOUT);
              rom(arg1, 2);
              rom(arg2, 1);
              ec();
            }
            aa("cry") {
              vlog_txt("CRY\n");
              arg1 = GetNum("CRY");
              if (!gffs) {
                return;
              }
              BASIC(CRY_CRY);
              rom(arg1, 2);
              ec();
            }
            aa("playsound") {
              vlog_txt("PLAYSOUND\n");
              arg1 = GetNum("PLAYSOUND");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PLAYSOUND);
              rom(arg1, 2);
              ec();
            }
            aa("passtoengine") {
              vlog_txt("PASSTOENGINE\n");
              arg1 = GetNum("PASSTOENGINE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PASSTOENGINE);
              rom(arg1, 3);
              ec();
            }
            aa("newloadmap") {
              vlog_txt("NEWLOADMAP\n");
              arg1 = GetNum("NEWLOADMAP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_NEWLOADMAP);
              rom(arg1, 1);
              ec();
            }
            aa("pause") {
              vlog_txt("PAUSE\n");
              arg1 = GetNum("PAUSE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PAUSE);
              rom(arg1, 1);
              ec();
            }
            aa("deactivatefacing") {
              vlog_txt("DEACTIVATEFACING\n");
              arg1 = GetNum("DEACTIVATEFACING");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DEACTIVATEFACING);
              rom(arg1, 1);
              ec();
            }
            aa("priorityjump") {
              vlog_txt("PRIORITYJUMP\n");
              arg1 = GetNum("PRIORITYJUMP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PRIORITYJUMP);
              rom(arg1, 2);
              ec();
            }
            aa("ptpriorityjump") {
              vlog_txt("PTPRIORITYJUMP\n");
              arg1 = GetNum("PTPRIORITYJUMP");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PTPRIORITYJUMP);
              rom(arg1, 2);
              ec();
            }
            aa("pokemart") {
              vlog_txt("POKEMART\n");
              arg1 = GetNum("POKEMART");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("POKEMART");
              if (!gffs) {
                return;
              }
              BASIC(CRY_POKEMART);
              rom(arg1, 1);
              rom(arg2, 2);
              ec();
            }
            aa("elevator") {
              vlog_txt("ELEVATOR\n");
              arg1 = GetNum("ELEVATOR");
              if (!gffs) {
                return;
              }
              BASIC(CRY_ELEVATOR);
              rom(arg1, 2);
              ec();
            }
            aa("trade") {
              vlog_txt("TRADE\n");
              arg1 = GetNum("TRADE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_TRADE);
              rom(arg1, 1);
              ec();
            }
            aa("askforphonenumber") {
              vlog_txt("ASKFORPHONENUMBER\n");
              arg1 = GetNum("ASKFORPHONENUMBER");
              if (!gffs) {
                return;
              }
              BASIC(CRY_ASKFORPHONENUMBER);
              rom(arg1, 1);
              ec();
            }
            aa("phonecall") {
              vlog_txt("PHONECALL\n");
              arg1 = GetNum("PHONECALL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_PHONECALL);
              rom(arg1, 2);
              ec();
            }
            aa("describedecoration") {
              vlog_txt("DESCRIBEDECORATION\n");
              arg1 = GetNum("DESCRIBEDECORATION");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DESCRIBEDECORATION);
              rom(arg1, 1);
              ec();
            }
            aa("fruittree") {
              vlog_txt("FRUITTREE\n");
              arg1 = GetNum("FRUITTREE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_FRUITTREE);
              rom(arg1, 1);
              ec();
            }
            aa("specialphonecall") {
              vlog_txt("SPECIALPHONECALL\n");
              arg1 = GetNum("SPECIALPHONECALL");
              if (!gffs) {
                return;
              }
              BASIC(CRY_SPECIALPHONECALL);
              rom(arg1, 1);
              ec();
            }
            aa("verbosegiveitem") {
              vlog_txt("VERBOSEGIVEITEM\n");
              arg1 = GetNum("VERBOSEGIVEITEM");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("VERBOSEGIVEITEM");
              if (!gffs) {
                return;
              }
              BASIC(CRY_VERBOSEGIVEITEM);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("loadwilddata") {
              vlog_txt("LOADWILDDATA\n");
              arg1 = GetNum("LOADWILDDATA");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("LOADWILDDATA");
              if (!gffs) {
                return;
              }
              BASIC(CRY_LOADWILDDATA);
              rom(arg1, 1);
              rom(arg2, 1);
              ec();
            }
            aa("storetext") {
              vlog_txt("STORETEXT\n");
              arg1 = GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              arg3 = GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              BASIC(CRY_STORETEXT);
              rom(arg1, 2);
              rom(arg2, 1);
              rom(arg3, 1);
              ec();
            }
            aa("displaylocation") {
              vlog_txt("DISPLAYLOCATION\n");
              arg1 = GetNum("DISPLAYLOCATION");
              if (!gffs) {
                return;
              }
              BASIC(CRY_DISPLAYLOCATION);
              rom(arg1, 1);
              ec();
            }
            aa("writebyte") {
              vlog_txt("WRITEBYTE\n");
              arg1 = GetNum("WRITEBYTE");
              if (!gffs) {
                return;
              }
              arg2 = GetNum("WRITEBYTE");
              if (!gffs) {
                return;
              }
              BASIC(CRY_WRITEBYTE);
              rom(arg1, 2);
              rom(arg2, 1);
              ec();
            }
            try_asm();
            if (*buf == ':') {
              if (c) add_label(buf, c, c->size, &cl);
              ec();
            }
            aa("m") {
              vlog_txt("Movement data...\n");
              add_data(c, trans, transbackmove(Script, &i));
              ec();
            }
            aa("=") {
              vlog_txt("[STRING]\n");
              if (chr == ' ') {
                i++;
              } else {
                log_txt("Should have a space after the =\n", 33 - 1);
              }
              temp_ptr = transbackstr(script, i - fst, c);
              while (chr != '\n' && chr != 0) {
                i++;
              }
              sprintf(buf2, "   -> %s\n", (char *)temp_ptr);
              free(temp_ptr);
              vlog_txt(buf2);
            }
            aa(".") {
              vlog_txt("[BINARY]\n   ->");
              i = skip_whitespace(Script, i);
              k = 0;
              while (chr != '\n' && chr != 0) {
                k = 1 - k;
                i = skip_whitespace(Script, i);
                j = 0;
                while (((char *)("0123456789abcdef"))[j] != 0) {
                  if (((char *)("0123456789abcdef"))[j] == chr) {
                    break;
                  }
                  j += 1;
                }
                if (((char *)("0123456789abcdef"))[j] == 0) {
                  sprintf(buf2, "Failed to understand hex character '%c'\n",
                          chr);
                  log_txt(buf2, strlen(buf2));
                  return;
                }
                if (k == 0) {
                  l |= j;
                  rom(j, 1);
                  if (IsVerbose) {
                    sprintf(buf2, " %02X", l);
                    log_txt(buf2, strlen(buf2));
                  }
                } else {
                  l = j << 4;
                }
                i++;
              }
              vlog_txt("\n");
            }
            else {
              sprintf(buf2, "Warning: Unknown command \"%s\"\n", buf);
              log_txt(buf2, strlen(buf2));
              while (chr != '\n' && chr != 0) {
                i++;
              }
            }
            break;
        }
        i++;
      }
    } else if (mode == GOLD) {
      while (i < scriptlen) {
        switch (chr)  // Behave differently according to char
        {
          case 0:  // E O Script
            return;
          case '\'':
            while (chr != '\n' && chr != 0) {
              i++;
            }
            i--;
            break;
          case ' ':  // Ignore spaces
          case '\n':
            break;
          default:
            j = 0;
            while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
              buf[j] = chr;
              i++;
              j++;
            }
            buf[j] = 0;
            if (!strcmp(buf, "#define") || !strcmp(buf, "#d")) {
              vlog_txt("#DEFINE\n");
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Premature end to #DEFINE!\n", 27 - 1);
                return;
              }
              j = 0;
              while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;  // Append null
              sprintf(buf2, "   -> %s\n", buf);
              vlog_txt(buf2);
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Premature end to #DEFINE!\n", 27 - 1);
                return;
              }
              k = GetNum("#DEFINE");
              if (!gffs) {
                return;
              }
              bsearch_upsert(defines, buf, (void *)(intptr_t)k);
              ec();
            }
            aa("#dyn") {
              vlog_txt("#DYN\n");
              start = GetNum("#DYN");
              if (!gffs) return;
              dyntype = 0;
              bsearch_upsert(defines, "findfromgold", (void *)(intptr_t)start);
              ec();
            }
            aa("#dyn2") {
              vlog_txt("#DYN2\n");
              start = GetNum("#DYN2");
              if (!gffs) return;
              dyntype = 1;
              ec();
            }
            aa("#dynamic") {
              vlog_txt("#DYNAMIC\n");
              start = GetNum("#DYNAMIC");
              if (!gffs) return;
              dyntype = 0;
              bsearch_upsert(defines, "findfromgold", (void *)(intptr_t)start);
              ec();
            }
            aa("#org") {
              eorg = 0;
              vlog_txt("#ORG\n");
              i = skip_whitespace(Script, i);
              buf[0] = 0;
              if (chr == '@') {
                if (start == 0) {
                  log_txt("Need #dyn with dynamic offsets\n", 32 - 1);
                  return;
                }
                dynu = 1;
                j = 0;
                while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                  buf[j] = chr;
                  i++;
                  j++;
                }
                buf[j] = 0;
                sprintf(buf2, "   -> %s\n", buf);
                vlog_txt(buf2);
              } else {
                k = GetNum("#ORG");
                if (!gffs) {
                  return;
                }
              }
              d = malloc(sizeof(codeblock));
              if (*buf == 0)
                init_codeblock(d, NULL, k);
              else
                init_codeblock(d, buf, (start & 0x0FFFFFFF) | (dyntype << 24));
              d->prev = c;
              if (c != NULL) c->next = d;
              c = d;
              ec();
            }
            aa("#eorg") {
              eorg = 1;
              vlog_txt("#EORG\n");
              i = skip_whitespace(Script, i);
              buf[0] = 0;
              if (chr == '@') {
                j = 0;
                while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                  buf[j] = chr;
                  i++;
                  j++;
                }
                buf[j] = 0;
                sprintf(buf2, "   -> %s\n", buf);
                vlog_txt(buf2);
              } else {
                k = GetNum("#EORG");
                if (!gffs) {
                  return;
                }
              }
              d = malloc(sizeof(codeblock));
              if (*buf == 0)
                init_codeblock(d, NULL, k);
              else
                init_codeblock(d, buf, (start & 0x0FFFFFFF) | (dyntype << 24));
              d->prev = c;
              if (c != NULL) c->next = d;
              c = d;
              ec();
            }
            aa("#quiet") {
              IsVerbose = 0;
              ec();
            }
            aa("#loud") {
              IsVerbose = 1;
              ec();
            }
            aa("#thumb") {
              thumb = 1;
              ec();
            }
            aa("#arm") {
              thumb = 0;
              ec();
            }
            aa("#gs") {
              mode = GOLD;
            gsc:
              ec();
            }
            aa("#c") {
              mode = CRYSTAL;
              goto cry;
            }
            aa("#frlg") {
              mode = FIRE_RED;
              goto frlg;
            }
            aa("#rse") {
              mode = RUBY;
              goto rse;
            }
            aa("#dp") {
              mode = DIAMOND;
              goto dp;
            }
            aa("#raw") {
              vlog_txt("#RAW\n");
              arg1 = 1;
              while (arg1) {
                k = GetNum("#RAW");
                if (!gffs) {
                  return;
                }
                BASIC(k);
                i = skip_whitespace(Script, i);
                arg1 = (chr >= '0' && chr <= '9') || chr == '$';
              }
              ec();
            }
            else if (!strcmp(buf, "#2ptr") || !strcmp(buf, "#word")) {
              vlog_txt("#2PTR\n");
              arg1 = 1;
              while (arg1) {
                k = GetNum("#2PTR");
                if (!gffs) {
                  return;
                }
                rom(k, 2);
                i = skip_whitespace(Script, i);
                arg1 = (chr >= '0' && chr <= '9') || chr == '$';
              }
              ec();
            }
            aa("#3ptr") {
              vlog_txt("#3PTR\n");
              arg1 = 1;
              while (arg1) {
                k = GetNum("#3PTR");
                if (!gffs) {
                  return;
                }
                rom(k, 3);
                i = skip_whitespace(Script, i);
                arg1 = (chr >= '0' && chr <= '9') || chr == '$';
              }
              ec();
            }
            aa("#dword") {
              vlog_txt("#DWORD\n");
              arg1 = 1;
              while (arg1) {
                k = GetNum("#DWORD");
                if (!gffs) {
                  return;
                }
                rom(k, 4);
                i = skip_whitespace(Script, i);
                arg1 = (chr >= '0' && chr <= '9') || chr == '$';
              }
              ec();
            }
            aa("end") {
              BASIC(GLD_END);
              ec();
            }
            aa("return") {
              BASIC(GLD_RETURN);
              ec();
            }
            aa("reloadandreturn") {
              BASIC(GLD_RELOADANDRETURN);
              ec();
            }
            aa("checktriggers") {
              BASIC(GLD_CHECKTRIGGERS);
              ec();
            }
            aa("checkver") {
              BASIC(GLD_CHECKVER);
              ec();
            }
            aa("wildoff") {
              BASIC(GLD_WILDOFF);
              ec();
            }
            aa("wildon") {
              BASIC(GLD_WILDON);
              ec();
            }
            aa("itemnotify") {
              BASIC(GLD_ITEMNOTIFY);
              ec();
            }
            aa("pocketisfull") {
              BASIC(GLD_POCKETISFULL);
              ec();
            }
            aa("loadfont") {
              BASIC(GLD_LOADFONT);
              ec();
            }
            aa("loadmovesprites") {
              BASIC(GLD_LOADMOVESPRITES);
              ec();
            }
            aa("release") {
              BASIC(GLD_LOADMOVESPRITES);
              ec();
            }
            aa("yesorno") {
              BASIC(GLD_YESORNO);
              ec();
            }
            aa("writebackup") {
              BASIC(GLD_WRITEBACKUP);
              ec();
            }
            aa("closetext") {
              BASIC(GLD_CLOSETEXT);
              ec();
            }
            aa("keeptextopen") {
              BASIC(GLD_KEEPTEXTOPEN);
              ec();
            }
            aa("pokepicyesorno") {
              BASIC(GLD_POKEPICYESORNO);
              ec();
            }
            aa("interpretmenu") {
              BASIC(GLD_INTERPRETMENU);
              ec();
            }
            aa("interpretmenu2") {
              BASIC(GLD_INTERPRETMENU2);
              ec();
            }
            aa("loadpikadata") {
              BASIC(GLD_LOADPIKADATA);
              ec();
            }
            aa("clearfight") {
              BASIC(GLD_CLEARFIGHT);
              ec();
            }
            aa("loadtrainerseen") {
              BASIC(GLD_LOADTRAINERSEEN);
              ec();
            }
            aa("startbattle") {
              BASIC(GLD_STARTBATTLE);
              ec();
            }
            aa("returnafterbattle") {
              BASIC(GLD_RETURNAFTERBATTLE);
              ec();
            }
            aa("talkafter") {
              BASIC(GLD_TALKAFTER);
              ec();
            }
            aa("talkaftercancel") {
              BASIC(GLD_TALKAFTERCANCEL);
              ec();
            }
            aa("talkaftercheck") {
              BASIC(GLD_TALKAFTERCHECK);
              ec();
            }
            aa("faceplayer") {
              BASIC(GLD_FACEPLAYER);
              ec();
            }
            aa("stopfollow") {
              BASIC(GLD_STOPFOLLOW);
              ec();
            }
            aa("reloadmap") {
              BASIC(GLD_RELOADMAP);
              ec();
            }
            aa("reloadmappart") {
              BASIC(GLD_RELOADMAPPART);
              ec();
            }
            aa("playrammusic") {
              BASIC(GLD_PLAYRAMMUSIC);
              ec();
            }
            aa("playmapmusic") {
              BASIC(GLD_PLAYMAPMUSIC);
              ec();
            }
            aa("reloadmapmusic") {
              BASIC(GLD_RELOADMAPMUSIC);
              ec();
            }
            aa("waitbutton") {
              BASIC(GLD_WAITBUTTON);
              ec();
            }
            aa("warpsound") {
              BASIC(GLD_WARPSOUND);
              ec();
            }
            aa("specialsound") {
              BASIC(GLD_SPECIALSOUND);
              ec();
            }
            aa("warpcheck") {
              BASIC(GLD_WARPCHECK);
              ec();
            }
            aa("resetfuncs") {
              BASIC(GLD_RESETFUNCS);
              ec();
            }
            aa("hangup") {
              BASIC(GLD_HANGUP);
              ec();
            }
            aa("checkphonecall") {
              BASIC(GLD_CHECKPHONECALL);
              ec();
            }
            aa("halloffame") {
              BASIC(GLD_HALLOFFAME);
              ec();
            }
            aa("credits") {
              BASIC(GLD_CREDITS);
              ec();
            }
            aa("text-newline") {
              BASIC(5);
              ec();
            }
            aa("text-waitbutton") {
              BASIC(6);
              ec();
            }
            aa("text-newlinewitharrow") {
              BASIC(7);
              ec();
            }
            aa("text-switchtoasm") {
              BASIC(8);
              ec();
            }
            aa("text-interpretdata") {
              BASIC(10);
              ec();
            }
            aa("text-playsound0") {
              BASIC(11);
              ec();
            }
            aa("text-waitbutton2") {
              BASIC(13);
              ec();
            }
            aa("text-playsound9") {
              BASIC(14);
              ec();
            }
            aa("text-playsound1") {
              BASIC(15);
              ec();
            }
            aa("text-playsound2") {
              BASIC(16);
              ec();
            }
            aa("text-playsounda") {
              BASIC(17);
              ec();
            }
            aa("text-playsoundd") {
              BASIC(18);
              ec();
            }
            aa("text-playsoundc") {
              BASIC(19);
              ec();
            }
            aa("text-day") {
              BASIC(21);
              ec();
            }
            aa("2call") {
              vlog_txt("2CALL\n");
              BASIC(GLD_2CALL);
              arg1 = GetNum("2CALL");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("3call") {
              vlog_txt("3CALL\n");
              BASIC(GLD_3CALL);
              arg1 = GetNum("3CALL");
              if (!gffs) {
                return;
              }
              rom(arg1, 3);
              ec();
            }
            aa("2ptcall") {
              vlog_txt("2PTCALL\n");
              BASIC(GLD_2PTCALL);
              arg1 = GetNum("2PTCALL");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("2jump") {
              vlog_txt("2JUMP\n");
              BASIC(GLD_2JUMP);
              arg1 = GetNum("2JUMP");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("3jump") {
              vlog_txt("3JUMP\n");
              BASIC(GLD_3JUMP);
              arg1 = GetNum("3JUMP");
              if (!gffs) {
                return;
              }
              rom(arg1, 3);
              ec();
            }
            aa("2ptjump") {
              vlog_txt("2PTJUMP\n");
              BASIC(GLD_2PTJUMP);
              arg1 = GetNum("2PTJUMP");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("text-ram") {
              vlog_txt("TEXT-RAM\n");
              BASIC(1);
              arg1 = GetNum("TEXT-RAM");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("text-reloc") {
              vlog_txt("TEXT-RELOC\n");
              BASIC(3);
              arg1 = GetNum("TEXT-RELOC");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("text-box") {
              vlog_txt("TEXT-BOX\n");
              BASIC(4);
              arg1 = GetNum("TEXT-BOX");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("TEXT-BOX");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("TEXT-BOX");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("text-interpretxdata") {
              vlog_txt("TEXT-INTERPRETXDATA\n");
              BASIC(12);
              arg1 = GetNum("TEXT-INTERPRETXDATA");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("text-buffer") {
              vlog_txt("TEXT-BUFFER\n");
              BASIC(20);
              arg1 = GetNum("TEXT-BUFFER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("text-newtxt") {
              vlog_txt("TEXT-NEWTXT\n");
              BASIC(22);
              arg1 = GetNum("TEXT-NEWTXT");
              if (!gffs) {
                return;
              }
              rom(arg1 >> 8, 2);
              rom(arg1, 1);
              ec();
            }
            aa("text-number") {
              vlog_txt("TEXT-NUMBER\n");
              BASIC(9);
              arg1 = GetNum("TEXT-NUMBER");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = (0xF & GetNum("TEXT-NUMBER")) << 4;
              if (!gffs) {
                return;
              }
              arg2 |= 0xF & GetNum("TEXT-NUMBER");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("text-hex") {
              vlog_txt("TEXT-HEX\n");
              BASIC(2);
              arg1 = GetNum("TEXT-HEX");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("TEXT-HEX");
              if (!gffs) {
                return;
              }
              arg2 &= 0x1F;
              i = skip_whitespace(Script, i);
              j = 0;
              while (chr != '\n' && chr != ' ' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;
              if (!strcmp(buf, "money-no-zeros")) {
                arg2 |= (7 << 5);
              } else if (!strcmp(buf, "no-zeros")) {
                arg2 |= (6 << 5);
              } else if (!strcmp(buf, "money-spaces")) {
                arg2 |= (5 << 5);
              } else if (!strcmp(buf, "spaces")) {
                arg2 |= (4 << 5);
              } else if (!strcmp(buf, "money2")) {
                arg2 |= (3 << 5);
              } else if (!strcmp(buf, "normal2")) {
                arg2 |= (2 << 5);
              } else if (!strcmp(buf, "money")) {
                arg2 |= (1 << 5);
              } else if (!strcmp(buf, "normal")) {
                arg2 |= (0 << 5);
              } else {
                log_txt("Incorrect arguments to text-hex\n", 33 - 1);
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("if") {
              vlog_txt("IF\n");
              i = skip_whitespace(Script, i);
              if (chr == '\n' || chr == 0 || chr == '\'') {
                log_txt("Incorrect arguments to IF.\n", 28 - 1);
              }
              j = 0;
              while (chr != '\n' && chr != ' ' && chr != 0 && chr != '\'') {
                buf[j] = chr;
                i++;
                j++;
              }
              buf[j] = 0;
              arg1 = 0;
              if (!strcmp(buf, "=") || !strcmp(buf, "==")) {
                BASIC(GLD_EQBYTE);
              } else if (!strcmp(buf, "!=") || !strcmp(buf, "<>")) {
                BASIC(GLD_NEQBYTE);
              } else if (!strcmp(buf, "<") || !strcmp(buf, "<<")) {
                BASIC(GLD_LTBYTE);
              } else if (!strcmp(buf, ">") || !strcmp(buf, ">>")) {
                BASIC(GLD_GTBYTE);
              } else if (!strcmp(buf, "==0") || !strcmp(buf, "=0") ||
                         !strcmp(buf, "false")) {
                BASIC(GLD_EQZERO);
                arg1 = -1;
              } else if (!strcmp(buf, "!=0") || !strcmp(buf, "<>0") ||
                         !strcmp(buf, "true")) {
                BASIC(GLD_NEQZERO);
                arg1 = -1;
              } else {
                log_txt("Incorrect arguments to IF\n", 29 - 1);
                return;
              }
              if (arg1 != -1) {
                arg1 = GetNum("IF");
                if (!gffs) {
                  return;
                }
              }
              if (arg1 != -1) rom(arg1, 1);
              arg2 = GetNum("IF");
              if (!gffs) {
                return;
              }
              rom(arg2, 2);
              ec();
            }
            aa("jumpstd") {
              vlog_txt("JUMPSTD\n");
              BASIC(GLD_JUMPSTD);
              arg1 = GetNum("JUMPSTD");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("callstd") {
              vlog_txt("CALLSTD\n");
              BASIC(GLD_CALLSTD);
              arg1 = GetNum("CALLSTD");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("3callasm") {
              vlog_txt("3CALLASM\n");
              BASIC(GLD_3CALLASM);
              arg1 = GetNum("3CALLASM");
              if (!gffs) {
                return;
              }
              rom(arg1, 3);
              ec();
            }
            aa("special") {
              vlog_txt("SPECIAL\n");
              BASIC(GLD_SPECIAL);
              arg1 = GetNum("SPECIAL");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("2ptcallasm") {
              vlog_txt("2PTCALLASM\n");
              BASIC(GLD_2PTCALLASM);
              arg1 = GetNum("2PTCALLASM");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("checkmaptriggers") {
              vlog_txt("CHECKMAPTRIGGERS\n");
              BASIC(GLD_CHECKMAPTRIGGERS);
              arg1 = GetNum("CHECKMAPTRIGGERS");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("CHECKMAPTRIGGERS");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("domaptrigger") {
              vlog_txt("DOMAPTRIGGER\n");
              BASIC(GLD_DOMAPTRIGGER);
              arg1 = GetNum("DOMAPTRIGGER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("DOMAPTRIGGER");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("DOMAPTRIGGER");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("dotrigger") {
              vlog_txt("DOTRIGGER\n");
              BASIC(GLD_DOTRIGGER);
              arg1 = GetNum("DOTRIGGER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("loadvar") {
              vlog_txt("LOADVAR\n");
              BASIC(GLD_LOADVAR);
              arg1 = GetNum("LOADVAR");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("addvar") {
              vlog_txt("ADDVAR\n");
              BASIC(GLD_ADDVAR);
              arg1 = GetNum("ADDVAR");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("random") {
              vlog_txt("RANDOM\n");
              BASIC(GLD_RANDOM);
              arg1 = GetNum("RANDOM");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("copybytetovar") {
              vlog_txt("COPYBYTETOVAR\n");
              BASIC(GLD_COPYBYTETOVAR);
              arg1 = GetNum("COPYBYTETOVAR");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("copyvartobyte") {
              vlog_txt("COPYVARTOBYTE\n");
              BASIC(GLD_COPYVARTOBYTE);
              arg1 = GetNum("COPYVARTOBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("checkcode") {
              vlog_txt("CHECKCODE\n");
              BASIC(GLD_CHECKCODE);
              arg1 = GetNum("CHECKCODE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("writevarcode") {
              vlog_txt("WRITEVARCODE\n");
              BASIC(GLD_WRITEVARCODE);
              arg1 = GetNum("WRITEVARCODE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("writecode") {
              vlog_txt("WRITECODE\n");
              BASIC(GLD_WRITECODE);
              arg1 = GetNum("WRITECODE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("WRITECODE");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("giveitem") {
              vlog_txt("GIVEITEM\n");
              BASIC(GLD_GIVEITEM);
              arg1 = GetNum("GIVEITEM");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("GIVEITEM");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("takeitem") {
              vlog_txt("TAKEITEM\n");
              BASIC(GLD_TAKEITEM);
              arg1 = GetNum("TAKEITEM");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("TAKEITEM");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("givemoney") {
              vlog_txt("GIVEMONEY\n");
              BASIC(GLD_GIVEMONEY);
              arg1 = GetNum("GIVEMONEY");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("GIVEMONEY");
              if (!gffs) {
                return;
              }
              rom(arg2, 3);
              ec();
            }
            aa("takemoney") {
              vlog_txt("TAKEMONEY\n");
              BASIC(GLD_TAKEMONEY);
              arg1 = GetNum("TAKEMONEY");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("TAKEMONEY");
              if (!gffs) {
                return;
              }
              rom(arg2, 3);
              ec();
            }
            aa("checkmoney") {
              vlog_txt("CHECKMONEY\n");
              BASIC(GLD_CHECKMONEY);
              arg1 = GetNum("CHECKMONEY");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("CHECKMONEY");
              if (!gffs) {
                return;
              }
              rom(arg2, 3);
              ec();
            }
            aa("checkitem") {
              vlog_txt("CHECKITEM\n");
              BASIC(GLD_CHECKITEM);
              arg1 = GetNum("CHECKITEM");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("givephonenumber") {
              vlog_txt("GIVEPHONENUMBER\n");
              BASIC(GLD_GIVEPHONENUMBER);
              arg1 = GetNum("GIVEPHONENUMBER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("takephonenumber") {
              vlog_txt("TAKEPHONENUMBER\n");
              BASIC(GLD_TAKEPHONENUMBER);
              arg1 = GetNum("TAKEPHONENUMBER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("checkphonenumber") {
              vlog_txt("CHECKPHONENUMBER\n");
              BASIC(GLD_CHECKPHONENUMBER);
              arg1 = GetNum("CHECKPHONENUMBER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("checktime") {
              vlog_txt("CHECKTIME\n");
              BASIC(GLD_CHECKTIME);
              arg1 = GetNum("CHECKTIME");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("checkpoke") {
              vlog_txt("CHECKPOKE\n");
              BASIC(GLD_CHECKPOKE);
              arg1 = GetNum("CHECKPOKE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("givepoke") {
              vlog_txt("GIVEPOKE\n");
              BASIC(GLD_GIVEPOKE);
              arg1 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              arg4 = GetNum("GIVEPOKE");
              if (!gffs) {
                return;
              }
              if (arg4 == 1) {
                arg5 = GetNum("GIVEPOKE");
                if (!gffs) {
                  return;
                }
                arg6 = GetNum("GIVEPOKE");
                if (!gffs) {
                  return;
                }
              }
              rom(arg4, 1);
              if (arg4 == 1) {
                rom(arg5, 1);
                rom(arg6, 1);
              }
              ec();
            }
            aa("giveegg") {
              vlog_txt("GIVEEGG\n");
              BASIC(GLD_GIVEEGG);
              arg1 = GetNum("GIVEEGG");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("GIVEEGG");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("givepokeitem") {
              vlog_txt("GIVEPOKEITEM\n");
              BASIC(GLD_GIVEPOKEITEM);
              arg1 = GetNum("GIVEPOKEITEM");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("takeifletter") {
              vlog_txt("TAKEIFLETTER\n");
              BASIC(GLD_TAKEIFLETTER);
              arg1 = GetNum("TAKEIFLETTER");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("checkbit1") {
              vlog_txt("CHECKBIT1\n");
              BASIC(GLD_CHECKBIT1);
              arg1 = GetNum("CHECKBIT1");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("clearbit1") {
              vlog_txt("CLEARBIT1\n");
              BASIC(GLD_CLEARBIT1);
              arg1 = GetNum("CLEARBIT1");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("setbit1") {
              vlog_txt("SETBIT1\n");
              BASIC(GLD_SETBIT1);
              arg1 = GetNum("SETBIT1");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("checkbit2") {
              vlog_txt("CHECKBIT2\n");
              BASIC(GLD_CHECKBIT2);
              arg1 = GetNum("CHECKBIT2");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("clearbit2") {
              vlog_txt("CLEARBIT2\n");
              BASIC(GLD_CLEARBIT2);
              arg1 = GetNum("CLEARBIT2");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("setbit2") {
              vlog_txt("SETBIT2\n");
              BASIC(GLD_SETBIT2);
              arg1 = GetNum("SETBIT2");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("givecoins") {
              vlog_txt("GIVECOINS\n");
              BASIC(GLD_GIVECOINS);
              arg1 = GetNum("GIVECOINS");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("takecoins") {
              vlog_txt("TAKECOINS\n");
              BASIC(GLD_TAKECOINS);
              arg1 = GetNum("TAKECOINS");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("checkcoins") {
              vlog_txt("CHECKCOINS\n");
              BASIC(GLD_CHECKCOINS);
              arg1 = GetNum("CHECKCOINS");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("xycompare") {
              vlog_txt("XYCOMPARE\n");
              BASIC(GLD_XYCOMPARE);
              arg1 = GetNum("XYCOMPARE");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("warpmod") {
              vlog_txt("WARPMOD\n");
              BASIC(GLD_WARPMOD);
              arg1 = GetNum("WARPMOD");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("WARPMOD");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("WARPMOD");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("warp") {
              vlog_txt("WARP\n");
              BASIC(GLD_WARP);
              arg1 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              arg4 = GetNum("WARP");
              if (!gffs) {
                return;
              }
              rom(arg4, 1);
              ec();
            }
            aa("warpfacing") {
              vlog_txt("WARPFACING\n");
              BASIC(GLD_WARPFACING);
              arg1 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              arg4 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              rom(arg4, 1);
              arg5 = GetNum("WARPFACING");
              if (!gffs) {
                return;
              }
              rom(arg5, 1);
              ec();
            }
            aa("blackoutmod") {
              vlog_txt("BLACKOUTMOD\n");
              BASIC(GLD_BLACKOUTMOD);
              arg1 = GetNum("BLACKOUTMOD");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("BLACKOUTMOD");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("moneytotext") {
              vlog_txt("MONEYTOTEXT\n");
              BASIC(GLD_MONEYTOTEXT);
              arg1 = GetNum("MONEYTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("MONEYTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("coinstotext") {
              vlog_txt("COINSTOTEXT\n");
              BASIC(GLD_COINSTOTEXT);
              arg1 = GetNum("COINSTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("vartotext") {
              vlog_txt("VARTOTEXT\n");
              BASIC(GLD_VARTOTEXT);
              arg1 = GetNum("VARTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("poketotext") {
              vlog_txt("POKETOTEXT\n");
              BASIC(GLD_POKETOTEXT);
              arg1 = GetNum("POKETOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("POKETOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("itemtotext") {
              vlog_txt("ITEMTOTEXT\n");
              BASIC(GLD_ITEMTOTEXT);
              arg1 = GetNum("ITEMTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("ITEMTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("locationtotext") {
              vlog_txt("LOCATIONTOTEXT\n");
              BASIC(GLD_LOCATIONTOTEXT);
              arg1 = GetNum("LOCATIONTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("trainertotext") {
              vlog_txt("TRAINERTOTEXT\n");
              BASIC(GLD_TRAINERTOTEXT);
              arg1 = GetNum("TRAINERTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("TRAINERTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("TRAINERTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("stringtotext") {
              vlog_txt("STRINGTOTEXT\n");
              BASIC(GLD_STRINGTOTEXT);
              arg1 = GetNum("STRINGTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("STRINGTOTEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("refreshscreen") {
              vlog_txt("REFRESHSCREEN\n");
              BASIC(GLD_REFRESHSCREEN);
              arg1 = GetNum("REFRESHSCREEN");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("c1celoadbyte") {
              vlog_txt("C1CELOADBYTE\n");
              BASIC(GLD_C1CELOADBYTE);
              arg1 = GetNum("C1CELOADBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("3writetext") {
              vlog_txt("3WRITETEXT\n");
              BASIC(GLD_3WRITETEXT);
              arg1 = GetNum("3WRITETEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 3);
              ec();
            }
            aa("2writetext") {
              vlog_txt("2WRITETEXT\n");
              BASIC(GLD_2WRITETEXT);
              arg1 = GetNum("2WRITETEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("repeattext") {
              vlog_txt("REPEATTEXT\n");
              BASIC(GLD_REPEATTEXT);
              arg1 = GetNum("REPEATTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("loadmenudata") {
              vlog_txt("LOADMENUDATA\n");
              BASIC(GLD_LOADMENUDATA);
              arg1 = GetNum("LOADMENUDATA");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("jumptextfaceplayer") {
              vlog_txt("JUMPTEXTFACEPLAYER\n");
              BASIC(GLD_JUMPTEXTFACEPLAYER);
              arg1 = GetNum("JUMPTEXTFACEPLAYER");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("jumptext") {
              vlog_txt("JUMPTEXT\n");
              BASIC(GLD_JUMPTEXT);
              arg1 = GetNum("JUMPTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("pokepic") {
              vlog_txt("POKEPIC\n");
              BASIC(GLD_POKEPIC);
              arg1 = GetNum("POKEPIC");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("loadpokedata") {
              vlog_txt("LOADPOKEDATA\n");
              BASIC(GLD_LOADPOKEDATA);
              arg1 = GetNum("LOADPOKEDATA");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("LOADPOKEDATA");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("loadtrainer") {
              vlog_txt("LOADTRAINER\n");
              BASIC(GLD_LOADTRAINER);
              arg1 = GetNum("LOADTRAINER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("LOADTRAINER");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("catchtutorial") {
              vlog_txt("CATCHTUTORIAL\n");
              BASIC(GLD_CATCHTUTORIAL);
              arg1 = GetNum("CATCHTUTORIAL");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("trainertext") {
              vlog_txt("TRAINERTEXT\n");
              BASIC(GLD_TRAINERTEXT);
              arg1 = GetNum("TRAINERTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("trainerstatus") {
              vlog_txt("TRAINERSTATUS\n");
              BASIC(GLD_TRAINERSTATUS);
              arg1 = GetNum("TRAINERSTATUS");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("winlosstext") {
              vlog_txt("WINLOSSTEXT\n");
              BASIC(GLD_WINLOSSTEXT);
              arg1 = GetNum("WINLOSSTEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("WINLOSSTEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 2);
              ec();
            }
            aa("setlasttalked") {
              vlog_txt("SETLASTTALKED\n");
              BASIC(GLD_SETLASTTALKED);
              arg1 = GetNum("SETLASTTALKED");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("applymovement") {
              vlog_txt("APPLYMOVEMENT\n");
              BASIC(GLD_APPLYMOVEMENT);
              arg1 = GetNum("APPLYMOVEMENT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("APPLYMOVEMENT");
              if (!gffs) {
                return;
              }
              rom(arg2, 2);
              ec();
            }
            aa("applymoveother") {
              vlog_txt("APPLYMOVEOTHER\n");
              BASIC(GLD_APPLYMOVEOTHER);
              arg1 = GetNum("APPLYMOVEOTHER");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("faceperson") {
              vlog_txt("FACEPERSON\n");
              BASIC(GLD_FACEPERSON);
              arg1 = GetNum("FACEPERSON");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("FACEPERSON");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("variablesprite") {
              vlog_txt("VARIABLESPRITE\n");
              BASIC(GLD_VARIABLESPRITE);
              arg1 = GetNum("VARIABLESPRITE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("VARIABLESPRITE");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("disappear") {
              vlog_txt("DISAPPEAR\n");
              BASIC(GLD_DISAPPEAR);
              arg1 = GetNum("DISAPPEAR");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("appear") {
              vlog_txt("APPEAR\n");
              BASIC(GLD_APPEAR);
              arg1 = GetNum("APPEAR");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("follow") {
              vlog_txt("FOLLOW\n");
              BASIC(GLD_FOLLOW);
              arg1 = GetNum("FOLLOW");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("FOLLOW");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("moveperson") {
              vlog_txt("MOVEPERSON\n");
              BASIC(GLD_MOVEPERSON);
              arg1 = GetNum("MOVEPERSON");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("MOVEPERSON");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("MOVEPERSON");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("writepersonloc") {
              vlog_txt("WRITEPERSONLOC\n");
              BASIC(GLD_WRITEPERSONLOC);
              arg1 = GetNum("WRITEPERSONLOC");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("loademote") {
              vlog_txt("LOADEMOTE\n");
              BASIC(GLD_LOADEMOTE);
              arg1 = GetNum("LOADEMOTE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("showemote") {
              vlog_txt("SHOWEMOTE\n");
              BASIC(GLD_SHOWEMOTE);
              arg1 = GetNum("SHOWEMOTE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("SHOWEMOTE");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("SHOWEMOTE");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("spriteface") {
              vlog_txt("SPRITEFACE\n");
              BASIC(GLD_SPRITEFACE);
              arg1 = GetNum("SPRITEFACE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("SPRITEFACE");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("follownotexact") {
              vlog_txt("FOLLOWNOTEXACT\n");
              BASIC(GLD_FOLLOWNOTEXACT);
              arg1 = GetNum("FOLLOWNOTEXACT");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("FOLLOWNOTEXACT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("earthquake") {
              vlog_txt("EARTHQUAKE\n");
              BASIC(GLD_EARTHQUAKE);
              arg1 = GetNum("EARTHQUAKE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("swapmaps") {
              vlog_txt("SWAPMAPS\n");
              BASIC(GLD_SWAPMAPS);
              arg1 = GetNum("SWAPMAPS");
              if (!gffs) {
                return;
              }
              rom(arg1, 3);
              ec();
            }
            aa("changeblock") {
              vlog_txt("CHANGEBLOCK\n");
              BASIC(GLD_CHANGEBLOCK);
              arg1 = GetNum("CHANGEBLOCK");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("CHANGEBLOCK");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("CHANGEBLOCK");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("writecmdqueue") {
              vlog_txt("WRITECMDQUEUE\n");
              BASIC(GLD_WRITECMDQUEUE);
              arg1 = GetNum("WRITECMDQUEUE");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("delcmdqueue") {
              vlog_txt("DELCMDQUEUE\n");
              BASIC(GLD_DELCMDQUEUE);
              arg1 = GetNum("DELCMDQUEUE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("playmusic") {
              vlog_txt("PLAYMUSIC\n");
              BASIC(GLD_PLAYMUSIC);
              arg1 = GetNum("PLAYMUSIC");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("musicfadeout") {
              vlog_txt("MUSICFADEOUT\n");
              BASIC(GLD_MUSICFADEOUT);
              arg1 = GetNum("MUSICFADEOUT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("MUSICFADEOUT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("cry") {
              vlog_txt("CRY\n");
              BASIC(GLD_CRY);
              arg1 = GetNum("CRY");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("playsound") {
              vlog_txt("PLAYSOUND\n");
              BASIC(GLD_PLAYSOUND);
              arg1 = GetNum("PLAYSOUND");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("passtoengine") {
              vlog_txt("PASSTOENGINE\n");
              BASIC(GLD_PASSTOENGINE);
              arg1 = GetNum("PASSTOENGINE");
              if (!gffs) {
                return;
              }
              rom(arg1, 3);
              ec();
            }
            aa("newloadmap") {
              vlog_txt("NEWLOADMAP\n");
              BASIC(GLD_NEWLOADMAP);
              arg1 = GetNum("NEWLOADMAP");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("pause") {
              vlog_txt("PAUSE\n");
              BASIC(GLD_PAUSE);
              arg1 = GetNum("PAUSE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("deactivatefacing") {
              vlog_txt("DEACTIVATEFACING\n");
              BASIC(GLD_DEACTIVATEFACING);
              arg1 = GetNum("DEACTIVATEFACING");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("priorityjump") {
              vlog_txt("PRIORITYJUMP\n");
              BASIC(GLD_PRIORITYJUMP);
              arg1 = GetNum("PRIORITYJUMP");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("ptpriorityjump") {
              vlog_txt("PTPRIORITYJUMP\n");
              BASIC(GLD_PTPRIORITYJUMP);
              arg1 = GetNum("PTPRIORITYJUMP");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("pokemart") {
              vlog_txt("POKEMART\n");
              BASIC(GLD_POKEMART);
              arg1 = GetNum("POKEMART");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("POKEMART");
              if (!gffs) {
                return;
              }
              rom(arg2, 2);
              ec();
            }
            aa("elevator") {
              vlog_txt("ELEVATOR\n");
              BASIC(GLD_ELEVATOR);
              arg1 = GetNum("ELEVATOR");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("trade") {
              vlog_txt("TRADE\n");
              BASIC(GLD_TRADE);
              arg1 = GetNum("TRADE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("askforphonenumber") {
              vlog_txt("ASKFORPHONENUMBER\n");
              BASIC(GLD_ASKFORPHONENUMBER);
              arg1 = GetNum("ASKFORPHONENUMBER");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("phonecall") {
              vlog_txt("PHONECALL\n");
              BASIC(GLD_PHONECALL);
              arg1 = GetNum("PHONECALL");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              ec();
            }
            aa("describedecoration") {
              vlog_txt("DESCRIBEDECORATION\n");
              BASIC(GLD_DESCRIBEDECORATION);
              arg1 = GetNum("DESCRIBEDECORATION");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("fruittree") {
              vlog_txt("FRUITTREE\n");
              BASIC(GLD_FRUITTREE);
              arg1 = GetNum("FRUITTREE");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("specialphonecall") {
              vlog_txt("SPECIALPHONECALL\n");
              BASIC(GLD_SPECIALPHONECALL);
              arg1 = GetNum("SPECIALPHONECALL");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("verbosegiveitem") {
              vlog_txt("VERBOSEGIVEITEM\n");
              BASIC(GLD_VERBOSEGIVEITEM);
              arg1 = GetNum("VERBOSEGIVEITEM");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("VERBOSEGIVEITEM");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("loadwilddata") {
              vlog_txt("LOADWILDDATA\n");
              BASIC(GLD_LOADWILDDATA);
              arg1 = GetNum("LOADWILDDATA");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              arg2 = GetNum("LOADWILDDATA");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            aa("storetext") {
              vlog_txt("STORETEXT\n");
              BASIC(GLD_STORETEXT);
              arg1 = GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              arg3 = GetNum("STORETEXT");
              if (!gffs) {
                return;
              }
              rom(arg3, 1);
              ec();
            }
            aa("displaylocation") {
              vlog_txt("DISPLAYLOCATION\n");
              BASIC(GLD_DISPLAYLOCATION);
              arg1 = GetNum("DISPLAYLOCATION");
              if (!gffs) {
                return;
              }
              rom(arg1, 1);
              ec();
            }
            aa("writebyte") {
              vlog_txt("WRITEBYTE\n");
              BASIC(GLD_WRITEBYTE);
              arg1 = GetNum("WRITEBYTE");
              if (!gffs) {
                return;
              }
              rom(arg1, 2);
              arg2 = GetNum("WRITEBYTE");
              if (!gffs) {
                return;
              }
              rom(arg2, 1);
              ec();
            }
            else if (*buf == '-') try_asm();
            else if (*buf == ':') {
              if (c) add_label(buf, c, c->size, &cl);
              ec();
            }
            aa("m") {
              vlog_txt("Movement data...\n");
              add_data(c, trans, transbackmove(Script, &i));
              ec();
            }
            aa("=") {
              vlog_txt("[STRING]\n");
              if (chr == ' ') {
                i++;
              } else {
                log_txt("Should have a space after the =\n", 33 - 1);
              }
              temp_ptr = transbackstr(script, i - fst, c);
              while (chr != '\n' && chr != 0) {
                i++;
              }
              sprintf(buf2, "   -> %s\n", (char *)temp_ptr);
              free(temp_ptr);
              vlog_txt(buf2);
            }
            aa(".") {
              vlog_txt("[BINARY]\n   ->");
              i = skip_whitespace(Script, i);
              k = 0;
              while (chr != '\n' && chr != 0) {
                k = 1 - k;
                i = skip_whitespace(Script, i);
                j = 0;
                while (((char *)("0123456789abcdef"))[j] != 0) {
                  if (((char *)("0123456789abcdef"))[j] == chr) {
                    break;
                  }
                  j += 1;
                }
                if (((char *)("0123456789abcdef"))[j] == 0) {
                  sprintf(buf2, "Failed to understand hex character '%c'\n",
                          chr);
                  log_txt(buf2, strlen(buf2));
                  return;
                }
                if (k == 0) {
                  l |= j;
                  rom(j, 1);
                  if (IsVerbose) {
                    sprintf(buf2, " %02X", l);
                    log_txt(buf2, strlen(buf2));
                  }
                } else {
                  l = j << 4;
                }
                i++;
              }
              vlog_txt("\n");
            }
            else {
              sprintf(buf2, "Warning: Unknown command \"%s\"\n", buf);
              log_txt(buf2, strlen(buf2));
              while (chr != '\n' && chr != 0) {
                i++;
              }
            }
            break;
        }
        i++;
      }
    } else  // Fire Red
    {
    frlg:
    rse:
      while (i < scriptlen) {
        switch (chr)  // Behave differently according to char
        {
          case 0:  // E O Script
            return;
          case '\'':
            while (chr != '\n' && chr != 0) {
              i++;
            }
            i--;
            break;
          case ' ':  // Ignore spaces
          case '\n':
            break;
          default:
            j = 0;
            buf_loc = i;
            while (chr != ' ' && chr != '\n' && chr != 0) {
              buf[j] = chr;
              i++;
              j++;
            }
            buf[j] = 0;
            // aa    is just shorthand for  "else if (!strcmp (buf, x))"
            // ec    is End Command
            // log   is self-explanatory
            // rom   writes to rom
            // basic is shorthand for rom(x,1)
            switch (*buf) {
              case '#':
                if (!strcmp(buf, "#define") || !strcmp(buf, "#d")) {
                  vlog_txt("#DEFINE\n");
                  i = skip_whitespace(Script, i);
                  if (chr == '\n' || chr == 0 || chr == '\'') {
                    log_txt("Premature end to #DEFINE!\n", 27 - 1);
                    return;
                  }
                  j = 0;
                  while (chr != ' ' && chr != '\n' && chr != 0 && chr != '\'') {
                    buf[j] = chr;
                    i++;
                    j++;
                  }
                  buf[j] = 0;  // Append null
                  sprintf(buf2, "   -> %s\n", buf);
                  vlog_txt(buf2);
                  i = skip_whitespace(Script, i);
                  if (chr == '\n' || chr == 0 || chr == '\'') {
                    log_txt("Premature end to #DEFINE!\n", 27 - 1);
                    return;
                  }
                  k = GetNum("#DEFINE");
                  if (!gffs) return;
                  bsearch_upsert(defines, buf, (void *)(intptr_t)k);
                }
                aa("#dyn") {
                  vlog_txt("#DYN\n");
                  start = GetNum("#DYN");
                  if (!gffs) return;
                  dyntype = 0;
                  bsearch_upsert(defines, "findfrom", (void *)(intptr_t)start);
                }
                aa("#dyn2") {
                  vlog_txt("#DYN2\n");
                  start = GetNum("#DYN2");
                  if (!gffs) return;
                  dyntype = 1;
                }
                aa("#dynamic") {
                  vlog_txt("#DYNAMIC\n");
                  start = GetNum("#DYNAMIC");
                  if (!gffs) return;
                  dyntype = 0;
                  bsearch_upsert(defines, "findfrom", (void *)(intptr_t)start);
                }
                aa("#org") {
                  eorg = 0;
                  vlog_txt("#ORG\n");
                  i = skip_whitespace(Script, i);
                  buf[0] = 0;
                  if (chr == '@') {
                    if (start == 0) {
                      log_txt("Need #dyn with dynamic offsets\n", 32 - 1);
                      return;
                    }
                    dynu = 1;
                    j = 0;
                    while (chr != ' ' && chr != '\n' && chr != 0 &&
                           chr != '\'') {
                      buf[j] = chr;
                      i++;
                      j++;
                    }
                    buf[j] = 0;
                    sprintf(buf2, "   -> %s\n", buf);
                    vlog_txt(buf2);
                  } else {
                    k = GetNum("#ORG");
                    if (!gffs) return;
                  }
                  d = malloc(sizeof(codeblock));
                  if (*buf == 0)
                    init_codeblock(d, NULL, k);
                  else
                    init_codeblock(d, buf,
                                   (start & 0x0FFFFFFF) | (dyntype << 24));
                  d->prev = c;
                  if (c != NULL) c->next = d;
                  c = d;
                }
                aa("#orgal") {
                  eorg = 0;
                  vlog_txt("#ORGAL\n");
                  i = skip_whitespace(Script, i);
                  buf[0] = 0;
                  if (chr == '@') {
                    if (start == 0) {
                      log_txt("Need #dyn with dynamic offsets\n", 32 - 1);
                      return;
                    }
                    dynu = 1;
                    j = 0;
                    while (chr != ' ' && chr != '\n' && chr != 0 &&
                           chr != '\'') {
                      buf[j] = chr;
                      i++;
                      j++;
                    }
                    buf[j] = 0;
                    sprintf(buf2, "   -> %s\n", buf);
                    vlog_txt(buf2);
                  } else {
                    k = GetNum("#ORGAL");
                    if (!gffs) return;
                  }
                  d = malloc(sizeof(codeblock));
                  if (*buf == 0)
                    init_codeblock(d, NULL, k);
                  else
                    init_codeblock(d, buf,
                                   (start & 0x0FFFFFFF) | (dyntype << 24));
                  d->prev = c;
                  if (c != NULL) c->next = d;
                  c = d;
                  c->align = GetNum("#ORGAL");
                  if (!gffs) return;
                }
                aa("#eorg") {
                  eorg = 1;
                  vlog_txt("#EORG\n");
                  i = skip_whitespace(Script, i);
                  buf[0] = 0;
                  if (chr == '@') {
                    j = 0;
                    while (chr != ' ' && chr != '\n' && chr != 0 &&
                           chr != '\'') {
                      buf[j] = chr;
                      i++;
                      j++;
                    }
                    buf[j] = 0;
                    sprintf(buf2, "   -> %s\n", buf);
                    vlog_txt(buf2);
                  } else {
                    k = GetNum("#EORG");
                    if (!gffs) {
                      return;
                    }
                  }
                  d = malloc(sizeof(codeblock));
                  if (*buf == 0)
                    init_codeblock(d, NULL, k);
                  else
                    init_codeblock(d, buf,
                                   (start & 0x0FFFFFFF) | (dyntype << 24));
                  d->prev = c;
                  if (c != NULL) c->next = d;
                  c = d;
                }
                aa("#quiet") { IsVerbose = 0; }
                aa("#loud") { IsVerbose = 1; }
                aa("#thumb") {
                  thumb = 1;
                  ec();
                }
                aa("#arm") {
                  thumb = 0;
                  ec();
                }
                aa("#gs") {
                  mode = GOLD;
                  goto gsc;
                }
                aa("#c") {
                  mode = CRYSTAL;
                  goto cry;
                }
                aa("#frlg") { mode = FIRE_RED; }
                aa("#rse") { mode = RUBY; }
                aa("#dp") {
                  mode = DIAMOND;
                  goto dp;
                }
                aa("#raw") {
                  vlog_txt("#RAW\n");
                  arg1 = 1;
                  while (arg1) {
                    k = GetNum("#RAW");
                    if (!gffs) {
                      return;
                    }
                    BASIC(k);
                    i = skip_whitespace(Script, i);
                    arg1 = (chr >= '0' && chr <= '9') || chr == '$';
                  }
                  ec();
                }
                aa("#ptr") {
                  vlog_txt("#PTR\n");
                  k = GetNum("#PTR");
                  if (!gffs) {
                    return;
                  }
                  k |= 0x08000000;
                  rom(k, 4);
                }
                aa("#dword") {
                  vlog_txt("#DWORD\n");
                  k = GetNum("#DWORD");
                  if (!gffs) {
                    return;
                  }
                  rom(k, 4);
                }
                aa("#word") {
                  vlog_txt("#WORD\n");
                  k = GetNum("#WORD");
                  if (!gffs) {
                    return;
                  }
                  rom(k, 2);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'a':
                if (!strcmp(buf, "applymovement")) {
                  vlog_txt("APPLYMOVEMENT\n");
                  BASIC(CMD_APPLYMOVEMENT);
                  arg1 = GetNum("APPLYMOVEMENT");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                  arg2 = GetNum("APPLYMOVEMENT");
                  if (!gffs) {
                    return;
                  }
                  if ((arg2 & 0xff000000) == 0) {
                    arg2 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg2);
                    vlog_txt(buf3);
                  }
                  rom(arg2, 4);
                }
                aa("applymovementfinishat") {
                  vlog_txt("APPLYMOVEMENTFINISHAT\n");
                  BASIC(CMD_APPLYMOVEMENTFINISHAT);
                  arg1 = GetNum("APPLYMOVEMENTFINISHAT");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                  arg2 = GetNum("APPLYMOVEMENTFINISHAT");
                  if (!gffs) {
                    return;
                  }
                  if ((arg2 & 0xff000000) == 0) {
                    arg2 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg2);
                    vlog_txt(buf3);
                  }
                  rom(arg2, 4);
                  arg3 = GetNum("APPLYMOVEMENTFINISHAT");
                  if (!gffs) {
                    return;
                  }
                  rom(arg3, 1);
                  arg4 = GetNum("APPLYMOVEMENTFINISHAT");
                  if (!gffs) {
                    return;
                  }
                  rom(arg4, 1);
                }
                aa("addfurniture") {
                  vlog_txt("ADDFURNITURE\n");
                  arg1 = GetNum("ADDFURNITURE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_ADDFURNITURE);
                  rom(arg1, 2);
                }
                aa("addpokemon") {
                  vlog_txt("ADDPOKEMON\n");
                  BASIC(CMD_ADDPOKEMON);
                  arg1 = GetNum("ADDPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                  arg2 = GetNum("ADDPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 1);
                  arg3 = GetNum("ADDPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  rom(arg3, 2);
                  arg4 = GetNum("ADDPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  rom(arg4, 1);
                  arg5 = GetNum("ADDPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  rom(arg5, 4);
                  arg6 = GetNum("ADDPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  rom(arg6, 4);
                }
                aa("addvar") {
                  vlog_txt("ADDVAR\n");
                  arg1 = GetNum("ADDVAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("ADDVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_ADDVAR);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("additem") {
                  vlog_txt("ADDITEM\n");
                  arg1 = GetNum("ADDITEM");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("ADDITEM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_ADDITEM);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'b':
                if (!strcmp(buf, "boxset")) {
                  vlog_txt("BOXSET\n");
                  arg1 = GetNum("BOXSET");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CALLSTD);
                  BASIC(arg1);
                }
                aa("braille") {
                  vlog_txt("BRAILLE\n");
                  BASIC(CMD_BRAILLE);
                  arg1 = GetNum("BRAILLE");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("battle") {
                  vlog_txt("BATTLE\n");
                  arg1 = GetNum("BATTLE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("BATTLE");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("BATTLE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_BATTLE);
                  rom(arg1, 2);
                  rom(arg2, 1);
                  rom(arg3, 2);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'c':
                if (!strcmp(buf, "checkgender")) {
                  BASIC(CMD_CHECKGENDER);
                }
                aa("closemsg") { BASIC(CMD_CLOSEMSG); }
                aa("countpokemon") { BASIC(CMD_COUNTPOKEMON); }
                aa("cmd_b2") { BASIC(CMD_B2); }
                aa("choosecontestpokemon") { BASIC(CMD_CHOOSECONTESTPOKEMON); }
                aa("cmd_2d") { BASIC(CMD_2D); }
                aa("cmd_8a") {
                  vlog_txt("CMD_8A\n");
                  arg1 = GetNum("CMD_8A");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CMD_8A");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("CMD_8A");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_8A);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                aa("comparehiddenvar") {
                  vlog_txt("COMPAREHIDDENVAR\n");
                  arg1 = GetNum("COMPAREHIDDENVAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COMPAREHIDDENVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COMPAREHIDDENVAR);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("checkobedience") {
                  vlog_txt("CHECKOBEDIENCE\n");
                  arg1 = GetNum("CHECKOBEDIENCE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKOBEDIENCE);
                  rom(arg1, 2);
                }
                aa("checkanimation") {
                  vlog_txt("CHECKANIMATION\n");
                  arg1 = GetNum("CHECKANIMATION");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKANIMATION);
                  rom(arg1, 2);
                }
                aa("call") {
                  BASIC(CMD_CALL);
                  vlog_txt("CALL\n");
                  arg1 = GetNum("CALL");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("callstd") {
                  vlog_txt("CALLSTD\n");
                  arg1 = GetNum("CALLSTD");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CALLSTD);
                  BASIC(arg1);
                }
                aa("compare") {
                  vlog_txt("COMPARE\n");
                  arg1 = GetNum("COMPARE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COMPARE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COMPARE);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("copyvar") {
                  vlog_txt("COPYVAR\n");
                  arg1 = GetNum("COPYVAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COPYVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COPYVAR);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("copyvarifnotzero") {
                  vlog_txt("COPYVARIFNOTZERO\n");
                  arg1 = GetNum("COPYVARIFNOTZERO");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COPYVARIFNOTZERO");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COPYVARIFNOTZERO);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("copyscriptbanks") {
                  vlog_txt("COPYSCRIPTBANKS\n");
                  arg1 = GetNum("COPYSCRIPTBANKS");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COPYSCRIPTBANKS");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COPYSCRIPTBANKS);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("copybyte") {
                  vlog_txt("COPYBYTE\n");
                  BASIC(CMD_COPYBYTE);
                  arg1 = GetNum("COPYBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 4);
                  arg2 = GetNum("COPYBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 4);
                }
                aa("comparevartobyte") {
                  vlog_txt("COMPAREVARTOBYTE\n");
                  arg1 = GetNum("COMPAREVARTOBYTE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COMPAREVARTOBYTE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COMPAREVARTOBYTE);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                aa("comparevartofarbyte") {
                  vlog_txt("COMPAREVARTOFARBYTE\n");
                  BASIC(CMD_COMPAREVARTOFARBYTE);
                  arg1 = GetNum("COMPAREVARTOFARBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                  arg2 = GetNum("COMPAREVARTOFARBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 4);
                }
                aa("comparefarbytetovar") {
                  vlog_txt("COMPAREFARBYTETOVAR\n");
                  BASIC(CMD_COMPAREFARBYTETOVAR);
                  arg1 = GetNum("COMPAREFARBYTETOVAR");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 4);
                  arg2 = GetNum("COMPAREFARBYTETOVAR");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 2);
                }
                aa("comparefarbytetobyte") {
                  vlog_txt("COMPAREFARBYTETOBYTE\n");
                  BASIC(CMD_COMPAREFARBYTETOBYTE);
                  arg1 = GetNum("COMPAREFARBYTETOBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 4);
                  arg2 = GetNum("COMPAREFARBYTETOBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 1);
                }
                aa("comparefarbytetofarbyte") {
                  vlog_txt("COMPAREFARBYTETOFARBYTE\n");
                  BASIC(CMD_COMPAREFARBYTETOFARBYTE);
                  arg1 = GetNum("COMPAREFARBYTETOFARBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 4);
                  arg2 = GetNum("COMPAREFARBYTETOFARBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 4);
                }
                aa("callasm") {
                  vlog_txt("CALLASM\n");
                  BASIC(CMD_CALLASM);
                  arg1 = GetNum("CALLASM");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("callasm2") {
                  vlog_txt("CALLASM2\n");
                  BASIC(CMD_CALLASM2);
                  arg1 = GetNum("CALLASM2");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("clearflag") {
                  vlog_txt("CLEARFLAG\n");
                  arg1 = GetNum("CLEARFLAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CLEARFLAG);
                  rom(arg1, 2);
                }
                aa("checkflag") {
                  vlog_txt("CHECKFLAG\n");
                  arg1 = GetNum("CHECKFLAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKFLAG);
                  rom(arg1, 2);
                }
                aa("cry") {
                  vlog_txt("CRY\n");
                  arg1 = GetNum("CRY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CRY");
                  if (!gffs) {
                    return;
                  }
                  if (mode == FIRE_RED) {
                    BASIC(CMD_CRYFR);
                    rom(arg1, 2);
                    rom(arg2, 2);
                  } else {
                    arg3 = GetNum("CRY");
                    if (!gffs) {
                      return;
                    }
                    BASIC(CMD_CRY);
                    rom(arg1, 1);
                    rom(arg2, 2);
                    rom(arg3, 2);
                  }
                }
                aa("checksound") {
                  vlog_txt("CHECKSOUND\n");
                  BASIC(CMD_CRY);
                }
                aa("cmd_c3") {
                  vlog_txt("CMD_C3\n");
                  arg1 = GetNum("CMD_C3");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_C3);
                  rom(arg1, 1);
                }
                aa("checkitem") {
                  vlog_txt("CHECKITEM\n");
                  arg1 = GetNum("CHECKITEM");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CHECKITEM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKITEM);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("checkitemspaceinbag") {
                  vlog_txt("CHECKITEMSPACEINBAG\n");
                  arg1 = GetNum("CHECKITEMSPACEINBAG");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CHECKITEMSPACEINBAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKITEMSPACEINBAG);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("cmd_2c") {
                  vlog_txt("CMD_2C\n");
                  arg1 = GetNum("CMD_2C");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CMD_2C");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_2C);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("checkattack") {
                  vlog_txt("CHECKATTACK\n");
                  arg1 = GetNum("CHECKATTACK");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKATTACK);
                  rom(arg1, 2);
                }
                aa("checkfurniture") {
                  vlog_txt("CHECKFURNITURE\n");
                  BASIC(CMD_CHECKFURNITURE);
                  arg1 = GetNum("CHECKFURNITURE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                }
                aa("checkifroomforfurniture") {
                  vlog_txt("CHECKIFROOMFORFURNITURE\n");
                  BASIC(CMD_CHECKIFROOMFORFURNITURE);
                  arg1 = GetNum("CHECKIFROOMFORFURNITURE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                }
                aa("checkitemtype") {
                  vlog_txt("CHECKITEMTYPE\n");
                  arg1 = GetNum("CHECKITEMTYPE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKITEMTYPE);
                  rom(arg1, 2);
                }
                aa("checkiteminpc") {
                  vlog_txt("CHECKITEMINPC\n");
                  arg1 = GetNum("CHECKITEMINPC");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CHECKITEMINPC");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKITEMINPC);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("createtempsprite") {
                  vlog_txt("CREATETEMPSPRITE\n");
                  arg1 = GetNum("CREATETEMPSPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CREATETEMPSPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("CREATETEMPSPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("CREATETEMPSPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg5 = GetNum("CREATETEMPSPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg6 = GetNum("CREATETEMPSPRITE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CREATETEMPSPRITE);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 2);
                  rom(arg4, 2);
                  rom(arg5, 1);
                  rom(arg6, 1);
                }
                aa("cmd_a6") {
                  vlog_txt("CMD_A6\n");
                  arg1 = GetNum("CMD_A6");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_A6);
                  rom(arg1, 1);
                }
                aa("cmd_b1") {
                  vlog_txt("CMD_B1\n");
                  arg1 = GetNum("CMD_B1");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CMD_B1");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("CMD_B1");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("CMD_B1");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_B1);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 2);
                  rom(arg4, 2);
                }
                aa("cmd_d3") {
                  vlog_txt("CMD_D3\n");
                  BASIC(CMD_D3);
                  arg1 = GetNum("CMD_D3");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("cmd_fb") {
                  vlog_txt("CMD_FB\n");
                  BASIC(CMD_FB);
                  arg1 = GetNum("CMD_FB");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("checkattack") {
                  vlog_txt("CHECKATTACK\n");
                  arg1 = GetNum("CHECKATTACK");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CHECKATTACK");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKATTACK);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("coincasetovar") {
                  vlog_txt("CoinCaseToVar\n");
                  arg1 = GetNum("COINCASETOVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COINCASETOVAR);
                  rom(arg1, 2);
                }
                aa("callstdif") {
                  vlog_txt("callstdif\n");
                  arg1 = GetNum("callstdif");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("callstdif");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CALLSTDIF);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("checkmoney") {
                  vlog_txt("CHECKMONEY\n");
                  arg1 = GetNum("CHECKMONEY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("CHECKMONEY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKMONEY);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                aa("cmd_96") {
                  vlog_txt("CMD_96\n");
                  arg1 = GetNum("CMD_96");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_96);
                  rom(arg1, 2);
                }
                aa("cmd_fe") {
                  vlog_txt("CMD_FE\n");
                  arg1 = GetNum("CMD_FE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FE);
                  rom(arg1, 1);
                }
                aa("cleartrainerflag") {
                  vlog_txt("CLEARTRAINERFLAG\n");
                  arg1 = GetNum("CLEARTRAINERFLAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CLEARTRAINERFLAG);
                  rom(arg1, 2);
                }
                aa("comparevars") {
                  vlog_txt("COMPAREVARS\n");
                  arg1 = GetNum("COMPAREVARS");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COMPAREVARS");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COMPAREVARS);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("comparevars2") {
                  vlog_txt("COMPAREVARS2\n");
                  arg1 = GetNum("COMPAREVARS2");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("COMPAREVARS2");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_COMPAREVARS2);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("checktrainerflag") {
                  vlog_txt("CHECKTRAINERFLAG\n");
                  arg1 = GetNum("CHECKTRAINERFLAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_CHECKTRAINERFLAG);
                  rom(arg1, 2);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'd':
                if (!strcmp(buf, "doweather")) {
                  BASIC(CMD_DOWEATHER);
                }
                aa("doorchange") { BASIC(CMD_DOORCHANGE); }
                aa("doanimation") {
                  vlog_txt("DOANIMATION\n");
                  arg1 = GetNum("DOANIMATION");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_DOANIMATION);
                  rom(arg1, 2);
                }
                aa("disappear") {
                  vlog_txt("DISAPPEAR\n");
                  arg1 = GetNum("DISAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_DISAPPEAR);
                  rom(arg1, 2);
                }
                aa("darkenroom") {
                  vlog_txt("DARKENROOM\n");
                  arg1 = GetNum("DARKENROOM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_DARKENROOM);
                  rom(arg1, 2);
                }
                aa("disappearat") {
                  vlog_txt("DISAPPEARAT\n");
                  arg1 = GetNum("DISAPPEARAT");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("DISAPPEARAT");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("DISAPPEARAT");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_DISAPPEARAT);
                  rom(arg1, 2);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'e':
                if (!strcmp(buf, "end")) {
                  BASIC(CMD_END);  // BASIC() writes 1 byte to ROM
                }
                aa("executeram") { BASIC(CMD_EXECUTERAM); }
                aa("endtrainerbattle") { BASIC(CMD_ENDTRAINERBATTLE); }
                aa("endtrainerbattle2") { BASIC(CMD_ENDTRAINERBATTLE2); }
                aa("endmart") {
                  vlog_txt("ENDMART\n");
                  rom(0, 2);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'f':
                if (!strcmp(buf, "faceplayer")) {
                  BASIC(CMD_FACEPLAYER);
                }
                aa("fadedefault") { BASIC(CMD_FADEDEFAULT); }
                aa("fadeout") {
                  vlog_txt("FADEOUT\n");
                  arg1 = GetNum("FADEOUT");
                  if (!gffs) return;
                  BASIC(CMD_FADEOUT);
                  rom(arg1, 1);
                }
                aa("fadein") {
                  vlog_txt("FADEIN\n");
                  arg1 = GetNum("FADEIN");
                  if (!gffs) return;
                  BASIC(CMD_FADEIN);
                  rom(arg1, 1);
                }
                aa("fadedefault") { BASIC(CMD_FADEDEFAULT); }
                aa("falldownhole") {
                  vlog_txt("FALLDOWNHOLE\n");
                  arg1 = GetNum("FALLDOWNHOLE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("FALLDOWNHOLE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FALLDOWNHOLE);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("fadescreen") {
                  vlog_txt("FADESCREEN\n");
                  arg1 = GetNum("FADESCREEN");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FADESCREEN);
                  rom(arg1, 1);
                }
                aa("fadescreendelay") {
                  vlog_txt("FADESCREENDELAY\n");
                  arg1 = GetNum("FADESCREENDELAY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("FADESCREENDELAY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FADESCREENDELAY);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("fanfare") {
                  vlog_txt("FANFARE\n");
                  arg1 = GetNum("FANFARE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FANFARE);
                  rom(arg1, 2);
                }
                aa("fadesong") {
                  vlog_txt("FADESONG\n");
                  arg1 = GetNum("FADESONG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FADESONG);
                  rom(arg1, 2);
                }
                aa("fadedefault") {
                  vlog_txt("FADEDEFAULT\n");
                  arg1 = GetNum("FADEDEFAULT");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FADEDEFAULT);
                }
                aa("fakejumpstd") {
                  vlog_txt("FAKEJUMPSTD\n");
                  arg1 = GetNum("FAKEJUMPSTD");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FAKEJUMPSTD);
                  rom(arg1, 1);
                }
                aa("fakecallstd") {
                  vlog_txt("FAKECALLSTD\n");
                  arg1 = GetNum("FAKECALLSTD");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FAKECALLSTD);
                  rom(arg1, 1);
                }
                aa("farreappear") {
                  vlog_txt("FARREAPPEAR\n");
                  arg1 = GetNum("FARREAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("FARREAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("FARREAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FARREAPPEAR);
                  rom(arg1, 2);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                aa("fardisappear") {
                  vlog_txt("FARDISAPPEAR\n");
                  arg1 = GetNum("FARDISAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("FARDISAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("FARDISAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_FARDISAPPEAR);
                  rom(arg1, 2);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'g':
                if (!strcmp(buf, "getplayerxy")) {
                  vlog_txt("GETPLAYERXY\n");
                  arg1 = GetNum("GETPLAYERXY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("GETPLAYERXY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_GETPLAYERXY);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("goto") goto JUMP;
                aa("giveitemtopc") {
                  vlog_txt("GIVEITEMTOPC\n");
                  arg1 = GetNum("GIVEITEMTOPC");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("GIVEITEMTOPC");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_GIVEITEMTOPC);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("giveegg") {
                  vlog_txt("GIVEEGG\n");
                  arg1 = GetNum("GIVEEGG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_GIVEEGG);
                  rom(arg1, 2);
                }
                aa("givetocoincase") {
                  vlog_txt("GIVETOCOINCASE\n");
                  arg1 = GetNum("GIVETOCOINCASE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_GIVETOCOINCASE);
                  rom(arg1, 2);
                }
                aa("givemoney") {
                  vlog_txt("GIVEMONEY\n");
                  BASIC(CMD_GIVEMONEY);
                  arg1 = GetNum("GIVEMONEY");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 4);
                  arg2 = GetNum("GIVEMONEY");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'h':
                if (!strcmp(buf, "hidepokepic")) {
                  BASIC(CMD_HIDEPOKEPIC);
                }
                aa("hidebox") {
                  vlog_txt("HIDEBOX\n");
                  arg1 = GetNum("HIDEBOX");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("HIDEBOX");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("HIDEBOX");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("HIDEBOX");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_HIDEBOX);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  rom(arg4, 1);
                }
                aa("hidecoins") {
                  vlog_txt("HIDECOINS\n");
                  arg1 = GetNum("HIDECOINS");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("HIDECOINS");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_HIDECOINS);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("hidemoney") {
                  vlog_txt("HIDEMONEY\n");
                  arg1 = GetNum("HIDEMONEY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("HIDEMONEY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_HIDEMONEY);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'i':
                if (!strcmp(buf, "if")) {
                  vlog_txt("IF\n");
                  arg1 = GetNum("IF");
                  if (!gffs) {
                    return;
                  }
                  i = skip_whitespace(Script, i);
                  j = 0;
                  while (chr != ' ' && chr != 0 && chr != '\'' && chr != ':' &&
                         chr != '@') {
                    buf[j] = chr;
                    i++;
                    j++;
                  }
                  buf[j] = 0;
                  if (!strcmp(buf, "jump") || !strcmp(buf, "goto")) {
                  JUMP:
                    BASIC(CMD_JUMPIF);
                    BASIC(arg1);
                    vlog_txt("   -> JUMP\n");
                    arg2 = GetNum("IF (JUMP)");
                    if (!gffs) {
                      return;
                    }
                    if ((arg2 & 0xff000000) == 0) {
                      arg2 |= 0x08000000;
                      sprintf(buf3, "   -> Converted to 0x%x\n", arg2);
                      vlog_txt(buf3);
                    }
                    rom(arg2, 4);
                  } else if (!strcmp(buf, "call")) {
                    BASIC(CMD_CALLIF);
                    BASIC(arg1);
                    vlog_txt("   -> CALL\n");
                    arg2 = GetNum("IF (CALL)");
                    if (!gffs) {
                      return;
                    }
                    if ((arg2 & 0xff000000) == 0) {
                      arg2 |= 0x08000000;
                      sprintf(buf3, "   -> Converted to 0x%x\n", arg2);
                      vlog_txt(buf3);
                    }
                    rom(arg2, 4);
                  } else if (!strcmp(buf, "callstd")) {
                    BASIC(CMD_CALLSTDIF);
                    BASIC(arg1);
                    vlog_txt("   -> CALLSTD\n");
                    arg2 = GetNum("IF (CALLSTD)");
                    if (!gffs) {
                      return;
                    }
                    rom(arg2, 1);
                  } else if (!strcmp(buf, "jumpstd")) {
                    BASIC(CMD_JUMPSTDIF);
                    BASIC(arg1);
                    vlog_txt("   -> JUMPSTD\n");
                    arg2 = GetNum("IF (JUMPSTD)");
                    if (!gffs) {
                      return;
                    }
                    rom(arg2, 1);
                  } else {
                    sprintf(buf2,
                            "IF (%s) was not correctly used. Use with "
                            "jump/goto, call, callstd and jumpstd.\n",
                            buf);
                    log_txt(buf2, strlen(buf2));
                    return;
                  }
                }
                aa("item") {
                  vlog_txt("ITEM\n");
                  arg1 = GetNum("ITEM");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'j':
                if (!strcmp(buf, "jumpram")) {
                  BASIC(CMD_JUMPRAM);
                }
                aa("jump") {
                  BASIC(CMD_JUMP);
                  vlog_txt("JUMP\n");
                  arg1 = GetNum("JUMP");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("jumpstd") {
                  vlog_txt("JUMPSTD\n");
                  arg1 = GetNum("JUMPSTD");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_JUMPSTD);
                  BASIC(arg1);
                }
                aa("jumpstdif") {
                  vlog_txt("jumpstdif\n");
                  arg1 = GetNum("jumpstdif");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("jumpstdif");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_JUMPSTDIF);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'k':
                if (!strcmp(buf, "killscript")) {
                  BASIC(CMD_KILLSCRIPT);
                  ec();
                } else
                  goto unk_cmd_fr;
                break;
              case 'l':
                if (!strcmp(buf, "lockall")) {
                  BASIC(CMD_LOCKALL);
                }
                aa("lock") { BASIC(CMD_LOCK); }
                aa("lastbattle") { BASIC(CMD_LASTBATTLE); }
                aa("lasttrainerbattle") { BASIC(CMD_LASTTRAINERBATTLE); }
                aa("loadpointer") {
                  BASIC(CMD_MSGBOX);
                  vlog_txt("LOADPOINTER\n");
                  arg1 = GetNum("LOADPOINTER");
                  if (!gffs) {
                    return;
                  }
                  BASIC(arg1);
                  arg2 = GetNum("LOADPOINTER");
                  if (!gffs) {
                    return;
                  }
                  if ((arg2 & 0xff000000) == 0) {
                    arg2 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg2);
                    vlog_txt(buf3);
                  }
                  rom(arg2, 4);
                }
                aa("loadbytefrompointer") {
                  vlog_txt("LOADBYTEFROMPOINTER\n");
                  arg1 = GetNum("LOADBYTEFROMPOINTER");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_LOADBYTEFROMPOINTER);
                  rom(arg1, 1);
                  arg2 = GetNum("LOADBYTEFROMPOINTER");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 4);
                }
                aa("lightroom") {
                  vlog_txt("LIGHTROOM\n");
                  arg1 = GetNum("LIGHTROOM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_LIGHTROOM);
                  rom(arg1, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'm':
                if (!strcmp(buf, "msgboxsign")) {
                  BASIC(CMD_MSGBOXSIGN);
                }
                aa("msgboxnormal") { BASIC(CMD_MSGBOXNORMAL); }
                aa("msgbox") {
                  BASIC(CMD_MSGBOX);
                  BASIC(0);
                  vlog_txt("MSGBOX\n");
                  arg1 = GetNum("MSGBOX");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("msgbox2") {
                  BASIC(CMD_MSGBOX2);
                  BASIC(0);
                  vlog_txt("MSGBOX2\n");
                  arg1 = GetNum("MSGBOX2");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("martend") {
                  vlog_txt("MARTEND\n");
                  rom(0, 2);
                }
                aa("message") {
                  vlog_txt("MESSAGE\n");
                  BASIC(CMD_MESSAGE);
                  arg1 = GetNum("MESSAGE");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("multichoice") {
                  vlog_txt("MULTICHOICE\n");
                  arg1 = GetNum("MULTICHOICE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("MULTICHOICE");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("MULTICHOICE");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("MULTICHOICE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_MULTICHOICE);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  rom(arg4, 1);
                }
                aa("multichoice2") {
                  vlog_txt("MULTICHOICE2\n");
                  arg1 = GetNum("MULTICHOICE2");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("MULTICHOICE2");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("MULTICHOICE2");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("MULTICHOICE2");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_MULTICHOICE2);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  rom(arg4, 2);
                }
                aa("multichoice3") {
                  vlog_txt("MULTICHOICE3\n");
                  arg1 = GetNum("MULTICHOICE3");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("MULTICHOICE3");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("MULTICHOICE3");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("MULTICHOICE3");
                  if (!gffs) {
                    return;
                  }
                  arg5 = GetNum("MULTICHOICE3");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_MULTICHOICE3);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  rom(arg4, 1);
                  rom(arg5, 1);
                }
                aa("movesprite2") {
                  vlog_txt("movesprite2\n");
                  arg1 = GetNum("movesprite2");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("movesprite2");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("movesprite2");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_MOVESPRITE2);
                  rom(arg1, 2);
                  rom(arg2, 2);
                  rom(arg3, 2);
                }
                aa("movesprite") {
                  vlog_txt("MOVESPRITE\n");
                  arg1 = GetNum("MOVESPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("MOVESPRITE");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("MOVESPRITE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_MOVESPRITE);
                  rom(arg1, 2);
                  rom(arg2, 2);
                  rom(arg3, 2);
                }
                aa("moveoffscreen") {
                  vlog_txt("MOVEOFFSCREEN\n");
                  arg1 = GetNum("MOVEOFFSCREEN");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_MOVEOFFSCREEN);
                  rom(arg1, 2);
                }
                aa("m") {
                  vlog_txt("Movement data...\n");
                  add_data(c, trans, transbackmove(Script, &i));
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'n':
                if (!strcmp(buf, "nop") || !strcmp(buf, "nop0")) {
                  BASIC(CMD_NOP0);
                }
                aa("nop1") { BASIC(CMD_NOP1); }
                aa("namepokemon") {
                  BASIC(CMD_FADESCREEN);
                  BASIC(1);
                  BASIC(CMD_SPECIAL);
                  rom(0x9E, 2);
                  BASIC(CMD_WAITSPECIAL);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'p':
                if (!strcmp(buf, "pause")) {
                  vlog_txt("PAUSE\n");
                  arg1 = GetNum("PAUSE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_PAUSE);
                  rom(arg1, 2);
                }
                aa("playsound") {
                  vlog_txt("PLAYSOUND\n");
                  arg1 = GetNum("PLAYSOUND");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("PLAYSOUND");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_PLAYSOUND);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                aa("playsong") {
                  vlog_txt("PLAYSONG\n");
                  arg1 = GetNum("PLAYSONG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_PLAYSONG);
                  rom(arg1, 2);
                }
                aa("pauseevent") {
                  BASIC(CMD_PAUSEEVENT);
                  vlog_txt("PAUSEEVENT\n");
                  arg1 = GetNum("PAUSEEVENT");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 2);
                }
                aa("picture") {
                  vlog_txt("PICTURE\n");
                  arg1 = GetNum("PICTURE");
                  if (!gffs) {
                    return;  // GFFS stands for Get-For-Function success. GetNum
                             // sets this to 0 on failure.
                  }
                  BASIC(CMD_PICTURE);
                  rom(arg1, 1);
                }
                aa("pokemart") {
                  vlog_txt("POKEMART\n");
                  BASIC(CMD_POKEMART);
                  arg1 = GetNum("POKEMART");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("pokemart2") {
                  vlog_txt("POKEMART2\n");
                  BASIC(CMD_POKEMART2);
                  arg1 = GetNum("POKEMART2");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("pokemart3") {
                  vlog_txt("POKEMART3\n");
                  BASIC(CMD_POKEMART3);
                  arg1 = GetNum("POKEMART3");
                  if (!gffs) {
                    return;
                  }
                  if ((arg1 & 0xff000000) == 0) {
                    arg1 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg1);
                    vlog_txt(buf3);
                  }
                  rom(arg1, 4);
                }
                aa("paymoney") {
                  vlog_txt("PAYMONEY\n");
                  arg1 = GetNum("PAYMONEY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("PAYMONEY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_PAYMONEY);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'r':
                if (!strcmp(buf, "return")) {
                  BASIC(CMD_RETURN);
                }
                aa("release") { BASIC(CMD_RELEASE); }
                aa("releaseall") { BASIC(CMD_RELEASEALL); }
                aa("resetweather") { BASIC(CMD_RESETWEATHER); }
                aa("resetvars") { BASIC(CMD_RESETVARS); }
                aa("random") {
                  vlog_txt("RANDOM\n");
                  arg1 = GetNum("RANDOM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_RANDOM);
                  rom(arg1, 2);
                }
                aa("removeitem") {
                  vlog_txt("REMOVEITEM\n");
                  arg1 = GetNum("REMOVEITEM");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("REMOVEITEM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_REMOVEITEM);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("resetspritelevel") {
                  vlog_txt("RESETSPRITELEVEL\n");
                  arg1 = GetNum("RESETSPRITELEVEL");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("RESETSPRITELEVEL");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("RESETSPRITELEVEL");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_RESETSPRITELEVEL);
                  rom(arg1, 2);
                  rom(arg2, 2);
                  rom(arg3, 2);
                }
                aa("reappear") {
                  vlog_txt("REAPPEAR\n");
                  arg1 = GetNum("REAPPEAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_REAPPEAR);
                  rom(arg1, 2);
                }
                aa("reappearat") {
                  vlog_txt("REAPPEARAT\n");
                  arg1 = GetNum("REAPPEARAT");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("REAPPEARAT");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("REAPPEARAT");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_REAPPEARAT);
                  rom(arg1, 2);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                break;
              case 's':
                if (!strcmp(buf, "showmsg")) {
                  BASIC(CMD_SHOWMSG);
                }
                aa("startcontest") { BASIC(CMD_STARTCONTEST); }
                aa("startwireless") { BASIC(CMD_STARTWIRELESS); }
                aa("showpokepic") {
                  vlog_txt("SHOWPOKEPIC\n");
                  arg1 = GetNum("SHOWPOKEPIC");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SHOWPOKEPIC");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("SHOWPOKEPIC");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SHOWPOKEPIC);
                  rom(arg1, 2);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                aa("storeitems") {
                  vlog_txt("STOREITEMS\n");
                  arg1 = GetNum("STOREITEMS");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREITEMS");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("STOREITEMS");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREITEMS);
                  rom(arg1, 1);
                  rom(arg2, 2);
                  rom(arg3, 2);
                }
                aa("setpokemonpp") {
                  vlog_txt("SETPOKEMONPP\n");
                  arg1 = GetNum("SETPOKEMONPP");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETPOKEMONPP");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("SETPOKEMONPP");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETPOKEMONPP);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 2);
                }
                aa("storepartypokemon") {
                  vlog_txt("STOREPARTYPOKEMON\n");
                  arg1 = GetNum("STOREPARTYPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREPARTYPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREPARTYPOKEMON);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storefirstpokemon") {
                  vlog_txt("STOREFIRSTPOKEMON\n");
                  arg1 = GetNum("STOREFIRSTPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREFIRSTPOKEMON);
                  rom(arg1, 1);
                }
                aa("setcatchlocation") {
                  vlog_txt("SETCATCHLOCATION\n");
                  arg1 = GetNum("SETCATCHLOCATION");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETCATCHLOCATION");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETCATCHLOCATION);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                aa("setdoorclosed") {
                  vlog_txt("SETDOORCLOSED\n");
                  arg1 = GetNum("SETDOORCLOSED");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETDOORCLOSED");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETDOORCLOSED);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setdoorclosedstatic") {
                  vlog_txt("SETDOORCLOSEDSTATIC\n");
                  arg1 = GetNum("SETDOORCLOSEDSTATIC");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETDOORCLOSEDSTATIC");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETDOORCLOSEDSTATIC);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setdooropened") {
                  vlog_txt("SETDOOROPENED\n");
                  arg1 = GetNum("SETDOOROPENED");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETDOOROPENED");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETDOOROPENED);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setdooropenedstatic") {
                  vlog_txt("SETDOOROPENEDSTATIC\n");
                  arg1 = GetNum("SETDOOROPENEDSTATIC");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETDOOROPENEDSTATIC");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETDOOROPENEDSTATIC);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setanimation") {
                  vlog_txt("SETANIMATION\n");
                  arg1 = GetNum("SETANIMATION");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETANIMATION");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETANIMATION);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("setobedience") {
                  vlog_txt("SETOBEDIENCE\n");
                  arg1 = GetNum("SETOBEDIENCE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETOBEDIENCE);
                  rom(arg1, 2);
                }
                aa("setbyte") {
                  vlog_txt("SETBYTE\n");
                  arg1 = GetNum("SETBYTE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETBYTE);
                  BASIC(arg1);
                }
                aa("spriteface") {
                  vlog_txt("SPRITEFACE\n");
                  arg1 = GetNum("SPRITEFACE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SPRITEFACE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SPRITEFACE);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                aa("sethealingplace") {
                  vlog_txt("SETHEALINGPLACE\n");
                  arg1 = GetNum("SETHEALINGPLACE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETHEALINGPLACE);
                  rom(arg1, 2);
                }
                aa("special") {
                  vlog_txt("SPECIAL\n");
                  arg1 = GetNum("SPECIAL");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SPECIAL);
                  rom(arg1, 2);
                }
                aa("setworldmapflag") {
                  vlog_txt("SETWORLDMAPFLAG\n");
                  arg1 = GetNum("setworldmapflag");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETWORLDMAPFLAG);
                  rom(arg1, 2);
                }
                aa("special2") {
                  vlog_txt("SPECIAL2\n");
                  arg1 = GetNum("SPECIAL2");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SPECIAL2");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SPECIAL2);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setvar") {
                  vlog_txt("SETVAR\n");
                  arg1 = GetNum("SETVAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETVAR);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setflag") {
                  vlog_txt("SETFLAG\n");
                  arg1 = GetNum("SETFLAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETFLAG);
                  rom(arg1, 2);
                }
                aa("sound") {
                  vlog_txt("SOUND\n");
                  arg1 = GetNum("SOUND");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SOUND);
                  rom(arg1, 2);
                }
                aa("storepokemon") {
                  vlog_txt("STOREPOKEMON\n");
                  arg1 = GetNum("STOREPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREPOKEMON");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREPOKEMON);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storeboxname") {
                  vlog_txt("STOREBOXNAME\n");
                  arg1 = GetNum("STOREBOXNAME");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREBOXNAME");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREBOXNAME);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storevar") {
                  vlog_txt("STOREVAR\n");
                  arg1 = GetNum("STOREVAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREVAR);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storetext") {
                  vlog_txt("STORETEXT\n");
                  BASIC(CMD_STORETEXT);
                  arg1 = GetNum("STORETEXT");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 1);
                  arg2 = GetNum("STORETEXT");
                  if (!gffs) {
                    return;
                  }
                  if ((arg2 & 0xff000000) == 0) {
                    arg2 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg2);
                    vlog_txt(buf3);
                  }
                  rom(arg2, 4);
                }
                aa("setmaptile") {
                  vlog_txt("SETMAPTILE\n");
                  arg1 = GetNum("SETMAPTILE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETMAPTILE");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("SETMAPTILE");
                  if (!gffs) {
                    return;
                  }
                  arg4 = GetNum("SETMAPTILE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETMAPTILE);
                  rom(arg1, 2);
                  rom(arg2, 2);
                  rom(arg3, 2);
                  rom(arg4, 2);
                }
                aa("setmapfooter") {
                  vlog_txt("SETMAPFOOTER\n");
                  arg1 = GetNum("SETMAPFOOTER");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETMAPFOOTER);
                  rom(arg1, 2);
                }
                aa("setweather") {
                  vlog_txt("SETWEATHER\n");
                  arg1 = GetNum("SETWEATHER");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETWEATHER);
                  if (mode == FIRE_RED) {
                    rom(arg1, 2);
                  } else {
                    arg2 = GetNum("SETWEATHER");
                    if (!gffs) {
                      return;
                    }
                    rom(arg1, 1);
                    rom(arg2, 1);
                  }
                }
                aa("storeitem") {
                  vlog_txt("STOREITEM\n");
                  arg1 = GetNum("STOREITEM");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREITEM");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREITEM);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storeattack") {
                  vlog_txt("STOREATTACK\n");
                  arg1 = GetNum("STOREATTACK");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREATTACK");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREATTACK);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storefurniture") {
                  vlog_txt("STOREFURNITURE\n");
                  arg1 = GetNum("STOREFURNITURE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STOREFURNITURE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STOREFURNITURE);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("storecomp") {
                  vlog_txt("STORECOMP\n");
                  arg1 = GetNum("STORECOMP");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("STORECOMP");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_STORECOMP);
                  rom(arg1, 1);
                  rom(arg2, 2);
                }
                aa("slotmachine") {
                  vlog_txt("SLOTMACHINE\n");
                  arg1 = GetNum("SLOTMACHINE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SLOTMACHINE);
                  rom(arg1, 2);
                }
                aa("showmoney") {
                  vlog_txt("SHOWMONEY\n");
                  arg1 = GetNum("SHOWMONEY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SHOWMONEY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SHOWMONEY);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                aa("showyesno") {
                  vlog_txt("SHOWYESNO\n");
                  arg1 = GetNum("SHOWYESNO");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SHOWYESNO");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SHOWYESNO);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("settrainerflag") {
                  vlog_txt("SETTRAINERFLAG\n");
                  arg1 = GetNum("SETTRAINERFLAG");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETTRAINERFLAG);
                  rom(arg1, 2);
                }
                aa("showcoins") {
                  vlog_txt("SHOWCOINS\n");
                  arg1 = GetNum("SHOWCOINS");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SHOWCOINS");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SHOWCOINS);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("subtractvar") {
                  vlog_txt("SUBTRACTVAR\n");
                  arg1 = GetNum("SUBTRACTVAR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SUBTRACTVAR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SUBTRACTVAR);
                  rom(arg1, 2);
                  rom(arg2, 2);
                }
                aa("setfarbyte") {
                  vlog_txt("SETFARBYTE\n");
                  BASIC(CMD_SETFARBYTE);
                  arg1 = GetNum("SETFARBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 1);
                  arg2 = GetNum("SETFARBYTE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 4);
                }
                aa("setbyte2") {
                  vlog_txt("SETBYTE2\n");
                  arg1 = GetNum("SETBYTE2");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("SETBYTE2");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SETBYTE2);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("spritebehave") {
                  vlog_txt("spritebehave\n");
                  arg1 = GetNum("spritebehave");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("spritebehave");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_SPRITEBEHAVE);
                  rom(arg1, 2);
                  rom(arg2, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 't':
                if (!strcmp(buf, "takefurniture")) {
                  vlog_txt("TAKEFURNITURE\n");
                  arg1 = GetNum("TAKEFURNITURE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_TAKEFURNITURE);
                  rom(arg1, 2);
                }
                aa("tempspriteface") {
                  vlog_txt("TEMPSPRITEFACE\n");
                  arg1 = GetNum("TEMPSPRITEFACE");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("TEMPSPRITEFACE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_TEMPSPRITEFACE);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("trainerbattle") {
                  BASIC(CMD_TRAINERBATTLE);
                  vlog_txt("TRAINERBATTLE\n");
                  arg1 = GetNum("TRAINERBATTLE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg1, 1);
                  arg2 = GetNum("TRAINERBATTLE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 2);
                  arg3 = GetNum("TRAINERBATTLE");
                  if (!gffs) {
                    return;
                  }
                  rom(arg3, 2);
                  arg4 = GetNum("TRAINERBATTLE");
                  if (!gffs) {
                    return;
                  }
                  if ((arg4 & 0xff000000) == 0) {
                    arg4 |= 0x08000000;
                    sprintf(buf3, "   -> Converted to 0x%x\n", arg4);
                    vlog_txt(buf3);
                  }
                  rom(arg4, 4);
                  if (arg1 != 3) {
                    arg5 = GetNum("TRAINERBATTLE");
                    if (!gffs) {
                      return;
                    }
                    if ((arg5 & 0xff000000) == 0) {
                      arg5 |= 0x08000000;
                      sprintf(buf3, "   -> Converted to 0x%x\n", arg4);
                      vlog_txt(buf3);
                    }
                    rom(arg5, 4);
                  }
                  if (arg1 == 1 || arg1 == 2) {
                    arg6 = GetNum("TRAINERBATTLE");
                    if (!gffs) {
                      return;
                    }
                    if ((arg6 & 0xff000000) == 0) {
                      arg6 |= 0x08000000;
                      sprintf(buf3, "   -> Converted to 0x%x\n", arg4);
                      vlog_txt(buf3);
                    }
                    rom(arg6, 4);
                  }
                }
                aa("takefromcoincase") {
                  vlog_txt("TAKEFROMCOINCASE\n");
                  arg1 = GetNum("TAKEFROMCOINCASE");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_TAKEFROMCOINCASE);
                  rom(arg1, 2);
                }
                else if (!strcmp(buf, "textcolor") ||
                         !strcmp(buf, "textcolour")) {
                  vlog_txt("TEXTCOLOUR\n");
                  arg1 = GetNum("TEXTCOLOUR");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_TEXTCOLOR);
                  rom(arg1, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'u':
                if (!strcmp(buf, "updatecoins")) {
                  vlog_txt("UPDATECOINS\n");
                  arg1 = GetNum("UPDATECOINS");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("UPDATECOINS");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_UPDATECOINS);
                  rom(arg1, 1);
                  rom(arg2, 1);
                }
                aa("updatemoney") {
                  vlog_txt("UPDATEMONEY\n");
                  arg1 = GetNum("UPDATEMONEY");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("UPDATEMONEY");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("UPDATEMONEY");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_UPDATEMONEY);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case 'w':
                if (!strcmp(buf, "waitbutton")) {
                  BASIC(CMD_WAITBUTTON);
                }
                aa("waitfanfare") { BASIC(CMD_WAITFANFARE); }
                aa("waitspecial") { BASIC(CMD_WAITSPECIAL); }
                aa("warpteleport2") {
                  vlog_txt("WARPTELEPORT2\n");
                  arg1 = GetNum("WARPTELEPORT2");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARPTELEPORT2");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARPTELEPORT2");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARPTELEPORT2");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARPTELEPORT2");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARPTELEPORT2);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warp6") {
                  vlog_txt("WARP6\n");
                  arg1 = GetNum("WARP6");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARP6");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARP6");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARP6");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARP6");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARP6);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warpmuted") {
                  vlog_txt("WARPMUTED\n");
                  arg1 = GetNum("WARPMUTED");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARPMUTED");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARPMUTED");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARPMUTED");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARPMUTED");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARPMUTED);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warpwalking") {
                  vlog_txt("WARPWALKING\n");
                  arg1 = GetNum("WARPWALKING");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARPWALKING");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARPWALKING");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARPWALKING");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARPWALKING");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARPWALKING);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warpteleport") {
                  vlog_txt("WARPTELEPORT\n");
                  arg1 = GetNum("WARPTELEPORT");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARPTELEPORT");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARPTELEPORT");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARPTELEPORT");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARPTELEPORT");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARPTELEPORT);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warp3") {
                  vlog_txt("WARP3\n");
                  arg1 = GetNum("WARP3");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARP3");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARP3");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARP3");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARP3");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARP3);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warp4") {
                  vlog_txt("WARP4\n");
                  arg1 = GetNum("WARP4");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARP4");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARP4");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARP4");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARP4");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARP4);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warp5") {
                  vlog_txt("WARP5\n");
                  arg1 = GetNum("WARP5");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARP5");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARP5");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARP5");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARP5");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARP5);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("warpelevator") {
                  vlog_txt("WARPELEVATOR\n");
                  arg1 = GetNum("WARPELEVATOR");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARPELEVATOR");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARPELEVATOR");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARPELEVATOR");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARPELEVATOR");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARPELEVATOR);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                aa("writebytetooffset") {
                  vlog_txt("WRITEBYTETOOFFSET\n");
                  arg1 = GetNum("WRITEBYTETOOFFSET");
                  if (!gffs) {
                    return;
                  }
                  BASIC(CMD_WRITEBYTETOOFFSET);
                  rom(arg1, 1);
                  arg2 = GetNum("WRITEBYTETOOFFSET");
                  if (!gffs) {
                    return;
                  }
                  rom(arg2, 4);
                }
                aa("waitcry") {
                  vlog_txt("WAITCRY\n");
                  BASIC(CMD_WAITCRY);
                }
                aa("warp") {
                  vlog_txt("WARP\n");
                  arg1 = GetNum("WARP");
                  if (!gffs) {
                    return;
                  }
                  arg2 = GetNum("WARP");
                  if (!gffs) {
                    return;
                  }
                  arg3 = GetNum("WARP");
                  if (!gffs) {
                    return;
                  }
                  if (arg3 == 0xFF) {
                    arg4 = GetNum("WARP");
                    if (!gffs) {
                      return;
                    }
                    arg5 = GetNum("WARP");
                    if (!gffs) {
                      return;
                    }
                  }
                  BASIC(CMD_WARP);
                  rom(arg1, 1);
                  rom(arg2, 1);
                  rom(arg3, 1);
                  if (arg3 == 0xFF) {
                    rom(arg4, 2);
                    rom(arg5, 2);
                  }
                }
                else goto unk_cmd_fr;
                ec();
                break;
              case '-':
                try_asm();
                break;
              case ':':
                if (c) add_label(buf, c, c->size, &cl);
                ec();
                break;
              case '=':
                if (strcmp(buf, "=")) goto unk_cmd_fr;
                vlog_txt("[STRING]\n");
                if (chr == ' ') {
                  i++;
                } else {
                  log_txt("Should have a space after the =\n", 33 - 1);
                }
                temp_ptr = transbackstr(script, i - fst, c);
                while (chr != '\n' && chr != 0) {
                  i++;
                }
                sprintf(buf2, "   -> %s\n", (char *)temp_ptr);
                free(temp_ptr);
                vlog_txt(buf2);
                break;
              case '.':
                vlog_txt("[BINARY]\n   ->");
                i = buf_loc + 1;
                i = skip_whitespace(Script, i);
                k = 0;
                while (chr == ' ' || chr == '-' || chr == '.' || chr == ':') {
                  i++;
                }
                while (chr != '\n' && chr != 0 && chr != '\'') {
                  k = 1 - k;
                  j = 0;
                  if (chr >= '0' && chr <= '9')
                    j = chr - '0';
                  else if (chr >= 'a' && chr <= 'f')
                    j = chr - ('a' - 0xA);
                  else {
                    sprintf(buf2, "Failed to understand hex character '%c'\n",
                            chr);
                    log_txt(buf2, strlen(buf2));
                    return;
                  }
                  if (k == 0) {
                    l |= j;
                    rom(l, 1);
                    if (IsVerbose) {
                      sprintf(buf2, " %02X", l);
                      log_txt(buf2, strlen(buf2));
                    }
                  } else {
                    l = j << 4;
                  }
                  i++;
                  while (chr == ' ' || chr == '-' || chr == '.' || chr == ':') {
                    i++;
                  }
                }
                vlog_txt("\n");
                break;
              default:
              unk_cmd_fr:
                sprintf(buf2, "Warning: Unknown command \"%s\"\n", buf);
                log_txt(buf2, strlen(buf2));
                while (chr != '\n' && chr != 0) {
                  i++;
                }
            }
            break;
        }
        i++;
      }
    }
    free(Script);
    if (IncFile) fclose(IncFile);
#ifndef DLL
    if (strcmp(script, "")) fclose(CurrFile);
  } else {
    printf("Error opening script.");
    return;
  }
#endif
#ifdef WIN32
  OutputDebugString("Finished compiler loop, processing #ORGs");
#ifdef DLL
  SendMessage(GetDlgItem(HW_DLG, 3), LB_RESETCONTENT, 0, 0);
#endif
#endif
  if (dynu && start == 0) {
    log_txt("Error: No #dyn used with dynamic offsets!\n", 43 - 1);
  } else {
    vlog_txt("\n#ORG: data\n");
    calc_org(c, start, romfn, defines);
    process_inserts(c, cl);
#ifdef WIN32
    OutputDebugString("Calculated ORGs, processed inserts");
#endif
    if (c != NULL) c = rewind_codeblock(c);
    d = c;
    while (c) {
      c->org &= 0x07FFFFFF;
      if (c->name) {
        if (mode != GOLD && mode != CRYSTAL)
          sprintf(buf, "   -> %s <-> 0x%X (0x%X bytes)\n", c->name, c->org,
                  c->size);
        else {
          j = OffsetToPointer(c->org);
          sprintf(buf, "   -> %s <-> 0x%X -> %02X:%04X (0x%X bytes)\n", c->name,
                  c->org, j & 0xFF, (j >> 8) & 0xFFFF, c->size);
        }
        log_txt(buf, strlen(buf));
#ifdef WIN32
#ifdef DLL
        strings = LocalAlloc(LPTR, strlen(c->name) + 60);
        if (mode != GOLD && mode != CRYSTAL)
          sprintf(strings, "%s <-> 0x%X (0x%X bytes)", c->name, c->org,
                  c->size);
        else {
          j = OffsetToPointer(c->org);
          sprintf(strings, "%s <-> 0x%X -> %02X:%04X (0x%X bytes)", c->name,
                  c->org, j & 0xFF, (j >> 8) & 0xFFFF, c->size);
        }
        SendMessage(GetDlgItem(HW_DLG, 3), LB_ADDSTRING, 0, (LPARAM)strings);
        LocalFree(strings);
        needdlg = 1;
#endif
#endif
      } else {
        sprintf(buf, "   -> 0x%X (0x%X bytes)\n", c->org, c->size);
        log_txt(buf, strlen(buf));
      }
      for (cl2 = cl; cl2; cl2 = cl2->next) {
        if (cl2->block == c) {
          if (mode != GOLD && mode != CRYSTAL)
            sprintf(buf, "      -> %s <-> 0x%X\n", cl2->name,
                    c->org + cl2->pos);
          else {
            j = OffsetToPointer(c->org + cl2->pos);
            sprintf(buf, "      -> %s <-> 0x%X -> %02X:%04X\n", cl2->name,
                    c->org + cl2->pos, j & 0xFF, (j >> 8) & 0xFFFF);
          }
          log_txt(buf, strlen(buf));
#ifdef WIN32
#ifdef DLL
          strings = LocalAlloc(LPTR, strlen(cl2->name) + 60);
          if (mode != GOLD && mode != CRYSTAL)
            sprintf(strings, "-> %s <-> 0x%X", cl2->name, c->org + cl2->pos);
          else {
            j = OffsetToPointer(c->org + cl2->pos);
            sprintf(strings, "-> %s <-> 0x%X -> %02X:%04X", cl2->name,
                    c->org + cl2->pos, j & 0xFF, (j >> 8) & 0xFFFF);
          }
          SendMessage(GetDlgItem(HW_DLG, 3), LB_ADDSTRING, 0, (LPARAM)strings);
          LocalFree(strings);
          needdlg = 1;
#endif
#endif
        }
      }
      if (!testing) {
        fseek(RomFile, c->org, SEEK_SET);
        fwrite(c->data, 1, c->size, RomFile);
      }
      c = c->next;
    }
  }
  fclose(RomFile);
  c = d;
  while (c) {
    d = c->next;
    delete_inserts(c);
    if (c->data) free(c->data);
    if (c->name) free(c->name);
    free(c);
    c = d;
  }
  while (cl) {
    cl2 = cl->next;
    if (cl->name) free(cl->name);
    free(cl);
    cl = cl2;
  }
#ifdef WIN32
#ifdef DLL
  if (needdlg) {
    ShowWindow(HW_DLG, SW_SHOW);
    SetWindowPos(HW_DLG, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
  } else
    ShowWindow(HW_DLG, SW_HIDE);
#endif
#endif
#ifndef DLL
  fclose(LogFile);
#endif
  return;
}
