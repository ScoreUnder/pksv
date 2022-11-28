#include <string.h>
#include <stdio.h>

#include "romutil.h"
#include "isdone.h"

char asm_buf[200];
int dec_thumb(short opcode, int place) {
  char lbuf[30];
  int retval = 1;
  int arg1;
  switch ((opcode & 0xE000) >> 13) {
    case 0:
      switch ((opcode & 0x1800) >> 11) {
        case 0:
          sprintf(asm_buf, "-LSL   R%u,R%u,%X", opcode & 0x0007,
                  (opcode & 0x0038) >> 3, (opcode & 0x07C0) >> 6);
          break;
        case 1:
          sprintf(asm_buf, "-LSR   R%u,R%u,%X", opcode & 0x0007,
                  (opcode & 0x0038) >> 3, (opcode & 0x07C0) >> 6);
          break;
        case 2:
          sprintf(asm_buf, "-ASR   R%u,R%u,%X", opcode & 0x0007,
                  (opcode & 0x0038) >> 3, (opcode & 0x07C0) >> 6);
          break;
        case 3:
          switch ((opcode & 0x0600) >> 9) {
            case 0:
              sprintf(asm_buf, "-ADD   R%u,R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3, (opcode & 0x01C0) >> 6);
              break;
            case 1:
              sprintf(asm_buf, "-SUB   R%u,R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3, (opcode & 0x01C0) >> 6);
              break;
            case 2:
              sprintf(asm_buf, "-ADD   R%u,R%u,%x", opcode & 0x0007,
                      (opcode & 0x0038) >> 3, (opcode & 0x01C0) >> 6);
              break;
            case 3:
              sprintf(asm_buf, "-SUB   R%u,R%u,%x", opcode & 0x0007,
                      (opcode & 0x0038) >> 3, (opcode & 0x01C0) >> 6);
              break;
          }
          break;
      }
      break;
    case 1:
      switch ((opcode & 0x1800) >> 11) {
        case 0:
          sprintf(asm_buf, "-MOV   R%u,%X", (opcode & 0x0700) >> 8,
                  opcode & 0x00FF);
          break;
        case 1:
          sprintf(asm_buf, "-CMP   R%u,%X", (opcode & 0x0700) >> 8,
                  opcode & 0x00FF);
          break;
        case 2:
          sprintf(asm_buf, "-ADD   R%u,%X", (opcode & 0x0700) >> 8,
                  opcode & 0x00FF);
          break;
        case 3:
          sprintf(asm_buf, "-SUB   R%u,%X", (opcode & 0x0700) >> 8,
                  opcode & 0x00FF);
          break;
      }
      break;
    case 2:
      switch ((opcode & 0x1C00) >> 10) {
        case 0:
          // ALU
          switch ((opcode & 0x03C0) >> 6) {
            case 0:
              sprintf(asm_buf, "-AND   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 1:
              sprintf(asm_buf, "-EOR   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 2:
              sprintf(asm_buf, "-LSL   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 3:
              sprintf(asm_buf, "-LSR   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 4:
              sprintf(asm_buf, "-ASR   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 5:
              sprintf(asm_buf, "-ADC   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 6:
              sprintf(asm_buf, "-SBC   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 7:
              sprintf(asm_buf, "-ROR   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 8:
              sprintf(asm_buf, "-TST   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 9:
              sprintf(asm_buf, "-NEG   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 10:
              sprintf(asm_buf, "-CMP   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 11:
              // NEG-compare
              sprintf(asm_buf, "-CMN   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 12:
              sprintf(asm_buf, "-ORR   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 13:
              sprintf(asm_buf, "-MUL   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 14:
              // Bit-Clear
              sprintf(asm_buf, "-BIC   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
            case 15:
              // NOT
              sprintf(asm_buf, "-MVN   R%u,R%u", opcode & 0x0007,
                      (opcode & 0x0038) >> 3);
              break;
          }
          break;
        case 1:
          switch ((opcode & 0x0300) >> 8) {
            case 0:
              sprintf(asm_buf, "-ADD   R%u,R%u",
                      (opcode & 0x0007) | ((opcode & 0x0080) >> 4),
                      (opcode & 0x0028) >> 3);
              break;
            case 1:
              sprintf(asm_buf, "-CMP   R%u,R%u",
                      (opcode & 0x0007) | ((opcode & 0x0080) >> 4),
                      (opcode & 0x0028) >> 3);
              break;
            case 2:
              if ((opcode & 0xFF) == 0xC0)
                strcpy(asm_buf, "-NOP");
              else
                sprintf(asm_buf, "-MOV   R%u,R%u",
                        (opcode & 0x0007) | ((opcode & 0x0080) >> 4),
                        (opcode & 0x0078) >> 3);
              break;
            case 3:
              if (opcode & 0x0080)
                // CALL
                sprintf(asm_buf, "-BLX   R%u", (opcode & 0x0078) >> 3);
              else {
                // JUMP
                sprintf(asm_buf, "-BX    R%u", (opcode & 0x0078) >> 3);
                retval = 0;
              }
              break;
          }
          break;
        case 7:
        case 6:
        case 5:
        case 4:
          if (opcode & 0x0200) {
            switch ((opcode >> 10) & 3)  // 0C00
            {
              case 0:
                sprintf(asm_buf, "-STRH  R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
              case 1:
                sprintf(asm_buf, "-LDSB  R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
              case 2:
                sprintf(asm_buf, "-LDRH  R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
              case 3:
                sprintf(asm_buf, "-LDSH  R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
            }
          } else {
            switch ((opcode >> 10) & 3) {
              case 0:
                sprintf(asm_buf, "-STR   R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
              case 1:
                sprintf(asm_buf, "-STRB  R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
              case 2:
                sprintf(asm_buf, "-LDR   R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
              case 3:
                sprintf(asm_buf, "-LDRB  R%u,[R%u,R%u]", opcode & 7,
                        (opcode >> 3) & 7, (opcode >> 6) & 7);
                break;
            }
          }
          break;
        case 2:
        case 3:
          sprintf(asm_buf, "-LDR   R%u,[PC,%X] '%X", (opcode >> 8) & 7,
                  (4 * (opcode & 0xFF)),
                  (4 * (opcode & 0xFF)) + ((place + 4) & (~2)));
          DoDword(((4 * (opcode & 0xFF)) + ((place + 4) & (~2))) & ~ROM_BASE_ADDRESS);
          break;
      }
      break;
    case 3:
      switch ((opcode >> 11) & 3) {
        case 0:
          sprintf(asm_buf, "-STR   R%u,[R%u,%X]", opcode & 7, (opcode >> 3) & 7,
                  (opcode >> 6) & 0x1F);
          break;
        case 1:
          sprintf(asm_buf, "-LDR   R%u,[R%u,%X]", opcode & 7, (opcode >> 3) & 7,
                  (opcode >> 6) & 0x1F);
          break;
        case 2:
          sprintf(asm_buf, "-STRB  R%u,[R%u,%X]", opcode & 7, (opcode >> 3) & 7,
                  (opcode >> 6) & 0x1F);
          break;
        case 3:
          sprintf(asm_buf, "-LDRB  R%u,[R%u,%X]", opcode & 7, (opcode >> 3) & 7,
                  (opcode >> 6) & 0x1F);
          break;
      }
      break;
    case 4:
      if (opcode & 0x1000) {
        if (opcode & 0x0800)
          sprintf(asm_buf, "-LDR   R%u,[SP,%X]", (opcode >> 8) & 7,
                  4 * (opcode & 0xFF));
        else
          sprintf(asm_buf, "-STR   R%u,[SP,%X]", (opcode >> 8) & 7,
                  4 * (opcode & 0xFF));
      } else {
        if (opcode & 0x0800)
          sprintf(asm_buf, "-LDRH  R%u,[R%u,%X]", opcode & 7, (opcode >> 3) & 7,
                  2 * ((opcode >> 6) & 0x1F));
        else
          sprintf(asm_buf, "-STRH  R%u,[R%u,%X]", opcode & 7, (opcode >> 3) & 7,
                  2 * ((opcode >> 6) & 0x1F));
      }
      break;
    case 5:
      if (opcode & 0x1000) {
        if ((opcode & 0x0F00) == 0) {
          if (opcode & 0x0080)
            sprintf(asm_buf, "-ADD   SP,-%X", 4 * (opcode & 0x7F));
          else
            sprintf(asm_buf, "-ADD   SP,%X", 4 * (opcode & 0x7F));
        } else {
          if (((opcode >> 9) & 3) == 2) {
            if (opcode & 0x0800)
              strcpy(asm_buf, "-POP   {");
            else
              strcpy(asm_buf, "-PUSH  {");
            if (opcode & 0x80) {
              strcat(asm_buf, "R7");
              if (opcode & 0x17F) strcat(asm_buf, ",");
            }
            if (opcode & 0x40) {
              strcat(asm_buf, "R6");
              if (opcode & 0x13F) strcat(asm_buf, ",");
            }
            if (opcode & 0x20) {
              strcat(asm_buf, "R5");
              if (opcode & 0x11F) strcat(asm_buf, ",");
            }
            if (opcode & 0x10) {
              strcat(asm_buf, "R4");
              if (opcode & 0x10F) strcat(asm_buf, ",");
            }
            if (opcode & 0x8) {
              strcat(asm_buf, "R3");
              if (opcode & 0x107) strcat(asm_buf, ",");
            }
            if (opcode & 0x4) {
              strcat(asm_buf, "R2");
              if (opcode & 0x103) strcat(asm_buf, ",");
            }
            if (opcode & 0x2) {
              strcat(asm_buf, "R1");
              if (opcode & 0x101) strcat(asm_buf, ",");
            }
            if (opcode & 0x1) {
              strcat(asm_buf, "R0");
              if (opcode & 0x0100) strcat(asm_buf, ",");
            }
            if (opcode & 0x0100) {
              if (opcode & 0x0800) {
                strcat(asm_buf, "PC}");
                retval = 0;
              } else
                strcat(asm_buf, "LR}");
            } else
              strcat(asm_buf, "}");
          } else if (((opcode >> 8) & 7) == 6)
            sprintf(asm_buf, "-BKPT  %X", opcode & 0xFF);
          else
            sprintf(asm_buf, "#WORD  0x%X", opcode);
        }
      } else {
        if (opcode & 0x0800)
          sprintf(asm_buf, "-ADD   R%u,SP,%X", (opcode >> 8) & 7,
                  4 * (opcode & 0xFF));
        else
          sprintf(asm_buf, "-ADD   R%u,PC,%X", (opcode >> 8) & 7,
                  4 * (opcode & 0xFF));
      }
      break;
    case 6:
      if (opcode & 0x1000) {
        if (opcode & 0x80) {
          arg1 = 4 - (2 * (0x100 - (opcode & 0xFF)));
          if ((signed)arg1 < 0) {
            arg1 &= 0xFF;
            sprintf(lbuf, "-%X", arg1);
          } else {
            sprintf(lbuf, "+%X", arg1);
          }
          if (((opcode >> 8) & 0xF) < 14)
            DoThumb(place + 4 - (2 * (0x100 - (opcode & 0xFF))));
        } else {
          sprintf(lbuf, "+%X", 4 + (2 * (opcode & 0xFF)));
          if (((opcode >> 8) & 0xF) < 14)
            DoThumb(place + 4 + (2 * (opcode & 0xFF)));
        }
        switch ((opcode >> 8) & 0xF) {
          case 0:
            sprintf(asm_buf, "-BEQ   %s", lbuf);
            break;
          case 1:
            sprintf(asm_buf, "-BNE   %s", lbuf);
            break;
          case 2:
            sprintf(asm_buf, "-BCS   %s", lbuf);
            break;
          case 3:
            sprintf(asm_buf, "-BCC   %s", lbuf);
            break;
          case 4:
            sprintf(asm_buf, "-BMI   %s", lbuf);
            break;
          case 5:
            sprintf(asm_buf, "-BPL   %s", lbuf);
            break;
          case 6:
            sprintf(asm_buf, "-BVS   %s", lbuf);
            break;
          case 7:
            sprintf(asm_buf, "-BVC   %s", lbuf);
            break;
          case 8:
            sprintf(asm_buf, "-BHI   %s", lbuf);
            break;
          case 9:
            sprintf(asm_buf, "-BLS   %s", lbuf);
            break;
          case 10:
            sprintf(asm_buf, "-BGE   %s", lbuf);
            break;
          case 11:
            sprintf(asm_buf, "-BLT   %s", lbuf);
            break;
          case 12:
            sprintf(asm_buf, "-BGT   %s", lbuf);
            break;
          case 13:
            sprintf(asm_buf, "-BLE   %s", lbuf);
            break;
          case 14:
            sprintf(asm_buf, "#WORD  0x%X", opcode);
            break;
          case 15:
            sprintf(asm_buf, "-SWI   %X", opcode & 0xFF);
            break;
        }
      } else {
        if (opcode & 0x0800)
          sprintf(asm_buf, "-LDMIA R%u,{", (opcode >> 8) & 7);
        else
          sprintf(asm_buf, "-STMIA R%u,{", (opcode >> 8) & 7);
        if (opcode & 0x80) {
          strcat(asm_buf, "R7");
          if (opcode & 0x7F) strcat(asm_buf, ",");
        }
        if (opcode & 0x40) {
          strcat(asm_buf, "R6");
          if (opcode & 0x3F) strcat(asm_buf, ",");
        }
        if (opcode & 0x20) {
          strcat(asm_buf, "R5");
          if (opcode & 0x1F) strcat(asm_buf, ",");
        }
        if (opcode & 0x10) {
          strcat(asm_buf, "R4");
          if (opcode & 0xF) strcat(asm_buf, ",");
        }
        if (opcode & 0x8) {
          strcat(asm_buf, "R3");
          if (opcode & 0x7) strcat(asm_buf, ",");
        }
        if (opcode & 0x4) {
          strcat(asm_buf, "R2");
          if (opcode & 0x3) strcat(asm_buf, ",");
        }
        if (opcode & 0x2) {
          strcat(asm_buf, "R1");
          if (opcode & 0x1) strcat(asm_buf, ",");
        }
        if (opcode & 0x1) strcat(asm_buf, "R0");
        strcat(asm_buf, "}");
      }
      break;
    case 7:
      switch ((opcode >> 11) & 3) {
        case 0:
          if (opcode & 0x0400) {
            arg1 = 4 - (2 * (2048 - (opcode & 0x07FF)));
            if ((signed)arg1 >= 0)
              sprintf(asm_buf, "-B     +%X '%X", arg1,
                      place + 4 - (2 * (2048 - (opcode & 0x07FF))));
            else
              sprintf(asm_buf, "-B     -%X '%X", -arg1,
                      place + 4 - (2 * (2048 - (opcode & 0x07FF))));
            DoThumb(place + 4 - (2 * (2048 - (opcode & 0x07FF))));
          } else {
            sprintf(asm_buf, "-B     +%X '%X", 4 + (2 * (opcode & 0x7FF)),
                    place + 4 + (2 * (opcode & 0x7FF)));
            DoThumb(place + 4 + (2 * (opcode & 0x7FF)));
          }
          retval = 0;
          break;
        case 1:
          sprintf(asm_buf, "-BLXH  %X", opcode & 0x7FF);
          break;
        case 2:
          sprintf(asm_buf, "-BL_PART1  %X", (opcode & 0x7FF) << 12);
          break;
        case 3:
          sprintf(asm_buf, "-BLH   %X", opcode & 0x7FF);
          break;
      }
      break;
  }
  return retval;
}
