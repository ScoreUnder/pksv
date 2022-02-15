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
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "textproc.h"
#include "codeproc.h"
#include "pksv.h"

#include "sublang/moves.h"

char trans[65536];

void log_txt(char*str, size_t length)
{
 	fwrite(str, 1, length, LogFile ? LogFile : stderr);
}

char*transtxt(int howfar,char*file)
{
	unsigned int pt=0,sl,arg1,arg2;
	unsigned char p,code;
	const char hex[17]="0123456789ABCDEF";
	char buf[128];
	unsigned int read;
	char still_going,readagain,sub_going;
	FILE* fileC;
	read=0;
	fileC=fopen(file,"rb");
	*trans=0;
	if (fileC!=NULL)
	{
		fseek(fileC,(howfar&0x7ffffff),SEEK_SET);
		still_going=1;
		if (mode!=GOLD&&mode!=CRYSTAL) {
			while (still_going)
			{
				read=(int)fread(&p,1,1,fileC);
				if (read==0)
				{
					break;
				}
				else
				{
					if (p>=0xbb&&p<=0xd4) {
						p-=0x7a;    //capital letter
					}
					else if (p>=0xd5&&p<=0xee) {
						p-=0x74;    //lower letter
					}
					else if (p>=0xA1&&p<=0xAA) {
						p-=0x71;
					}
					else if (p==0) {
						p=0x20;    //space
					}
					else if (p==0x1b) {
						trans[pt]='\\';    //e acute
						pt++;
						p='e';
					}
					else if (p==(unsigned char)0xFC) {
						read=(int)fread(&p,1,1,fileC);
						if(p==9)
						{
							trans[pt]='\\';
							pt++;
							p='w';
						}
						else
						{
							trans[pt]='\\';    //control code
							pt++;
							trans[pt]='c';
							pt++;
							trans[pt]='\\';
							pt++;
							trans[pt]='h';
							pt++;
							trans[pt]=hex[(p&0xf0)>>4];
							pt++;
							p=hex[p&0xf];
						}
					}
					else if (p==(unsigned char)0xac) {
						p='?';    //(unsigned char) keeps Microsoft VS happy
					}
					else if (p==0xad) {
						p='.';
					}
					else if (p==0xb8) {
						p=',';
					}
					else if (p==0xb4) {
						p='\'';
					}
					else if (p==0xae) {
						p='-';
					}
					else if (p==0xb1) {
						p='"';
					}
					else if (p==0xb2) {
						trans[pt]='[';
						pt++;
						trans[pt]='"';
						pt++;
						p=']';
					}
					else if (p==0xfe) // \n
					{
						trans[pt]='\\';
						pt++;
						p='n';
					}
					else if (p==0xb7) // [p]
					{
						trans[pt]='[';
						pt++;
						trans[pt]='p';
						pt++;
						p=']';
					}
					else if (p==0xfa) // \l
					{
						trans[pt]='\\';
						pt++;
						p='l';
					}
					else if (p==0xfb) // \p
					{
						trans[pt]='\\';
						pt++;
						p='p';
					}
					else if (p==0xff)
					{
						p=0; //EOS
						still_going=0;
					}
					else if (p==0xf0) {
						p=':';
					}
					else if (p==0xfd)
					{
						trans[pt]='\\';
						pt++;
						trans[pt]='v';
						pt++;
						trans[pt]='\\';
						pt++;
						trans[pt]='h';
						pt++;
						fread(&p,1,1,fileC);
						trans[pt]=hex[(p&0xf0)>>4];
						pt++;
						p=hex[p&0xf];
					}
					else if (p==0xab) {
						p='!';
					}
					else if (p==0xb0)
					{
						trans[pt]='[';
						pt++;
						trans[pt]='.';
						pt++;
						p=']';
					}
					else
					{
						trans[pt]='\\';
						pt++;
						trans[pt]='h';
						pt++;
						trans[pt]=hex[(p&0xf0)>>4];
						pt++;
						p=hex[p&0xf];
					}
					if (pt>=65525) //cut smaller for safety, 10 off because some commands are multichar.
					{
						trans[pt]=0;
						break;
					}
					trans[pt]=p;
					pt++;
				}
			}
		}
		else
		{
			strcpy(trans,"");
			while (still_going)
			{
				read=(unsigned int)fread(&code,1,1,fileC);
				if (read==0) {
					break;
				}
				sub_going=1;
				readagain=1;
				if (code==0)
				{
					strcat(trans,"= ");
					while (sub_going)
					{
						sl=(unsigned int)strlen(trans);
						if (readagain)
							read=(unsigned int)fread(&p,1,1,fileC);
						readagain=1;
						if (read==0) {
							still_going=sub_going=0;
							break;
						}
						if ((p>=0x80&&p<=0x99)||(p>=0xA0&&p<=0xB9)) {
							trans[sl]=p-0x3F;    //letter
							trans[sl+1]=0;
						}
						else if (p==0x7F) {
							trans[sl]=' ';
							trans[sl+1]=0;
						}
						else if (p==0x75) {
							strcat(trans,"[.]");
						}
						else if (p==0x56) {
							strcat(trans,"[..]");
						}
						else if (p==0x9C) {
							trans[sl]=':';
							trans[sl+1]=0;
						}
						else if (p==0xE6) {
							trans[sl]='?';
							trans[sl+1]=0;
						}
						else if (p==0xE7) {
							trans[sl]='!';
							trans[sl+1]=0;
						}
						else if (p==0xE8) {
							trans[sl]='.';
							trans[sl+1]=0;
						}
						else if (p==0xF4) {
							trans[sl]=',';
							trans[sl+1]=0;
						}
						else if (p==0xE3) {
							trans[sl]='-';
							trans[sl+1]=0;
						}
						else if (p==0xE0) {
							trans[sl]='\'';
							trans[sl+1]=0;
						}
						else if (p==0xF0) {
							trans[sl]='$';
							trans[sl+1]=0;
						}
						else if (p==0xF6) {
							trans[sl]='0';
							trans[sl+1]=0;
						}
						else if (p==0xF7) {
							trans[sl]='1';
							trans[sl+1]=0;
						}
						else if (p==0xF8) {
							trans[sl]='2';
							trans[sl+1]=0;
						}
						else if (p==0xF9) {
							trans[sl]='3';
							trans[sl+1]=0;
						}
						else if (p==0xFA) {
							trans[sl]='4';
							trans[sl+1]=0;
						}
						else if (p==0xFB) {
							trans[sl]='5';
							trans[sl+1]=0;
						}
						else if (p==0xFC) {
							trans[sl]='6';
							trans[sl+1]=0;
						}
						else if (p==0xFD) {
							trans[sl]='7';
							trans[sl+1]=0;
						}
						else if (p==0xFE) {
							trans[sl]='8';
							trans[sl+1]=0;
						}
						else if (p==0xFF) {
							trans[sl]='9';
							trans[sl+1]=0;
						}
						else if (p==0xD0) {
							strcat(trans,"'d");
						}
						else if (p==0xD1) {
							strcat(trans,"'l");
						}
						else if (p==0xD2) {
							strcat(trans,"'m");
						}
						else if (p==0xD3) {
							strcat(trans,"'r");
						}
						else if (p==0xD4) {
							strcat(trans,"'s");
						}
						else if (p==0xD5) {
							strcat(trans,"'t");
						}
						else if (p==0xD6) {
							strcat(trans,"'v");
						}
						else if (p==0x50) {
							sub_going=0;
						}
						else if (p==0x51) {
							strcat(trans,"\\p");
						}
						else if (p==0x52) {
							strcat(trans,"[PLAYER]");
						}
						else if (p==0x54) {
							strcat(trans,"[POK�]");
						}
						else if (p==0x55) {
							strcat(trans,"\\l");
						}
						else if (p==0x4F) {
							strcat(trans,"\\n");
						}
						else if (p==0x57)
						{
							readagain=0;
							fread(&p,1,1,fileC);
							if (p==0) {
								strcat(trans,"\\e\r\n");
								still_going=0;
								break;
							}
							strcat(trans,"\\h57");
						}
						else
						{
							sprintf(buf,"\\h%02X",p);
							strcat(trans,buf);
						}
					}
					strcat(trans,"\r\n");
				}
				else if (code==1)
				{
					pt=0;
					fread(&pt,1,2,fileC);
					sprintf(buf,"text-ram 0x%X\r\n",pt);
					strcat(trans,buf);
				}
				else if (code==2)
				{
					pt=0;
					fread(&pt,1,2,fileC);
					fread(&p,1,1,fileC);
					if (p>>5==7)
					{
						sprintf(buf,"0x%X 0x%X money-no-zeros",pt,p&0x1F);
					}
					else if (p>>5==6)
					{
						sprintf(buf,"0x%X 0x%X no-zeros",pt,p&0x1F);
					}
					else if (p>>5==5)
					{
						sprintf(buf,"0x%X 0x%X money-spaces",pt,p&0x1F);
					}
					else if (p>>5==4)
					{
						sprintf(buf,"0x%X 0x%X spaces",pt,p&0x1F);
					}
					else if (p>>5==3)
					{
						sprintf(buf,"0x%X 0x%X money2",pt,p&0x1F);
					}
					else if (p>>5==2)
					{
						sprintf(buf,"0x%X 0x%X normal2",pt,p&0x1F);
					}
					else if (p>>5==1)
					{
						sprintf(buf,"0x%X 0x%X money",pt,p&0x1F);
					}
					else
					{
						sprintf(buf,"0x%X 0x%X normal",pt,p&0x1F);
					}
					strcat(trans,"text-hex ");
					strcat(trans,buf);
					strcat(trans,"\r\n");
				}
				else if (code==3)
				{
					pt=0;
					fread(&pt,1,2,fileC);
					sprintf(buf,"text-reloc 0x%X\r\n",pt);
					strcat(trans,buf);
				}
				else if (code==4)
				{
					pt=arg1=arg2=0;
					fread(&pt,1,2,fileC);
					fread(&arg1,1,1,fileC);
					fread(&arg2,1,1,fileC);
					sprintf(buf,"text-box 0x%X 0x%X 0x%X\r\n",pt,arg1,arg2);
					strcat(trans,buf);
				}
				else if (code==5)
				{
					strcat(trans,"text-newline\r\n");
				}
				else if (code==6)
				{
					strcat(trans,"text-waitbutton\r\n");
				}
				else if (code==7)
				{
					strcat(trans,"text-newlinewitharrow\r\n");
				}
				else if (code==8)
				{
					strcat(trans,"text-switchtoasm\r\n");
					still_going=0;
				}
				else if (code==9)
				{
					pt=arg1=0;
					fread(&pt,1,2,fileC);
					fread(&arg1,1,1,fileC);
					sprintf(buf,"text-number 0x%X 0x%X 0x%X\r\n",pt,(arg1&0xF0)>>8,arg1&0xF);
					strcat(trans,buf);
				}
				else if (code==10)
				{
					strcat(trans,"text-interpretdata\r\n");
				}
				else if (code==11)
				{
					strcat(trans,"text-playsound0\r\n");
				}
				else if (code==12)
				{
					pt=0;
					fread(&pt,1,1,fileC);
					sprintf(buf,"text-interpretxdata 0x%X\r\n",pt);
					strcat(trans,buf);
				}
				else if (code==13)
				{
					strcat(trans,"text-waitbutton2\r\n");
				}
				else if (code==14)
				{
					strcat(trans,"text-playsound9\r\n");
				}
				else if (code==15)
				{
					strcat(trans,"text-playsound1\r\n");
				}
				else if (code==16)
				{
					strcat(trans,"text-playsound2\r\n");
				}
				else if (code==17)
				{
					strcat(trans,"text-playsounda\r\n");
				}
				else if (code==18)
				{
					strcat(trans,"text-playsoundd\r\n");
				}
				else if (code==19)
				{
					strcat(trans,"text-playsoundc\r\n");
				}
				else if (code==20)
				{
					pt=0;
					fread(&pt,1,1,fileC);
					sprintf(buf,"text-buffer 0x%X\r\n",pt);
					strcat(trans,buf);
				}
				else if (code==21)
				{
					strcat(trans,"text-day\r\n");
				}
				else if (code==22)
				{
					pt=arg1=0;
					fread(&pt,1,2,fileC);
					fread(&arg1,1,1,fileC);
					sprintf(buf,"text-newtxt 0x%X ' 0x%X\r\n",arg1|(pt<<8),PointerToOffset(arg1|(pt<<8)));
					strcat(trans,buf);
					if (PointerToOffset(arg1|(pt<<8))!=0xFFFFFFFF)
					{
						sprintf(buf,"\r\n#org 0x%X\r\n",PointerToOffset(arg1|(pt<<8)));
						strcat(trans,buf);
						fseek(fileC,PointerToOffset(arg1|(pt<<8)),SEEK_SET);
					}
				}
				else
				{
					sprintf(buf,"#raw 0x%X\r\n",code);
					strcat(trans,buf);
				}
			}
		}
		fclose(fileC);
	}
	else
	{
		strcpy(trans,"Error in translating string...");
	}
	return trans;
}
////////////////////////////////////////////////////////////////////////////////////////////
char*transbrl(int howfar,char*file,FILE*fsend)
{
	unsigned int pt=0;
	unsigned char p;
	int read;
	char still_going;
	FILE*fileC;
	read=0;
	fileC=fopen(file,"rb");
	if (fileC!=NULL)
	{
		fseek(fileC,(howfar&0xffffff),SEEK_SET);
		still_going=1;
		fprintf(fsend,"        '");
		while (still_going)
		{
			fread(&p,1,1,fileC);
			pt=0;
			if (p==0xff) {
				still_going=0;
				break;
			}
			fprintf(fsend,"  ");
			if (p&1) fprintf(fsend,".");
			else fprintf(fsend," ");
			if (p&2) fprintf(fsend,".");
			else fprintf(fsend," ");
		}
		fprintf(fsend,"\r\n        '");
		fseek(fileC,(howfar&0xffffff),SEEK_SET);
		still_going=1;
		while (still_going)
		{
			fread(&p,1,1,fileC);
			pt=0;
			if (p==0xff) {
				still_going=0;
				break;
			}
			p=p>>2;
			fprintf(fsend,"  ");
			if (p&1) fprintf(fsend,".");
			else fprintf(fsend," ");
			if (p&2) fprintf(fsend,".");
			else fprintf(fsend," ");
		}
		fprintf(fsend,"\r\n        '");
		fseek(fileC,(howfar&0xffffff),SEEK_SET);
		still_going=1;
		while (still_going)
		{
			fread(&p,1,1,fileC);
			pt=0;
			if (p==0xff) {
				still_going=0;
				break;
			}
			p=p>>4;
			fprintf(fsend,"  ");
			if (p&1) fprintf(fsend,".");
			else fprintf(fsend," ");
			if (p&2) fprintf(fsend,".");
			else fprintf(fsend," ");
		}
		fprintf(fsend,"\r\n");
		fclose(fileC);
	}
	else
	{
		fprintf(fsend,"          'Error in translating braille...");
	}
	return 0;
}

