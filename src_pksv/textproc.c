/*
                Sophie Daffern (Score_Under)'s PKSV - Pokemon ROM script
   viewer/editor Copyright (C) 2007  Sophie Daffern

                This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as published by
                the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

                This program is distributed in the hope that it will be useful,
                but WITHOUT ANY WARRANTY; without even the implied warranty of
                MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                GNU General Public License for more details.

                You should have received a copy of the GNU General Public
   License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "textproc.h"
#include "codeproc.h"
#include "textutil.h"
#include "pksv.h"

#include "sublang/moves.h"

char trans[65536];

void log_txt(const char *str, size_t length) {
  fwrite(str, 1, length, LogFile ? LogFile : stderr);
}

char *transtxt(int howfar, const char *file, size_t word_wrap,
               uint32_t *resume_out) {
  unsigned int pt = 0, sl, arg1, arg2;
  unsigned char p, code;
  const char hex[17] = "0123456789ABCDEF";
  char buf[128];
  unsigned int read;
  char readagain, sub_going;
  FILE *fileC;
  bool resume = false;
  read = 0;
  fileC = fopen(file, "rb");
  *trans = 0;
  if (fileC != NULL) {
    fseek(fileC, (howfar & 0x7ffffff), SEEK_SET);
    if (mode != GOLD && mode != CRYSTAL) {
      while (true) {
        read = (int)fread(&p, 1, 1, fileC);
        if (read == 0) {
          strcpy(&trans[pt], "\\x");
          break;
        } else {
          if (p >= 0xbb && p <= 0xd4) {
            p -= 0x7a;  // capital letter
          } else if (p >= 0xd5 && p <= 0xee) {
            p -= 0x74;  // lower letter
          } else if (p >= 0xA1 && p <= 0xAA) {
            p -= 0x71;
          } else if (p == 0) {
            if (word_wrap && pt >= word_wrap) {
              strcpy(&trans[pt], " \\x");
              resume = true;
              break;
            }
            p = 0x20;  // space
          } else if (p == 0x1b) {
            trans[pt] = '\\';  // e acute
            pt++;
            p = 'e';
          } else if (p == (unsigned char)0xFC) {
            read = (int)fread(&p, 1, 1, fileC);
            if (p == 9) {
              trans[pt] = '\\';
              pt++;
              p = 'w';
            } else {
              trans[pt] = '\\';  // control code
              pt++;
              trans[pt] = 'c';
              pt++;
              trans[pt] = '\\';
              pt++;
              trans[pt] = 'h';
              pt++;
              trans[pt] = hex[(p & 0xf0) >> 4];
              pt++;
              p = hex[p & 0xf];
            }
          } else if (p == (unsigned char)0xac) {
            p = '?';  //(unsigned char) keeps Microsoft VS happy
          } else if (p == 0xad) {
            p = '.';
          } else if (p == 0xb8) {
            p = ',';
          } else if (p == 0xb4) {
            p = '\'';
          } else if (p == 0xae) {
            p = '-';
          } else if (p == 0xb1) {
            p = '"';
          } else if (p == 0xb2) {
            trans[pt] = '[';
            pt++;
            trans[pt] = '"';
            pt++;
            p = ']';
          } else if (p == 0xfe)  // \n
          {
            if (word_wrap) {
              strcpy(&trans[pt], "\\n\\x");
              resume = true;
              break;
            }
            trans[pt] = '\\';
            pt++;
            p = 'n';
          } else if (p == 0xb7)  // [p]
          {
            trans[pt] = '[';
            pt++;
            trans[pt] = 'p';
            pt++;
            p = ']';
          } else if (p == 0xfa)  // \l
          {
            if (word_wrap) {
              strcpy(&trans[pt], "\\l\\x");
              resume = true;
              break;
            }
            trans[pt] = '\\';
            pt++;
            p = 'l';
          } else if (p == 0xfb)  // \p
          {
            if (word_wrap) {
              strcpy(&trans[pt], "\\p\\x");
              resume = true;
              break;
            }
            trans[pt] = '\\';
            pt++;
            p = 'p';
          } else if (p == 0xff) {
            // end of text
            trans[pt] = '\0';
            break;
          } else if (p == 0xf0) {
            p = ':';
          } else if (p == 0xfd) {
            trans[pt] = '\\';
            pt++;
            trans[pt] = 'v';
            pt++;
            trans[pt] = '\\';
            pt++;
            trans[pt] = 'h';
            pt++;
            fread(&p, 1, 1, fileC);
            trans[pt] = hex[(p & 0xf0) >> 4];
            pt++;
            p = hex[p & 0xf];
          } else if (p == 0xab) {
            p = '!';
          } else if (p == 0xb0) {
            trans[pt] = '[';
            pt++;
            trans[pt] = '.';
            pt++;
            p = ']';
          } else {
            trans[pt] = '\\';
            pt++;
            trans[pt] = 'h';
            pt++;
            trans[pt] = hex[(p & 0xf0) >> 4];
            pt++;
            p = hex[p & 0xf];
          }
          if (pt >= sizeof trans - 30) {
            strcpy(&trans[pt], "\\x");
            resume = true;
            break;
          }
          trans[pt] = p;
          pt++;
        }
      }
    } else {
      strcpy(trans, "");
      char still_going = 1;
      while (still_going) {
        read = (unsigned int)fread(&code, 1, 1, fileC);
        if (read == 0) {
          break;
        }
        sub_going = 1;
        readagain = 1;
        if (code == 0) {
          strcat(trans, "= ");
          while (sub_going) {
            sl = (unsigned int)strlen(trans);
            if (readagain) read = (unsigned int)fread(&p, 1, 1, fileC);
            readagain = 1;
            if (read == 0) {
              still_going = sub_going = 0;
              break;
            }
            if ((p >= 0x80 && p <= 0x99) || (p >= 0xA0 && p <= 0xB9)) {
              trans[sl] = p - 0x3F;  // letter
              trans[sl + 1] = 0;
            } else if (p == 0x7F) {
              trans[sl] = ' ';
              trans[sl + 1] = 0;
            } else if (p == 0x75) {
              strcat(trans, "[.]");
            } else if (p == 0x56) {
              strcat(trans, "[..]");
            } else if (p == 0x9C) {
              trans[sl] = ':';
              trans[sl + 1] = 0;
            } else if (p == 0xE6) {
              trans[sl] = '?';
              trans[sl + 1] = 0;
            } else if (p == 0xE7) {
              trans[sl] = '!';
              trans[sl + 1] = 0;
            } else if (p == 0xE8) {
              trans[sl] = '.';
              trans[sl + 1] = 0;
            } else if (p == 0xF4) {
              trans[sl] = ',';
              trans[sl + 1] = 0;
            } else if (p == 0xE3) {
              trans[sl] = '-';
              trans[sl + 1] = 0;
            } else if (p == 0xE0) {
              trans[sl] = '\'';
              trans[sl + 1] = 0;
            } else if (p == 0xF0) {
              trans[sl] = '$';
              trans[sl + 1] = 0;
            } else if (p == 0xF6) {
              trans[sl] = '0';
              trans[sl + 1] = 0;
            } else if (p == 0xF7) {
              trans[sl] = '1';
              trans[sl + 1] = 0;
            } else if (p == 0xF8) {
              trans[sl] = '2';
              trans[sl + 1] = 0;
            } else if (p == 0xF9) {
              trans[sl] = '3';
              trans[sl + 1] = 0;
            } else if (p == 0xFA) {
              trans[sl] = '4';
              trans[sl + 1] = 0;
            } else if (p == 0xFB) {
              trans[sl] = '5';
              trans[sl + 1] = 0;
            } else if (p == 0xFC) {
              trans[sl] = '6';
              trans[sl + 1] = 0;
            } else if (p == 0xFD) {
              trans[sl] = '7';
              trans[sl + 1] = 0;
            } else if (p == 0xFE) {
              trans[sl] = '8';
              trans[sl + 1] = 0;
            } else if (p == 0xFF) {
              trans[sl] = '9';
              trans[sl + 1] = 0;
            } else if (p == 0xD0) {
              strcat(trans, "'d");
            } else if (p == 0xD1) {
              strcat(trans, "'l");
            } else if (p == 0xD2) {
              strcat(trans, "'m");
            } else if (p == 0xD3) {
              strcat(trans, "'r");
            } else if (p == 0xD4) {
              strcat(trans, "'s");
            } else if (p == 0xD5) {
              strcat(trans, "'t");
            } else if (p == 0xD6) {
              strcat(trans, "'v");
            } else if (p == 0x50) {
              sub_going = 0;
            } else if (p == 0x51) {
              strcat(trans, "\\p");
            } else if (p == 0x52) {
              strcat(trans, "[PLAYER]");
            } else if (p == 0x54) {
              strcat(trans, "[POKe]");
            } else if (p == 0x55) {
              strcat(trans, "\\l");
            } else if (p == 0x4F) {
              strcat(trans, "\\n");
            } else if (p == 0x57) {
              readagain = 0;
              fread(&p, 1, 1, fileC);
              if (p == 0) {
                strcat(trans, "\\e\n");
                still_going = 0;
                break;
              }
              strcat(trans, "\\h57");
            } else {
              sprintf(buf, "\\h%02X", p);
              strcat(trans, buf);
            }
          }
          strcat(trans, "\n");
        } else if (code == 1) {
          pt = 0;
          fread(&pt, 1, 2, fileC);
          sprintf(buf, "text-ram 0x%X\n", pt);
          strcat(trans, buf);
        } else if (code == 2) {
          pt = 0;
          fread(&pt, 1, 2, fileC);
          fread(&p, 1, 1, fileC);
          if (p >> 5 == 7) {
            sprintf(buf, "0x%X 0x%X money-no-zeros", pt, p & 0x1F);
          } else if (p >> 5 == 6) {
            sprintf(buf, "0x%X 0x%X no-zeros", pt, p & 0x1F);
          } else if (p >> 5 == 5) {
            sprintf(buf, "0x%X 0x%X money-spaces", pt, p & 0x1F);
          } else if (p >> 5 == 4) {
            sprintf(buf, "0x%X 0x%X spaces", pt, p & 0x1F);
          } else if (p >> 5 == 3) {
            sprintf(buf, "0x%X 0x%X money2", pt, p & 0x1F);
          } else if (p >> 5 == 2) {
            sprintf(buf, "0x%X 0x%X normal2", pt, p & 0x1F);
          } else if (p >> 5 == 1) {
            sprintf(buf, "0x%X 0x%X money", pt, p & 0x1F);
          } else {
            sprintf(buf, "0x%X 0x%X normal", pt, p & 0x1F);
          }
          strcat(trans, "text-hex ");
          strcat(trans, buf);
          strcat(trans, "\n");
        } else if (code == 3) {
          pt = 0;
          fread(&pt, 1, 2, fileC);
          sprintf(buf, "text-reloc 0x%X\n", pt);
          strcat(trans, buf);
        } else if (code == 4) {
          pt = arg1 = arg2 = 0;
          fread(&pt, 1, 2, fileC);
          fread(&arg1, 1, 1, fileC);
          fread(&arg2, 1, 1, fileC);
          sprintf(buf, "text-box 0x%X 0x%X 0x%X\n", pt, arg1, arg2);
          strcat(trans, buf);
        } else if (code == 5) {
          strcat(trans, "text-newline\n");
        } else if (code == 6) {
          strcat(trans, "text-waitbutton\n");
        } else if (code == 7) {
          strcat(trans, "text-newlinewitharrow\n");
        } else if (code == 8) {
          strcat(trans, "text-switchtoasm\n");
          still_going = 0;
        } else if (code == 9) {
          pt = arg1 = 0;
          fread(&pt, 1, 2, fileC);
          fread(&arg1, 1, 1, fileC);
          sprintf(buf, "text-number 0x%X 0x%X 0x%X\n", pt, (arg1 & 0xF0) >> 8,
                  arg1 & 0xF);
          strcat(trans, buf);
        } else if (code == 10) {
          strcat(trans, "text-interpretdata\n");
        } else if (code == 11) {
          strcat(trans, "text-playsound0\n");
        } else if (code == 12) {
          pt = 0;
          fread(&pt, 1, 1, fileC);
          sprintf(buf, "text-interpretxdata 0x%X\n", pt);
          strcat(trans, buf);
        } else if (code == 13) {
          strcat(trans, "text-waitbutton2\n");
        } else if (code == 14) {
          strcat(trans, "text-playsound9\n");
        } else if (code == 15) {
          strcat(trans, "text-playsound1\n");
        } else if (code == 16) {
          strcat(trans, "text-playsound2\n");
        } else if (code == 17) {
          strcat(trans, "text-playsounda\n");
        } else if (code == 18) {
          strcat(trans, "text-playsoundd\n");
        } else if (code == 19) {
          strcat(trans, "text-playsoundc\n");
        } else if (code == 20) {
          pt = 0;
          fread(&pt, 1, 1, fileC);
          sprintf(buf, "text-buffer 0x%X\n", pt);
          strcat(trans, buf);
        } else if (code == 21) {
          strcat(trans, "text-day\n");
        } else if (code == 22) {
          pt = arg1 = 0;
          fread(&pt, 1, 2, fileC);
          fread(&arg1, 1, 1, fileC);
          sprintf(buf, "text-newtxt 0x%X ' 0x%X\n", arg1 | (pt << 8),
                  PointerToOffset(arg1 | (pt << 8)));
          strcat(trans, buf);
          if (PointerToOffset(arg1 | (pt << 8)) != 0xFFFFFFFF) {
            sprintf(buf, "\n#org 0x%X\n", PointerToOffset(arg1 | (pt << 8)));
            strcat(trans, buf);
            fseek(fileC, PointerToOffset(arg1 | (pt << 8)), SEEK_SET);
          }
        } else {
          sprintf(buf, "#raw 0x%X\n", code);
          strcat(trans, buf);
        }
      }
    }
    if (resume_out != NULL) {
      if (resume) {
        *resume_out = (uint32_t)ftell(fileC);
      } else {
        *resume_out = 0;
      }
    }
    fclose(fileC);
  } else {
    if (resume_out != NULL) {
      *resume_out = 0;
    }
    strcpy(trans, "\\x\n' Could not reopen ROM file to translate string");
  }
  return trans;
}
////////////////////////////////////////////////////////////////////////////////////////////
char *transbrl(int howfar, const char *file, FILE *fsend) {
  unsigned int pt = 0;
  unsigned char p;
  int read;
  char still_going;
  FILE *fileC;
  read = 0;
  fileC = fopen(file, "rb");
  if (fileC != NULL) {
    fseek(fileC, (howfar & 0xffffff), SEEK_SET);
    still_going = 1;
    fprintf(fsend, "        '");
    while (still_going) {
      fread(&p, 1, 1, fileC);
      pt = 0;
      if (p == 0xff) {
        still_going = 0;
        break;
      }
      fprintf(fsend, "  ");
      if (p & 1)
        fprintf(fsend, ".");
      else
        fprintf(fsend, " ");
      if (p & 2)
        fprintf(fsend, ".");
      else
        fprintf(fsend, " ");
    }
    fprintf(fsend, "\n        '");
    fseek(fileC, (howfar & 0xffffff), SEEK_SET);
    still_going = 1;
    while (still_going) {
      fread(&p, 1, 1, fileC);
      pt = 0;
      if (p == 0xff) {
        still_going = 0;
        break;
      }
      p = p >> 2;
      fprintf(fsend, "  ");
      if (p & 1)
        fprintf(fsend, ".");
      else
        fprintf(fsend, " ");
      if (p & 2)
        fprintf(fsend, ".");
      else
        fprintf(fsend, " ");
    }
    fprintf(fsend, "\n        '");
    fseek(fileC, (howfar & 0xffffff), SEEK_SET);
    still_going = 1;
    while (still_going) {
      fread(&p, 1, 1, fileC);
      pt = 0;
      if (p == 0xff) {
        still_going = 0;
        break;
      }
      p = p >> 4;
      fprintf(fsend, "  ");
      if (p & 1)
        fprintf(fsend, ".");
      else
        fprintf(fsend, " ");
      if (p & 2)
        fprintf(fsend, ".");
      else
        fprintf(fsend, " ");
    }
    fprintf(fsend, "\n");
    fclose(fileC);
  } else {
    fprintf(fsend, "          'Error in translating braille...");
  }
  return 0;
}

char *transmove(int howfar, const char *file) {
  char still_going, nextraw, failsafe = 0;
  FILE *fileC;
  unsigned char p;
  char buf[10];
  fileC = fopen(file, "rb");
  if (fileC != NULL) {
    fseek(fileC, (howfar & 0x7ffffff), SEEK_SET);
    still_going = 1;
    strcpy(trans, "");
    while (still_going) {
      fread(&p, 1, 1, fileC);
      if (p == 0xFF || p == 0)
        failsafe++;
      else
        failsafe = 0;
      if (failsafe >= 100 || strlen(trans) > 65520) {
        strcat(trans, "'FAILSAFE");
        break;
      }
      if (mode == GOLD || mode == CRYSTAL) {
        const char *cmd = lookup_gsc_move_reverse(p);
        if (cmd != NULL) {
          strcat(trans, cmd);
        } else {
          sprintf(buf, "raw_%02X", p);
          strcat(trans, buf);
        }

        nextraw = 0;
        if (p == 0x47)
          break;  // end command
        else if (p == 0x46)
          nextraw = 1;  // pause command
        else if (p == 0x55)
          nextraw = 1;  // earthquake command

        strcat(trans, " ");

        if (nextraw) {
          fread(&p, 1, 1, fileC);
          sprintf(buf, "raw_%X ", p);
          strcat(trans, buf);
        }
      } else if (mode == FIRE_RED || mode == RUBY) {
        const char *cmd;
        if (mode == FIRE_RED)
          cmd = lookup_frlg_move_reverse(p);
        else
          cmd = lookup_rse_move_reverse(p);

        if (cmd != NULL) {
          strcat(trans, cmd);
        } else {
          sprintf(buf, "raw_%02X", p);
          strcat(trans, buf);
        }

        if (p == 0xfe) break;  // end command

        strcat(trans, " ");
      } else {
        // Unknown version (e.g. DIAMOND for now)
        sprintf(buf, "raw_%02X", p);
        strcat(trans, buf);

        if (p == 0xfe) break;  // end command

        strcat(trans, " ");
      }
    }
    fclose(fileC);
  } else {
    strcpy(trans, "Error in translating movement data!");
  }
  return trans;
}

void err_bad_hex(const char *ptr, size_t len) {
  char *cpy = malloc(len + 1);
  char *log_str = malloc(len + 100);
  memcpy(cpy, ptr, len);
  cpy[len] = '\0';
  int log_str_len = sprintf(log_str, "Invalid hex string: \"%s\"\n", cpy);
  log_txt(log_str, log_str_len);
  free(log_str);
  free(cpy);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
char *transbackstr(char *scrfn, unsigned int pos, codeblock *c) {
  char *NewSpace, *ret;
  char cch, noend = 0;
  char str[65536];
  unsigned int i = 0, j = 0, k, l;
#ifndef DLL
  unsigned int read;
  FILE *scrfile;
  scrfile = fopen(scrfn, "rt");

  if (pos != 0xFFFFFFFF)
    fseek(scrfile, pos, SEEK_SET);
  else {
#ifdef WIN32
    MessageBox(NULL, "Darn file pointers. Always spoiling the fun.", "Error",
               0x10);
#endif
    return NULL;
  }
  if (scrfile == NULL) {
#ifdef WIN32
    MessageBox(NULL, "Darn file handles. Always being complicated on purpose.",
               "Error", 0x10);
#endif
    return NULL;
  }
#else
  unsigned int spos = pos;
#endif
  k = 0;
  while (1) {
#ifndef DLL
    read = (unsigned int)fread(&cch, 1, 1, scrfile);
    if (read == 0) break;
#else
    cch = scrfn[spos];
    spos++;
    if (cch == 0) break;
#endif
    if (cch == '\n') break;
    if (cch == '\r') break;
    str[k] = cch;
    k++;
  }
#ifndef DLL
  fclose(scrfile);
#endif
  str[k] = 0;
  ret = malloc(strlen(str) + 1);
  strcpy(ret, str);
  NewSpace = malloc(strlen(str) + 1);
  if (mode == GOLD || mode == CRYSTAL) {
    NewSpace[0] = 0;
    j = 1;
    l = strlen(str);
    while (i < l) {
      if ((str[i] >= 'A' && str[i] <= 'Z') ||
          (str[i] >= 'a' && str[i] <= 'z')) {
        NewSpace[j] = str[i] + 0x3F;
      } else if (str[i] >= '0' && str[i] <= '9') {
        NewSpace[j] = str[i] + 0xC6;
      } else if (str[i] == ' ') {
        NewSpace[j] = 0x7F;
      } else if (str[i] == '?') {
        NewSpace[j] = 0xE6;
      } else if (str[i] == '!') {
        NewSpace[j] = 0xE7;
      } else if (str[i] == '.') {
        NewSpace[j] = 0xE8;
      } else if (str[i] == ',') {
        NewSpace[j] = 0xF4;
      } else if (str[i] == '-') {
        NewSpace[j] = 0xE3;
      } else if (str[i] == '$') {
        NewSpace[j] = 0xF0;
      } else if (str[i] == '\'') {
        i++;
        if (str[i] == 'd') {
          NewSpace[j] = 0xD0;
        } else if (str[i] == 'l') {
          NewSpace[j] = 0xD1;
        } else if (str[i] == 'm') {
          NewSpace[j] = 0xD2;
        } else if (str[i] == 'r') {
          NewSpace[j] = 0xD3;
        } else if (str[i] == 's') {
          NewSpace[j] = 0xD4;
        } else if (str[i] == 't') {
          NewSpace[j] = 0xD5;
        } else if (str[i] == 'v') {
          NewSpace[j] = 0xD6;
        } else {
          i--;
          NewSpace[j] = 0xE0;
        }
      } else if (str[i] == '\\') {
        i++;
        if (str[i] == 'n') {
          NewSpace[j] = 0x4F;
        } else if (str[i] == 'l') {
          NewSpace[j] = 0x55;
        } else if (str[i] == 'p') {
          NewSpace[j] = 0x51;
        } else if (str[i] == 'e') {
          NewSpace[j] = 0x57;
          j++;
          NewSpace[j] = 0x0;
          j++;
          break;
        } else if (str[i] == 'x') {
          noend = 1;
          j--;
        } else if (str[i] == 'h') {
          // Raw hex escape
          if (hex_to_uint32(str + i + 1, 2, &k) != str + i + 3) {
            err_bad_hex(str + i + 1, 2);
            // return?
          }
          i += 2;
          k = k & 0xff;
          NewSpace[j] = k;
        } else if (str[i] == '\n') {
          while (str[i] == '\n') i++;
          i--;
        } else {
          i--;
        }
      } else if (str[i] == ':') {
        NewSpace[j] = 0x9C;
      } else if (str[i] == '[') {
        i++;
        if (str[i] == '.') {
          i++;
          if (str[i] == ']') {
            NewSpace[j] = 0x75;
          } else if (str[i] == '.') {
            i++;
            if (str[i] == ']') {
              NewSpace[j] = 0x56;
            } else {
              i -= 3;
            }
          } else {
            i -= 2;
          }
        } else if (str[i++] == 'P') {
          if (str[i] == 'L') {
            i++;
            if (str[i] == 'A') {
              i++;
              if (str[i] == 'Y') {
                i++;
                if (str[i] == 'E') {
                  i++;
                  if (str[i] == 'R') {
                    i++;
                    if (str[i] == ']') {
                      NewSpace[j] = 0x52;
                    } else {
                      i -= 7;
                    }
                  } else {
                    i -= 6;
                  }
                } else {
                  i -= 5;
                }
              } else {
                i -= 4;
              }
            } else {
              i -= 3;
            }
          } else if (str[i] == 'O') {
            i++;
            if (str[i] == 'K') {
              i++;
              if ((unsigned char)str[i] == (unsigned char)0xE9 ||
                  str[i] == 'e' || str[i] == 'E' ||
                  (unsigned char)str[i] == (unsigned char)0xC9) {
                i++;
                if (str[i] == ']') {
                  NewSpace[j] = 0x54;
                } else {
                  i -= 5;
                }
              } else {
                i -= 4;
              }
            } else {
              i -= 3;
            }
          } else {
            i -= 2;
          }
        } else {
          i--;
        }
      }
      i++;
      j++;
    }
    if (!noend && (j < 2 || NewSpace[j - 2] != 0x57)) {
      NewSpace[j] = 0x50;
      j++;
    }
  } else {
    noend = 0;
    while (i < strlen(str)) {
      if (str[i] >= 'A' && str[i] <= 'Z') {
        NewSpace[j] = str[i] + 0x7a;
      } else if (str[i] >= 'a' && str[i] <= 'z') {
        NewSpace[j] = str[i] + 0x74;
      } else if (str[i] >= '0' && str[i] <= '9') {
        NewSpace[j] = str[i] + 0x71;
      } else if (str[i] == ' ') {
        NewSpace[j] = 0;
      } else if ((unsigned char)str[i] == (unsigned char)0xE9) {
        NewSpace[j] = 0x1B;
      } else if (str[i] == '?') {
        NewSpace[j] = 0xAC;
      } else if (str[i] == '.') {
        NewSpace[j] = 0xAD;
      } else if (str[i] == ',') {
        NewSpace[j] = 0xB8;
      } else if (str[i] == '"') {
        NewSpace[j] = 0xB1;
      } else if (str[i] == '\'') {
        NewSpace[j] = 0xB4;
      } else if (str[i] == '-') {
        NewSpace[j] = 0xAE;
      } else if (str[i] == '\\') {
        i++;
        if (str[i] == 'n') {
          NewSpace[j] = 0xFE;
        } else if (str[i] == 'x') {
          noend = 1;
          j--;
        } else if (str[i] == 'w') {
          NewSpace[j] = 0xFC;
          j++;
          NewSpace[j] = 0x09;
        } else if (str[i] == 'l') {
          NewSpace[j] = 0xFA;
        } else if (str[i] == 'p') {
          NewSpace[j] = 0xFB;
        } else if (str[i] == 'v') {
          NewSpace[j] = 0xFD;
        } else if (str[i] == 'e') {
          NewSpace[j] = 0x1B;
        } else if (str[i] == 'c') {
          NewSpace[j] = 0xFC;
        } else if (str[i] == 'h') {
          // Hex escape
          if (hex_to_uint32(str + i + 1, 2, &k) != str + i + 3) {
            err_bad_hex(str + i + 1, 2);
          }
          i += 2;
          k = k & 0xff;
          NewSpace[j] = k;
        } else {
          i--;
        }
      } else if (str[i] == ':') {
        NewSpace[j] = 0xF0;
      } else if (str[i] == '!') {
        NewSpace[j] = 0xAB;
      } else if (str[i] == '[') {
        i++;
        if (str[i] == '.') {
          i++;
          if (str[i] == ']') {
            NewSpace[j] = 0xB0;
          } else {
            i--;
            i--;
          }
        } else if (str[i] == '"') {
          i++;
          if (str[i] == ']') {
            NewSpace[j] = 0xB2;
          } else {
            i--;
            i--;
          }
        } else if (str[i] == 'p') {
          i++;
          if (str[i] == ']') {
            NewSpace[j] = 0xB7;
          } else {
            i--;
            i--;
          }
        } else {
          i--;
        }
      }
      i++;
      j++;
    }
    if (!noend) {
      NewSpace[j] = 0xFF;
      j++;
    }
  }
  if (eorg) memset(NewSpace, search, j);

  add_data(c, NewSpace, j);
  free(NewSpace);
  return ret;
}

static const char *get_romtype_name() {
  switch (mode) {
    case GOLD:
      return "Gold/Silver";
    case CRYSTAL:
      return "Crystal";
    case RUBY:
      return "Ruby/Sapphire/Emerald";
    case FIRE_RED:
      return "FireRed/LeafGreen";
    default:
      return "unidentified";
  }
}

unsigned int transbackmove(char *script, unsigned int *ii) {
  unsigned int i, len = 0;
  char cmdbuf[100];
  char xbuf[100];
  i = *ii;
  while (script[i] != '\n' && script[i] != 0 && script[i] != '\'') {
    i = skip_whitespace(script, i);

    size_t cmdbuf_size = 0;
    while (script[i] != ' ' && script[i] != '\n' && script[i] != 0 &&
           script[i] != '\'') {
      cmdbuf[cmdbuf_size] = script[i];
      i++;
      cmdbuf_size++;
    }
    cmdbuf[cmdbuf_size] = 0;

    const struct move_data *move_data = NULL;
    if (eorg) {
      trans[len] = search;
    } else if (cmdbuf[0] == '\'') {
      break;  // Stop processing movement at a comment
    } else if ((mode == GOLD || mode == CRYSTAL) &&
               (move_data = lookup_gsc_move(cmdbuf, cmdbuf_size)) != NULL) {
      trans[len] = move_data->value;
    } else if (mode == FIRE_RED &&
               (move_data = lookup_frlg_move(cmdbuf, cmdbuf_size)) != NULL) {
      trans[len] = move_data->value;
    } else if (mode == RUBY &&
               (move_data = lookup_rse_move(cmdbuf, cmdbuf_size)) != NULL) {
      trans[len] = move_data->value;
    } else if ((cmdbuf_size == 6 || cmdbuf_size == 5) &&
               memcmp(cmdbuf, "raw_", 4) == 0) {
      // RAW handler.
      uint32_t j;
      const char *hex_end = hex_to_uint32(&cmdbuf[4], 2, &j);
      if (hex_end != &cmdbuf[cmdbuf_size]) {
        sprintf(xbuf, "Invalid raw command '%s'.\n", cmdbuf);
        log_txt(xbuf, strlen(xbuf));
        len--;
      } else {
        trans[len] = j;
      }
    } else {
      len--;
      sprintf(xbuf, "Unknown %s move command \"%s\"\n", get_romtype_name(),
              cmdbuf);
      log_txt(xbuf, strlen(xbuf));
    }
    len++;
  }
  *ii = i;
  return len;
}