char*transmove(int howfar,char*file)
{
	unsigned int read;
	char still_going,nextraw,failsafe=0;
	FILE*fileC;
	unsigned char p;
	char buf[10];
	read=0;
	fileC=fopen(file,"rb");
	if (fileC!=NULL)
	{
		fseek(fileC,(howfar&0x7ffffff),SEEK_SET);
		still_going=1;
		strcpy(trans,"");
		while (still_going)
		{
			fread(&p,1,1,fileC);
			if(p==0xFF||p==0)failsafe++;
			else failsafe=0;
			if(failsafe>=100||strlen(trans)>65520)
			{
				strcat(trans,"'FAILSAFE");
				break;
			}
			if (mode==GOLD||mode==CRYSTAL)
			{
				char *cmd = lookup_gsc_move_reverse(p);
				if (cmd != NULL) {
					strcat(trans, cmd);
				} else {
					sprintf(buf,"raw_%02X",p);
					strcat(trans,buf);
				}

				nextraw=0;
				if (p == 0x47) break; // end command
				else if (p == 0x46) nextraw = 1; // pause command
				else if (p == 0x55) nextraw = 1; // earthquake command

				strcat(trans," ");

				if (nextraw)
				{
					fread(&p,1,1,fileC);
					sprintf(buf,"raw_%X ",p);
					strcat(trans,buf);
				}
			}
			else if (mode==FIRE_RED)
			{
				switch (p)
				{
				case 254:
					strcat(trans,"end");
					still_going=0;
					break;
				case 0x1:
					strcat(trans,"look_up");
					break;
				case 0x2:
					strcat(trans,"look_left");
					break;
				case 0x3:
					strcat(trans,"look_right");
					break;
				case 0x4:
					strcat(trans,"look_down");
					break;
				case 0x8:
					strcat(trans,"walk_down_vslow");
					break;
				case 0x9:
					strcat(trans,"walk_up_vslow");
					break;
				case 0xA:
					strcat(trans,"walk_left_vslow");
					break;
				case 0xB:
					strcat(trans,"walk_right_vslow");
					break;
				case 0xC:
					strcat(trans,"walk_down_slow");
					break;
				case 0xD:
					strcat(trans,"walk_up_slow");
					break;
				case 0xE:
					strcat(trans,"walk_left_slow");
					break;
				case 0xF:
					strcat(trans,"walk_right_slow");
					break;
				case 0x10:
					strcat(trans,"walk_down");
					break;
				case 0x11:
					strcat(trans,"walk_up");
					break;
				case 0x12:
					strcat(trans,"walk_left");
					break;
				case 0x13:
					strcat(trans,"walk_right");
					break;
				case 0x14:
					strcat(trans,"jump_down2");
					break;
				case 0x15:
					strcat(trans,"jump_up2");
					break;
				case 0x16:
					strcat(trans,"jump_left2");
					break;
				case 0x17:
					strcat(trans,"jump_right2");
					break;
				case 0x18:strcat(trans,"pause_xshort");break;
				case 0x19:strcat(trans,"pause_vshort");break;
				case 0x1A:strcat(trans,"pause_short");break;
				case 0x1B:strcat(trans,"pause");break;
				case 0x1C:strcat(trans,"pause_long");break;
				case 0x1D:
					strcat(trans,"run_down");
					break;
				case 0x1E:
					strcat(trans,"run_up");
					break;
				case 0x1F:
					strcat(trans,"run_left");
					break;
				case 0x20:
					strcat(trans,"run_right");
					break;
				case 0x21:
					strcat(trans,"onspot_down_slow");
					break;
				case 0x22:
					strcat(trans,"onspot_up_slow");
					break;
				case 0x23:
					strcat(trans,"onspot_left_slow");
					break;
				case 0x24:
					strcat(trans,"onspot_right_slow");
					break;
				case 0x25:
					strcat(trans,"onspot_down");
					break;
				case 0x26:
					strcat(trans,"onspot_up");
					break;
				case 0x27:
					strcat(trans,"onspot_left");
					break;
				case 0x28:
					strcat(trans,"onspot_right");
					break;
				case 0x29:
					strcat(trans,"onspot_down_fast");
					break;
				case 0x2A:
					strcat(trans,"onspot_up_fast");
					break;
				case 0x2B:
					strcat(trans,"onspot_left_fast");
					break;
				case 0x2C:
					strcat(trans,"onspot_right_fast");
					break;
				case 0x2D:
					strcat(trans,"look_down_delayed");
					break;
				case 0x2E:
					strcat(trans,"look_up_delayed");
					break;
				case 0x2F:
					strcat(trans,"look_left_delayed");
					break;
				case 0x30:
					strcat(trans,"look_right_delayed");
					break;
				case 0x31:
					strcat(trans,"slide_down");
					break;
				case 0x32:
					strcat(trans,"slide_up");
					break;
				case 0x33:
					strcat(trans,"slide_left");
					break;
				case 0x34:
					strcat(trans,"slide_right");
					break;
				case 0x35:strcat(trans,"run_down_vfast");break;
				case 0x36:strcat(trans,"run_up_vfast");break;
				case 0x37:strcat(trans,"run_left_vfast");break;
				case 0x38:strcat(trans,"run_right_vfast");break;
				case 0x39:strcat(trans,"slide_down_vfast");break;
				case 0x3A:strcat(trans,"slide_up_vfast");break;
				case 0x3B:strcat(trans,"slide_left_vfast");break;
				case 0x3C:strcat(trans,"slide_right_vfast");break;
				case 0x3D:
					strcat(trans,"slide_down2");
					break;
				case 0x3E:
					strcat(trans,"slide_up2");
					break;
				case 0x3F:
					strcat(trans,"slide_left2");
					break;
				case 0x40:
					strcat(trans,"slide_right2");
					break;
				case 0x41:
					strcat(trans,"slide_down3");
					break;
				case 0x42:
					strcat(trans,"slide_up3");
					break;
				case 0x43:
					strcat(trans,"slide_left3");
					break;
				case 0x44:
					strcat(trans,"slide_right3");
					break;
				case 0x45:strcat(trans,"walk_onspot");break;
				case 0x46:
					strcat(trans,"look_left_jump_down");
					break;
				case 0x47:
					strcat(trans,"look_down_jump_up");
					break;
				case 0x48:
					strcat(trans,"look_up_jump_left");
					break;
				case 0x49:
					strcat(trans,"look_left_jump_right");
					break;
				case 0x4A:
					strcat(trans,"faceplayer");
					break;
				case 0x4B:
					strcat(trans,"face_away");
					break;
				case 0x4E:
					strcat(trans,"jump_down1");
					break;
				case 0x4F:
					strcat(trans,"jump_up1");
					break;
				case 0x50:
					strcat(trans,"jump_left1");
					break;
				case 0x51:
					strcat(trans,"jump_right1");
					break;
				case 0x52:
					strcat(trans,"jump_down");
					break;
				case 0x53:
					strcat(trans,"jump_up");
					break;
				case 0x54:
					strcat(trans,"jump_left");
					break;
				case 0x55:
					strcat(trans,"jump_right");
					break;
				case 0x56:
					strcat(trans,"jump_downup");
					break;
				case 0x57:
					strcat(trans,"jump_updown");
					break;
				case 0x58:
					strcat(trans,"jump_leftright");
					break;
				case 0x59:
					strcat(trans,"jump_rightleft");
					break;
				case 0x5A:
					strcat(trans,"face_default");
					break;
				case 0x60:
					strcat(trans,"hide");
					break;
				case 0x62:
					strcat(trans,"say_!");
					break;
				case 0x63:
					strcat(trans,"say_?");
					break;
				case 0x64:
					strcat(trans,"say_x");
					break;
				case 0x65:
					strcat(trans,"say_!!");
					break;
				case 0x66:
					strcat(trans,"say_:)");
					break;
				case 0x68:
					strcat(trans,"rock_animate");
					break;
				case 0x69:
					strcat(trans,"tree_animate");
					break;
				default:
					sprintf(buf,"raw_%02X",p);
					strcat(trans,buf);
					break;
				}
				if (still_going)
					strcat(trans," ");
			}
			else
			{
				switch (p)
				{
				case 254:
					strcat(trans,"end");
					still_going=0;
					break;
				case 0x54:
					strcat(trans,"hide");
					break;
				case 0x55:
					strcat(trans,"show");
					break;
				case 0x56:
					strcat(trans,"alert");
					break;
				case 0x57:
					strcat(trans,"question");
					break;
				case 0x58:
					strcat(trans,"love");
					break;
				case 0x5A:
					strcat(trans,"pokeball");
					break;
				case 0x10:
					strcat(trans,"pause0");
					break;
				case 0x11:
					strcat(trans,"pause1");
					break;
				case 0x12:
					strcat(trans,"pause2");
					break;
				case 0x13:
					strcat(trans,"pause3");
					break;
				case 0x14:
					strcat(trans,"pause4");
					break;
				case 0x00:
					strcat(trans,"look_down");
					break;
				case 0x01:
					strcat(trans,"look_up");
					break;
				case 0x02:
					strcat(trans,"look_left");
					break;
				case 0x03:
					strcat(trans,"look_right");
					break;
				case 0x04:
					strcat(trans,"walk_down_vslow");
					break;
				case 0x05:
					strcat(trans,"walk_up_vslow");
					break;
				case 0x06:
					strcat(trans,"walk_left_vslow");
					break;
				case 0x07:
					strcat(trans,"walk_right_vslow");
					break;
				case 0x08:
					strcat(trans,"walk_down_slow");
					break;
				case 0x09:
					strcat(trans,"walk_up_slow");
					break;
				case 0x0A:
					strcat(trans,"walk_left_slow");
					break;
				case 0x0B:
					strcat(trans,"walk_right_slow");
					break;
				case 0x17:
					strcat(trans,"walk_down");
					break;
				case 0x18:
					strcat(trans,"walk_up");
					break;
				case 0x15:
					strcat(trans,"walk_left");
					break;
				case 0x16:
					strcat(trans,"walk_right");
					break;
				case 0x2D:
					strcat(trans,"walk_down_fast");
					break;
				case 0x2E:
					strcat(trans,"walk_up_fast");
					break;
				case 0x2F:
					strcat(trans,"walk_left_fast");
					break;
				case 0x30:
					strcat(trans,"walk_right_fast");
					break;
				case 0x35:
					strcat(trans,"run_down");
					break;
				case 0x36:
					strcat(trans,"run_up");
					break;
				case 0x37:
					strcat(trans,"run_left");
					break;
				case 0x38:
					strcat(trans,"run_right");
					break;
				case 0x7e:
					strcat(trans,"run_down_fast");
					break;
				case 0x7f:
					strcat(trans,"run_up_fast");
					break;
				case 0x80:
					strcat(trans,"run_left_fast");
					break;
				case 0x81:
					strcat(trans,"run_right_fast");
					break;
				default:
					sprintf(buf,"raw_%02X",p);
					strcat(trans,buf);
					break;
				}
				if (still_going)
					strcat(trans," ");
			}
		}
		fclose(fileC);
	}
	else
	{
		strcpy(trans,"Error in translating movement data!");
	}
	return trans;
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
char* transbackstr(char*scrfn,unsigned int pos,codeblock*c)
{
	char*NewSpace,*ret;
	char cch,noend=0;
	char str[65536];
	unsigned int i=0,j=0,k,l;
	char lb[5]; //Little Buffer
#ifndef DLL
	unsigned int read;
	FILE*scrfile;
	scrfile=fopen(scrfn,"rb");

	if (pos!=0xFFFFFFFF)fseek(scrfile,pos,SEEK_SET);
	else {
#ifdef WIN32
		MessageBox(NULL,"Darn file pointers. Always spoiling the fun.","Error",0x10);
#endif
		return NULL;
	}
	if (scrfile==NULL) {
#ifdef WIN32
		MessageBox(NULL,"Darn file handles. Always being complicated on purpose.","Error",0x10);
#endif
		return NULL;
	}
#else
	unsigned int spos=pos;
#endif
	k=0;
	while (1)
	{
#ifndef DLL
		read=(unsigned int)fread(&cch,1,1,scrfile);
		if (read==0)break;
#else
		cch=scrfn[spos];
		spos++;
		if (cch==0)break;
#endif
		if (cch=='\n')break;
		if (cch=='\r')break;
		str[k]=cch;
		k++;
	}
#ifndef DLL
	fclose(scrfile);
#endif
	str[k]=0;
	ret=malloc(strlen(str)+1);
	strcpy(ret,str);
	NewSpace=malloc(strlen(str)+1);
	if (mode==GOLD||mode==CRYSTAL) {
		NewSpace[0]=0;
		j=1;
		l=strlen(str);
		while (i<l)
		{
			if ((str[i]>='A'&&str[i]<='Z')||(str[i]>='a'&&str[i]<='z')) {
				NewSpace[j]=str[i]+0x3F;
			}
			else if (str[i]>='0'&&str[i]<='9') {
				NewSpace[j]=str[i]+0xC6;
			}
			else if (str[i]==' ') {
				NewSpace[j]=0x7F;
			}
			else if (str[i]=='?') {
				NewSpace[j]=0xE6;
			}
			else if (str[i]=='!') {
				NewSpace[j]=0xE7;
			}
			else if (str[i]=='.') {
				NewSpace[j]=0xE8;
			}
			else if (str[i]==',') {
				NewSpace[j]=0xF4;
			}
			else if (str[i]=='-') {
				NewSpace[j]=0xE3;
			}
			else if (str[i]=='$') {
				NewSpace[j]=0xF0;
			}
			else if (str[i]=='\'')
			{
				i++;
				if (str[i]=='d') {
					NewSpace[j]=0xD0;
				}
				else if (str[i]=='l') {
					NewSpace[j]=0xD1;
				}
				else if (str[i]=='m') {
					NewSpace[j]=0xD2;
				}
				else if (str[i]=='r') {
					NewSpace[j]=0xD3;
				}
				else if (str[i]=='s') {
					NewSpace[j]=0xD4;
				}
				else if (str[i]=='t') {
					NewSpace[j]=0xD5;
				}
				else if (str[i]=='v') {
					NewSpace[j]=0xD6;
				}
				else
				{
					i--;
					NewSpace[j]=0xE0;
				}
			}
			else if (str[i]=='\\')
			{
				i++;
				if (str[i]=='n') {
					NewSpace[j]=0x4F;
				}
				else if (str[i]=='l') {
					NewSpace[j]=0x55;
				}
				else if (str[i]=='p') {
					NewSpace[j]=0x51;
				}
				else if (str[i]=='e') {
					NewSpace[j]=0x57;
					j++;
					NewSpace[j]=0x0;
					break;
				}
				else if (str[i]=='h') {
					i++;
					lb[0]=str[i];
					i++;
					lb[1]=str[i];
					lb[2]=0;
					sscanf(lb,"%x",&k);
					k=k&0xff;
					NewSpace[j]=k;
				}
				else if(str[i]=='\n')
				{
					while(str[i]=='\n')i++;
					i--;
				}
				else
				{
					i--;
				}
			}
			else if (str[i]==':') {
				NewSpace[j]=0x9C;
			}
			else if (str[i]=='[')
			{
				i++;
				if (str[i]=='.')
				{
					i++;
					if (str[i]==']')
					{
						NewSpace[j]=0x75;
					}
					else if (str[i]=='.')
					{
						i++;
						if (str[i]==']')
						{
							NewSpace[j]=0x56;
						} else {
							i-=3;
						}
					} else {
						i-=2;
					}
				}
				else if (str[i++]=='P')
				{
					if (str[i]=='L')
					{
						i++;
						if (str[i]=='A')
						{
							i++;
							if (str[i]=='Y')
							{
								i++;
								if (str[i]=='E')
								{
									i++;
									if (str[i]=='R')
									{
										i++;
										if (str[i]==']')
										{
											NewSpace[j]=0x52;
										} else {
											i-=7;
										}
									} else {
										i-=6;
									}
								} else {
									i-=5;
								}
							} else {
								i-=4;
							}
						} else {
							i-=3;
						}
					}
					else if (str[i]=='O')
					{
						i++;
						if (str[i]=='K')
						{
							i++;
							if ((unsigned char)str[i]==(unsigned char)0xE9||str[i]=='e'||str[i]=='E'||(unsigned char)str[i]==(unsigned char)0xC9)
							{
								i++;
								if (str[i]==']')
								{
									NewSpace[j]=0x54;
								} else {
									i-=5;
								}
							} else {
								i-=4;
							}
						} else {
							i-=3;
						}
					} else {
						i-=2;
					}
				} else {
					i--;
				}
			}
			i++;
			j++;
		}
		if (NewSpace[j-1]!=0x57) {
			NewSpace[j]=0x50;
		}
		j++;
	}
	else
	{
		noend=0;
		while (i<strlen(str))
		{
			if (str[i]>='A'&&str[i]<='Z') {
				NewSpace[j]=str[i]+0x7a;
			}
			else if (str[i]>='a'&&str[i]<='z') {
				NewSpace[j]=str[i]+0x74;
			}
			else if (str[i]>='0'&&str[i]<='9') {
				NewSpace[j]=str[i]+0x71;
			}
			else if (str[i]==' ') {
				NewSpace[j]=0;
			}
			else if ((unsigned char)str[i]==(unsigned char)0xE9) {
				NewSpace[j]=0x1B;
			}
			else if (str[i]=='?') {
				NewSpace[j]=0xAC;
			}
			else if (str[i]=='.') {
				NewSpace[j]=0xAD;
			}
			else if (str[i]==',') {
				NewSpace[j]=0xB8;
			}
			else if (str[i]=='"') {
				NewSpace[j]=0xB1;
			}
			else if (str[i]=='\'') {
				NewSpace[j]=0xB4;
			}
			else if (str[i]=='-') {
				NewSpace[j]=0xAE;
			}
			else if (str[i]=='\\')
			{
				i++;
				if (str[i]=='n') {
					NewSpace[j]=0xFE;
				}
				else if (str[i]=='x') {
					noend=1;
				}
				else if (str[i]=='w') {
					NewSpace[j]=0xFC;
					j++;
					NewSpace[j]=0x09;
				}
				else if (str[i]=='l') {
					NewSpace[j]=0xFA;
				}
				else if (str[i]=='p') {
					NewSpace[j]=0xFB;
				}
				else if (str[i]=='v') {
					NewSpace[j]=0xFD;
				}
				else if (str[i]=='e') {
					NewSpace[j]=0x1B;
				}
				else if (str[i]=='c') {
					NewSpace[j]=0xFC;
				}
				else if (str[i]=='h') {
					i++;
					lb[0]=str[i];
					i++;
					lb[1]=str[i];
					lb[2]=0;
					sscanf(lb,"%x",&k);
					k=k&0xff;
					NewSpace[j]=k;
				}
				else
				{
					i--;
				}
			}
			else if (str[i]==':') {
				NewSpace[j]=0xF0;
			}
			else if (str[i]=='!') {
				NewSpace[j]=0xAB;
			}
			else if (str[i]=='[')
			{
				i++;
				if (str[i]=='.')
				{
					i++;
					if (str[i]==']')
					{
						NewSpace[j]=0xB0;
					} else {
						i--;
						i--;
					}
				}
				else if (str[i]=='"')
				{
					i++;
					if (str[i]==']')
					{
						NewSpace[j]=0xB2;
					} else {
						i--;
						i--;
					}
				}
				else if (str[i]=='p')
				{
					i++;
					if (str[i]==']')
					{
						NewSpace[j]=0xB7;
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
		if(!noend)
		{
			NewSpace[j]=0xFF;
			j++;
		}
	}
	if (eorg)
		for (k=0;k<j;k++)
			NewSpace[k]=search;
	add_data(c,NewSpace,j);
	free(NewSpace);
	return ret;
}

unsigned int transbackmove(char*script,unsigned int*ii)
{
	unsigned int i,len=0,k=0;
	char cmdbuf[100];
	char xbuf[100];
	//unsigned int read;
	i=*ii;
	while (script[i]!='\n'&&script[i]!=0&&script[i]!='\'')
	{
		while (script[i]==' ') {
			i++;
		}
		size_t cmdbuf_size = 0;
		while (script[i]!=' '&&script[i]!='\n'&&script[i]!=0&&script[i]!='\'')
		{
			cmdbuf[cmdbuf_size]=script[i];
			i++;
			cmdbuf_size++;
		}
		cmdbuf[cmdbuf_size]=0;
#define aaa(x) else if(!strcmp(cmdbuf,x))
#define move trans[len]=
		if (mode==GOLD||mode==CRYSTAL)
		{
			const struct move_data* move_data = NULL;
			if (eorg) {
				move 0x00;
			}
			else if ((move_data = lookup_gsc_move(cmdbuf, cmdbuf_size)) != NULL) {
				move move_data->value;
			}
			else if (cmdbuf[0]=='\'')
			{
				*ii=i;
				return len;
			}
			else if (cmdbuf_size == 6 && memcmp(cmdbuf, "raw_", 4) == 0)
			{
				//RAW handler.
				unsigned int j=0;
				while (j<16)
				{
					if (cmdbuf[4]=="0123456789abcdef"[j])
					{
						break;
					}
					j++;
				}
				if (j==16)
				{
					len--;
					sprintf(xbuf,"Invalid hex char '%c'.\r\n",cmdbuf[4]);
					log_txt(xbuf,strlen(xbuf));
				}
				else
				{
					k=j;
					j=0;
					while (j<16)
					{
						if (cmdbuf[5]=="0123456789abcdef"[j])
						{
							break;
						}
						j++;
					}
					if (j==16)
					{
						len--;
						sprintf(xbuf,"Invalid hex char '%c'.\r\n",cmdbuf[5]);
						log_txt(xbuf,strlen(xbuf));
					}
					else
					{
						k=k<<4;
						k|=j;
						move k;
					}
				}
			}
			else {
				len--;
				sprintf(xbuf,"Unknown Gold/Silver/Crystal move command \"%s\"\r\n",cmdbuf);
				log_txt(xbuf,strlen(xbuf));
			}
			len++;

		}
		else if (mode==FIRE_RED)
		{
			if (eorg) {
				move 0xFF;
			}
			aaa("end") {
				move 0xfe;
			}
			aaa("look_up") {
				move 0x01;
			}
			aaa("look_left") {
				move 0x02;
			}
			aaa("look_right") {
				move 0x03;
			}
			aaa("look_down") {
				move 0x04;
			}
			aaa("walk_down_vslow") {
				move 0x08;
			}
			aaa("walk_up_vslow") {
				move 0x09;
			}
			aaa("walk_left_vslow") {
				move 0x0A;
			}
			aaa("walk_right_vslow") {
				move 0x0B;
			}
			aaa("walk_down_slow") {
				move 0x0C;
			}
			aaa("walk_up_slow") {
				move 0x0D;
			}
			aaa("walk_left_slow") {
				move 0x0E;
			}
			aaa("walk_right_slow") {
				move 0x0F;
			}
			aaa("walk_down") {
				move 0x10;
			}
			aaa("walk_up") {
				move 0x11;
			}
			aaa("walk_left") {
				move 0x12;
			}
			aaa("walk_right") {
				move 0x13;
			}
			aaa("jump_down2") {
				move 0x14;
			}
			aaa("jump_up2") {
				move 0x15;
			}
			aaa("jump_left2") {
				move 0x16;
			}
			aaa("jump_right2") {
				move 0x17;
			}
			aaa("pause_xshort"){move 0x18;}
			aaa("pause_vshort"){move 0x19;}
			aaa("pause_short"){move 0x1A;}
			aaa("pause"){move 0x1B;}
			aaa("pause_long"){move 0x1C;}
			aaa("run_down") {
				move 0x1D;
			}
			aaa("run_up") {
				move 0x1E;
			}
			aaa("run_left") {
				move 0x1F;
			}
			aaa("run_right") {
				move 0x20;
			}
				aaa("onspot_down_slow"){move 0x21;}
				aaa("onspot_up_slow"){move 0x22;}
				aaa("onspot_left_slow"){move 0x23;}
				aaa("onspot_right_slow"){move 0x24;}
				aaa("onspot_down"){move 0x25;}
				aaa("onspot_up"){move 0x26;}
				aaa("onspot_left"){move 0x27;}
				aaa("onspot_right"){move 0x28;}
				aaa("onspot_down_fast"){move 0x29;}
				aaa("onspot_up_fast"){move 0x2A;}
				aaa("onspot_left_fast"){move 0x2B;}
				aaa("onspot_right_fast"){move 0x2C;}
				aaa("look_down_delayed"){move 0x2D;}
				aaa("look_up_delayed"){move 0x2E;}
				aaa("look_left_delayed"){move 0x2F;}
				aaa("look_right_delayed"){move 0x30;}
				aaa("slide_down"){move 0x31;}
				aaa("slide_up"){move 0x32;}
				aaa("slide_left"){move 0x33;}
				aaa("slide_right"){move 0x34;}
				aaa("run_down_vfast"){move 0x35;}
				aaa("run_up_vfast"){move 0x36;}
				aaa("run_left_vfast"){move 0x37;}
				aaa("run_right_vfast"){move 0x38;}
				aaa("slide_down_vfast"){move 0x39;}
				aaa("slide_up_vfast"){move 0x3A;}
				aaa("slide_left_vfast"){move 0x3B;}
				aaa("slide_right_vfast"){move 0x3C;}
				aaa("slide_down2"){move 0x3D;}
				aaa("slide_up2"){move 0x3E;}
				aaa("slide_left2"){move 0x3F;}
				aaa("slide_right2"){move 0x40;}
				aaa("slide_down3"){move 0x41;}
				aaa("slide_up3"){move 0x42;}
				aaa("slide_left3"){move 0x43;}
				aaa("slide_right3"){move 0x44;}
				aaa("walk_onspot"){move 0x45;}
			aaa("look_left_jump_down") {
				move 0x46;
			}
			aaa("look_down_jump_up") {
				move 0x47;
			}
			aaa("look_up_jump_left") {
				move 0x48;
			}
			aaa("look_left_jump_right") {
				move 0x49;
			}
			aaa("faceplayer") {
				move 0x4A;
			}
			aaa("face_away") {
				move 0x4B;
			}
			aaa("jump_down1") {
				move 0x4E;
			}
			aaa("jump_up1") {
				move 0x4F;
			}
			aaa("jump_left1") {
				move 0x50;
			}
			aaa("jump_right1") {
				move 0x51;
			}
			aaa("jump_down") {
				move 0x52;
			}
			aaa("jump_up") {
				move 0x53;
			}
			aaa("jump_left") {
				move 0x54;
			}
			aaa("jump_right") {
				move 0x55;
			}
			aaa("jump_downup") {
				move 0x56;
			}
			aaa("jump_updown") {
				move 0x57;
			}
			aaa("jump_leftright") {
				move 0x58;
			}
			aaa("jump_rightleft") {
				move 0x59;
			}
			aaa("face_default") {
				move 0x5A;
			}
			aaa("hide") {
				move 0x60;
			}
			aaa("say_!") {
				move 0x62;
			}
			aaa("say_?") {
				move 0x63;
			}
			aaa("say_x") {
				move 0x64;
			}
			aaa("say_!!") {
				move 0x65;
			}
			aaa("say_:)") {
				move 0x66;
			}
			aaa("rock_animate") {
				move 0x68;
			}
			aaa("tree_animate") {
				move 0x69;
			}
			else if (cmdbuf[0]=='\'')
			{
				*ii=i;
				return len;
			}
			else if (cmdbuf_size == 6 && memcmp(cmdbuf, "raw_", 4) == 0)
			{
				//RAW handler.
				unsigned int j=0;
				while (j<16)
				{
					if (cmdbuf[4]=="0123456789abcdef"[j])
					{
						break;
					}
					j++;
				}
				if (j==16)
				{
					len--;
					sprintf(xbuf,"Invalid hex char '%c'.\r\n",cmdbuf[4]);
					log_txt(xbuf,strlen(xbuf));
				}
				else
				{
					k=j;
					j=0;
					while (j<16)
					{
						if (cmdbuf[5]=="0123456789abcdef"[j])
						{
							break;
						}
						j++;
					}
					if (j==16)
					{
						len--;
						sprintf(xbuf,"Invalid hex char '%c'.\r\n",cmdbuf[5]);
						log_txt(xbuf,strlen(xbuf));
					}
					else
					{
						k=k<<4;
						k|=j;
						move k;
					}
				}
			}
			else if (*cmdbuf)
			{
				len--;
				sprintf(xbuf,"Unknown FR/LG move command \"%s\"\r\n",cmdbuf);
				log_txt(xbuf,strlen(xbuf));
			}
			len++;
		}
		else
		{
			if (eorg) {
				move 0xFF;
			}
			aaa("end") {
				move 0xfe;
			}
			aaa("hide") {
				move 0x54;
			}
			aaa("show") {
				move 0x55;
			}
			aaa("alert") {
				move 0x56;
			}
			aaa("question") {
				move 0x57;
			}
			aaa("love") {
				move 0x58;
			}
			aaa("pokeball") {
				move 0x5A;
			}
			aaa("pause0") {
				move 0x10;
			}
			aaa("pause1") {
				move 0x11;
			}
			aaa("pause2") {
				move 0x12;
			}
			aaa("pause3") {
				move 0x13;
			}
			aaa("pause4") {
				move 0x14;
			}
			aaa("look_down") {
				move 0x00;
			}
			aaa("look_up") {
				move 0x01;
			}
			aaa("look_left") {
				move 0x02;
			}
			aaa("look_right") {
				move 0x03;
			}
			aaa("walk_down_vslow") {
				move 0x04;
			}
			aaa("walk_up_vslow") {
				move 0x05;
			}
			aaa("walk_left_vslow") {
				move 0x06;
			}
			aaa("walk_right_vslow") {
				move 0x07;
			}
			aaa("walk_down_slow") {
				move 0x08;
			}
			aaa("walk_up_slow") {
				move 0x09;
			}
			aaa("walk_left_slow") {
				move 0x0A;
			}
			aaa("walk_right_slow") {
				move 0x0B;
			}
			aaa("walk_down") {
				move 0x17;
			}
			aaa("walk_up") {
				move 0x18;
			}
			aaa("walk_left") {
				move 0x15;
			}
			aaa("walk_right") {
				move 0x16;
			}
			aaa("walk_down_fast") {
				move 0x2D;
			}
			aaa("walk_up_fast") {
				move 0x2E;
			}
			aaa("walk_left_fast") {
				move 0x2F;
			}
			aaa("walk_right_fast") {
				move 0x30;
			}
			aaa("run_down") {
				move 0x35;
			}
			aaa("run_up") {
				move 0x36;
			}
			aaa("run_left") {
				move 0x37;
			}
			aaa("run_right") {
				move 0x38;
			}
			aaa("run_down_fast") {
				move 0x7e;
			}
			aaa("run_up_fast") {
				move 0x7f;
			}
			aaa("run_left_fast") {
				move 0x80;
			}
			aaa("run_right_fast") {
				move 0x81;
			}
			else if (cmdbuf[0]=='\'')
			{
				*ii=i;
				return len;
			}
			else if (cmdbuf_size == 6 && memcmp(cmdbuf, "raw_", 4) == 0)
			{
				//RAW handler.
				unsigned int j=0;
				while (j<16)
				{
					if (cmdbuf[4]=="0123456789abcdef"[j])
					{
						break;
					}
					j++;
				}
				if (j==16)
				{
					len--;
					sprintf(xbuf,"Invalid hex char '%c'.\r\n",cmdbuf[4]);
					log_txt(xbuf,strlen(xbuf));
				}
				else
				{
					k=j;
					j=0;
					while (j<16)
					{
						if (cmdbuf[5]=="0123456789abcdef"[j])
						{
							break;
						}
						j++;
					}
					if (j==16)
					{
						len--;
						sprintf(xbuf,"Invalid hex char '%c'.\r\n",cmdbuf[5]);
						log_txt(xbuf,strlen(xbuf));
					}
					else
					{
						k=k<<4;
						k|=j;
						move k;
					}
				}
			}
			else {
				len--;
				sprintf(xbuf,"Unknown R/S move command \"%s\"\r\n",cmdbuf);
				log_txt(xbuf,strlen(xbuf));
			}
			len++;
		}
	}
	*ii=i;
	return len;
}
