#ifndef DOES_NOT_UPDATE
#define INTERNAL_VERSION "2.1.1"
#else
#define INTERNAL_VERSION "Debug Build"
#endif

#define _CRT_SECURE_NO_DEPRECATE
#pragma comment( lib, "comctl32.lib" )

#include "sqrt.h"
#include "XPMData.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "scintilla.h"
#include "scilexer.h"
#ifndef DOES_NOT_UPDATE
#include <winsock.h>
#endif
#include "resource.h"

#define WIDTH  640
#define HEIGHT 480

#define STYLE_NORMAL    10
#define STYLE_IMPORTANT 11
#define STYLE_SPECIAL   12
#define STYLE_COMMAND   13
#define STYLE_ARGUMENT  14
#define STYLE_MESSAGE   15
#define STYLE_LABEL     16

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void UpdateScintLine();
void GoToLine(unsigned int,unsigned int);
unsigned int modechar;

unsigned int CalcMem=0;
char FileOpen[MAX_PATH],FileOpen2[MAX_PATH+40],FileOpen3[MAX_PATH],RomOpen[MAX_PATH],
RomOpen2[MAX_PATH],SuperBuffer[MAX_PATH*4],GreatBuffer[MAX_PATH*2],DirBuffer[MAX_PATH*2],
DefRomPath[MAX_PATH],DefScriptPath[MAX_PATH],MegaBuffer[10240];
char AOT=1,autoautocomplete=0,FlagsShown=0;
HWND MainWnd,StatusBar,Text,FindReplaceWin,GotoWin,FFSWin,
DecWin,GenWin,InputWin,Input2Win,NoteWin,ToolBar,HexWin,
MovePlanWin,FlagsWin,ScriptSearchWin,LockWin
#ifdef SCRAP
,PickupWin,ScrapPunk
#endif
;
HANDLE ScintLib,PKSV,RomFile=NULL,FileSave
#ifdef SCRAP
                             ,SearchBox
#endif
                             ;
int (__cdecl *VersionMismatch)(unsigned char*);
int (__cdecl *DetermineMode)(unsigned char*);
char* (__cdecl *decompile)(unsigned char*,int,int);
char* (__cdecl *decompileASM)(unsigned char*,int);
char* (__cdecl *decompileLevel)(unsigned char*,int);
char* (__cdecl *decompileText)(unsigned char*,int);
char* (__cdecl *decompileMart)(unsigned char*,int);
char* (__cdecl *decompileMoves)(unsigned char*,int);
char* (__cdecl *decompilePointer)(unsigned char*,int);
char* (__cdecl *decompileAttacks)(unsigned char*,int);
char* (__cdecl *NewMode)(int);
int (__cdecl *compile)(unsigned char*,unsigned char*);
int (__cdecl *DebugCompile)(unsigned char*,unsigned char*);
int (__cdecl *SetDynamic)(int,int);
#ifdef SCRAP
int (__cdecl *CreateListClass)();
#endif

unsigned int LastCalc;
signed int LastFlag=-1;
#define OP_NONE 0
#define OP_MULT 1
#define OP_DIVI 2
#define OP_ADIT 3
#define OP_SUBT 4
#define OP_POWR 5
#define OP_SHIL 6
#define OP_SHIR 7
#define OP_ROTL 8
#define OP_ROTR 9
#define OP_MODU 10
#define OP_ASCI 11
#define OP_BXOR 12
#define OP_BWOR 13
#define OP_BAND 14

#define GBA 0
#define GBC 1

char CalcPressed=0,LastCalcOp=OP_NONE,NotSaved=0,collapseatend=1,
     collapselines=0,cancelled=0,inputrunning=0,pokemon_version=GBA;
#define FIRE_RED  0
#define RUBY      1
#define GOLD      2
#define DIAMOND   3
#define CRYSTAL   4
HINSTANCE inst;
char strbuffer[2048],strbuffer2[2048];
char*GetString1(unsigned short id)
{
	LoadString(inst,id,strbuffer,sizeof(strbuffer));
	return strbuffer;
}
char*GetString2(unsigned short id)
{
	LoadString(inst,id,strbuffer2,sizeof(strbuffer2));
	return strbuffer2;
}
BOOL CALLBACK FindReplaceFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK GotoFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK FFSFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK DecFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK GenFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK InputFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK Input2Func(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK NoteFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK HexFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MovePlanFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK FlagsFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ScriptSearchFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK LockFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
#ifdef SCRAP
BOOL CALLBACK PickupFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
#endif
void FillItems(HWND);
unsigned char search=0xff;
DWORD maintid;
#define SendEditor(x,y,z) SendMessage(Text,x,y,z)

void SaveRegSetting(char*sub,char*set,int val)
{
	HKEY regkey;
	DWORD dummy;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	RegCreateKeyEx(HKEY_CURRENT_USER,subbuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&regkey,&dummy);
	RegSetValueEx(regkey,set,0,REG_DWORD,(CONST BYTE*)&val,4);
	RegCloseKey(regkey);
}

int LoadRegSetting(char*sub,char*set)
{
	int Data;
	DWORD DataSize,Type;
	HKEY regkey;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	RegOpenKeyEx(HKEY_CURRENT_USER,subbuf,0,KEY_READ,&regkey);
	DataSize=sizeof(Data);
	Data=0;
	RegQueryValueEx(regkey,set,NULL,&Type,(LPBYTE)&Data,&DataSize);
	RegCloseKey(regkey);
	return Data;
}

int LoadRegSettingDef(char*sub,char*set,int def)
{
	int Data;
	DWORD DataSize,Type;
	HKEY regkey;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	RegOpenKeyEx(HKEY_CURRENT_USER,subbuf,0,KEY_READ,&regkey);
	DataSize=sizeof(Data);
	Data=def;
	RegQueryValueEx(regkey,set,NULL,&Type,(LPBYTE)&Data,&DataSize);
	RegCloseKey(regkey);
	return Data;
}

void SaveRegSettingStr(char*sub,char*set,char*val)
{
	HKEY regkey;
	DWORD dummy;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	RegCreateKeyEx(HKEY_CURRENT_USER,subbuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&regkey,&dummy);
	RegSetValueEx(regkey,set,0,REG_SZ,val,strlen(val)+1);
	RegCloseKey(regkey);
}

char* LoadRegSettingStr(char*sub,char*set)
{
	DWORD DataSize,Type;
	HKEY regkey;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	*GreatBuffer=0;
	RegOpenKeyEx(HKEY_CURRENT_USER,subbuf,0,KEY_READ,&regkey);
	DataSize=sizeof(GreatBuffer);
	RegQueryValueEx(regkey,set,NULL,&Type,(LPBYTE)&GreatBuffer,&DataSize);
	RegCloseKey(regkey);
	GreatBuffer[sizeof(GreatBuffer)-1]=0;
	return GreatBuffer;
}

signed int PointerToOffset(unsigned int ptr)
{
	unsigned int pointer=0;
	unsigned int bank=0;
	unsigned int offset=0;
	bank=ptr&0xFF;
	pointer=(ptr&0xFFFF00)>>8;
	if (pointer<0x4000||pointer>0x7FFF)return -1;
	pointer&=0x3FFF;
	pointer|=(bank&3)<<14;
	bank>>=2;
	return pointer|(bank<<16);
}
signed int OffsetToPointer(unsigned int offset)
{
	unsigned int pointer=0;
	unsigned int bank=0;

	bank=((offset&0xFF0000)>>14);
	if (bank>0xFF) {
		return -1;
	}
	if ((offset&0xFF000000)) {
		return -1;
	}
	pointer=offset&0xFFFF;
	bank|=((pointer&0xF000)>>14);
	pointer&=0x3FFF;
	pointer|=0x4000;
	return (pointer<<8)|bank;
}

#ifndef DOES_NOT_UPDATE
#ifndef ZH
#pragma comment( lib, "wsock32.lib" )
//UPDATEME
BOOL CALLBACK UpdatesDlgProc(HWND,UINT,WPARAM,LPARAM);
DWORD WINAPI UpdateMe(LPVOID nothing)
{
	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in address;
	struct hostent * host;
	int i,j,sl_sock;
	char buffer[4096],*UpdateBuffer;

	if (WSAStartup(MAKEWORD(1,1),&wsaData)==0)
	{
		if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
		{
			MessageBox(NULL,GetString1(3000),GetString2(3001),0x10);
		}
		address.sin_family=AF_INET;
		address.sin_port=htons(80);
		if ((host=gethostbyname("pk-script-view.sourceforge.net"))!=NULL)
		{
			address.sin_addr.s_addr=*((unsigned long*)host->h_addr);
			connect(sock,(struct sockaddr*)&address,sizeof(address));
#define get_update "GET /ver.txt HTTP/1.1\nHost: pk-script-view.sourceforge.net\nAccept-Encoding: None\nConnection: Close\n\n"
			send(sock,get_update,(int)strlen(get_update),0);
			recv(sock,buffer,4096,0);
			i=0;
			while (((buffer[i]>'9'||buffer[i]<'0')||(buffer[i+1]>'9'||buffer[i+1]<'0'))&&buffer[i]!=0)
			{
				i++;
			}
			if (buffer[i]=='2')
			{
				i=0;
				sl_sock=(int)strlen(buffer);
				while (i<sl_sock)
				{
					if (buffer[i]=='V'&&buffer[i+1]=='='&&buffer[i+2]=='=')
					{
						break;
					}
					i++;
				}
				if (i==sl_sock)
					return 0;
				j=0;
				while (j+i<sl_sock)
				{
					if (buffer[i+j]=='\n') {
						buffer[i+j]=0;
						break;
					}
					j++;
				}
				if (strcmp(INTERNAL_VERSION,(buffer+i+3)))
				{
					i+=3;
					j=0;
					while (j<(int)strlen(buffer+i))
					{
						if (buffer[i+j]=='.') {
							buffer[i+j]='-';
						}
						j++;
					}
					if (DialogBox(inst,MAKEINTRESOURCE(3077),MainWnd,UpdatesDlgProc))
					{
						UpdateBuffer=alloca(79+strlen(buffer+i));
						strcpy(UpdateBuffer,"http://downloads.sourceforge.net/pk-script-view/pksvui_pkg");
						strcat(UpdateBuffer,buffer+i);
						strcat(UpdateBuffer,".zip?use_mirror=osdn");
						ShellExecute(GenWin,NULL,UpdateBuffer,NULL,NULL,SW_SHOWDEFAULT);
					}
				}
				else if(nothing)
				{
					DialogBox(inst,MAKEINTRESOURCE(3076),MainWnd,UpdatesDlgProc);//Dialog, NO UPDATE AVAILABLE
				}
			}
		}
	}
	else
	{
		MessageBox(NULL,GetString1(3004),GetString2(3001),0x10);
	}
	WSACleanup();
	return 0;
}
BOOL CALLBACK UpdatesDlgProc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch(m)
	{
		case WM_INITDIALOG:
			CheckDlgButton(h,1,LoadRegSettingDef(NULL,"AutoUpdate",1));
			break;
		case WM_COMMAND:
			switch(LOWORD(w))
			{
				case 1:
					SaveRegSetting(NULL,"AutoUpdate",IsDlgButtonChecked(h,1)==BST_CHECKED);
					break;
				case 2:
					EndDialog(h,1);
					break;
				case 3:
					EndDialog(h,0);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(h,0);
			break;
		default:
			return 0;
	}
	return 1;
}
#endif
#endif
//TOLOWER
char*ToLower(char*str)
{
	while (*str!=0)
	{
		if (*str>='A'&&*str<='Z')
		{
			*str+=' '; //Coincidence?
		}
		str++;
	}
	return str;
}
//CTOLOWER
char cToLower(char chr)
{
	if (chr>='A'&&chr<='Z')
		chr+=' ';
	return chr;
}
//GETDIRFROMGREATBUFFER
void GetDirFromGreatBuffer()
{
	/*
	#ifdef MSVC
	  char*a=GreatBuffer;
	  __asm
	  {
		mov eax,a
		cmp byte ptr [eax],0x22
		pushf
		xor edx,edx
		mov ebx,eax
		popf
		jne NoQuote
		mov dl,0x22
	BkToLp:
		inc ebx
	NoQuote:
		cmp byte ptr [ebx],dl
		jnz BkToLp
	BkToSLp:
		dec ebx
		cmp byte ptr [ebx],0x5C
		jnz BkToSLp
		inc ebx
		mov byte ptr [ebx],0
		xor ecx,ecx
		cmp edx,0
		je GDGBEnd
	BkToTlp:
		mov bl,byte ptr [eax+ecx+1]
		mov byte ptr [eax+ecx],bl
		inc ecx
		cmp byte ptr [eax+ecx],0
		jnz BkToTlp
	GDGBEnd:
	  }
	  return;
	#else
	  asm volatile ( ".intel_syntax noprefix\n"
	        "push %%ebx\n"
	        "push %%ecx\n"
	        "push %%edx\n"
	        "cmp byte ptr [%%eax],0x22\n"
	        "pushf\n"
	        "xor %%edx,%%edx\n"
	        "mov %%ebx,%%eax\n"
	        "popf\n"
	        "jne NoQuote\n"
	        "mov %%dl,0x22\n"
	 "BkToLp:inc %%ebx\n"
	"NoQuote:cmp byte ptr [%%ebx],%%dl\n"
	        "jnz BkToLp\n"
	"BkToSLp:dec %%ebx\n"
	        "cmp byte ptr [%%ebx],0x5C\n"
	        "jnz BkToSLp\n"
	        "inc %%ebx\n"
	        "mov byte ptr [%%ebx],0\n"
	        "xor %%ecx,%%ecx\n"
	        "cmp %%edx,0\n"
	        "je GDGBEnd\n"
	"BkToTlp:mov %%bl,byte ptr [%%eax+%%ecx+1]\n"
	        "mov byte ptr [%%eax+%%ecx],%%bl\n"
	        "inc %%ecx\n"
	        "cmp byte ptr [%%eax+%%ecx],0\n"
	        "jnz BkToTlp\n"
	        "GDGBEnd:\n"
	        "pop %%edx\n"
	        "pop %%ecx\n"
	        "pop %%ebx\n"
	        ".att_syntax\n"::"a"(GreatBuffer));
	        return;
	#endif
	*/
	register char* b=GreatBuffer;
	register int using_quotes=0;
	if (*b=='"')
	{
		using_quotes='"';
		b++;
	}
	while (*b&&*b!=using_quotes)b++;
	while (b>GreatBuffer&&*b!='\\')b--;
	if (*b=='\\')*b=0;
	if (*GreatBuffer=='"')
	{
		b=GreatBuffer;
		while (*b)
		{
			*b=b[1];
			b++;
		}
	}
}
//GETINPUT
void GetInput(char*title,char*prompt,char*def)
{
	inputrunning=1;
	SetWindowText(InputWin,title);
	SetDlgItemText(InputWin,3,prompt);
	SetDlgItemText(InputWin,4,def);
	ShowWindow(InputWin,SW_SHOW);
	SetForegroundWindow(InputWin);
	SetFocus(GetDlgItem(InputWin,4));
	EnableWindow(MainWnd,0);
}
//GETINPUT
void GetInput2(char*title,char*prompt,char*def,void (*func)(HWND))
{
	inputrunning=1;
	func(GetDlgItem(Input2Win,4));
	SetWindowText(Input2Win,title);
	SetDlgItemText(Input2Win,3,prompt);
	SetDlgItemText(Input2Win,4,def);
	ShowWindow(Input2Win,SW_SHOW);
	SetForegroundWindow(Input2Win);
	SetFocus(GetDlgItem(Input2Win,4));
	EnableWindow(MainWnd,0);
	SendMessage(GetDlgItem(Input2Win,4),CB_SHOWDROPDOWN,1,0);
}
#ifdef SCRAP
#include "miscfuncs.h"
#endif
int foundfont=0;
//FONTENUMARATOR
int CALLBACK FontEnumerator(ENUMLOGFONT *enf,NEWTEXTMETRIC*z,int x,LPARAM c)
{
	ToLower(enf->elfFullName);
	if (!strcmp(enf->elfFullName,"lucida console"))
		foundfont=1;
	return 1;
}
TBBUTTON ToolButtons[]={
	{0,ID_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{1,ID_OPEN,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{2,ID_SAVE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{3,ID_SAVEAS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{7,1015,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{8,1016,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{4,1008,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{5,1009,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{6,1010,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{17,1012,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{15,1032,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{14,1006,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{9,1005,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{10,1014,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{11,1004,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{12,1030,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{19,1202,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{16,1111,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{18,1013,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1},
	{13,1100,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0}
};

void SaveRegSettingBin(char*sub,char*set,char*val,int len)
{
	HKEY regkey;
	DWORD dummy;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	RegCreateKeyEx(HKEY_CURRENT_USER,subbuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&regkey,&dummy);
	RegSetValueEx(regkey,set,0,REG_BINARY,(CONST BYTE*)val,len);
	RegCloseKey(regkey);
}

int LoadRegSettingBin(char*sub,char*set,char**data)
{
	DWORD DataSize,Type;
	HKEY regkey;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	*GreatBuffer=0;
	RegOpenKeyEx(HKEY_CURRENT_USER,subbuf,0,KEY_READ,&regkey);
	DataSize=0;
	RegQueryValueEx(regkey,set,NULL,&Type,NULL,&DataSize);
	*data=GlobalAlloc(GPTR,DataSize);
	RegQueryValueEx(regkey,set,NULL,&Type,(LPBYTE)*data,&DataSize);
	RegCloseKey(regkey);
	return DataSize;
}

void DeleteRegSetting(char*sub,char*set)
{
	HKEY regkey;
	char subbuf[2000];
	strcpy(subbuf,"Software\\Score_Under\\PKSV-UI");
	if (sub&&*sub)
	{
		strcat(subbuf,"\\");
		strcat(subbuf,sub);
	}
	RegOpenKeyEx(HKEY_CURRENT_USER,subbuf,0,KEY_WRITE,&regkey);
	RegDeleteValue(regkey,set);
	RegCloseKey(regkey);
}

short*FlagArr=NULL;
int FlagArrAlloc=0;
int FlagArrLen=0;
void AddFlag(char*a,char*b,char*c)
{
	short*newarr;
	int flag;
	if ((FlagArrAlloc>>1)<=FlagArrLen)
	{
		newarr=GlobalAlloc(GPTR,FlagArrAlloc+512);
		FlagArrAlloc+=512;
		if (FlagArr)
		{
			memcpy(newarr,FlagArr,FlagArrLen<<1);
			GlobalFree(FlagArr);
		}
		FlagArr=newarr;
	}
	sscanf(a,"%X",&flag);
	FlagArr[FlagArrLen]=flag;
	FlagArrLen++;
	strcpy(MegaBuffer,"0x");
	strcpy(MegaBuffer+2,a);
	strcat(MegaBuffer," - ");
	strcat(MegaBuffer,b);
	SendMessage(GetDlgItem(FlagsWin,2),LB_ADDSTRING,0,(LPARAM)MegaBuffer);
	SaveRegSettingStr("Flags\\ShortDesc",a,b);
	SaveRegSettingStr("Flags\\LongDesc",a,c);
	SaveRegSettingBin("Flags","UsedFlags",(char*)FlagArr,FlagArrLen<<1);
}

void SortFlags()
{
	register int i,j=1;
	short k;
	while (j)
	{
		j=0;
		for (i=0;i<FlagArrLen-1;i++)
		{
			if (FlagArr[i]>FlagArr[i+1])
			{
				k=FlagArr[i];
				FlagArr[i]=FlagArr[i+1];
				FlagArr[i+1]=k;
				j=1;
			}
		}
	}
}

void DeleteLastFlag()
{
	register int i;
	char a[5];
	if (LastFlag==-1)return;
	FlagArrLen--;
	sprintf(a,"%04X",FlagArr[LastFlag]);
	for (i=LastFlag;i<FlagArrLen;i++)
		FlagArr[i]=FlagArr[i+1];
	LastFlag=-1;
	DeleteRegSetting("Flags\\ShortDesc",a);
	DeleteRegSetting("Flags\\LongDesc",a);
	SaveRegSettingBin("Flags","UsedFlags",(char*)FlagArr,FlagArrLen<<1);
}

void ReAddFlags()
{
	char buf[5];
	register int i;
	FlagArrLen=(FlagArrAlloc=LoadRegSettingBin("Flags","UsedFlags",(char**)&FlagArr))>>1;
	SortFlags();
	for (i=0;i<FlagArrLen;i++)
	{
		sprintf(buf,"%04X",FlagArr[i]);
		strcpy(MegaBuffer,"0x");
		strcpy(MegaBuffer+2,buf);
		strcat(MegaBuffer," - ");
		LoadRegSettingStr("Flags\\ShortDesc",buf);
		strcat(MegaBuffer,GreatBuffer);
		SendMessage(GetDlgItem(FlagsWin,2),LB_ADDSTRING,0,(LPARAM)MegaBuffer);
	}
}

int ScriptSearchType=11;
//WINMAIN
HBITMAP bmpHero,bmpArrow
#ifdef SCRAP
,SPIcon
#endif
;
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,INT nCmdShow)
{
	MSG msg;
	HDC dc;
	char tch[256],determineMode[4],*x,*y,*ptr,IsRom=0;
	WNDCLASSEX WndClass;
	HACCEL hAccel;
	RECT WinSize;
	unsigned int GoToPlace,i;
	int StatusSize[1],Data,DataSize,Nothing,fsize;
	DWORD read;
	HANDLE fileM;
	MENUITEMINFO mii;
	HKEY regkey;
	TC_ITEM tab;

	maintid=GetCurrentThreadId();
	ZeroMemory(DefRomPath,sizeof(DefRomPath));
	ZeroMemory(DefScriptPath,sizeof(DefScriptPath));
	strcpy(GreatBuffer,GetCommandLine());
	GetDirFromGreatBuffer();
	//GreatBuffer now contains the dir of the EXE.
	strcpy(DirBuffer,GreatBuffer);
	inst=hInstance; //Globalise variable
	//Init window class
	ZeroMemory(&WndClass,sizeof(WNDCLASSEX));
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)MainWndProc;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(1));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+11);
	WndClass.lpszMenuName = MAKEINTRESOURCE(4);
	WndClass.lpszClassName = "Score_Under_PKSVUI";
	WndClass.hIconSm = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(1));

	//Register window with OS
	if (!RegisterClassEx(&WndClass)) {
		MessageBox(NULL,GetString1(3005),GetString2(3001),0x10);
		return 1;
	}
	if (!(ScintLib=LoadLibrary("Scintilla.dll")))
	{
		MessageBox(NULL,GetString1(3007),GetString2(3001),0x10);
		//Failed to load Scintilla.dll
		return 0;
	}
	if (!(PKSV=LoadLibrary("PKSV.dll")))
	{
		MessageBox(NULL,GetString1(3069),GetString2(3001),0x10);
		//Failed to load PKSV.DLL or failed to load one of PKSV.DLL's functions.
		return 0;
	}
	VersionMismatch=(int (__cdecl *)(unsigned char*))GetProcAddress(PKSV,"VersionMismatch");
	DetermineMode=(int (__cdecl *)(unsigned char*))GetProcAddress(PKSV,"DetermineMode");
	decompile=(char* (__cdecl *)(unsigned char*,int,int))GetProcAddress(PKSV,"decompile");
	decompileASM=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompileASM");
	decompileLevel=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompileLevel");
	decompileText=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompileText");
	decompileMart=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompileMart");
	decompileMoves=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompileMoves");
	decompilePointer=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompilePointer");
	decompileAttacks=(char* (__cdecl *)(unsigned char*,int))GetProcAddress(PKSV,"decompileAttacks");
	NewMode=(char* (__cdecl *)(int))GetProcAddress(PKSV,"NewMode");
	compile=(int (__cdecl *)(unsigned char*,unsigned char*))GetProcAddress(PKSV,"compile");
	DebugCompile=(int (__cdecl *)(unsigned char*,unsigned char*))GetProcAddress(PKSV,"DebugCompile");
	SetDynamic=(int (__cdecl *)(int,int))GetProcAddress(PKSV,"SetDynamic");
	if (!VersionMismatch||!NewMode||!DetermineMode||!decompile||!decompileASM||!decompileLevel||!decompileText||!compile||!decompileMart||!decompileMoves||!decompilePointer||!decompileAttacks||!SetDynamic)
	{
		MessageBox(NULL,GetString1(3069),GetString2(3001),0x10);
		//Failed to load PKSV.DLL or failed to load one of PKSV.DLL's functions.
		return 0;
	}
	if (VersionMismatch(INTERNAL_VERSION))
	{
		MessageBox(NULL,GetString1(3070),GetString2(3071),0x30);
		//Warning: PKSV.dll version mismatch! Try downloading the package again.
	}
#ifdef SCRAP
	SearchBox=LoadLibrary("searchbox.dll");
	if (!SearchBox)
	{
		MessageBox(NULL,"Failed to load searchbox.dll",GetString2(3001),0x10);
		return 0;
	}
	CreateListClass=(int (__cdecl *)())GetProcAddress(SearchBox,"CreateListClass");
	if (!CreateListClass)
	{
		MessageBox(NULL,"Failed to load a function in searchbox.dll",GetString2(3001),0x10);
		return 0;
	}
	CreateListClass();
#endif
	*RomOpen=0;
	*RomOpen2=0;

	WinSize.top=WinSize.left=0;
	WinSize.right=WIDTH;
	WinSize.bottom=HEIGHT;
	AdjustWindowRect(&WinSize,WS_OVERLAPPEDWINDOW|WS_POPUPWINDOW,1);
	strcpy(FileOpen,GetString1(3008));
	strcpy(FileOpen3,strbuffer);

	//Bitmaps
	bmpHero=LoadBitmap(inst,MAKEINTRESOURCE(204));
	bmpArrow=LoadBitmap(inst,MAKEINTRESOURCE(205));

	//Settings
	RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Score_Under\\PKSV-UI",0,KEY_READ,&regkey);
	DataSize=sizeof(Data);
	Data=0;
	RegQueryValueEx(regkey,"CollapseBeforeEnd",NULL,(LPDWORD)&Nothing,(LPBYTE)&Data,(LPDWORD)&DataSize);
	if (Data==0)
		collapseatend=1;
	else
		collapseatend=0;
	Data=0;
	RegQueryValueEx(regkey,"CollapseEmptyLines",NULL,(LPDWORD)&Nothing,(LPBYTE)&Data,(LPDWORD)&DataSize);
	if (Data!=0)
		collapselines=1;
	else
		collapselines=0;
	Data=0;
	RegQueryValueEx(regkey,"AutoAutoComplete",NULL,(LPDWORD)&Nothing,(LPBYTE)&Data,(LPDWORD)&DataSize);
	if (Data!=0)
		autoautocomplete=1;
	else
		autoautocomplete=0;
	RegCloseKey(regkey);

	LoadRegSettingStr(NULL,"Version");
	if (strcmp(GreatBuffer,INTERNAL_VERSION))
	{
		MessageBox(MainWnd,GetString1(3010),GetString2(3009),0x40);
		SaveRegSettingStr(NULL,"Version",INTERNAL_VERSION);
	}

	//Create it!
	MainWnd = CreateWindow("Score_Under_PKSVUI","",WS_OVERLAPPEDWINDOW|WS_POPUPWINDOW,
	                       (GetSystemMetrics(SM_CXFULLSCREEN)/2)-((WIDTH+GetSystemMetrics(SM_CXFIXEDFRAME)*2)/2),
	                       (GetSystemMetrics(SM_CYFULLSCREEN)/2)-((HEIGHT+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CXFIXEDFRAME))/2),
	                       WinSize.right+GetSystemMetrics(SM_CXFRAME),
	                       WinSize.bottom+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYFRAME),
	                       HWND_DESKTOP, NULL, hInstance, NULL);
	StatusBar=CreateWindow(STATUSCLASSNAME,"Status Bar",WS_CHILD|WS_VISIBLE,0,0,0,0,MainWnd,NULL,hInstance,NULL);
#ifdef SCRAP
	Text=CreateWindowEx(WS_EX_CLIENTEDGE,"Scintilla","",WS_CHILD|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_NOHIDESEL,0,24,WIDTH-32,HEIGHT-67,MainWnd,NULL,hInstance,NULL);
	ScrapPunk=CreateWindow("Button","",WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,WIDTH-32,24,32,32,MainWnd,NULL,hInstance,NULL);
#else
	Text=CreateWindowEx(WS_EX_CLIENTEDGE,"Scintilla","",WS_CHILD|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_NOHIDESEL,0,24,WIDTH,HEIGHT-67,MainWnd,NULL,hInstance,NULL);
#endif
	InitCommonControls();
	dc=GetDC(MainWnd);
	if(GetDeviceCaps(dc,BITSPIXEL)<32)
		ToolBar=CreateToolbarEx(MainWnd,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_ADJUSTABLE|CCS_NODIVIDER|CCS_NOMOVEY|TBSTYLE_TOOLTIPS,5000,20,inst,6,ToolButtons,sizeof(ToolButtons)/sizeof(TBBUTTON),16,16,16,16,sizeof(TBBUTTON));
	else
		ToolBar=CreateToolbarEx(MainWnd,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_ADJUSTABLE|CCS_NODIVIDER|CCS_NOMOVEY|TBSTYLE_TOOLTIPS,5000,20,inst,5,ToolButtons,sizeof(ToolButtons)/sizeof(TBBUTTON),16,16,16,16,sizeof(TBBUTTON));
	DragAcceptFiles(MainWnd,1);
	if (MainWnd==NULL)
	{
		MessageBox(NULL,GetString1(3011),GetString2(3001),0x10);
		return 0;
	}
	if (Text==NULL)
	{
		MessageBox(NULL,GetString1(3012),GetString2(3001),0x10);
		return 0;
	}

	FindReplaceWin=CreateDialog(inst,MAKEINTRESOURCE(100),MainWnd,(DLGPROC)&FindReplaceFunc);
	if (FindReplaceWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	GotoWin=CreateDialog(inst,MAKEINTRESOURCE(101),MainWnd,(DLGPROC)&GotoFunc);
	if (GotoWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	FFSWin=CreateDialog(inst,MAKEINTRESOURCE(102),MainWnd,(DLGPROC)&FFSFunc);
	if (FFSWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	SetDlgItemText(FFSWin,3,"0x740000");
	SetDlgItemText(FFSWin,4,"0x100");
	SetDlgItemText(FFSWin,5,"N/A");
	DecWin=CreateDialog(inst,MAKEINTRESOURCE(103),MainWnd,(DLGPROC)&DecFunc);
	if (DecWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	CheckDlgButton(DecWin,8,1);
	LoadRegSettingStr("Decompiler","Offset");
	if (!*GreatBuffer)strcpy(GreatBuffer,"1BDF13");
	SetDlgItemText(DecWin,3,GreatBuffer);
	GenWin=CreateDialog(inst,MAKEINTRESOURCE(104),MainWnd,(DLGPROC)&GenFunc);
	if (GenWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	InputWin=CreateDialog(inst,MAKEINTRESOURCE(105),MainWnd,(DLGPROC)&InputFunc);
	if (InputWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	Input2Win=CreateDialog(inst,MAKEINTRESOURCE(3070),MainWnd,(DLGPROC)&Input2Func);
	if (Input2Win==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	NoteWin=CreateDialog(inst,MAKEINTRESOURCE(106),MainWnd,(DLGPROC)&NoteFunc);
	if (NoteWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	SetDlgItemText(NoteWin,2,"0");
	SetDlgItemText(NoteWin,3,"00");
	SetDlgItemText(NoteWin,4,"4000");
	HexWin=CreateDialog(inst,MAKEINTRESOURCE(107),MainWnd,(DLGPROC)&HexFunc);
	if (HexWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	SetDlgItemText(HexWin,1,"F");
	SetDlgItemText(HexWin,2,"15");
	MovePlanWin=CreateDialog(inst,MAKEINTRESOURCE(108),MainWnd,(DLGPROC)&MovePlanFunc);
	if (MovePlanWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	ScriptSearchWin=CreateDialog(inst,MAKEINTRESOURCE(109),MainWnd,(DLGPROC)&ScriptSearchFunc);
	if (ScriptSearchWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	FlagsWin=CreateDialog(inst,MAKEINTRESOURCE(3071),MainWnd,(DLGPROC)&FlagsFunc);
	if (FlagsWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	LockWin=CreateDialog(inst,MAKEINTRESOURCE(3072),MainWnd,(DLGPROC)&LockFunc);
	if (LockWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
#ifdef SCRAP
	PickupWin=CreateDialog(inst,MAKEINTRESOURCE(3073),MainWnd,(DLGPROC)&PickupFunc);
	if (PickupWin==NULL)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,tch,sizeof(tch),NULL);
		MessageBox(MainWnd,tch,GetString2(3001),0x10);
	}
	tab.mask=TCIF_TEXT;
	tab.pszText="Pickup Editor";
	SendMessage(GetDlgItem(PickupWin,1000),TCM_INSERTITEM,0,(LPARAM)&tab);
	tab.mask=TCIF_TEXT;
	tab.pszText="Item Editor";
	SendMessage(GetDlgItem(PickupWin,1000),TCM_INSERTITEM,1,(LPARAM)&tab);
	SPIcon=LoadBitmap(inst,MAKEINTRESOURCE(4));
	SendMessage(PickupWin,WM_SETICON,0,(LPARAM)LoadIcon(inst,MAKEINTRESOURCE(5)));
#endif
	CheckDlgButton(FFSWin,6,BST_CHECKED);
	CheckDlgButton(ScriptSearchWin,11,BST_CHECKED);
	CheckDlgButton(MovePlanWin,103,BST_CHECKED);
	CheckDlgButton(LockWin,3,BST_CHECKED);
	LoadRegSettingStr("Decompiler","Dynamic Offset");
	if (!*GreatBuffer)strcpy(GreatBuffer,"0x0740000");
	SetDlgItemText(DecWin,19,GreatBuffer);
	CheckDlgButton(DecWin,18,LoadRegSetting("Decompiler","Use Dynamic"));

	LoadRegSettingStr("Notes","Text");
	if (*GreatBuffer)
	{
		SetDlgItemText(NoteWin,1,GreatBuffer);
	}

	SendMessage(StatusBar,WM_SETFONT,(LPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	StatusSize[0]=-1;
	SendMessage(StatusBar,SB_SETPARTS,1,(LPARAM)StatusSize);
	SendMessage(StatusBar,SB_SETTEXT,0,(LPARAM)GetString1(3014));

	SendEditor(SCI_CLEARDOCUMENTSTYLE,0,0);
	EnumFontFamilies(dc,NULL,(void*)FontEnumerator,0);
	ReleaseDC(MainWnd,dc);
	if (!foundfont)
	{
		SendEditor(SCI_STYLESETFONT,STYLE_DEFAULT,(LPARAM)"Courier New");
	}
	else
	{
		SendEditor(SCI_STYLESETFONT,STYLE_DEFAULT,(LPARAM)"Lucida Console");
	}
	SendEditor(SCI_SETLEXER,SCLEX_CONTAINER,0);
	SendEditor(SCI_STYLESETFORE,STYLE_DEFAULT,0);
	SendEditor(SCI_STYLESETBACK,STYLE_DEFAULT,0xffffff); //In BGR format
	SendEditor(SCI_STYLESETSIZE,STYLE_DEFAULT,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_DEFAULT,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_DEFAULT,0);
	SendEditor(SCI_STYLECLEARALL,0,0);
	SendEditor(SCI_STYLESETFORE,STYLE_NORMAL,0);
	SendEditor(SCI_STYLESETBACK,STYLE_NORMAL,0xffffff); //In BGR format
	SendEditor(SCI_STYLESETSIZE,STYLE_NORMAL,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_NORMAL,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_NORMAL,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_NORMAL,0);
	SendEditor(SCI_STYLESETFORE,STYLE_IMPORTANT,0xc00000);
	SendEditor(SCI_STYLESETSIZE,STYLE_IMPORTANT,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_IMPORTANT,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_IMPORTANT,1);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_IMPORTANT,0);
	SendEditor(SCI_STYLESETFORE,STYLE_SPECIAL,0x0080ff);
	SendEditor(SCI_STYLESETSIZE,STYLE_SPECIAL,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_SPECIAL,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_SPECIAL,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_SPECIAL,0);
	SendEditor(SCI_STYLESETFORE,STYLE_COMMAND,0xff8000);
	SendEditor(SCI_STYLESETSIZE,STYLE_COMMAND,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_COMMAND,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_COMMAND,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_COMMAND,0);
	SendEditor(SCI_STYLESETFORE,STYLE_ARGUMENT,0x0000ff);
	SendEditor(SCI_STYLESETSIZE,STYLE_ARGUMENT,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_ARGUMENT,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_ARGUMENT,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_ARGUMENT,0);
	SendEditor(SCI_STYLESETFORE,STYLE_MESSAGE,0x008000);
	SendEditor(SCI_STYLESETSIZE,STYLE_MESSAGE,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_MESSAGE,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_MESSAGE,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_MESSAGE,0);
	SendEditor(SCI_STYLESETFORE,STYLE_LABEL,0x000000);
	SendEditor(SCI_STYLESETSIZE,STYLE_LABEL,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_LABEL,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_LABEL,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_LABEL,1);
	SendEditor(SCI_STYLESETFORE,STYLE_LINENUMBER,0x0);
	SendEditor(SCI_STYLESETSIZE,STYLE_LINENUMBER,10);
	SendEditor(SCI_STYLESETITALIC,STYLE_LINENUMBER,0);
	SendEditor(SCI_STYLESETUNDERLINE,STYLE_LINENUMBER,0);
	SendEditor(SCI_STYLESETBOLD,STYLE_LINENUMBER,0);

	SendEditor(SCI_SETPROPERTY,(WPARAM)"fold",(LPARAM)"1");
	SendEditor(SCI_SETPROPERTY,(WPARAM)"fold.compact",(LPARAM)"0");
	SendEditor(SCI_SETPROPERTY,(WPARAM)"fold.comment", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY,(WPARAM)"fold.preprocessor", (LPARAM)"1");
	SendEditor(SCI_SETMARGINWIDTHN,1,0);
	SendEditor(SCI_SETMARGINTYPEN,1,SC_MARGIN_SYMBOL);
	SendEditor(SCI_SETMARGINMASKN,1,SC_MASK_FOLDERS);
	SendEditor(SCI_SETMARGINWIDTHN,1,20);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
	SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDEROPEN,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDEROPEN,0);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDER,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDER,0);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDERSUB,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDERSUB,0);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDERTAIL,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDERTAIL,0);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDEREND,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDEREND,0);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDEROPENMID,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDEROPENMID,0);
	SendEditor(SCI_MARKERSETFORE,SC_MARKNUM_FOLDERMIDTAIL,0xffffff);
	SendEditor(SCI_MARKERSETBACK,SC_MARKNUM_FOLDERMIDTAIL,0);
	SendEditor(SCI_SETFOLDFLAGS, 16, 0);
	SendEditor(SCI_SETMARGINSENSITIVEN, 1, 1);

	SendEditor(SCI_SETSTYLEBITS,0x0D,0);
	SendEditor(SCI_AUTOCSETMAXHEIGHT,15,0);
	SendEditor(SCI_AUTOCSETSEPARATOR,'|',0);
	SendEditor(SCI_AUTOCSETTYPESEPARATOR,';',0);
	SendEditor(SCI_REGISTERIMAGE,0,(LPARAM)unknown_xpm);
	SendEditor(SCI_REGISTERIMAGE,1,(LPARAM)Gary_xpm);
	SendEditor(SCI_REGISTERIMAGE,2,(LPARAM)sign_xpm);
	SendEditor(SCI_REGISTERIMAGE,3,(LPARAM)poke_xpm);
	SendEditor(SCI_REGISTERIMAGE,4,(LPARAM)pokeball_xpm);
	SendEditor(SCI_REGISTERIMAGE,5,(LPARAM)furniture_xpm);
	SendEditor(SCI_REGISTERIMAGE,6,(LPARAM)flag_xpm);
	SendEditor(SCI_REGISTERIMAGE,7,(LPARAM)hero_xpm);
	SendEditor(SCI_REGISTERIMAGE,8,(LPARAM)special_xpm);
	SendEditor(SCI_REGISTERIMAGE,9,(LPARAM)bin_xpm);
	SendEditor(SCI_REGISTERIMAGE,10,(LPARAM)music_xpm);
	SendEditor(SCI_REGISTERIMAGE,11,(LPARAM)money_xpm);
	SendEditor(SCI_REGISTERIMAGE,12,(LPARAM)rsmove_xpm);
	SendEditor(SCI_REGISTERIMAGE,13,(LPARAM)frmove_xpm);
	SendEditor(SCI_AUTOCSETIGNORECASE,1,0);
	SendEditor(SCI_AUTOCSETAUTOHIDE,0,0);
	SendEditor(SCI_AUTOCSETFILLUPS,0,(LPARAM)"\n\r\t");
	SendEditor(SCI_AUTOCSTOPS,0,(LPARAM)" '");

	SendMessage(MainWnd,WM_COMMAND,ID_NEW,0);
	x=GetCommandLine();
	if (x[0]=='"')
	{
		x++;
		while (x[0]!='"'&&x[0]!=0) {
			x++;
		}
	}
	else
	{
		while (x[0]!=' '&&x[0]!=0) {
			x++;
		}
	}
	if (x[0]!=0)
	{
		x++;
		while (x[0]==' ') {
			x++;
		}
		y=0;
		if (x[0]!=0)
		{
			Nothing=0;
			if (x[0]=='"') {
				x++;
				Data='"';
			}
			else {
				Data=' ';
			}
			while (x[Nothing]!=Data&&x[Nothing]!=0) {
				Nothing++;
			}
			y=x;
			if (x[0]!=0)
			{
				x=x+Nothing+1;
			}
		}
		while (x[0]==' '||x[0]=='"') x++;
		if (x[0]!=0)
		{
			GoToPlace=-1;
			sscanf(x,"%X",&GoToPlace);
			if (GoToPlace==-1)
			{
				sscanf(x,"0x%X",&GoToPlace);
				if (GoToPlace==-1)
				{
					sscanf(x,"$%X",&GoToPlace);
				}
			}
			if (GoToPlace!=-1)IsRom=1;
		}
		if (y!=0&&!IsRom)
		{
			x=y;
			x[Nothing]=0;
			strcpy(FileOpen,x);
			strcpy(FileOpen2,FileOpen);
			strcpy(FileOpen3,FileOpen);
			strcpy(GreatBuffer,FileOpen);
			GetDirFromGreatBuffer();
			strcpy(DefScriptPath,GreatBuffer);
			strcat(FileOpen2," - Score_Under's PKSV-UI");
			SetWindowText(MainWnd,FileOpen2);
			fileM=CreateFile(FileOpen,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (GetLastError()!=0)
			{
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,(char*)&ptr,2000,NULL);
				MessageBox(MainWnd,ptr,ptr,0x10);
				LocalFree(ptr);
				return 0;
			}
			fsize=SetFilePointer(fileM,0,NULL,FILE_END);
			ptr=GlobalAlloc(GPTR,fsize+1);
			SetFilePointer(fileM,0,NULL,FILE_BEGIN);
			ReadFile(fileM,ptr,fsize,&read,NULL);
			if (read!=fsize)
			{
				MessageBox(MainWnd,GetString1(3015),GetString2(3001),0x10);
			}
			SendEditor(SCI_CANCEL,0,0);
			SendEditor(SCI_SETUNDOCOLLECTION,0,0);
			SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
			SendEditor(SCI_CLEARALL,0,0);
			SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
			SendEditor(SCI_SETSCROLLWIDTH,2048,0);
			SendEditor(SCI_ADDTEXT,fsize,(LPARAM)ptr);
			SendEditor(SCI_SETUNDOCOLLECTION,1,0);
			SendEditor(EM_EMPTYUNDOBUFFER,0,0);
			SendEditor(SCI_SETSAVEPOINT,0,0);
			SendEditor(SCI_GOTOPOS,0,0);
			SendEditor(SCI_CHOOSECARETX,0,0);
			GlobalFree(ptr);
			CloseHandle(fileM);
			SetFocus(Text);
			SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
			SendEditor(SCI_SETSEL,0,0);
		}
		else if (IsRom)
		{
			x=y;
			x[Nothing]=0;
			strcpy(RomOpen,x);
			strcpy(RomOpen2,RomOpen);
			strcpy(GreatBuffer,RomOpen);
			GetDirFromGreatBuffer();
			strcpy(DefRomPath,GreatBuffer);
			fileM=CreateFile(RomOpen,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (fileM!=INVALID_HANDLE_VALUE)
			{
				SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
				ReadFile(fileM,&modechar,4,&read,NULL);
				SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
				ReadFile(fileM,&determineMode,3,&read,NULL);
				if ((determineMode[0]=='A'&&determineMode[1]=='A')||(determineMode[0]=='S'&&determineMode[1]=='M'))
				{//Detect GSC and Prism
					pokemon_version=GBC;
				}
				else
				{
					SetFilePointer(fileM,0xAC,NULL,FILE_BEGIN);
					ReadFile(fileM,&modechar,4,&read,NULL);
					pokemon_version=GBA;
				}
				CloseHandle(fileM);
			}
			else
			{
				*RomOpen=0;
				*RomOpen2=0;
			}
			sprintf(GreatBuffer,"0x%X",GoToPlace);
			SetDlgItemText(DecWin,3,GreatBuffer);
			DecFunc(NULL,WM_COMMAND,1,0);
		}
	}
	if (!IsRom)
	{
		//OPENROM
		LoadRegSettingStr(NULL,"ROMFile");
		strcpy(RomOpen,GreatBuffer);
		strcpy(RomOpen2,RomOpen);
		strcpy(GreatBuffer,RomOpen);
		GetDirFromGreatBuffer();
		strcpy(DefRomPath,GreatBuffer);
		fileM=CreateFile(RomOpen,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (fileM!=INVALID_HANDLE_VALUE)
		{
			SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
			ReadFile(fileM,&modechar,4,&read,NULL);
			SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
			ReadFile(fileM,&determineMode,3,&read,NULL);
			if ((determineMode[0]=='A'&&determineMode[1]=='A')||(determineMode[0]=='S'&&determineMode[1]=='M'))
			{//Detect GSC and Prism
				pokemon_version=GBC;
			}
			else
			{
				SetFilePointer(fileM,0xAC,NULL,FILE_BEGIN);
				ReadFile(fileM,&modechar,4,&read,NULL);
				pokemon_version=GBA;
			}
			CloseHandle(fileM);
		}
		else
		{
			*RomOpen=0;
			*RomOpen2=0;
		}
	}
	//Menu items
	mii.cbSize=sizeof(mii);
	mii.fMask=MIIM_STATE;
	if (collapselines)
		mii.fState=MFS_CHECKED;
	else
		mii.fState=MFS_UNCHECKED;
	SetMenuItemInfo(GetMenu(MainWnd),1021,0,&mii);
	if (!collapseatend)
	{
		mii.fState=MFS_CHECKED;
		SetMenuItemInfo(GetMenu(MainWnd),1020,0,&mii);
		mii.fState=MFS_UNCHECKED|MFS_GRAYED;
		SetMenuItemInfo(GetMenu(MainWnd),1021,0,&mii);
		collapselines=0;
	}
	else
	{
		mii.fState=MFS_UNCHECKED;
		SetMenuItemInfo(GetMenu(MainWnd),1020,0,&mii);
		mii.fState=MFS_ENABLED;
		if (collapselines) {
			mii.fState|=MFS_CHECKED;
		}
		SetMenuItemInfo(GetMenu(MainWnd),1021,0,&mii);
	}
	if (autoautocomplete)
		mii.fState=MFS_CHECKED;
	else
		mii.fState=MFS_UNCHECKED;
	SetMenuItemInfo(GetMenu(MainWnd),1112,0,&mii);

	hAccel = LoadAccelerators(hInstance,MAKEINTRESOURCE(10));
#ifndef DOES_NOT_UPDATE
#ifndef ZH
	if(LoadRegSettingDef(NULL,"AutoUpdate",1))
		CreateThread(NULL,0,UpdateMe,NULL,0,(LPVOID)&Nothing);
#endif
#endif
	UpdateScintLine();
	UpdateWindow(MainWnd);
	ShowWindow(MainWnd,SW_SHOW);
	while (GetMessage(&msg,NULL,0,0))
	{
		if (!TranslateAccelerator(MainWnd,hAccel,&msg))
		{
			if (!IsDialogMessage(FFSWin,&msg)
			        &&!IsDialogMessage(FindReplaceWin,&msg)
			        &&!IsDialogMessage(GotoWin,&msg)
			        &&!IsDialogMessage(NoteWin,&msg)
			        &&!IsDialogMessage(MovePlanWin,&msg)
			        &&!IsDialogMessage(HexWin,&msg)
			        &&!IsDialogMessage(InputWin,&msg)
			        &&!IsDialogMessage(Input2Win,&msg)
			        &&!IsDialogMessage(GenWin,&msg)
			        &&!IsDialogMessage(DecWin,&msg)
			        &&!IsDialogMessage(ScriptSearchWin,&msg)
			        &&!IsDialogMessage(LockWin,&msg)
			        &&!IsDialogMessage(FlagsWin,&msg)
#ifdef SCRAP
			        &&!IsDialogMessage(PickupWin,&msg)
#endif
			   )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	FreeLibrary(ScintLib);


	i=(unsigned int)SendMessage(GetDlgItem(NoteWin,1),WM_GETTEXTLENGTH,0,0);
	ptr=GlobalAlloc(GPTR,i+1);
	GetDlgItemText(NoteWin,1,ptr,i+1);
	SaveRegSettingStr("Notes","Text",ptr);
	return (int)msg.wParam;
}

void SaveFlag()
{
	char buf[5];

	if (LastFlag==-1)return;
	sprintf(buf,"%04X",FlagArr[LastFlag]);
	GetDlgItemText(FlagsWin,5,GreatBuffer,sizeof(GreatBuffer));
	SaveRegSettingStr("Flags\\ShortDesc",buf,GreatBuffer);
	GetDlgItemText(FlagsWin,6,MegaBuffer,sizeof(MegaBuffer));
	SaveRegSettingStr("Flags\\LongDesc",buf,MegaBuffer);
	SendMessage(GetDlgItem(FlagsWin,2),LB_DELETESTRING,LastFlag,0);
	sprintf(MegaBuffer,"0x%s - %s",buf,GreatBuffer);
	SendMessage(GetDlgItem(FlagsWin,2),LB_ADDSTRING,0,(LPARAM)MegaBuffer);
}

void exiting()
{
	unsigned int i;
	char*ptr;
	i=(unsigned int)SendMessage(GetDlgItem(NoteWin,1),WM_GETTEXTLENGTH,0,0);
	ptr=GlobalAlloc(GPTR,i+1);
	GetDlgItemText(NoteWin,1,ptr,i+1);
	SaveRegSettingStr("Notes","Text",ptr);
}

struct SCNotification* scn;
int TCompile(char*a)
{
	char*b;
	b=malloc(strlen(RomOpen2)+1);
	strcpy(b,RomOpen2);
	compile(b,a);
	free(b);
	GlobalFree(a);
	return 0;
}
int TDebugCompile(char*a)
{
	char*b;
	b=malloc(strlen(RomOpen2)+1);
	strcpy(b,RomOpen2);
	DebugCompile(b,a);
	free(b);
	GlobalFree(a);
	return 0;
}
#ifdef SCRAP
unsigned char PickupBuf[0x40];
#endif
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	OPENFILENAME ofn;
	HANDLE fileM;
	void*ptr;
	char*lbuf;
	MENUITEMINFO mii;
	char buf[1024];
	LPNMHDR nmh=(LPNMHDR)lParam;
	unsigned long fsize;
	char determineMode[4];
	DWORD read;
	HKEY regkey;
	HDC dc;
	unsigned int sges;
	LPTOOLTIPTEXT text;
	//OSVERSIONINFO ver;
	//BLENDFUNCTION blend;
	int line,start,end,len,ostart,i,dontsetlevel,start2;
#define SciCharAt(x) (char)SendEditor(SCI_GETCHARAT,x,0)
#define StrDefault GetString1(3016)
	switch (msg)
	{
#ifdef SCRAP
	case WM_DRAWITEM:
		if (((LPDRAWITEMSTRUCT)lParam)->hwndItem==ScrapPunk)
		{
			dc=CreateCompatibleDC(((LPDRAWITEMSTRUCT)lParam)->hDC);
			SelectObject(dc,(HBRUSH)(COLOR_WINDOW+11));
			Rectangle(((LPDRAWITEMSTRUCT)lParam)->hDC,0,0,32,32);
			SelectObject(dc,SPIcon);/*
        GetVersionEx(&ver);
		if(ver.dwMajorVersion<=4)
		{*/
			BitBlt(((LPDRAWITEMSTRUCT)lParam)->hDC,
			       0,0,32,32,dc,0,0,SRCCOPY);/*
		}
		else
		{
		  blend.AlphaFormat=AC_SRC_ALPHA;
		  blend.BlendFlags=0;
		  blend.BlendOp=AC_SRC_OVER;
		  blend.SourceConstantAlpha=255;
		  AlphaBlend(((LPDRAWITEMSTRUCT)lParam)->hDC,
			0,0,32,32,dc,0,0,32,32,blend);
		}*/
			DeleteDC(dc);
		}
		return 1;
#endif
	case WM_NOTIFY:
		if (hwnd==MainWnd)
		{
			switch (nmh->code)
			{
			case TTN_NEEDTEXT:
				text=(LPTOOLTIPTEXT)lParam;
				text->hinst=inst;
				switch (text->hdr.idFrom)
				{
				case ID_NEW:
					text->lpszText="New";
					break;
				case ID_OPEN:
					text->lpszText="Open";
					break;
				case ID_SAVE:
					text->lpszText="Save";
					break;
				case ID_SAVEAS:
					text->lpszText="Save As";
					break;
				case 1015:
					text->lpszText="Undo";
					break;
				case 1016:
					text->lpszText="Redo";
					break;
				case 1008:
					text->lpszText="Cut";
					break;
				case 1009:
					text->lpszText="Copy";
					break;
				case 1010:
					text->lpszText="Paste";
					break;
				case 1012:
					text->lpszText="Find/Replace";
					break;
				case 1032:
					text->lpszText="Tools (Calculator and Notes)";
					break;
				case 1006:
					text->lpszText="Open ROM";
					break;
				case 1005:
					text->lpszText="Find Free Space";
					break;
				case 1014:
					text->lpszText="Decompile";
					break;
				case 1004:
					text->lpszText="Compile";
					break;
				case 1030:
					text->lpszText="Play Game";
					break;
				case 1111:
					text->lpszText="Activate Autocomplete";
					break;
				case 1013:
					text->lpszText="Script Generator";
					break;
				case 1100:
					text->lpszText="About...";
					break;
				case 1202:
					text->lpszText="Debug";
					break;
				}
				return DefWindowProc(hwnd, msg, wParam, lParam);
				break;
			case TBN_QUERYINSERT:
			case TBN_QUERYDELETE:
				return 1;
			case TBN_GETBUTTONINFO:
				if (((TBNOTIFY*)nmh)->iItem>=0&&((TBNOTIFY*)nmh)->iItem<(sizeof(ToolButtons)/sizeof(TBBUTTON)))
				{
					((TBNOTIFY*)nmh)->tbButton=ToolButtons[((TBNOTIFY*)nmh)->iItem];
					return 1;
				}
				return 0;
			case SCN_MARGINCLICK:
				scn=(struct SCNotification*)nmh;
				SendEditor(SCI_TOGGLEFOLD,SendEditor(SCI_LINEFROMPOSITION,scn->position,0),0);
				break;
			case SCN_CHARADDED:
				if (autoautocomplete&&((((struct SCNotification*)nmh)->ch)!='\n')&&((((struct SCNotification*)nmh)->ch)!='\r'))
				{
					len=0;
					i=(int)SendEditor(SCI_GETSELECTIONSTART,0,0);
					start=(int)SendEditor(SCI_POSITIONFROMLINE,SendEditor(SCI_LINEFROMPOSITION,i,0),0);
					while (SciCharAt(start)==' ')start++;
					while (i>start)
					{
						if (SciCharAt(i)=='\''||(SciCharAt(i)=='/'&&SciCharAt(i+1)=='/')) {
							break;
						}
						len++;
						i--;
					}
					if (SciCharAt(i)!='\''&&(SciCharAt(i)!='/'||SciCharAt(i+1)!='/'))
						SendMessage(MainWnd,WM_COMMAND,1111,1111);
				}
				break;
				/*
			case SCN_UPDATEUI:
				//do something for command list
				break;*/
			case SCN_MODIFIED:
			case SCN_ZOOM:
				UpdateScintLine();
				break;
			case SCN_SAVEPOINTREACHED:
				NotSaved = FALSE;
				lbuf=strrchr(FileOpen3,'\\')+1;
				if((ptr=strrchr(FileOpen3,'/'))>(void*)lbuf)
					lbuf=ptr+1;
				if(lbuf<(char*)2)lbuf=FileOpen3;
				strcpy(FileOpen2,lbuf);
				strcat(FileOpen2," - Score_Under's PKSV-UI");
				SetWindowText(MainWnd,FileOpen2);
				break;
			case SCN_SAVEPOINTLEFT:
				NotSaved = TRUE;
				strcpy(FileOpen2,"* ");
				lbuf=strrchr(FileOpen3,'\\')+1;
				if((ptr=strrchr(FileOpen3,'/'))>(void*)lbuf)
					lbuf=ptr+1;
				if(lbuf<(char*)2)lbuf=FileOpen3;
				strcat(FileOpen2,lbuf);
				strcat(FileOpen2," - Score_Under's PKSV-UI");
				SetWindowText(MainWnd,FileOpen2);
				break;
			case SCN_STYLENEEDED:
				sges=(unsigned int)SendEditor(SCI_GETENDSTYLED,0,0)-1;
				while (sges!=SendEditor(SCI_GETENDSTYLED,0,0))
				{
					sges=(unsigned int)SendEditor(SCI_GETENDSTYLED,0,0);
					dontsetlevel=0;
					line=(int)SendEditor(SCI_LINEFROMPOSITION,SendMessage(Text,SCI_GETENDSTYLED,0,0),0);
					start=(int)SendEditor(SCI_POSITIONFROMLINE,line,0);
					start2=0;
					ostart=start;
					end=((struct SCNotification*)nmh)->position;
					len=(int)SendEditor(SCI_LINELENGTH,line,0);
					i=len;
					if (line!=0)
					{
						i=(int)SendEditor(SCI_LINELENGTH,line-1,0);
						if (i<len)i=len;
					}
					lbuf=GlobalAlloc(GPTR,i+3); // \r, \n, \0
					while (start<(ostart+len)&&(SciCharAt(start)==' '||SciCharAt(start)=='\t'))
					{
						start++;
					}
					SendEditor(SCI_STARTSTYLING,start,0x1f);
					if (SendEditor(SCI_GETCHARAT,start,0)=='#')
					{
						SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_SPECIAL);
						if ((cToLower(SciCharAt(start+1))=='o'&&cToLower(SciCharAt(start+2))=='r'&&cToLower(SciCharAt(start+3))=='g'&&cToLower(SciCharAt(start+4))==' ')
						        ||(cToLower(SciCharAt(start+1))=='e'&&cToLower(SciCharAt(start+2))=='o'&&cToLower(SciCharAt(start+3))=='r'&&cToLower(SciCharAt(start+4))=='g'&&cToLower(SciCharAt(start+5))==' '))
						{
							SendEditor(SCI_SETFOLDLEVEL,line,SC_FOLDLEVELHEADERFLAG|1024);
						}
						else
						{
							start2=(int)SendEditor(SCI_POSITIONFROMLINE,line-1,0);
							while (SciCharAt(start2)==' ') {
								start2++;
							}
							i=0;
							while ((i+start2)<ostart&&SciCharAt(start2+i)!=' '&&SciCharAt(start2+i)!='\''&&(SciCharAt(start2+i)!='/'||SciCharAt(start2+i+1)!='/')&&SciCharAt(start2+i)!='\n'&&SciCharAt(start2+i)!='\r')
							{
								lbuf[i]=SciCharAt(start2+i);
								i++;
							}
							lbuf[i]=0;
							ToLower(lbuf);
							if (!strcmp(lbuf,"end")||!strcmp(lbuf,"jump")||!strcmp(lbuf,"jumpstd")||!strcmp(lbuf,"goto")||!strcmp(lbuf,"return")||!strcmp(lbuf,"m")||!strcmp(lbuf,"="))
							{
								SendEditor(SCI_SETFOLDLEVEL,line,1024); //Don't fold blank lines at end.
							}
							else if (line==0||SendEditor(SCI_GETFOLDLEVEL,line-1,0)==1024)
							{
								SendEditor(SCI_SETFOLDLEVEL,line,1024);
							}
							else
							{
								SendEditor(SCI_SETFOLDLEVEL,line,1025);
							}
						}
						goto commentstyle;
						GlobalFree(lbuf);
						break;
					}
					else if (SciCharAt(start)=='='||SciCharAt(start)=='.'||(cToLower(SciCharAt(start))=='m'&&SciCharAt(start+1)==' '))
					{
						SendEditor(SCI_SETSTYLING,1,STYLE_ARGUMENT);
						SendEditor(SCI_STARTSTYLING,start+1,0x1f);
						SendEditor(SCI_SETSTYLING,(len-1)-(start-ostart),STYLE_MESSAGE);
						while (SciCharAt(ostart+len-1)=='\\')
						{
							ostart+=len;
							len=SendEditor(SCI_LINELENGTH,SendEditor(SCI_LINEFROMPOSITION,ostart,0),0);
							SendEditor(SCI_SETSTYLING,len,STYLE_MESSAGE);
							SendEditor(SCI_SETFOLDLEVEL,SendEditor(SCI_LINEFROMPOSITION,ostart,0),1025);
						}
						SendEditor(SCI_SETFOLDLEVEL,line,1025);
						GlobalFree(lbuf);
						break;
					}
					else if (SciCharAt(start)==':')
					{
						SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_LABEL);
						SendEditor(SCI_SETFOLDLEVEL,line,1025);
						GlobalFree(lbuf);
						break;
					}
					else
					{
						if (line!=0&&!collapselines)
						{
							start2=(int)SendEditor(SCI_POSITIONFROMLINE,line-1,0);
							while (SciCharAt(start2)==' ') {
								start2++;
							}
							i=0;
							while ((i+start2)<ostart&&SciCharAt(start2+i)!=' '&&SciCharAt(start2+i)!='\''&&(SciCharAt(start2+i)!='/'||SciCharAt(start2+i+1)!='/')&&SciCharAt(start2+i)!='\n'&&SciCharAt(start2+i)!='\r')
							{
								lbuf[i]=SciCharAt(start2+i);
								i++;
							}
							lbuf[i]=0;
							ToLower(lbuf);
							if (!strcmp(lbuf,"end")||!strcmp(lbuf,"jump")||!strcmp(lbuf,"jumpstd")||!strcmp(lbuf,"goto")||!strcmp(lbuf,"return")||!strcmp(lbuf,"m")||!strcmp(lbuf,"="))
							{
								SendEditor(SCI_SETFOLDLEVEL,line,1024); //Don't fold blank lines at end.
								dontsetlevel=1;
							}
						}
						if (line==0)
						{
							SendEditor(SCI_SETFOLDLEVEL,line,1024);
							dontsetlevel=1;
						}
						else if (SciCharAt(start)=='\n'||SciCharAt(start)=='\r'||SciCharAt(start)=='\''||(SciCharAt(start)=='/'&&SciCharAt(start+1)=='/')||SendEditor(SCI_GETFOLDLEVEL,line-1,0)==1024)
						{
							if (SendEditor(SCI_GETFOLDLEVEL,line-1,0)==1024&&!dontsetlevel)
							{
								SendEditor(SCI_SETFOLDLEVEL,line,1024);
							}
							else if (!dontsetlevel)
							{
								SendEditor(SCI_SETFOLDLEVEL,line,1025);
							}

							dontsetlevel=1;
							SendEditor(SCI_STARTSTYLING,ostart+2,0x1f);
							//SendEditor(SCI_SETSTYLING,(ostart-start),STYLE_NORMAL);
						}
						i=0;
						while (SciCharAt(start+i)!=' '&&SciCharAt(start+i)!='\''&&(SciCharAt(start+i)!='/'||SciCharAt(start+i+1)!='/')&&SciCharAt(start+i)!='\n'&&SciCharAt(start+i)!='\r'&&(i+start)<(ostart+len))
						{
							lbuf[i]=SciCharAt(start+i);
							i++;
						}
						lbuf[i]=0;
						ToLower(lbuf);
						if (!strcmp(lbuf,"end")||!strcmp(lbuf,"jump")||!strcmp(lbuf,"jumpstd")||!strcmp(lbuf,"goto")||!strcmp(lbuf,"return"))
						{
							SendEditor(SCI_STARTSTYLING,start,0x1f);
							SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_IMPORTANT);
							if (!dontsetlevel)
							{
								if (collapseatend)
								{
									SendEditor(SCI_SETFOLDLEVEL,line,1025);
									SendEditor(SCI_SETFOLDLEVEL,line+1,1024);
								}
								else
								{
									SendEditor(SCI_SETFOLDLEVEL,line,1024);
								}
							}
						}
						else if (!strcmp(lbuf,"lock")||!strcmp(lbuf,"faceplayer")||!strcmp(lbuf,"release")||!strcmp(lbuf,"msgbox")||!strcmp(lbuf,"callstd")||!strcmp(lbuf,"callasm")||!strcmp(lbuf,"message")||!strcmp(lbuf,"showmsg")||!strcmp(lbuf,"closemsg")||!strcmp(lbuf,"releaseall")||!strcmp(lbuf,"lockall")||!strcmp(lbuf,"waitbutton")||!strcmp(lbuf,"if"))
						{
							SendEditor(SCI_STARTSTYLING,start,0x1f);
							SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_IMPORTANT);
							if (!dontsetlevel)SendEditor(SCI_SETFOLDLEVEL,line,1025);
						}
						else
						{
							SendEditor(SCI_STARTSTYLING,start,0x1f);
							SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_NORMAL);
							if (!dontsetlevel)SendEditor(SCI_SETFOLDLEVEL,line,1025);
						}
					}
					while (SciCharAt(start)!='\''&&(SciCharAt(start)!='/'||SciCharAt(start+1)!='/')&&SciCharAt(start)!='\n'&&SciCharAt(start)!='\r'&&SciCharAt(start)!=' '&&SciCharAt(start)!='\t'&&start<(ostart+len))
					{
						start++;
					}
					if (start!=(ostart+len))
					{
						SendEditor(SCI_STARTSTYLING,start,0x1f);
						SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_ARGUMENT);
					}
commentstyle:
					while (SciCharAt(start)!='\''&&(SciCharAt(start)!='/'||SciCharAt(start+1)!='/')&&SciCharAt(start)!='\n'&&SciCharAt(start)!='\r'&&start<(ostart+len))
					{
						start++;
					}
					if (SciCharAt(start)=='\''||(SciCharAt(start)=='/'&&SciCharAt(start+1)=='/'))
					{
						SendEditor(SCI_STARTSTYLING,start,0x1f);
						SendEditor(SCI_SETSTYLING,len-(start-ostart),STYLE_MESSAGE);
					}
					GlobalFree(lbuf);
				}
				break;
			}
		}
		break;
	case WM_DROPFILES:
		DragQueryFile((HANDLE)wParam,0,lbuf=alloca(DragQueryFile((HANDLE)wParam,0,NULL,0)+1),sizeof(GreatBuffer));
		fileM=CreateFile(lbuf,GENERIC_READ,
		                 FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		                 OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (fileM!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(fileM);
			i=strlen(lbuf);
			if (!strcmp(lbuf+i-4,".gba")||
			        !strcmp(lbuf+i-4,".nds")||
			        !strcmp(lbuf+i-4,".gbc")||
			        !strcmp(lbuf+i-5,".narc")||
			        !strcmp(lbuf+i-3,".gb"))
			{
				strcpy(RomOpen,lbuf);
				goto OpenRoms;
			}
			else
			{
				strcpy(FileOpen,lbuf);
				goto OpenMe;
			}
		}
		DragFinish((HANDLE)wParam);
		break;
	case WM_COMMAND:
#ifdef SCRAP
		if (lParam==(LPARAM)ScrapPunk)
		{
			goto ScrapPunkCode;
		}
		else
#endif
			switch (LOWORD(wParam))
			{
			case 2000:
				ShowWindow(GotoWin,SW_HIDE);
				ShowWindow(GenWin,SW_HIDE);
#ifdef SCRAP
				SendMessage(PickupWin,WM_COMMAND,2000,0);
#endif
				SendMessage(InputWin,WM_COMMAND,2000,0);
				EnableWindow(MainWnd,1);
				SetFocus(Text);
				SendEditor(SCI_AUTOCCANCEL,0,0);
				break;
			case 1007:
				if (NotSaved)
				{
					i=MessageBox(MainWnd,GetString1(3017),GetString2(3018), MB_YESNOCANCEL | MB_ICONQUESTION);
					if (i == IDYES)
					{
						SendMessage(MainWnd,WM_COMMAND,ID_SAVE,0);
						if (NotSaved)
						{
							i = IDCANCEL;
						}
					}
				}
				if (i != IDCANCEL)
				{
					exiting();
					ExitProcess(0);
				}
				break;
			case ID_NEW:
				i=0;
				if (NotSaved)
				{
					i=MessageBox(MainWnd,GetString1(3017),GetString2(3019), MB_YESNOCANCEL | MB_ICONQUESTION);
					if (i==6)
					{
						SendMessage(MainWnd,WM_COMMAND,ID_SAVE,0);
						if (NotSaved)
						{
							i=2;
						}
					}
				}
				if (i!=2)
				{
					strcat(GetString1(3008)," - Score_Under's PKSV-UI");
					SetWindowText(MainWnd,strbuffer);
					strcpy(FileOpen,GetString1(3008));
					*FileOpen2=0;
					strcpy(FileOpen3,GetString1(3008));
					SendEditor(SCI_CANCEL,0,0);
					SendEditor(SCI_SETUNDOCOLLECTION,0,0);
					SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_CLEARALL,0,0);
					SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
					SendEditor(SCI_SETSCROLLWIDTH,2048,0);
					SendEditor(SCI_ADDTEXT,strlen(StrDefault),(LPARAM)StrDefault);
					SendEditor(SCI_SETUNDOCOLLECTION,1,0);
					SendEditor(EM_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SendEditor(SCI_GOTOPOS,0,0);
					SendEditor(SCI_CHOOSECARETX,0,0);
					SetFocus(Text);
					SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
					SendEditor(SCI_SETSEL,0,0);
				}
				break;
			case ID_OPEN:
				if (NotSaved)
				{
					i=MessageBox(MainWnd,GetString1(3017),GetString2(3019), MB_YESNOCANCEL | MB_ICONQUESTION);
					if (i == IDYES)
					{
						SendMessage(MainWnd,WM_COMMAND,ID_SAVE,0);
						if (NotSaved)
						{
							i = IDCANCEL;
						}
					}
					if (i == IDCANCEL)
					{
						break;
					}
				}
				ZeroMemory(&ofn,sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = MainWnd;
				ofn.hInstance = GetModuleHandle(NULL);
				ofn.lpstrFilter = "PKSV Scripts\0*.pks\0PKSV Temp\0_?_.pks.tmp\0Rubikon/XSE script (*.rbc;*.rbh)\0*.rbc;*.rbh\0Text Files\0*.txt\0All Files\0*.*\0";
				ofn.lpstrFile = FileOpen;
				ofn.nMaxFile = sizeof(FileOpen);
				LoadRegSettingStr(NULL,"ScriptPath");
				strcpy(DefScriptPath,GreatBuffer);
				ofn.lpstrInitialDir = DefScriptPath;
				ofn.Flags = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
				ofn.lpstrTitle = GetString2(3021);
				if (*DefScriptPath!=0)
					SetCurrentDirectory(DefScriptPath);
				if (GetOpenFileName(&ofn))
				{
					if (cToLower(FileOpen[strlen(FileOpen)-3])=='g'&&cToLower(FileOpen[strlen(FileOpen)-2])=='b'&&cToLower(FileOpen[strlen(FileOpen)-1])=='a')
					{
						MessageBox(MainWnd,"Warning: This appears to be a GBA ROM, yet you have opened it with the .PKS script opener.\nChoose Rom Tools->Open ROM for the expected behaviour if this was a ROM.","Warning",0x30);
					}
OpenMe:
					strcpy(FileOpen2,FileOpen);
					strcpy(FileOpen3,FileOpen);
					strcpy(GreatBuffer,FileOpen);
					GetDirFromGreatBuffer();
					strcpy(DefScriptPath,GreatBuffer);
					SaveRegSettingStr(NULL,"ScriptPath",GreatBuffer);
					if (strcmp(GreatBuffer,FileOpen))
						strcpy(FileOpen2,(char*)(FileOpen+strlen(GreatBuffer)));
					strcat(FileOpen2," - Score_Under's PKSV-UI");
					SetWindowText(MainWnd,FileOpen2);
					fileM=CreateFile(FileOpen,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if (GetLastError()!=0)
					{
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,(char*)&ptr,2000,NULL);
						MessageBox(MainWnd,ptr,ptr,0x10);
						LocalFree(ptr);
						return 0;
					}
					fsize=SetFilePointer(fileM,0,NULL,FILE_END);
					ptr=GlobalAlloc(GPTR,fsize+1);
					SetFilePointer(fileM,0,NULL,FILE_BEGIN);
					ReadFile(fileM,ptr,fsize,&read,NULL);
					if (read!=fsize)
					{
						MessageBox(MainWnd,GetString1(3015),GetString2(3001),0x10);
					}
					SendEditor(SCI_CANCEL,0,0);
					SendEditor(SCI_SETUNDOCOLLECTION,0,0);
					SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_CLEARALL,0,0);
					SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
					SendEditor(SCI_SETSCROLLWIDTH,2048,0);
					SendEditor(SCI_ADDTEXT,fsize,(LPARAM)ptr);
					SendEditor(SCI_SETUNDOCOLLECTION,1,0);
					SendEditor(EM_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SendEditor(SCI_GOTOPOS,0,0);
					SendEditor(SCI_CHOOSECARETX,0,0);
					GlobalFree(ptr);
					CloseHandle(fileM);
					SetFocus(Text);
					SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
					SendEditor(SCI_SETSEL,0,0);
				}
				break;
			case ID_SAVEAS:
				ZeroMemory(&ofn,sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = MainWnd;
				ofn.hInstance = GetModuleHandle(NULL);
				ofn.lpstrFilter = "PKSV Scripts\0*.pks\0Text Files\0*.txt\0All Files\0*.*\0";
				ofn.lpstrFile = FileOpen;
				LoadRegSettingStr(NULL,"ScriptPath");
				strcpy(DefScriptPath,GreatBuffer);
				ofn.lpstrInitialDir = DefScriptPath;
				ofn.nMaxFile = sizeof(FileOpen);
				ofn.Flags = OFN_OVERWRITEPROMPT;
				ofn.lpstrTitle = GetString2(3050);
				if (GetSaveFileName(&ofn)&&strcmp(ofn.lpstrFile,""))
				{
					i=(int)strlen(FileOpen);
					strcpy(GreatBuffer,FileOpen);
					ToLower(GreatBuffer);
					if (GreatBuffer[i-1]!='s'||GreatBuffer[i-2]!='k'||GreatBuffer[i-3]!='p'||GreatBuffer[i-4]!='.')
						strcat(FileOpen,".pks");
					strcpy(FileOpen3,FileOpen);
					strcpy(FileOpen2,FileOpen);
					strcpy(GreatBuffer,FileOpen);
					GetDirFromGreatBuffer();
					SaveRegSettingStr(NULL,"ScriptPath",GreatBuffer);
					if (strcmp(GreatBuffer,FileOpen))
						strcpy(FileOpen2,(char*)(FileOpen+strlen(GreatBuffer)));
					strcat(FileOpen2," - Score_Under's PKSV-UI");
					SetWindowText(MainWnd,FileOpen2);
					/*Wait... what?
					strcpy(FileOpen2,FileOpen);
					strcat(FileOpen2,".bak");
					CopyFile(FileOpen2,FileOpen,0);
					*/
					FileSave=CreateFile(FileOpen3,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
					if (FileSave!=INVALID_HANDLE_VALUE)
					{
						i=(int)SendEditor(SCI_GETLENGTH,0,0);
						ptr=GlobalAlloc(GPTR,i+1);
						SendEditor(SCI_GETTEXT,i+1,(LPARAM)ptr);
						WriteFile(FileSave,ptr,i,&read,NULL);
						start=0;
						end=10;
						while ((int)read<i-start)
						{
							WriteFile(FileSave,(void*)(((char*)ptr)+start),i-start,&read,NULL);
							start+=read;
							end--;
							if (!end)
							{
								FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,buf,sizeof(buf),NULL);
								MessageBox(MainWnd,GetString1(3023),GetString2(3022),0x40);
								MessageBox(MainWnd,buf,GetString1(3022),0x10);
								break;
							}
						}
						GlobalFree(ptr);
						CloseHandle(FileSave);
					}
					else
					{
						FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,buf,sizeof(buf),NULL);
						MessageBox(MainWnd,GetString1(3023),GetString2(3024),0x40);
						MessageBox(MainWnd,buf,GetString1(3024),0x10);
					}
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SetFocus(Text);
				}
				break;
			case ID_SAVE:
				if (strcmp(FileOpen3,GetString1(3008)))
				{
					FileSave=CreateFile(FileOpen3,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
					if (FileSave!=INVALID_HANDLE_VALUE)
					{
						i=(int)SendEditor(SCI_GETLENGTH,0,0);
						ptr=GlobalAlloc(GPTR,i+1);
						SendEditor(SCI_GETTEXT,i+1,(LPARAM)ptr);
						WriteFile(FileSave,ptr,i,&read,NULL);
						start=0;
						end=10;
						while ((int)read<(i-start))
						{
							WriteFile(FileSave,(char*)ptr+start,(i-start),&read,NULL);
							start+=read;
							end--;
							if (!end)
							{
								FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,buf,sizeof(buf),NULL);
								MessageBox(MainWnd,GetString1(3023),GetString2(3022),0x40);
								MessageBox(MainWnd,buf,GetString1(3022),0x10);
								break;
							}
						}
						GlobalFree(ptr);
						CloseHandle(FileSave);
					}
					else
					{
						FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,buf,sizeof(buf),NULL);
						MessageBox(MainWnd,GetString1(3023),GetString2(3024),0x40);
						MessageBox(MainWnd,buf,GetString2(3024),0x10);
					}
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SetFocus(Text);
				}
				else
				{
					SendMessage(hwnd,msg,ID_SAVEAS,0);
				}
				break;
			case 1008:
				SendEditor(SCI_CUT,0,0);
				break;
			case 1009:
				SendEditor(SCI_COPY,0,0);
				break;
			case 1010:
				SendEditor(SCI_PASTE,0,0);
				break;
			case 1015:
				SendEditor(SCI_UNDO,0,0);
				break;
			case 1016:
				SendEditor(SCI_REDO,0,0);
				break;
			case 1017:
				SendEditor(SCI_SELECTALL,0,0);
				break;
			case 1006:
				ZeroMemory(&ofn,sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = MainWnd;
				ofn.hInstance = GetModuleHandle(NULL);
				ofn.lpstrFilter = "GBA/GBC ROMs\0*.gb?;*.gb\0All Files\0*.*\0";
				ofn.lpstrFile = RomOpen;
				ofn.nMaxFile = sizeof(RomOpen);
				LoadRegSettingStr(NULL,"ROMPath");
				strcpy(DefRomPath,GreatBuffer);
				ofn.lpstrInitialDir = DefRomPath;
				ofn.Flags = OFN_PATHMUSTEXIST;
				ofn.lpstrTitle = GetString1(3026);
				if (GetOpenFileName(&ofn))
				{
OpenRoms:
					strcpy(RomOpen2,RomOpen);
					strcpy(GreatBuffer,RomOpen);
					GetDirFromGreatBuffer();
					strcpy(DefRomPath,GreatBuffer);
					SaveRegSettingStr(NULL,"ROMPath",DefRomPath);
					fileM=CreateFile(RomOpen,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
					ReadFile(fileM,&modechar,4,&read,NULL);
					SetFilePointer(fileM,0x13F,NULL,FILE_BEGIN);
					ReadFile(fileM,&determineMode,3,&read,NULL);
					if ((determineMode[0]=='A'&&determineMode[1]=='A')||(determineMode[0]=='S'&&determineMode[1]=='M'))
					{//Detect GSC and Prism
						pokemon_version=GBC;
					}
					else
					{
						SetFilePointer(fileM,0xAC,NULL,FILE_BEGIN);
						ReadFile(fileM,&modechar,4,&read,NULL);
						pokemon_version=GBA;
					}
					CloseHandle(fileM);
					SaveRegSettingStr(NULL,"ROMFile",RomOpen2);
				}
				break;
			case 1020:
				collapseatend=1-collapseatend;
				mii.cbSize=sizeof(mii);
				mii.fMask=MIIM_STATE;
				if (!collapseatend)
				{
					mii.fState=MFS_CHECKED;
					SetMenuItemInfo(GetMenu(MainWnd),1020,0,&mii);
					mii.fState=MFS_UNCHECKED|MFS_GRAYED;
					SetMenuItemInfo(GetMenu(MainWnd),1021,0,&mii);
					collapselines=0;
				}
				else
				{
					mii.fState=MFS_UNCHECKED;
					SetMenuItemInfo(GetMenu(MainWnd),1020,0,&mii);
					mii.fState=MFS_ENABLED;
					SetMenuItemInfo(GetMenu(MainWnd),1021,0,&mii);
				}
				SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
				RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Score_Under\\PKSV-UI",0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&regkey,(DWORD*)&start);
				i=1-collapseatend;
				RegSetValueEx(regkey,"CollapseBeforeEnd",0,REG_DWORD,(CONST BYTE*)&i,4);
				RegCloseKey(regkey);
				break;
			case 1021:
				collapselines=1-collapselines;
				mii.cbSize=sizeof(mii);
				mii.fMask=MIIM_STATE;
				if (collapselines)
					mii.fState=MFS_CHECKED;
				else
					mii.fState=MFS_UNCHECKED;
				SetMenuItemInfo(GetMenu(MainWnd),1021,0,&mii);
				SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
				RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Score_Under\\PKSV-UI",0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&regkey,(DWORD*)&start);
				i=collapselines;
				RegSetValueEx(regkey,"CollapseEmptyLines",0,REG_DWORD,(CONST BYTE*)&i,4);
				RegCloseKey(regkey);
				break;
			case 1112:
				autoautocomplete=1-autoautocomplete;
				mii.cbSize=sizeof(mii);
				mii.fMask=MIIM_STATE;
				if (autoautocomplete)
					mii.fState=MFS_CHECKED;
				else
					mii.fState=MFS_UNCHECKED;
				SetMenuItemInfo(GetMenu(MainWnd),1112,0,&mii);
				RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Score_Under\\PKSV-UI",0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&regkey,(DWORD*)&start);
				i=autoautocomplete;
				RegSetValueEx(regkey,"AutoAutoComplete",0,REG_DWORD,(CONST BYTE*)&i,4);
				RegCloseKey(regkey);
				break;
			case 1012:
				ShowWindow(FindReplaceWin,SW_SHOW);
				SetFocus(GetDlgItem(FindReplaceWin,6));
				break;
			case 1005:
				ShowWindow(FFSWin,SW_SHOW);
				SetFocus(GetDlgItem(FFSWin,4));
				break;
			case 1014:
				ShowWindow(DecWin,SW_SHOW);
				SetFocus(GetDlgItem(DecWin,3));
				break;
			case 1018:
				EnableWindow(MainWnd,0);
				ShowWindow(GotoWin,SW_SHOW);
				SetFocus(GetDlgItem(GotoWin,3));
				sprintf(buf,"%u",SendEditor(SCI_LINEFROMPOSITION,SendEditor(SCI_GETSELECTIONSTART,0,0),0));
				SetDlgItemText(GotoWin,3,buf);
				sprintf(buf,"%u",SendEditor(SCI_GETSELECTIONSTART,0,0)-SendEditor(SCI_POSITIONFROMLINE,SendEditor(SCI_LINEFROMPOSITION,SendEditor(SCI_GETSELECTIONSTART,0,0),0),0));
				SetDlgItemText(GotoWin,4,buf);
				break;
			case 1032:
				ShowWindow(NoteWin,SW_SHOW);
				break;
			case 1101:
				ShowWindow(HexWin,SW_SHOW);
				break;
			case 1031:
				if (AOT)
				{
					SetWindowPos(MainWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
					mii.cbSize=sizeof(mii);
					mii.fMask=MIIM_STATE;
					mii.fState=MFS_CHECKED;
					SetMenuItemInfo(GetMenu(MainWnd),1031,0,&mii);
				}
				else
				{
					SetWindowPos(MainWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
					mii.cbSize=sizeof(mii);
					mii.fMask=MIIM_STATE;
					mii.fState=MFS_UNCHECKED;
					SetMenuItemInfo(GetMenu(MainWnd),1031,0,&mii);
				}
				AOT=!AOT;
				break;
			case 1030:
				if (*RomOpen2!=0)
				{
					ShellExecute(MainWnd,NULL,RomOpen2,NULL,NULL,SW_SHOWDEFAULT);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3027),GetString2(3028),0x40);
				}
				break;
			case 1100:
				strcpy(GreatBuffer,GetString1(3029));
				strcat(GreatBuffer,INTERNAL_VERSION);
				strcat(GreatBuffer,GetString1(3030));
				MessageBox(MainWnd,GreatBuffer,GetString1(3031),0x40);
				break;
			case 1202:
				if (*RomOpen2!=0)
				{
					i=(int)SendEditor(SCI_GETLENGTH,0,0);
					ptr=GlobalAlloc(GPTR,i+1);
					SendEditor(SCI_GETTEXT,i+1,(LPARAM)ptr);
					DetermineMode(RomOpen2);
					CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TDebugCompile,(LPVOID)ptr,0,(LPVOID)&i);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3034),GetString2(3001),0x40);
				}
				break;
			case 1203:
				ShowWindow(MovePlanWin,SW_SHOW);
				break;
			case 1204:
				UpdateMe((void*)1);
				break;
			case 1102:
				if (*RomOpen2)
				{
					ShowWindow(ScriptSearchWin,SW_SHOW);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3027),GetString2(3028),0x30);
				}
				break;
			case 1104:
				if (*RomOpen2&&pokemon_version==GBA)
				{
					ShowWindow(LockWin,SW_SHOW);
				}
				else if (!*RomOpen2)
				{
					MessageBox(MainWnd,GetString1(3027),GetString2(3028),0x30);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3097),GetString2(3028),0x30);
				}
				break;
#ifdef SCRAP
			case 1105:
ScrapPunkCode:
				if (*RomOpen2&&((modechar)=='ERPB'||(modechar)=='EGPB'))
				{
					ShowWindow(PickupWin,SW_SHOW);
				}
				else if (!*RomOpen2)
				{
					MessageBox(MainWnd,GetString1(3027),GetString2(3028),0x30);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3098),GetString2(3028),0x30);
				}
				break;
#endif
			case 1103:
				FlagsShown=1;
				SendMessage(GetDlgItem(FlagsWin,2),LB_RESETCONTENT,0,0);
				if (FlagArr)GlobalFree(FlagArr);
				FlagArr=NULL;
				FlagArrAlloc=0;
				FlagArrLen=0;
				LastFlag=-1;
				SetDlgItemText(FlagsWin,4,"");
				SetDlgItemText(FlagsWin,5,"");
				SetDlgItemText(FlagsWin,6,"");
				if (!LoadRegSetting("Flags","FilledList"))
				{
					SaveRegSetting("Flags","FilledList",1);
#include "default_flags.h"
					SortFlags();
				}
				else
					ReAddFlags();
				ShowWindow(FlagsWin,SW_SHOW);
				break;
			case 1013: //script-gen
				i=0;
				if (NotSaved)
				{
					i=MessageBox(MainWnd,GetString1(3032),GetString2(3028), MB_YESNOCANCEL | MB_ICONQUESTION);
					if (i == IDYES)
					{
						SendMessage(MainWnd,WM_COMMAND,ID_SAVE,0);
						if (NotSaved)
						{
							i = IDCANCEL;
						}
					}
				}
				if (i != IDCANCEL)
				{
					if (*RomOpen2!=0)
					{
						ShowWindow(GenWin,SW_SHOW);
						EnableWindow(MainWnd,0);
						SetFocus(GetDlgItem(GenWin,1));
					}
					else
					{
						MessageBox(MainWnd,GetString1(3027),GetString2(3028),0);
					}
				}
				break;
			case 1111:
				i=(int)SendEditor(SCI_GETSELECTIONSTART,0,0);
				len=0;
				end=0; //End == type
				start=(int)SendEditor(SCI_POSITIONFROMLINE,SendEditor(SCI_LINEFROMPOSITION,i,0),0);
				while (SciCharAt(start)==' ')start++;
				if (SciCharAt(start)=='=')
				{
					end=1;
				}
				if ((SciCharAt(start)=='m'||SciCharAt(start)=='M')&&SciCharAt(start+1)==' ')
				{
					end=3;
				}
				if (SciCharAt(start)=='.'&&SciCharAt(start+1)==' ')
				{
					end=100;
				}
				while (i>start)
				{
					if (SciCharAt(i-1)==' ') {
						if (end==0) {
							end=2;
						}break;
					}
					len++;
					i--;
				}
				if (pokemon_version==GBA)
				{
					if (end==0)
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "#define;9|#eorg;9|#loud;9|#org;9|#quiet;9|#raw;9|.;9|=;2|addfurniture;5|additem;4|addpokemon;3|\
addvar;9|applymovement;1|\
applymovementfinishat;1|battle;3|braille;2|call;9|callasm;9|callasm2;9|callstd;2|callstdif;2|\
checkanimation;8|checkattack;3|checkflag;6|checkifroomforfurniture;5|checkitem;4|\
checkitemspaceinbag;4|checkiteminpc;4|checkitemtype;4|checkfurniture;5|checkgender;7|\
checkmoney;11|checkobedience;3|checksound;10|checktrainerflag;1|choosecontestpokemon;3|clearflag;6|cleartrainerflag;1|closemsg;2|\
CMD_2C;0|CMD_2D;0|CMD_60;0|CMD_8A;0|CMD_96;0|CMD_99;0|CMD_9A;0|CMD_A6;0|CMD_B1;0|CMD_B2;0|\
CMD_C0;0|CMD_C1;0|CMD_C2;0|CMD_C3;0|CMD_D3;0|CMD_FB;0|CMD_FE;0|\
coincasetovar;11|compare;9|comparefarbytetobyte;9|comparefarbytetofarbyte;9|comparefarbytetovar;9|\
comparehiddenvar;9|comparevars;9|comparevars2;9|comparevartobyte;9|comparevartofarbyte;9|copybyte;9|\
copyscriptbanks;9|copyvar;9|\
copyvarifnotzero;9|countpokemon;3|createtempsprite;1|cry;3|darkenroom;8|disappear;1|disappearat;1|\
doanimation;8|doorchange;8|doweather;8|end;9|endtrainerbattle;1|endtrainerbattle2;1|executeram;9|faceplayer;1|\
fadedefault;10|fadein;10|fadeout;10|fadescreen;8|fadescreendelay;8|fadesong;10|fadesound;10|fakecallstd;2|fakejumpstd;2|\
falldownhole;7|fardisappear;1|farreappear;1|fanfare;10|getplayerxy;7|giveegg;3|giveitemtopc;4|\
givemoney;11|givetocoincase;11|hidebox;2|hidecoins;11|hidepokepic;2|hidemoney;2|if;9|jump;9|\
jumpram;9|jumpstd;2|jumpstdif;2|killscript;9|lastbattle;3|lasttrainerbattle;1|lightroom;8|\
loadbytefrompointer;9|loadpointer;9|lock;1|lockall;1|\
m;1|message;2|moveoffscreen;1|movesprite;1|movesprite2;1|msgbox;2|msgbox2;2|msgboxnormal;2|msgboxsign;2|multichoice;2|\
multichoice2;2|multichoice3;2|nop|nop0|nop1|paymoney;11|pause;8|pauseevent;1|picture;2|playsong;10|playsound;10|pokemart;4|pokemart2;4|pokemart3;4|\
random;9|reappear;1|reappearat;1|release;1|releaseall;1|removeitem;4|resetspritelevel;1|resetvars;9|resetweather;8|return;9|setanimation;8|setbyte;9|setbyte2;9|setcatchlocation;3|\
setdoorclosed;8|setdoorclosedstatic;8|setdooropened;8|setdooropenedstatic;8|setfarbyte;9|setflag;6|sethealingplace;3|setmapfooter;9|setmaptile;8|setobedience;3|setpokemonpp;3|settrainerflag;1|setvar;9|\
setweather;8|setworldmapflag;6|showcoins;11|showmoney;11|showmsg;2|showpokepic;3|showyesno;2|slotmachine;4|sound;10|\
special;8|special2;8|spritebehave;1|spriteface;1|startcontest;8|startwireless;8|storeattack;2|storeboxname;2|storecomp;2|storefirstpokemon;2|storefurniture;2|\
storeitem;2|storeitems;2|storepokemon;2|storepokemonvar;2|storetext;2|storevar;2|subtractvar;9|takefromcoincase;11|takefurniture;5|tempspriteface;1|textcolor;2|trainerbattle;1|updatecoins;11|updatemoney;11|\
waitbutton;9|waitcry;3|waitfanfare;10|waitspecial;8|warp;7|warp3;7|warp4;7|warp5;7|warp6;7|warpelevator;7|warpmuted;7|warpteleport;7|warpteleport2;7|warpwalking;7|writebytetooffset;9");
					else if (end==1)
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "\\v\\h01|\\v\\h02|\\v\\h01 found one \\v\\h02!|\\v\\h01 obtained one \\v\\h02!|\
\\v\\h01 received the \\v\\h02!|\\v\\h01 received \\v\\h03 \\v\\h02s!");
					else if (end==2)
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "!=;9|<;9|<=;9|<>;9|=;9|==;9|>;9|>=;9|ABRA;3|ABSOL;3|ACROBIKE;4|AERODACTYL;3|AGGRON;3|AGUAVBERRY;4|AIPOM;3|ALAKAZAM;3|\
ALTARIA;3|AMPHAROS;3|AMULETCOIN;4|ANORITH;3|ANOTEMAT;5|ANTIDOTE;4|APICOTBERRY;3|ARBOK;3|ARCANINE;3|ARIADOS;3|\
ARMALDO;3|ARON;3|ARTICUNO;3|ASPEARBERRY;4|ATTRACTMAT;5|AURORATICKET;4|AWAKENING;4|AZUMARILL;3|AZURILL;3|\
AZURILLDOLL;5|BAGON;3|BALLCUSHION;5|BALLPOSTER;5|BALTOY;3|BALTOYDOLL;5|BANETTE;3|BARBOACH;3|BASEMENTKEY;4|\
BAYLEEF;3|BEADMAIL;4|BEAUTIFLY;3|BEEDRILL;3|BELDUM;3|BELLOSSOM;3|BELLSPROUT;3|BELUEBERRY;4|BERRYJUICE;4|\
BERRYPOUCH;4|BICYCLE;4|BIGMUSHROOM;4|BIGPEARL;4|BIGPLANT;5|BIKEVOUCHER;4|BLACKBELT;4|BLACKFLUTE;4|\
BLACKGLASSES;4|BLASTOISE;3|BLASTOISEDOLL;5|BLAZIKEN;3|BLISSEY;3|BLUEBALLOON;5|BLUEFLUTE;4|BLUEORB;4|\
BLUEPOSTER;5|BLUESCARF;4|BLUESHARD;4|BLUETENT;5|BLUKBERRY;4|BNOTEMAT;5|BOY;7|BREAKABLEDOOR;5|BRELOOM;3|\
BRICKCHAIR;5|BRICKDESK;5|BRIGHTPOWDER;4|BULBASAUR;3|BURNHEAL;4|BUTTERFREE;3|CACNEA;3|CACTURNE;3|CALCIUM;4|\
CAMERA;1|CAMERA_START;8|CAMERA_END;8|CAMERUPT;3|CAMPCHAIR;5|CAMPDESK;5|CARBOS;4|CARDKEY;4|CARVANHA;3|CASCOON;3|CASTFORM;3|CATCH_POKEMON;8|\
CATERPIE;3|CELEBI;3|CHANSEY;3|CHARCOAL;3|CHARIZARD;3|CHARIZARDDOLL;5|CHARMANDER;3|CHARMELEON;3|CHERIBERRY;4|\
CHESTOBERRY;4|CHIGHNOTEMAT;5|CHIKORITA;3|CHIKORITADOLL;5|CHIMECHO;3|CHINCHOU;3|CHOICEBAND;4|\
CHOOSE_POKEMON;8|CLAMPERL;3|CLAWFOSSIL;4|CLAYDOL;3|CLEANSETAG;4|CLEFABLE;3|CLEFAIRY;3|CLEFAIRYDOLL;5|\
CLEFFA;3|CLOWNOTEMAT;5|CLOYSTER;3|COINCASE;4|COLORFULPLANT;5|COMBUSKEN;3|COMFORTCHAIR;5|COMFORTDESK;5|\
CONTESTPASS;4|CORNNBERRY;4|CORPHISH;3|CORSOLA;3|CRADILY;3|CRAWDAUNT;3|CROBAT;3|CROCONAW;3|CUBONE;3|\
CUTEPOSTER;5|CUTETV;5|CYNDAQUIL;3|CYNDAQUILDOLL;5|DEEPSEASCALE;4|DEEPSEATOOTH;4|DELCATTY;3|DELIBIRD;3|\
DEOXYS;3|DEVONGOODS;4|DEVONSCOPE;4|DEWGONG;3|DIAMONDCUSHION;5|DIGLETT;3|DIREHIT;4|DITTO;3|DITTODOLL;5|\
DIVEBALL;4|DNOTEMAT;5|DODRIO;3|DODUO;3|DOMEFOSSIL;4|DONPHAN;3|DOWN;7|DRAGONAIR;3|DRAGONFANG;4|DRAGONITE;3|\
DRAGONSCALE;4|DRATINI;3|DREAMMAIL;4|DROWZEE;3|DUGTRIO;3|DUNSPARCE;3|DURINBERRY;4|DUSCLOPS;3|DUSKULL;3|\
DUSKULLDOLL;5|DUSTOX;3|EEVEE;3|EKANS;3|ELECTABUZZ;3|ELECTRIKE;3|ELECTRODE;3|ELEKID;3|ELIXIR;4|EM_BADGE_1;6|\
EM_BADGE_2;6|EM_BADGE_3;6|EM_BADGE_4;6|EM_BADGE_5;6|EM_BADGE_6;6|EM_BADGE_7;6|EM_BADGE_8;6|EM_NATIONAL_DEX;8|\
EM_POKEDEX;6|EM_POKEMON;6|EM_POKENAV;6|ENERGYPOWDER;4|ENERGYROOT;4|ENIGMABERRY;4|ENOTEMAT;5|ENTEI;3|EONTICKET;4|\
ESCAPEROPE;4|ESPEON;3|ETHER;4|EVERSTONE;4|EXEGGCUTE;3|EXEGGUTOR;3|EXPLOUD;3|EXPSHARE;4|FABMAIL;4|FAMECHECKER;4|\
FARFETCHD;3|FEAROW;3|FEEBAS;3|FENCELENGTH;5|FENCEWIDTH;5|FERALIGATR;3|FIGYBERRY;4|FINDFROM;9|FIREBLASTMAT;5|\
FIRECUSHION;4|FIRESTONE;4|FISSUREMAT;5|FLAAFFY;3|FLAREON;3|FLUFFYTAIL;4|FLYGON;3|FNOTEMAT;5|FOCUSBAND;4|\
FORRETRESS;3|FR_BADGE_1;6|FR_BADGE_2;6|FR_BADGE_3;6|FR_BADGE_4;6|FR_BADGE_5;6|FR_BADGE_6;6|FR_BADGE_7;6|\
FR_BADGE_8;6|FR_NATIONAL_DEX;8|FR_POKEDEX;6|FR_POKEMON;6|FRESHWATER;4|FULLHEAL;4|FULLRESTORE;4|FURRET;3|\
GANLONBERRY;4|GARDEVOIR;3|GASTLY;3|GENGAR;3|GEODUDE;3|GIRAFARIG;3|GIRL;7|GLALIE;3|GLASSORNAMENT;4|\
GLIGAR;3|GLITTERMAIL;4|GLITTERMAT;5|GLOOM;3|GNOTEMAT;5|GOGOGGLES;4|GOLBAT;3|GOLDEEN;3|\
GOLDSHIELD;4|GOLDTEETH;4|GOLDUCK;3|GOLEM;3|GOODROD;4|GOREBYSS;3|GORGEOUSPLANT;4|GRANBULL;3|\
GRASSCUSHION;4|GRAVELER;3|GREATBALL;4|GREENPOSTER;4|GREENSCARF;4|GREENSHARD;4|GREPABERRY;4|\
GRIMER;3|GROUDON;3|GROVYLE;3|GROWLITHE;3|GRUMPIG;3|GUARDSPEC;4|GULPIN;3|GULPINDOLL;5|GYARADOS;3|\
HARBORMAIL;4|HARDCHAIR;4|HARDDESK;5|HARDSTONE;4|HARIYAMA;3|HAUNTER;3|HEAL_POKEMON;8|HEALPOWDER;4|\
HEARTSCALE;4|HEAVYCHAIR;4|HEAVYDESK;5|HELIXFOSSIL;4|HERACROSS;3|HITMONCHAN;3|\
HITMONLEE;3|HITMONTOP;3|HM01;4|HM02;4|HM03;4|HM04;4|HM05;4|HM06;4|HM07;4|HM08;4|HONDEWBERRY;4|\
HO-OH;3|HOOTHOOT;3|HOPPIP;3|HORSEA;3|HOUNDOOM;3|HOUNDOUR;3|HPUP;4|HUNTAIL;3|HYPERPOTION;4|HYPNO;3|\
IAPAPABERRY;4|ICEHEAL;4|IGGLYBUFF;3|ILLUMISE;3|IRON;3|ITEMFINDER;4|IVYSAUR;3|JIGGLYPUFF;3|\
JIGGLYPUFFDOLL;5|JIRACHI;3|JOLTEON;3|JUMPLUFF;3|JUMPMAT;5|JYNX;3|KABUTO;3|KABUTOPS;3|KADABRA;3|\
KAKUNA;3|KANGASKHAN;3|KECLEON;3|KECLEONDOLL;5|KELPSYBERRY;4|KINGDRA;3|KINGLER;3|KINGSROCK;4|\
KIRLIA;3|KISSCUSHION;5|KISSPOSTER;5|KOFFING;3|KRABBY;3|KYOGRE;3|LAIRON;3|LANSATBERRY;4|LANTURN;3|\
LAPRAS;3|LAPRASDOLL;5|LARVITAR;3|LASTRESULT;9|LASTTALKED;9|LATIAS;3|LATIOS;3|LAVACOOKIE;4|LAXINCENSE;4|\
LEAFSTONE;4|LEDIAN;3|LEDYBA;3|LEFT;7|LEFTOVERS;4|LEMONADE;4|LEPPABERRY;4|LETTER;4|LICKITUNG;3|\
LIECHIBERRY;4|LIFTKEY;4|LIGHTBALL;4|LILEEP;3|LINOONE;3|LOMBRE;3|LONGPOSTER;5|LOTAD;3|LOTADDOLL;5|\
LOUDRED;3|LUCKYEGG;4|LUCKYPUNCH;4|LUDICOLO;3|LUGIA;3|LUMBERRY;3|LUNATONE;3|LUVDISC;3|\
LUXURYBALL;4|MACHAMP;3|MACHBIKE;4|MACHOBRACE;4|MACHOKE;3|MACHOP;3|MAGBY;3|MAGCARGO;3|\
MAGIKARP;3|MAGMAEMBLEM;4|MAGMAR;3|MAGNEMITE;3|MAGNET;4|MAGNETON;3|MAGOBERRY;4|MAGOSTBERRY;4|\
MAKUHITA;3|MANECTRIC;3|MANKEY;3|MANTINE;3|MAREEP;3|MARILL;3|MARILLDOLL;5|MAROWAK;3|MARSHTOMP;3|\
MASQUERAIN;3|MASTERBALL;4|MAWILE;3|MAXELIXIR;4|MAXETHER;4|MAXPOTION;4|MAXREPEL;4|MAXREVIVE;4|\
MECHMAIL;4|MEDICHAM;3|MEDITITE;3|MEGANIUM;3|MENTALHERB;4|MEOWTH;3|MEOWTHDOLL;5|METAGROSS;3|\
METALCOAT;4|METALPOWDER;4|METANG;3|METAPOD;3|METEORITE;4|MEW;3|MEWTWO;3|MIGHTYENA;3|MILOTIC;3|\
MILTANK;3|MINUN;3|MIRACLESEED;4|MISDREAVUS;3|MISSINGNO;3|MOLTRES;3|MOOMOOMILK;4|MOONSTONE;4|\
MRMIME;3|MSG_FIND;2|MSG_NOCLOSE;2|MSG_NORMAL;2|MSG_OBTAIN;2|MSG_SIGN;2|MSG_STANDARD;2|\
MSG_YESNO;2|MUDBALL;4|MUDKIP;3|MUDKIPDOLL;5|MUK;3|MURKROW;3|MYSTICTICKET;4|MYSTICWATER;4|\
NANABBERRY;4|NATU;3|NESTBALL;4|NETBALL;4|NEVERMELTICE;4|NIDOKING;3|NIDOQUEEN;3|NIDORANF;3|\
NIDORANM;3|NIDORINA;3|NIDORINO;3|NINCADA;3|NINETALES;3|NINJASK;3|NO;9|NOCTOWL;3|NOMELBERRY;4|NONE;4|\
NOSEPASS;3|NUGGET;4|NUMEL;3|NUZLEAF;3|OAKSPARCEL;4|OCTILLERY;3|ODDISH;3|OLDAMBER;4|OLDROD;4|OLDSEAMAP;4|\
OMANYTE;3|OMASTAR;3|ONIX;3|ORANBERRY;4|ORANGEMAIL;4|PAMTREBERRY;4|PARAS;3|PARASECT;3|PARLYZHEAL;4|\
PEARL;4|PECHABERRY;4|PELIPPER;3|PERSIAN;3|PERSIMBERRY;4|PETAYABERRY;4|PHANPY;3|PICHU;3|PICHUDOLL;5|\
PIDGEOT;3|PIDGEOTTO;3|PIDGEY;3|PIKACHU;3|PIKACHUDOLL;5|PIKACUSHION;5|PIKAPOSTER;5|PILOSWINE;3|\
PINAPBERRY;4|PINECO;3|PINKSCARF;4|PINSIR;3|PLAYER;1|PLAYERFACING;9|PLUSLE;3|POISONBARB;4|POKEBALL;4|\
POKEBLOCKCASE;4|POKEDOLL;5|POKEFLUTE;4|POKEMONCHAIR;5|POKEMONDESK;5|POLITOED;3|POLIWAG;3|POLIWHIRL;3|\
POLIWRATH;3|POMEGBERRY;4|PONYTA;3|POOCHYENA;3|PORYGON;3|PORYGON2;3|POTION;4|POWDERJAR;4|POWDERSNOWMAT;5|\
PPMAX;4|PPUP;4|PREMIERBALL;4|PRETTYCHAIR;5|PRETTYDESK;5|PRETTYFLOWERS;5|PRIMEAPE;3|PROTEIN;4|PSYDUCK;3|\
PUPITAR;3|QUAGSIRE;3|QUALOTBERRY;4|QUICKCLAW;4|QUILAVA;3|QWILFISH;3|RABUTABERRY;4|RAGGEDCHAIR;4|RAGGEDDESK;5|\
RAICHU;3|RAIKOU;3|RAINBOWPASS;4|RALTS;3|RAPIDASH;3|RARECANDY;4|RATICATE;3|RATTATA;3|RAWSTBERRY;4|\
RAYQUAZA;3|RAZZBERRY;4|REDBALLOON;5|REDBRICK;5|REDFLUTE;4|REDORB;4|REDPLANT;5|REDPOSTER;5|REDSCARF;4|\
REDSHARD;4|REDTENT;5|REGICE;3|REGICEDOLL;5|REGIROCK;3|REGIROCKDOLL;5|REGISTEEL;3|REGISTEELDOLL;5|\
RELICANTH;3|REMORAID;3|REPEATBALL;4|REPEL;4|RETROMAIL;4|REVIVALHERB;4|REVIVE;4|RHYDON;3|RHYDONDOLL;5|\
RHYHORN;3|RIGHT;7|RM1KEY;4|RM2KEY;4|RM4KEY;4|RM6KEY;4|ROOTFOSSIL;4|ROSELIA;3|ROUNDCUSHION;5|ROUNDTV;5|\
RS_BADGE_1;6|RS_BADGE_2;6|RS_BADGE_3;6|RS_BADGE_4;6|RS_BADGE_5;6|RS_BADGE_6;6|RS_BADGE_7;6|RS_BADGE_8;6|\
RS_POKEDEX;6|RS_POKEMON;6|RS_POKENAV;6|RS_RUNNING_SHOES;6|RUBY;4|SABLEYE;3|SACREDASH;4|SAFARIBALL;4|\
SALACBERRY;4|SALAMENCE;3|SANDORNAMENT;4|SANDSHREW;3|SANDSLASH;3|SAPPHIRE;4|SCANNER;4|SCEPTILE;3|SCIZOR;3|\
SCOPELENS;4|SCYTHER;3|SEADRA;3|SEAINCENSE;4|SEAKING;3|SEALEO;3|SEAPOSTER;5|SECRETKEY;4|SEEDOT;3|SEEDOTDOLL;5|\
SEEL;3|SENTRET;3|SEVIPER;3|SHADOWMAIL;4|SHARPBEAK;4|SHARPEDO;3|SHEDINJA;3|SHELGON;3|SHELLBELL;4|SHELLDER;3|\
SHIFTRY;3|SHOALSALT;4|SHOALSHELL;4|SHROOMISH;3|SHUCKLE;3|SHUPPET;3|SILCOON;3|SILKSCARF;4|SILPHSCOPE;4|\
SILVERPOWDER;4|SILVERSHIELD;4|SITRUSBERRY;4|SKARMORY;3|SKIPLOOM;3|SKITTY;3|SKITTYDOLL;5|SKYPOSTER;5|\
SLAKING;3|SLAKOTH;3|SLIDE;5|SLOWBRO;3|SLOWKING;3|SLOWPOKE;3|SLUGMA;3|SMALLCHAIR;5|SMALLDESK;5|SMEARGLE;3|\
SMOKEBALL;4|SMOOCHUM;3|SMOOCHUMDOLL;5|SNEASEL;3|SNORLAX;3|SNORLAXDOLL;5|SNORUNT;3|SNUBBULL;3|SODAPOP;4|\
SOFTSAND;4|SOLIDBOARD;5|SOLROCK;3|SOOTHEBELL;4|SOOTSACK;4|SOULDEW;4|SPEAROW;3|SPELLTAG;4|SPELONBERRY;4|\
SPHEAL;3|SPIKESMAT;5|SPINARAK;3|SPINCUSHION;5|SPINDA;3|SPINMAT;5|SPOINK;3|SQUIRTLE;3|SSTICKET;4|STAND;5|\
STANTLER;3|STARDUST;4|STARFBERRY;4|STARMIE;3|STARPIECE;4|STARYU;3|STEELIX;3|STICK;4|STORAGEKEY;4|SUDOWOODO;3|\
SUICUNE;3|SUNFLORA;3|SUNKERN;3|SUNSTONE;4|SUPERPOTION;4|SUPERREPEL;4|SUPERROD;4|SURFMAT;5|SURSKIT;3|SWABLU;3|\
SWABLUDOLL;5|SWALOT;3|SWAMPERT;3|SWELLOW;3|SWINUB;3|TAILLOW;3|TAMATOBERRY;4|TANGELA;3|TAUROS;3|TEA;4|TEACHYTV;4|\
TEDDIURSA;3|TENTACOOL;3|TENTACRUEL;3|THICKCLUB;4|THUNDERMAT;5|THUNDERSTONE;4|TIMERBALL;4|TINYMUSHROOM;4|\
TIRE;5|TM01;4|TM02;4|TM03;4|TM04;4|TM05;4|TM06;4|TM07;4|TM08;4|TM09;4|TM10;4|TM11;4|TM12;4|TM13;4|TM14;4|TM15;4|TM16;4|TM17;4|\
TM18;4|TM19;4|TM20;4|TM21;4|TM22;4|TM23;4|TM24;4|TM25;4|TM26;4|TM27;4|TM28;4|TM29;4|TM30;4|TM31;4|TM32;4|TM33;4|TM34;4|TM35;4|\
TM36;4|TM37;4|TM38;4|TM39;4|TM40;4|TM41;4|TM42;4|TM43;4|TM44;4|TM45;4|TM46;4|TM47;4|TM48;4|TM49;4|TM50;4|TMCASE;4|TOGEPI;3|\
TOGEPIDOLL;5|TOGETIC;3|TORCHIC;3|TORCHICDOLL;5|TORKOAL;3|TOTODILE;3|TOTODILEDOLL;5|TOWNMAP;4|TRAPINCH;3|\
TREECKO;3|TREECKODOLL;5|TRIPASS;4|TROPICALPLANT;5|TROPICMAIL;4|TROPIUS;3|TWISTEDSPOON;4|TYPHLOSION;3|\
TYRANITAR;3|TYROGUE;3|ULTRABALL;4|UMBREON;3|UNOWN;3|UP;7|UPGRADE;4|URSARING;3|VAPOREON;3|VENOMOTH;3|\
VENONAT;3|VENUSAUR;3|VENUSAURDOLL;5|VIBRAVA;3|VICTREEBEL;3|VIGOROTH;3|VILEPLUME;3|VOLBEAT;3|VOLTORB;3|\
VSSEEKER;4|VULPIX;3|WAILMER;3|WAILMERDOLL;5|WAILMERPAIL;4|WAILORD;3|WALREIN;3|WARTORTLE;3|WATERCUSHION;5|\
WATERSTONE;4|WATMELBERRY;4|WAVEMAIL;4|WEEDLE;3|WEEPINBELL;3|WEEZING;3|WEPEARBERRY;4|WHISCASH;3|WHISMUR;3|\
WHITEFLUTE;4|WHITEHERB;4|WIGGLYTUFF;3|WIKIBERRY;4|WINGULL;3|WOBBUFFET;3|WOODMAIL;4|WOOPER;3|WURMPLE;3|\
WYNAUT;3|WYNAUTDOLL;5|XACCURACY;4|XATTACK;4|XATU;3|XDEFEND;4|XSPECIAL;4|XSPEED;4|YANMA;3|YELLOWBALLOON;5|\
YELLOWBRICK;5|YELLOWFLUTE;4|YELLOWSCARF;4|YELLOWSHARD;4|YES;9|ZANGOOSE;3|ZAPDOS;3|ZIGZAGCUSHION;5|\
ZIGZAGOON;3|ZINC;4|ZUBAT;3|FALSE;6|TRUE;6");
					else if (end==3)
					{
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "alert;12|end;1|face_away;13|faceplayer;13|hide;1|jump_down;13|jump_down1;13|jump_down2;13|jump_downup;13|\
jump_left;13|jump_left1;13|jump_left2;13|jump_leftright;13|jump_right;13|jump_right1;13|jump_right2;13|\
jump_rightleft;13|jump_up;13|jump_up1;13|jump_up2;13|jump_updown;13|look_down;1|look_down_delayed;13|\
look_down_jump_up;13|look_left;1|look_left_delayed;13|look_left_jump_down;13|look_left_jump_right;13|\
look_right;1|look_right_delayed;13|look_up;1|look_up_delayed;13|look_up_jump_left;13|love;12|onspot_down;13|\
onspot_down_fast;13|onspot_down_slow;13|onspot_left;13|onspot_left_fast;13|onspot_left_slow;13|onspot_right;13|\
onspot_right_fast;13|onspot_right_slow;13|onspot_up;13|onspot_up_fast;13|onspot_up_slow;13|pause;13|pause0;12|pause1;12|\
pause2;12|pause3;12|pause4;12|pause_long;13|pause_short;13|pause_vshort;13|pause_xshort;13|pokeball;12|question;12|rock_animate;13|run_down;1|run_down_fast;12|run_down_vfast;13|run_left;1|run_left_fast;12|run_left_vfast;13|\
run_right;1|run_right_fast;12|run_right_vfast;13|run_up;1|run_up_fast;12|run_up_vfast;13|say_!!;13|say_!;13|say_:);13|say_?;13|say_X;13|show;12|\
slide_down;13|slide_down2;13|slide_down3;13|slide_down_vfast;13|slide_left;13|slide_left2;13|slide_left3;13|slide_left_vfast;13|slide_right;13|\
slide_right2;13|slide_right3;13|slide_right_vfast;13|slide_up;13|slide_up2;13|slide_up3;13|slide_up_vfast;13|tree_animate;13|walk_down;1|walk_down_fast;12|\
walk_down_slow;1|walk_down_vslow;1|walk_left;1|walk_left_fast;12|walk_left_slow;1|walk_left_vslow;1|walk_onspot;13|\
walk_right;1|walk_right_fast;12|walk_right_slow;1|walk_right_vslow;1|walk_up;1|walk_up_fast;12|walk_up_slow;1|\
walk_up_vslow;1");
					}
				}
				else
				{
					if (end==0)
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "#define;9|#eorg;9|#loud;9|#org;9|#quiet;9|#raw;9|.;9|=;2|\
2call;9|2jump;9|2ptcall;9|2ptcallasm;9|2ptjump;9|2writetext;9|3call;9|3callasm;9|3jump;9|3writetext;9|\
addvar;9|appear;1|applymovement;1|applymoveother;1|askforphonenumber;1|blackoutmod;8|c1celoadbyte;9|\
callstd;9|catchtutorial;7|changeblock;8|checkbit1;6|checkbit2;6|checkcode;9|checkcoins;11|checkitem;4|\
checkmaptriggers;8|checkmoney;11|checkphonecall;1|checkphonenumber;1|checkpoke;3|checktime;8|\
checktriggers;8|checkver;9|clearbit1;6|clearbit2;6|clearfight;1|closetext;2|coinstotext;11|\
copybytetovar;9|copyvartobyte;9|credits;2|cry;3|deactivatefacing;1|delcmdqueue;9|describedecoration;5|\
disappear;1|displaylocation;2|domaptrigger;8|dotrigger;8|earthquake;8|elevator;8|end;9|faceperson;1|\
faceplayer;1|follow;1|follownotexact;1|fruittree;4|givecoins;11|giveegg;3|giveitem;4|givemoney;11|\
givephonenumber;1|givepoke;3|givepokeitem;4|halloffame;8|hangup;1|if;9|interpretmenu;2|interpretmenu2;2|\
itemnotify;4|itemtotext;4|jumpstd;9|jumptext;2|jumptextfaceplayer;2|keeptextopen;2|loademote;1|\
loadfont;2|loadmenudata;2|loadmovesprites;1|loadpikadata;3|loadpokedata;3|loadtrainer;1|loadtrainerseen;1|\
loadvar;9|loadwilddata;3|locationtotext;2|moneytotext;11|moveperson;1|musicfadeout;10|newloadmap;8|\
passtoengine;9|pause;8|phonecall;1|playmapmusic;10|playmusic;10|playrammusic;10|playsound;10|pocketisfull;2|\
pokemart;2|pokepic;2|pokepicyesorno;2|poketotext;3|priorityjump;9|ptpriorityjump;9|random;9|refreshscreen;8|\
reloadandreturn;9|reloadmap;8|reloadmapmusic;10|reloadmappart;8|repeattext;2|resetfuncs;9|return;9|\
returnafterbattle;1|setbit1;6|setbit2;6|setlasttalked;1|showemote;1|special;8|specialphonecall;8|specialsound;10|\
spriteface;1|startbattle;3|stopfollow;1|storetext;2|stringtotext;2|swapmaps;8|takecoins;11|takeifletter;4|\
takeitem;4|takemoney;11|takephonenumber;1|talkafter;2|talkaftercancel;2|talkaftercheck;2|text-box;2|\
text-buffer;2|text-day;2|text-hex;2|text-interpretdata;9|text-interpretxdata;9|text-newline;2|\
text-newlinewitharrow;2|text-newtxt;2|text-number;2|text-playsound0;10|text-playsound1;10|text-playsound2;10|\
text-playsound9;10|text-playsounda;10|text-playsoundc;10|text-playsoundd;10|text-ram;2|text-reloc;9|\
text-switchtoasm;9|text-waitbutton;2|text-waitbutton2;2|trade;3|trainerstatus;1|trainertext;1|\
trainertotext;1|variablesprite;1|vartotext;9|verbosegiveitem;4|waitbutton;8|warp;8|warpcheck;8|\
warpfacing;8|warpmod;8|warpsound;10|wildoff;3|wildon;3|winlosstext;1|writebackup;9|writebyte;9|\
writecmdqueue;9|writecode;9|writepersonloc;1|writevarcode;9|xycompare;7|yesorno;");
					else if (end==1)
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "[PLAYER]|\\e|[PLAYER] found one |[PLAYER] obtained one |\
[PLAYER] received the |[PLAYER] received ");
					else if (end==2)
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "!=;9|<;9|<=;9|<>;9|=;9|==;9|>;9|>=;9|ABRA;3|ABSOL;3|ACROBIKE;4|AERODACTYL;3|AGGRON;3|AGUAVBERRY;4|AIPOM;3|ALAKAZAM;3|\
ALTARIA;3|AMPHAROS;3|AMULETCOIN;4|ANORITH;3|ANOTEMAT;5|ANTIDOTE;4|APICOTBERRY;3|ARBOK;3|ARCANINE;3|ARIADOS;3|\
ARMALDO;3|ARON;3|ARTICUNO;3|ASPEARBERRY;4|ATTRACTMAT;5|AURORATICKET;4|AWAKENING;4|AZUMARILL;3|AZURILL;3|\
AZURILLDOLL;5|BAGON;3|BALLCUSHION;5|BALLPOSTER;5|BALTOY;3|BALTOYDOLL;5|BANETTE;3|BARBOACH;3|BASEMENTKEY;4|\
BAYLEEF;3|BEADMAIL;4|BEAUTIFLY;3|BEEDRILL;3|BELDUM;3|BELLOSSOM;3|BELLSPROUT;3|BELUEBERRY;4|BERRYJUICE;4|\
BERRYPOUCH;4|BICYCLE;4|BIGMUSHROOM;4|BIGPEARL;4|BIGPLANT;5|BIKEVOUCHER;4|BLACKBELT;4|BLACKFLUTE;4|\
BLACKGLASSES;4|BLASTOISE;3|BLASTOISEDOLL;5|BLAZIKEN;3|BLISSEY;3|BLUEBALLOON;5|BLUEFLUTE;4|BLUEORB;4|\
BLUEPOSTER;5|BLUESCARF;4|BLUESHARD;4|BLUETENT;5|BLUKBERRY;4|BNOTEMAT;5|BOY;7|BREAKABLEDOOR;5|BRELOOM;3|\
BRICKCHAIR;5|BRICKDESK;5|BRIGHTPOWDER;4|BULBASAUR;3|BURNHEAL;4|BUTTERFREE;3|CACNEA;3|CACTURNE;3|CALCIUM;4|\
CAMERA;1|CAMERA_START;8|CAMERA_END;8|CAMERUPT;3|CAMPCHAIR;5|CAMPDESK;5|CARBOS;4|CARDKEY;4|CARVANHA;3|CASCOON;3|CASTFORM;3|CATCH_POKEMON;8|\
CATERPIE;3|CELEBI;3|CHANSEY;3|CHARCOAL;3|CHARIZARD;3|CHARIZARDDOLL;5|CHARMANDER;3|CHARMELEON;3|CHERIBERRY;4|\
CHESTOBERRY;4|CHIGHNOTEMAT;5|CHIKORITA;3|CHIKORITADOLL;5|CHIMECHO;3|CHINCHOU;3|CHOICEBAND;4|\
CHOOSE_POKEMON;8|CLAMPERL;3|CLAWFOSSIL;4|CLAYDOL;3|CLEANSETAG;4|CLEFABLE;3|CLEFAIRY;3|CLEFAIRYDOLL;5|\
CLEFFA;3|CLOWNOTEMAT;5|CLOYSTER;3|COINCASE;4|COLORFULPLANT;5|COMBUSKEN;3|COMFORTCHAIR;5|COMFORTDESK;5|\
CONTESTPASS;4|CORNNBERRY;4|CORPHISH;3|CORSOLA;3|CRADILY;3|CRAWDAUNT;3|CROBAT;3|CROCONAW;3|CUBONE;3|\
CUTEPOSTER;5|CUTETV;5|CYNDAQUIL;3|CYNDAQUILDOLL;5|DEEPSEASCALE;4|DEEPSEATOOTH;4|DELCATTY;3|DELIBIRD;3|\
DEOXYS;3|DEVONGOODS;4|DEVONSCOPE;4|DEWGONG;3|DIAMONDCUSHION;5|DIGLETT;3|DIREHIT;4|DITTO;3|DITTODOLL;5|\
DIVEBALL;4|DNOTEMAT;5|DODRIO;3|DODUO;3|DOMEFOSSIL;4|DONPHAN;3|DOWN;7|DRAGONAIR;3|DRAGONFANG;4|DRAGONITE;3|\
DRAGONSCALE;4|DRATINI;3|DREAMMAIL;4|DROWZEE;3|DUGTRIO;3|DUNSPARCE;3|DURINBERRY;4|DUSCLOPS;3|DUSKULL;3|\
DUSKULLDOLL;5|DUSTOX;3|EEVEE;3|EKANS;3|ELECTABUZZ;3|ELECTRIKE;3|ELECTRODE;3|ELEKID;3|ELIXIR;4|EM_BADGE_1;6|\
EM_BADGE_2;6|EM_BADGE_3;6|EM_BADGE_4;6|EM_BADGE_5;6|EM_BADGE_6;6|EM_BADGE_7;6|EM_BADGE_8;6|EM_NATIONAL_DEX;8|\
EM_POKEDEX;6|EM_POKEMON;6|EM_POKENAV;6|ENERGYPOWDER;4|ENERGYROOT;4|ENIGMABERRY;4|ENOTEMAT;5|ENTEI;3|EONTICKET;4|\
ESCAPEROPE;4|ESPEON;3|ETHER;4|EVERSTONE;4|EXEGGCUTE;3|EXEGGUTOR;3|EXPLOUD;3|EXPSHARE;4|FABMAIL;4|FAMECHECKER;4|\
FARFETCHD;3|FEAROW;3|FEEBAS;3|FENCELENGTH;5|FENCEWIDTH;5|FERALIGATR;3|FIGYBERRY;4|FINDFROM;9|FIREBLASTMAT;5|\
FIRECUSHION;4|FIRESTONE;4|FISSUREMAT;5|FLAAFFY;3|FLAREON;3|FLUFFYTAIL;4|FLYGON;3|FNOTEMAT;5|FOCUSBAND;4|\
FORRETRESS;3|FR_BADGE_1;6|FR_BADGE_2;6|FR_BADGE_3;6|FR_BADGE_4;6|FR_BADGE_5;6|FR_BADGE_6;6|FR_BADGE_7;6|\
FR_BADGE_8;6|FR_NATIONAL_DEX;8|FR_POKEDEX;6|FR_POKEMON;6|FR_RUNNING_SHOES;6|FRESHWATER;4|FULLHEAL;4|FULLRESTORE;4|FURRET;3|\
GANLONBERRY;4|GARDEVOIR;3|GASTLY;3|GENGAR;3|GEODUDE;3|GIRAFARIG;3|GIRL;7|GLALIE;3|GLASSORNAMENT;4|\
GLIGAR;3|GLITTERMAIL;4|GLITTERMAT;5|GLOOM;3|GNOTEMAT;5|GOGOGGLES;4|GOLBAT;3|GOLDEEN;3|\
GOLDSHIELD;4|GOLDTEETH;4|GOLDUCK;3|GOLEM;3|GOODROD;4|GOREBYSS;3|GORGEOUSPLANT;4|GRANBULL;3|\
GRASSCUSHION;4|GRAVELER;3|GREATBALL;4|GREENPOSTER;4|GREENSCARF;4|GREENSHARD;4|GREPABERRY;4|\
GRIMER;3|GROUDON;3|GROVYLE;3|GROWLITHE;3|GRUMPIG;3|GUARDSPEC;4|GULPIN;3|GULPINDOLL;5|GYARADOS;3|\
HARBORMAIL;4|HARDCHAIR;4|HARDDESK;5|HARDSTONE;4|HARIYAMA;3|HAUNTER;3|HEAL_POKEMON;8|HEALPOWDER;4|\
HEARTSCALE;4|HEAVYCHAIR;4|HEAVYDESK;5|HELIXFOSSIL;4|HERACROSS;3|HITMONCHAN;3|\
HITMONLEE;3|HITMONTOP;3|HM01;4|HM02;4|HM03;4|HM04;4|HM05;4|HM06;4|HM07;4|HM08;4|HONDEWBERRY;4|\
HO-OH;3|HOOTHOOT;3|HOPPIP;3|HORSEA;3|HOUNDOOM;3|HOUNDOUR;3|HPUP;4|HUNTAIL;3|HYPERPOTION;4|HYPNO;3|\
IAPAPABERRY;4|ICEHEAL;4|IGGLYBUFF;3|ILLUMISE;3|IRON;3|ITEMFINDER;4|IVYSAUR;3|JIGGLYPUFF;3|\
JIGGLYPUFFDOLL;5|JIRACHI;3|JOLTEON;3|JUMPLUFF;3|JUMPMAT;5|JYNX;3|KABUTO;3|KABUTOPS;3|KADABRA;3|\
KAKUNA;3|KANGASKHAN;3|KECLEON;3|KECLEONDOLL;5|KELPSYBERRY;4|KINGDRA;3|KINGLER;3|KINGSROCK;4|\
KIRLIA;3|KISSCUSHION;5|KISSPOSTER;5|KOFFING;3|KRABBY;3|KYOGRE;3|LAIRON;3|LANSATBERRY;4|LANTURN;3|\
LAPRAS;3|LAPRASDOLL;5|LARVITAR;3|LASTRESULT;9|LASTTALKED;9|LATIAS;3|LATIOS;3|LAVACOOKIE;4|LAXINCENSE;4|\
LEAFSTONE;4|LEDIAN;3|LEDYBA;3|LEFT;7|LEFTOVERS;4|LEMONADE;4|LEPPABERRY;4|LETTER;4|LICKITUNG;3|\
LIECHIBERRY;4|LIFTKEY;4|LIGHTBALL;4|LILEEP;3|LINOONE;3|LOMBRE;3|LONGPOSTER;5|LOTAD;3|LOTADDOLL;5|\
LOUDRED;3|LUCKYEGG;4|LUCKYPUNCH;4|LUDICOLO;3|LUGIA;3|LUMBERRY;3|LUNATONE;3|LUVDISC;3|\
LUXURYBALL;4|MACHAMP;3|MACHBIKE;4|MACHOBRACE;4|MACHOKE;3|MACHOP;3|MAGBY;3|MAGCARGO;3|\
MAGIKARP;3|MAGMAEMBLEM;4|MAGMAR;3|MAGNEMITE;3|MAGNET;4|MAGNETON;3|MAGOBERRY;4|MAGOSTBERRY;4|\
MAKUHITA;3|MANECTRIC;3|MANKEY;3|MANTINE;3|MAREEP;3|MARILL;3|MARILLDOLL;5|MAROWAK;3|MARSHTOMP;3|\
MASQUERAIN;3|MASTERBALL;4|MAWILE;3|MAXELIXIR;4|MAXETHER;4|MAXPOTION;4|MAXREPEL;4|MAXREVIVE;4|\
MECHMAIL;4|MEDICHAM;3|MEDITITE;3|MEGANIUM;3|MENTALHERB;4|MEOWTH;3|MEOWTHDOLL;5|METAGROSS;3|\
METALCOAT;4|METALPOWDER;4|METANG;3|METAPOD;3|METEORITE;4|MEW;3|MEWTWO;3|MIGHTYENA;3|MILOTIC;3|\
MILTANK;3|MINUN;3|MIRACLESEED;4|MISDREAVUS;3|MISSINGNO;3|MOLTRES;3|MOOMOOMILK;4|MOONSTONE;4|\
MRMIME;3|MSG_FIND;2|MSG_NOCLOSE;2|MSG_NORMAL;2|MSG_OBTAIN;2|MSG_SIGN;2|MSG_STANDARD;2|\
MSG_YESNO;2|MUDBALL;4|MUDKIP;3|MUDKIPDOLL;5|MUK;3|MURKROW;3|MYSTICTICKET;4|MYSTICWATER;4|\
NANABBERRY;4|NATU;3|NESTBALL;4|NETBALL;4|NEVERMELTICE;4|NIDOKING;3|NIDOQUEEN;3|NIDORANF;3|\
NIDORANM;3|NIDORINA;3|NIDORINO;3|NINCADA;3|NINETALES;3|NINJASK;3|NO;9|NOCTOWL;3|NOMELBERRY;4|NONE;4|\
NOSEPASS;3|NUGGET;4|NUMEL;3|NUZLEAF;3|OAKSPARCEL;4|OCTILLERY;3|ODDISH;3|OLDAMBER;4|OLDROD;4|OLDSEAMAP;4|\
OMANYTE;3|OMASTAR;3|ONIX;3|ORANBERRY;4|ORANGEMAIL;4|PAMTREBERRY;4|PARAS;3|PARASECT;3|PARLYZHEAL;4|\
PEARL;4|PECHABERRY;4|PELIPPER;3|PERSIAN;3|PERSIMBERRY;4|PETAYABERRY;4|PHANPY;3|PICHU;3|PICHUDOLL;5|\
PIDGEOT;3|PIDGEOTTO;3|PIDGEY;3|PIKACHU;3|PIKACHUDOLL;5|PIKACUSHION;5|PIKAPOSTER;5|PILOSWINE;3|\
PINAPBERRY;4|PINECO;3|PINKSCARF;4|PINSIR;3|PLAYER;1|PLAYERFACING;9|PLUSLE;3|POISONBARB;4|POKEBALL;4|\
POKEBLOCKCASE;4|POKEDOLL;5|POKEFLUTE;4|POKEMONCHAIR;5|POKEMONDESK;5|POLITOED;3|POLIWAG;3|POLIWHIRL;3|\
POLIWRATH;3|POMEGBERRY;4|PONYTA;3|POOCHYENA;3|PORYGON;3|PORYGON2;3|POTION;4|POWDERJAR;4|POWDERSNOWMAT;5|\
PPMAX;4|PPUP;4|PREMIERBALL;4|PRETTYCHAIR;5|PRETTYDESK;5|PRETTYFLOWERS;5|PRIMEAPE;3|PROTEIN;4|PSYDUCK;3|\
PUPITAR;3|QUAGSIRE;3|QUALOTBERRY;4|QUICKCLAW;4|QUILAVA;3|QWILFISH;3|RABUTABERRY;4|RAGGEDCHAIR;4|RAGGEDDESK;5|\
RAICHU;3|RAIKOU;3|RAINBOWPASS;4|RALTS;3|RAPIDASH;3|RARECANDY;4|RATICATE;3|RATTATA;3|RAWSTBERRY;4|\
RAYQUAZA;3|RAZZBERRY;4|REDBALLOON;5|REDBRICK;5|REDFLUTE;4|REDORB;4|REDPLANT;5|REDPOSTER;5|REDSCARF;4|\
REDSHARD;4|REDTENT;5|REGICE;3|REGICEDOLL;5|REGIROCK;3|REGIROCKDOLL;5|REGISTEEL;3|REGISTEELDOLL;5|\
RELICANTH;3|REMORAID;3|REPEATBALL;4|REPEL;4|RETROMAIL;4|REVIVALHERB;4|REVIVE;4|RHYDON;3|RHYDONDOLL;5|\
RHYHORN;3|RIGHT;7|RM1KEY;4|RM2KEY;4|RM4KEY;4|RM6KEY;4|ROOTFOSSIL;4|ROSELIA;3|ROUNDCUSHION;5|ROUNDTV;5|\
RS_BADGE_1;6|RS_BADGE_2;6|RS_BADGE_3;6|RS_BADGE_4;6|RS_BADGE_5;6|RS_BADGE_6;6|RS_BADGE_7;6|RS_BADGE_8;6|\
RS_POKEDEX;6|RS_POKEMON;6|RS_POKENAV;6|RS_RUNNING_SHOES;6|RUBY;4|SABLEYE;3|SACREDASH;4|SAFARIBALL;4|\
SALACBERRY;4|SALAMENCE;3|SANDORNAMENT;4|SANDSHREW;3|SANDSLASH;3|SAPPHIRE;4|SCANNER;4|SCEPTILE;3|SCIZOR;3|\
SCOPELENS;4|SCYTHER;3|SEADRA;3|SEAINCENSE;4|SEAKING;3|SEALEO;3|SEAPOSTER;5|SECRETKEY;4|SEEDOT;3|SEEDOTDOLL;5|\
SEEL;3|SENTRET;3|SEVIPER;3|SHADOWMAIL;4|SHARPBEAK;4|SHARPEDO;3|SHEDINJA;3|SHELGON;3|SHELLBELL;4|SHELLDER;3|\
SHIFTRY;3|SHOALSALT;4|SHOALSHELL;4|SHROOMISH;3|SHUCKLE;3|SHUPPET;3|SILCOON;3|SILKSCARF;4|SILPHSCOPE;4|\
SILVERPOWDER;4|SILVERSHIELD;4|SITRUSBERRY;4|SKARMORY;3|SKIPLOOM;3|SKITTY;3|SKITTYDOLL;5|SKYPOSTER;5|\
SLAKING;3|SLAKOTH;3|SLIDE;5|SLOWBRO;3|SLOWKING;3|SLOWPOKE;3|SLUGMA;3|SMALLCHAIR;5|SMALLDESK;5|SMEARGLE;3|\
SMOKEBALL;4|SMOOCHUM;3|SMOOCHUMDOLL;5|SNEASEL;3|SNORLAX;3|SNORLAXDOLL;5|SNORUNT;3|SNUBBULL;3|SODAPOP;4|\
SOFTSAND;4|SOLIDBOARD;5|SOLROCK;3|SOOTHEBELL;4|SOOTSACK;4|SOULDEW;4|SPEAROW;3|SPELLTAG;4|SPELONBERRY;4|\
SPHEAL;3|SPIKESMAT;5|SPINARAK;3|SPINCUSHION;5|SPINDA;3|SPINMAT;5|SPOINK;3|SQUIRTLE;3|SSTICKET;4|STAND;5|\
STANTLER;3|STARDUST;4|STARFBERRY;4|STARMIE;3|STARPIECE;4|STARYU;3|STEELIX;3|STICK;4|STORAGEKEY;4|SUDOWOODO;3|\
SUICUNE;3|SUNFLORA;3|SUNKERN;3|SUNSTONE;4|SUPERPOTION;4|SUPERREPEL;4|SUPERROD;4|SURFMAT;5|SURSKIT;3|SWABLU;3|\
SWABLUDOLL;5|SWALOT;3|SWAMPERT;3|SWELLOW;3|SWINUB;3|TAILLOW;3|TAMATOBERRY;4|TANGELA;3|TAUROS;3|TEA;4|TEACHYTV;4|\
TEDDIURSA;3|TENTACOOL;3|TENTACRUEL;3|THICKCLUB;4|THUNDERMAT;5|THUNDERSTONE;4|TIMERBALL;4|TINYMUSHROOM;4|\
TIRE;5|TM01;4|TM02;4|TM03;4|TM04;4|TM05;4|TM06;4|TM07;4|TM08;4|TM09;4|TM10;4|TM11;4|TM12;4|TM13;4|TM14;4|TM15;4|TM16;4|TM17;4|\
TM18;4|TM19;4|TM20;4|TM21;4|TM22;4|TM23;4|TM24;4|TM25;4|TM26;4|TM27;4|TM28;4|TM29;4|TM30;4|TM31;4|TM32;4|TM33;4|TM34;4|TM35;4|\
TM36;4|TM37;4|TM38;4|TM39;4|TM40;4|TM41;4|TM42;4|TM43;4|TM44;4|TM45;4|TM46;4|TM47;4|TM48;4|TM49;4|TM50;4|TMCASE;4|TOGEPI;3|\
TOGEPIDOLL;5|TOGETIC;3|TORCHIC;3|TORCHICDOLL;5|TORKOAL;3|TOTODILE;3|TOTODILEDOLL;5|TOWNMAP;4|TRAPINCH;3|\
TREECKO;3|TREECKODOLL;5|TRIPASS;4|TROPICALPLANT;5|TROPICMAIL;4|TROPIUS;3|TWISTEDSPOON;4|TYPHLOSION;3|\
TYRANITAR;3|TYROGUE;3|ULTRABALL;4|UMBREON;3|UNOWN;3|UP;7|UPGRADE;4|URSARING;3|VAPOREON;3|VENOMOTH;3|\
VENONAT;3|VENUSAUR;3|VENUSAURDOLL;5|VIBRAVA;3|VICTREEBEL;3|VIGOROTH;3|VILEPLUME;3|VOLBEAT;3|VOLTORB;3|\
VSSEEKER;4|VULPIX;3|WAILMER;3|WAILMERDOLL;5|WAILMERPAIL;4|WAILORD;3|WALREIN;3|WARTORTLE;3|WATERCUSHION;5|\
WATERSTONE;4|WATMELBERRY;4|WAVEMAIL;4|WEEDLE;3|WEEPINBELL;3|WEEZING;3|WEPEARBERRY;4|WHISCASH;3|WHISMUR;3|\
WHITEFLUTE;4|WHITEHERB;4|WIGGLYTUFF;3|WIKIBERRY;4|WINGULL;3|WOBBUFFET;3|WOODMAIL;4|WOOPER;3|WURMPLE;3|\
WYNAUT;3|WYNAUTDOLL;5|XACCURACY;4|XATTACK;4|XATU;3|XDEFEND;4|XSPECIAL;4|XSPEED;4|YANMA;3|YELLOWBALLOON;5|\
YELLOWBRICK;5|YELLOWFLUTE;4|YELLOWSCARF;4|YELLOWSHARD;4|YES;9|ZANGOOSE;3|ZAPDOS;3|ZIGZAGCUSHION;5|\
ZIGZAGOON;3|ZINC;4|ZUBAT;3|FALSE;6|TRUE;6");
					else if (end==3)
					{
						SendEditor(SCI_AUTOCSHOW,len,(LPARAM)
						           "apply_fixed_facing;1|dontfacehiro_step_down;1|dontfacehiro_step_left;1|dontfacehiro_step_right;1|\
dontfacehiro_step_up;1|earthquake;1|end;1|facehiro_step_down;1|facehiro_step_left;1|\
facehiro_step_right;1|facehiro_step_up;1|fall;1|fastjump_down;1|fastjump_left;1|fastjump_right;1|\
fastjump_up;1|fastslide_down;1|fastslide_left;1|fastslide_right;1|fastslide_up;1|halfstep_down;1|\
halfstep_left;1|halfstep_right;1|halfstep_up;1|halfstep2_down;1|halfstep2_left;1|halfstep2_right;1|\
halfstep2_up;1|hide;1|hide2;1|jump_down;1|jump_left;1|jump_right;1|jump_up;1|look_down;1|\
look_left;1|look_right;1|look_up;1|pause;1|remove_fixed_facing;1|slide_down;1|slide_left;1|\
slide_right;1|slide_up;1|slowjump_down;1|slowjump_left;1|slowjump_right;1|slowjump_up;1|\
slowslide_down;1|slowslide_left;1|slowslide_right;1|slowslide_up;1|slowstep_down;1|slowstep_left;1|\
slowstep_right;1|slowstep_up;1|speed_up;1|spin;1|step_down;1|step_left;1|step_right;1|step_up;1|\
teleport_from;1|teleport_to;1|tree_animate;1|waterfall_down;1|waterfall_left;1|waterfall_right;1|waterfall_up;1");
					}
				}
				break;
			case 1004: //compile, decompile is 1014
				if (*RomOpen2!=0)
				{
					i=(int)SendEditor(SCI_GETLENGTH,0,0);
					ptr=GlobalAlloc(GPTR,i+1);
					SendEditor(SCI_GETTEXT,i+1,(LPARAM)ptr);
					DetermineMode(RomOpen2);
					CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TCompile,(LPVOID)ptr,0,(LPVOID)&i);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3034),GetString2(3001),0x40);
				}
				break;
			}
		break;
	case WM_SIZE:
		if (hwnd==MainWnd)
		{
			SendMessage(StatusBar,msg,wParam,lParam);
			SendMessage(ToolBar,TB_AUTOSIZE,0,0);
#ifdef SCRAP
			SetWindowPos(Text,HWND_TOP,0,24,LOWORD(lParam)-32,HIWORD(lParam)-47,SWP_NOMOVE);
			SetWindowPos(ScrapPunk,HWND_TOP,LOWORD(lParam)-32,24,32,32,SWP_NOSIZE);
#else
			SetWindowPos(Text,HWND_TOP,0,24,LOWORD(lParam),HIWORD(lParam)-47,SWP_NOMOVE);
#endif
		}
		else
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		if (hwnd==MainWnd)
		{
			if (FlagsShown)
				SaveFlag();
			i=0;
			if (NotSaved)
			{
				i=MessageBox(MainWnd,GetString1(3017),GetString2(3018), MB_YESNOCANCEL | MB_ICONQUESTION);
				if (i == IDYES)
				{
					SendMessage(MainWnd,WM_COMMAND,ID_SAVE,0);
					if (NotSaved)
					{
						i = IDCANCEL;
					}
				}
			}
			if (i != IDCANCEL)
			{
				exiting();
				ExitProcess(0);
			}
		}
		else
		{
			ShowWindow(hwnd,SW_HIDE);
		}
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

void UpdateScintLine()
{
	char Lines[32];
	int  Width;
	int  NWidth;
	wsprintf(Lines,"_%i_",SendEditor(SCI_GETLINECOUNT,0,0));
	Width = (int)SendEditor(SCI_GETMARGINWIDTHN,0,0);
	NWidth = (int)SendEditor(SCI_TEXTWIDTH,STYLE_LINENUMBER,(LPARAM)Lines);
	if (Width!=NWidth) {
		SendEditor(SCI_SETMARGINWIDTHN,0,NWidth);
	}
}

void GoToLine(unsigned int line_number,unsigned int pos)
{
	SendEditor(SCI_ENSUREVISIBLEENFORCEPOLICY,line_number,0);
	if ((int)SendEditor(SCI_POSITIONFROMLINE,line_number+1,0)<=(int)(SendEditor(SCI_POSITIONFROMLINE,line_number,0)+pos))
		SendEditor(SCI_GOTOPOS,SendEditor(SCI_LINELENGTH,line_number,0),0);
	else
		SendEditor(SCI_GOTOPOS,pos+SendEditor(SCI_POSITIONFROMLINE,line_number,0),0);
}

char MatchCase=0,Upwards=0,RegExpr=0;

BOOL CALLBACK FindReplaceFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	char buf[32];
	char findbuf[4096],repbuf[4096];
	signed int found;
	unsigned int selstart,selend,count;
	WPARAM flags;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0x1: //Find
			GetDlgItemText(FindReplaceWin,6,findbuf,4095);
			GetDlgItemText(FindReplaceWin,8,repbuf,4095);
			flags=0;
			if (MatchCase)flags|=SCFIND_MATCHCASE;
			if (RegExpr)flags|=SCFIND_REGEXP|SCFIND_POSIX;
			SendEditor(SCI_SETSEARCHFLAGS,flags,0);
			SendEditor(SCI_SETTARGETSTART,0,0);
			SendEditor(SCI_SETTARGETEND,SendEditor(SCI_GETLENGTH,0,0),0);
			if (Upwards)
			{
				SendEditor(SCI_SETTARGETSTART,SendEditor(SCI_GETSELECTIONSTART,0,0),0);
				SendEditor(SCI_SETTARGETEND,0,0);
			}
			else
			{
				SendEditor(SCI_SETTARGETSTART,SendEditor(SCI_GETSELECTIONEND,0,0),0);
				SendEditor(SCI_SETTARGETEND,SendEditor(SCI_GETLENGTH,0,0),0);
			}
			found=(int)SendEditor(SCI_SEARCHINTARGET,strlen(findbuf),(LPARAM)findbuf);
			if (found==-1)
			{
				MessageBox(FindReplaceWin,GetString1(3035),"PKSVUI",0x40);
				if (Upwards)
				{
					SendEditor(SCI_SETSEARCHFLAGS,flags,0);
					SendEditor(SCI_SETTARGETSTART,SendEditor(SCI_GETLENGTH,0,0),0);
					SendEditor(SCI_SETTARGETEND,0,0);
					found=(int)SendEditor(SCI_SEARCHINTARGET,strlen(findbuf),(LPARAM)findbuf);
				}
				else
				{
					SendEditor(SCI_SETSEARCHFLAGS,flags,0);
					SendEditor(SCI_SETTARGETSTART,0,0);
					SendEditor(SCI_SETTARGETEND,SendEditor(SCI_GETLENGTH,0,0),0);
					found=(int)SendEditor(SCI_SEARCHINTARGET,strlen(findbuf),(LPARAM)findbuf);
				}
				if (found==-1)
				{
					MessageBox(FindReplaceWin,GetString1(3036),"PKSVUI",0x40);
				} else {
					SendEditor(SCI_SETSEL,SendEditor(SCI_GETTARGETSTART,0,0),SendEditor(SCI_GETTARGETEND,0,0));
				}
			} else {
				SendEditor(SCI_SETSEL,SendEditor(SCI_GETTARGETSTART,0,0),SendEditor(SCI_GETTARGETEND,0,0));
			}
			SendEditor(SCI_ENSUREVISIBLEENFORCEPOLICY,SendEditor(SCI_LINEFROMPOSITION,SendEditor(SCI_GETSELECTIONSTART,0,0),0),0);
			SendEditor(SCI_ENSUREVISIBLEENFORCEPOLICY,SendEditor(SCI_LINEFROMPOSITION,SendEditor(SCI_GETSELECTIONEND,0,0),0),0);
			break;
		case 0x2: //Replace
			GetDlgItemText(FindReplaceWin,6,findbuf,4095);
			GetDlgItemText(FindReplaceWin,8,repbuf,4095);
			selstart=(int)SendEditor(SCI_GETSELECTIONSTART,0,0);
			selend=(int)SendEditor(SCI_GETSELECTIONEND,0,0);
			SendEditor(SCI_SETTARGETSTART,selstart,0);
			SendEditor(SCI_SETTARGETEND,selend,0);
			if (selstart!=selend)
			{
				if (RegExpr)SendEditor(SCI_REPLACETARGETRE,(WPARAM)-1,(LPARAM)repbuf);
				else       SendEditor(SCI_REPLACETARGET,(WPARAM)-1,(LPARAM)repbuf);
			}
			break;
		case 0x3: //Replace All
			GetDlgItemText(FindReplaceWin,6,findbuf,4095);
			GetDlgItemText(FindReplaceWin,8,repbuf,4095);
			found=0;
			count=0;
			flags=0;
			if (MatchCase)flags|=SCFIND_MATCHCASE;
			if (RegExpr)flags|=SCFIND_REGEXP|SCFIND_POSIX;
			if (*findbuf!=0)
				while (found!=-1)
				{
					SendEditor(SCI_SETSEARCHFLAGS,flags,0);
					SendEditor(SCI_SETTARGETSTART,0,0);
					SendEditor(SCI_SETTARGETEND,SendEditor(SCI_GETLENGTH,0,0),0);
					found=(int)SendEditor(SCI_SEARCHINTARGET,strlen(findbuf),(LPARAM)findbuf);
					if (found!=-1)
					{
						count++;
						if (RegExpr)
							SendEditor(SCI_REPLACETARGETRE,-1,(LPARAM)repbuf);
						else
							SendEditor(SCI_REPLACETARGET,-1,(LPARAM)repbuf);
					}
				}
			sprintf(buf,GetString1(3037),count);
			MessageBox(FindReplaceWin,buf,GetString1(3038),0x40);
			break;
		case 0x4: //Cancel
			ShowWindow(FindReplaceWin,SW_HIDE);
			break;
		case 0x9: //Match case
			MatchCase=(IsDlgButtonChecked(FindReplaceWin,LOWORD(wParam))==BST_CHECKED);
			break;
		case 0xA: //Search up
			Upwards=(IsDlgButtonChecked(FindReplaceWin,LOWORD(wParam))==BST_CHECKED);
			break;
		case 0xB: //Regex
			RegExpr=(IsDlgButtonChecked(FindReplaceWin,LOWORD(wParam))==BST_CHECKED);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(FindReplaceWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}

BOOL CALLBACK GotoFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	char linebuf[4096],posbuf[4096];
	unsigned int line,pos;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0x1: //Goto
			GetDlgItemText(GotoWin,3,linebuf,4095);
			GetDlgItemText(GotoWin,4,posbuf,4095);
			sscanf(linebuf,"%u",&line);
			sscanf(posbuf,"%u",&pos);
			GoToLine(line,pos);
			//don't break;
		case 0x2: //Cancel
			ShowWindow(GotoWin,SW_HIDE);
			EnableWindow(MainWnd,1);
			SetFocus(Text);
			break;
		case 2000:
			ShowWindow(GotoWin,SW_HIDE);
			EnableWindow(MainWnd,1);
			SetFocus(Text);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(GotoWin,SW_HIDE);
		EnableWindow(MainWnd,1);
		SetFocus(Text);
		break;
	default:
		return 0;
	}
	return 1;
}
unsigned int FindFreeSpace(char*romname,unsigned int len,unsigned int start)
{
	HANDLE RomFile;
	unsigned int i,j=0;
	unsigned char cr,src2;
	DWORD read;
	src2=search;
	RomFile=CreateFile(romname,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	i=start&0x00ffffff;
	SetFilePointer(RomFile,i,NULL,FILE_BEGIN);
	while (i<0x1000000)
	{
		ReadFile(RomFile,&cr,1,&read,NULL);
		if (cr==src2)
		{
			j++;
		}else {
			j=0;
		}
		i++;
		if (j>len) {
			break;    //Yes, larger than
		}
	}
	i-=j;
	i++;
	CloseHandle(RomFile);
	return (0x08000000|i);
}
char doing=0;
DWORD WINAPI FindInThread(LPVOID lp)
{
	unsigned int*lenpos;
	unsigned int pos,len;
	char posbuf[32];
	lenpos=lp;
	len=*lenpos;
	pos=*(lenpos+4);
	SetDlgItemText(FFSWin,5,GetString1(3039));
	pos=FindFreeSpace(RomOpen2,len,pos);
	sprintf(posbuf,"0x%X",pos);
	SetDlgItemText(FFSWin,5,posbuf);
	GlobalFree(lenpos);
	EnableWindow(GetDlgItem(FFSWin,1),1);
	doing=0;
	return 0;
}
BOOL CALLBACK FFSFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	char posbuf[41],lenbuf[41];
	unsigned int pos,len;
	unsigned int*ptr;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0x1: //Find
			if (*RomOpen2!=0)
			{
				GetDlgItemText(FFSWin,3,posbuf,40);
				GetDlgItemText(FFSWin,4,lenbuf,40);
				if (posbuf[0]=='0'&&posbuf[1]=='x')
				{
					sscanf(posbuf+2,"%X",&pos);
				}
				else
				{
					sscanf(posbuf,"%X",&pos);
				}
				if (lenbuf[0]=='0'&&lenbuf[1]=='x')
				{
					sscanf(lenbuf+2,"%X",&len);
				}
				else
				{
					sscanf(lenbuf,"%X",&len);
				}
				ptr=GlobalAlloc(GPTR,8);
				*ptr=len;
				*(ptr+4)=pos;
				if (!doing)
				{
					EnableWindow(GetDlgItem(FFSWin,1),0);
					doing=1;
					CreateThread(NULL,0,FindInThread,ptr,0,(LPVOID)&pos);
				}
			}
			else
			{
				MessageBox(FFSWin,GetString1(3027),GetString2(3001),0x10);
			}
			break;
		case 0x6:
			search=0xFF;
			break;
		case 0x7:
			search=0x00;
			break;
		case 0x2: //Cancel
			ShowWindow(FFSWin,SW_HIDE);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(FFSWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}
char ChosenMode=17;
BOOL CALLBACK DecFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	char posbuf[41];
	unsigned int i,pos,bank,block,tbank;
	//STARTUPINFO si;
	//PROCESS_INFORMATION pi;
	//DWORD read;
	//unsigned int fsize;
	//HANDLE fileM;
	char*ptr;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0x1: //Decompile
			if (*RomOpen2!=0)
			{
				if (NotSaved)
				{
					i=MessageBox(MainWnd,GetString1(3017),GetString2(3040), MB_YESNOCANCEL | MB_ICONQUESTION);
					if (i == IDYES)
					{
						SendMessage(MainWnd,WM_COMMAND,ID_SAVE,0);
						if (NotSaved)
						{
							i = IDCANCEL;
						}
					}
					if (i == IDCANCEL)break;
				}
				GetDlgItemText(DecWin,3,posbuf,40);
				/*SetCurrentDirectory(DirBuffer);
				strcpy(SuperBuffer,"pksv \"");
				strcat(SuperBuffer,RomOpen2);
				strcat(SuperBuffer,"\" \"");
				strcat(SuperBuffer,posbuf);
				strcat(SuperBuffer,"\" _d_.pks.tmp");
				ZeroMemory(&si,sizeof(si));
				ZeroMemory(&pi,sizeof(pi));
				si.cb=sizeof(si);
				strcpy(GreatBuffer,DirBuffer);
				strcat(GreatBuffer,"pksv.exe");
				if(!CreateProcess(GreatBuffer,SuperBuffer,NULL,NULL,0,DETACHED_PROCESS,NULL,NULL,&si,&pi))
				{
				  MessageBox(MainWnd,GetString1(3041),GetString2(3001),0x10);
				  return 0;
				}
				WaitForSingleObject(pi.hProcess,60000);
				strcpy(FileOpen2,"_d_.pks.tmp");
				strcpy(FileOpen3,"_d_.pks.tmp");
				strcpy(FileOpen,FileOpen3);
				strcat(FileOpen2," - Score_Under's PKSV-UI");
				SetWindowText(MainWnd,FileOpen2);
				fileM=CreateFile(FileOpen3,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
				if(GetLastError()!=0)
				{
				  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,(char*)&ptr,2000,NULL);
				  MessageBox(MainWnd,ptr,ptr,0x10);
				  LocalFree(ptr);
				  return 0;
				}
				fsize=SetFilePointer(fileM,0,NULL,FILE_END);
				ptr=GlobalAlloc(GPTR,fsize+1);
				SetFilePointer(fileM,0,NULL,FILE_BEGIN);
				ReadFile(fileM,ptr,fsize,&read,NULL);
				if(read!=fsize)
				{
				  MessageBox(MainWnd,GetString1(3015),GetString2(3001),0x10);
				}*/
				strcpy(FileOpen2,GetString1(3008));
				strcat(FileOpen2," - Score_Under's PKSV-UI");
				SetWindowText(MainWnd,FileOpen2);
				strcpy(FileOpen,GetString1(3008));
				strcpy(FileOpen3,FileOpen);
				i=0;
				SaveRegSettingStr("Decompiler","Offset",posbuf);
				sscanf(posbuf,"%X",&i);
				if (!i)
				{
					sscanf(posbuf,"$%X",&i);
					if (!i)sscanf(posbuf,"0x%X",&i);
				}
				if (IsDlgButtonChecked(DecWin,12)==BST_CHECKED)
				{
					if (IsDlgButtonChecked(DecWin,13)==BST_CHECKED)
						NewMode(GOLD);
					else if (IsDlgButtonChecked(DecWin,14)==BST_CHECKED)
						NewMode(RUBY);
					else if (IsDlgButtonChecked(DecWin,15)==BST_CHECKED)
						NewMode(DIAMOND);
					else if (IsDlgButtonChecked(DecWin,16)==BST_CHECKED)
						NewMode(CRYSTAL);
					else
						NewMode(FIRE_RED);
				}
				else
					DetermineMode(RomOpen2);
				GetDlgItemText(DecWin,19,GreatBuffer,sizeof(GreatBuffer));
				block=-1;
				sscanf(GreatBuffer,"0x%X",&block);
				if (block==-1)
					sscanf(GreatBuffer,"%X",&block);
				if (block==-1)
					sscanf(GreatBuffer,"$%X",&block);
				SaveRegSetting("Decompiler","Use Dynamic",IsDlgButtonChecked(DecWin,18));
				SaveRegSettingStr("Decompiler","Dynamic Offset",GreatBuffer);
				SetDynamic(IsDlgButtonChecked(DecWin,18),block);
				if (IsDlgButtonChecked(DecWin,9)==BST_CHECKED)
					ptr=decompileASM(RomOpen2,i);
				else if (IsDlgButtonChecked(DecWin,10)==BST_CHECKED)
					ptr=decompileLevel(RomOpen2,i);
				else
					ptr=decompile(RomOpen2,i,0);

				SendEditor(SCI_CANCEL,0,0);
				SendEditor(SCI_SETUNDOCOLLECTION,0,0);
				SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_CLEARALL,0,0);
				SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
				SendEditor(SCI_SETSCROLLWIDTH,2048,0);
				if (ptr)
					SendEditor(SCI_ADDTEXT,strlen(ptr),(LPARAM)ptr);
				SendEditor(SCI_SETUNDOCOLLECTION,1,0);
				SendEditor(EM_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_SETSAVEPOINT,0,0);
				SendEditor(SCI_GOTOPOS,0,0);
				SendEditor(SCI_CHOOSECARETX,0,0);
				if (ptr)
					GlobalFree(ptr);
				/*
				CloseHandle(fileM);
				*/
				ShowWindow(DecWin,SW_HIDE);
				EnableWindow(MainWnd,1);
				SetFocus(Text);
			}
			else
			{
				MessageBox(FFSWin,GetString1(3027),GetString2(3001),0x10);
			}
			break;
		case 0x7:
			GetDlgItemText(DecWin,4,posbuf,40);
			sscanf(posbuf,"%x",&bank);
			GetDlgItemText(DecWin,5,posbuf,40);
			sscanf(posbuf,"%u",&block);
			GetDlgItemText(DecWin,6,posbuf,40);
			sscanf(posbuf,"%u",&tbank);
			pos=PointerToOffset((tbank<<16)|(block<<8)|bank);
			if (pos!=0xFFFFFFFF)
			{
				sprintf(posbuf,"0x%X",pos);
				SetDlgItemText(DecWin,3,posbuf);
			}
			else
			{
				SetDlgItemText(DecWin,3,"-ERROR-");
			}
			break;
		case 12:
			if (IsDlgButtonChecked(DecWin,12)==BST_CHECKED)
			{
				EnableWindow(GetDlgItem(DecWin,13),1);
				EnableWindow(GetDlgItem(DecWin,14),1);
				EnableWindow(GetDlgItem(DecWin,16),1);
				EnableWindow(GetDlgItem(DecWin,17),1);
				CheckDlgButton(DecWin,ChosenMode,1);
			}
			else
			{
				EnableWindow(GetDlgItem(DecWin,13),0);
				EnableWindow(GetDlgItem(DecWin,14),0);
				EnableWindow(GetDlgItem(DecWin,16),0);
				EnableWindow(GetDlgItem(DecWin,17),0);
				CheckDlgButton(DecWin,ChosenMode,0);
			}
			break;
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			if (IsDlgButtonChecked(DecWin,LOWORD(wParam)))
				ChosenMode=(char)LOWORD(wParam);
			break;
		case 0x2: //Cancel
		case 2000:
			ShowWindow(DecWin,SW_HIDE);
			//EnableWindow(MainWnd,1);
			SetFocus(Text);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(DecWin,SW_HIDE);
		//EnableWindow(MainWnd,1);
		SetFocus(Text);
		break;
	default:
		return 0;
	}
	return 1;
}

void FillItems(HWND hwndList)
{
	int i;
	char *pVarName[] = {"NONE","MASTERBALL","ULTRABALL","GREATBALL","POKEBALL",
	                    "SAFARIBALL","NETBALL","DIVEBALL","NESTBALL","REPEATBALL",
	                    "TIMERBALL","LUXURYBALL","PREMIERBALL","POTION","ANTIDOTE",
	                    "BURNHEAL","ICEHEAL","AWAKENING","PARLYZHEAL","FULLRESTORE",
	                    "MAXPOTION","HYPERPOTION","SUPERPOTION","FULLHEAL","REVIVE",
	                    "MAXREVIVE","FRESHWATER","SODAPOP","LEMONADE","MOOMOOMILK",
	                    "ENERGYPOWDER","ENERGYROOT","HEALPOWDER","REVIVALHERB","ETHER",
	                    "MAXETHER","ELIXIR","MAXELIXIR","LAVACOOKIE","BLUEFLUTE","YELLOWFLUTE",
	                    "REDFLUTE","BLACKFLUTE","WHITEFLUTE","BERRYJUICE","SACREDASH","SHOALSALT",
	                    "SHOALSHELL","REDSHARD","BLUESHARD","YELLOWSHARD","GREENSHARD","HPUP",
	                    "PROTEIN","IRON","CARBOS","CALCIUM","RARECANDY","PPUP","ZINC",
	                    "PPMAX","GUARDSPEC","DIREHIT","XATTACK","XDEFEND","XSPEED","XACCURACY",
	                    "XSPECIAL","POKEDOLL","FLUFFYTAIL","SUPERREPEL","MAXREPEL","ESCAPEROPE",
	                    "REPEL","SUNSTONE","MOONSTONE","FIRESTONE","THUNDERSTONE","WATERSTONE",
	                    "LEAFSTONE","TINYMUSHROOM","BIGMUSHROOM","PEARL","BIGPEARL","STARDUST",
	                    "STARPIECE","NUGGET","HEARTSCALE","ORANGEMAIL","HARBORMAIL","GLITTERMAIL",
	                    "MECHMAIL","WOODMAIL","WAVEMAIL","BEADMAIL","SHADOWMAIL","TROPICMAIL",
	                    "DREAMMAIL","FABMAIL","RETROMAIL","CHERIBERRY","CHESTOBERRY","PECHABERRY",
	                    "RAWSTBERRY","ASPEARBERRY","LEPPABERRY","ORANBERRY","PERSIMBERRY","LUMBERRY",
	                    "SITRUSBERRY","FIGYBERRY","WIKIBERRY","MAGOBERRY","AGUAVBERRY","IAPAPABERRY",
	                    "RAZZBERRY","BLUKBERRY","NANABBERRY","WEPEARBERRY","PINAPBERRY","POMEGBERRY",
	                    "KELPSYBERRY","QUALOTBERRY","HONDEWBERRY","GREPABERRY","TAMATOBERRY",
	                    "CORNNBERRY","MAGOSTBERRY","RABUTABERRY","NOMELBERRY","SPELONBERRY",
	                    "PAMTREBERRY","WATMELBERRY","DURINBERRY","BELUEBERRY","LIECHIBERRY",
	                    "GANLONBERRY","SALACBERRY","PETAYABERRY","APICOTBERRY","LANSATBERRY",
	                    "STARFBERRY","ENIGMABERRY","BRIGHTPOWDER","WHITEHERB","MACHOBRACE",
	                    "EXPSHARE","QUICKCLAW","SOOTHEBELL","MENTALHERB","CHOICEBAND",
	                    "KINGSROCK","SILVERPOWDER","AMULETCOIN","CLEANSETAG","SOULDEW",
	                    "DEEPSEATOOTH","DEEPSEASCALE","SMOKEBALL","EVERSTONE","FOCUSBAND",
	                    "LUCKYEGG","SCOPELENS","METALCOAT","LEFTOVERS","DRAGONSCALE","LIGHTBALL",
	                    "SOFTSAND","HARDSTONE","MIRACLESEED","BLACKGLASSES","BLACKBELT","MAGNET",
	                    "MYSTICWATER","SHARPBEAK","POISONBARB","NEVERMELTICE","SPELLTAG","TWISTEDSPOON",
	                    "CHARCOAL","DRAGONFANG","SILKSCARF","UPGRADE","SHELLBELL","SEAINCENSE","LAXINCENSE",
	                    "LUCKYPUNCH","METALPOWDER","THICKCLUB","STICK","REDSCARF","BLUESCARF",
	                    "PINKSCARF","GREENSCARF","YELLOWSCARF","MACHBIKE","COINCASE","ITEMFINDER",
	                    "OLDROD","GOODROD","SUPERROD","SSTICKET","CONTESTPASS","WAILMERPAIL",
	                    "DEVONGOODS","SOOTSACK","BASEMENTKEY","ACROBIKE","POKEBLOCKCASE","LETTER",
	                    "EONTICKET","REDORB","BLUEORB","SCANNER","GOGOGGLES","METEORITE","RM1KEY",
	                    "RM2KEY","RM4KEY","RM6KEY","STORAGEKEY","ROOTFOSSIL","CLAWFOSSIL",
	                    "DEVONSCOPE","TM01","TM02","TM03","TM04","TM05","TM06","TM07","TM08",
	                    "TM09","TM10","TM11","TM12","TM13","TM14","TM15","TM16","TM17","TM18",
	                    "TM19","TM20","TM21","TM22","TM23","TM24","TM25","TM26","TM27","TM28",
	                    "TM29","TM30","TM31","TM32","TM33","TM34","TM35","TM36","TM37","TM38",
	                    "TM39","TM40","TM41","TM42","TM43","TM44","TM45","TM46","TM47","TM48",
	                    "TM49","TM50","HM01","HM02","HM03","HM04","HM05","HM06","HM07","HM08",
	                    "OAKSPARCEL","POKEFLUTE","SECRETKEY","BIKEVOUCHER","GOLDTEETH",
	                    "OLDAMBER","CARDKEY","LIFTKEY","HELIXFOSSIL","DOMEFOSSIL","SILPHSCOPE",
	                    "BICYCLE","TOWNMAP","VSSEEKER","FAMECHECKER","TMCASE","BERRYPOUCH",
	                    "TEACHYTV","TRIPASS","RAINBOWPASS","TEA","MYSTICTICKET","AURORATICKET",
	                    "POWDERJAR","RUBY","SAPPHIRE","MAGMAEMBLEM","OLDSEAMAP"
	                   };
	SendMessage(hwndList, CB_RESETCONTENT, 0, 0);
	for (i=0; i<sizeof(pVarName)/sizeof(char*); i++)
		SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)pVarName[i]);

}


DWORD WINAPI FindItemScript(LPVOID x)
{
	//unsigned int fspace;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Item");
	if (!*GreatBuffer)strcpy(GreatBuffer,"MASTERBALL");
	GetInput2(GetString1(3042),GetString2(3043),GreatBuffer,FillItems);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
		strcpy(SuperBuffer,GreatBuffer);
		LoadRegSettingStr("ScriptGen","ItemAmt");
		if (!*GreatBuffer)strcpy(GreatBuffer,"1");
		GetInput(GetString1(3042),GetString2(3044),GreatBuffer);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","ItemAmt",GreatBuffer);
			//fspace=FindFreeSpace(RomOpen2,0x100,0x740000);
			sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\ncopyvarifnotzero 0x8000 %s\ncopyvarifnotzero 0x8001 %s\ncallstd MSG_FIND\nend\n",SuperBuffer,GreatBuffer);
			SendEditor(SCI_CANCEL,0,0);
			SendEditor(SCI_SETUNDOCOLLECTION,0,0);
			SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
			SendEditor(SCI_CLEARALL,0,0);
			SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
			SendEditor(SCI_SETSCROLLWIDTH,2048,0);
			SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
			SendEditor(SCI_SETUNDOCOLLECTION,1,0);
			SendEditor(EM_EMPTYUNDOBUFFER,0,0);
			SendEditor(SCI_SETSAVEPOINT,0,0);
			SendEditor(SCI_GOTOPOS,0,0);
			SendEditor(SCI_CHOOSECARETX,0,0);
			SetForegroundWindow(MainWnd);
			SetFocus(Text);
			SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
			SendEditor(SCI_SETSEL,0,0);
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

void FillMsgs(HWND hwndList)
{
	int i;
	char *pVarName[] = {"MSG_ITEM","MSG_NORMAL","MSG_YESNO","MSG_NOCLOSE","MSG_SIGN","MSG_STANDARD","MSG_FIND","MSG_OBTAIN"};
	SendMessage(hwndList, CB_RESETCONTENT, 0, 0);
	for (i=0; i<sizeof(pVarName)/sizeof(char*); i++)
		SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)pVarName[i]);

}

DWORD WINAPI TalkScript(LPVOID x)
{
	//unsigned int fspace;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Message");
	if (!*GreatBuffer)strcpy(GreatBuffer,"Hi, I'm a cow[.]\\nMoo.");
	GetInput(GetString1(3057),GetString2(3058),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Message",GreatBuffer);
		strcpy(SuperBuffer,GreatBuffer);
		LoadRegSettingStr("ScriptGen","MessageType");
		if (!*GreatBuffer)strcpy(GreatBuffer,"MSG_NORMAL");
		GetInput2(GetString1(3057),GetString2(3059),GreatBuffer,FillMsgs);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","MessageType",GreatBuffer);
			//fspace=FindFreeSpace(RomOpen2,0x400,0x740000);
			sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlock\nfaceplayer\nmsgbox @text ' %s\ncallstd %s\nrelease\nend\n\n#org @text\n= %s\n",
			        SuperBuffer,GreatBuffer,SuperBuffer);
			SendEditor(SCI_CANCEL,0,0);
			SendEditor(SCI_SETUNDOCOLLECTION,0,0);
			SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
			SendEditor(SCI_CLEARALL,0,0);
			SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
			SendEditor(SCI_SETSCROLLWIDTH,2048,0);
			SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
			SendEditor(SCI_SETUNDOCOLLECTION,1,0);
			SendEditor(EM_EMPTYUNDOBUFFER,0,0);
			SendEditor(SCI_SETSAVEPOINT,0,0);
			SendEditor(SCI_GOTOPOS,0,0);
			SendEditor(SCI_CHOOSECARETX,0,0);
			SetForegroundWindow(MainWnd);
			SetFocus(Text);
			SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
			SendEditor(SCI_SETSEL,0,0);
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI HealScript(LPVOID x)
{
	//unsigned int fspace;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	//fspace=FindFreeSpace(RomOpen2,0x100,0x740000);
	sprintf(MegaBuffer,"#dyn 0x740000\n#org @\nlockall\nmessage @healmsg\nshowmsg\nwaitbutton\nfadescreen 1\nclosemsg\nspecial 0\nfadescreen 0\nmsgbox @bettermsg\ncallstd MSG_NORMAL\nreleaseall\nend\n\n#org @healmsg\n= Your POKéMON look tired[.]\\nYou should give them a rest.\n\n#org @bettermsg\n= There. All better now!\n");
	SendEditor(SCI_CANCEL,0,0);
	SendEditor(SCI_SETUNDOCOLLECTION,0,0);
	SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
	SendEditor(SCI_CLEARALL,0,0);
	SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
	SendEditor(SCI_SETSCROLLWIDTH,2048,0);
	SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
	SendEditor(SCI_SETUNDOCOLLECTION,1,0);
	SendEditor(EM_EMPTYUNDOBUFFER,0,0);
	SendEditor(SCI_SETSAVEPOINT,0,0);
	SendEditor(SCI_GOTOPOS,0,0);
	SendEditor(SCI_CHOOSECARETX,0,0);
	SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
	SendEditor(SCI_SETSEL,0,0);
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI GiveItemScript(LPVOID x)
{
	char*buf1,*buf2;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Item");
	if (!*GreatBuffer)strcpy(GreatBuffer,"MASTERBALL");
	GetInput2(GetString1(3045),GetString2(3043),GreatBuffer,FillItems);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
		strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","ItemAmt");
		if (!*GreatBuffer)strcpy(GreatBuffer,"1");
		GetInput(GetString1(3045),GetString2(3044),GreatBuffer);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","ItemAmt",GreatBuffer);
			strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","Flag");
			if (!*GreatBuffer)strcpy(GreatBuffer,"0x250");
			GetInput(GetString1(3045),GetString2(3046),GreatBuffer);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
				//fspace=FindFreeSpace(RomOpen2,0x100,0x740000);
				sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\ncheckflag %s\nif 0x1 jump :end\ncopyvarifnotzero 0x8000 %s\ncopyvarifnotzero 0x8001 %s\ncallstd MSG_OBTAIN\nsetflag %s\n:end\nend\n",GreatBuffer,buf1,buf2,GreatBuffer);
				SendEditor(SCI_CANCEL,0,0);
				SendEditor(SCI_SETUNDOCOLLECTION,0,0);
				SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_CLEARALL,0,0);
				SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
				SendEditor(SCI_SETSCROLLWIDTH,2048,0);
				SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
				SendEditor(SCI_SETUNDOCOLLECTION,1,0);
				SendEditor(EM_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_SETSAVEPOINT,0,0);
				SendEditor(SCI_GOTOPOS,0,0);
				SendEditor(SCI_CHOOSECARETX,0,0);
				SetForegroundWindow(MainWnd);
				SetFocus(Text);
				SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
				SendEditor(SCI_SETSEL,0,0);
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

void FillPokemon(HWND hwndList)
{
	int i;
	char *pVarName[] = {"MISSINGNO","BULBASAUR","IVYSAUR","VENUSAUR",
	                    "CHARMANDER","CHARMELEON","CHARIZARD","SQUIRTLE","WARTORTLE",
	                    "BLASTOISE","CATERPIE","METAPOD","BUTTERFREE","WEEDLE","KAKUNA",
	                    "BEEDRILL","PIDGEY","PIDGEOTTO","PIDGEOT","RATTATA","RATICATE",
	                    "SPEAROW","FEAROW","EKANS","ARBOK","PIKACHU","RAICHU","SANDSHREW",
	                    "SANDSLASH","NIDORANF","NIDORINA","NIDOQUEEN","NIDORANM","NIDORINO",
	                    "NIDOKING","CLEFAIRY","CLEFABLE","VULPIX","NINETALES","JIGGLYPUFF",
	                    "WIGGLYTUFF","ZUBAT","GOLBAT","ODDISH","GLOOM","VILEPLUME","PARAS",
	                    "PARASECT","VENONAT","VENOMOTH","DIGLETT","DUGTRIO","MEOWTH",
	                    "PERSIAN","PSYDUCK","GOLDUCK","MANKEY","PRIMEAPE","GROWLITHE",
	                    "ARCANINE","POLIWAG","POLIWHIRL","POLIWRATH","ABRA","KADABRA",
	                    "ALAKAZAM","MACHOP","MACHOKE","MACHAMP","BELLSPROUT","WEEPINBELL",
	                    "VICTREEBEL","TENTACOOL","TENTACRUEL","GEODUDE","GRAVELER","GOLEM",
	                    "PONYTA","RAPIDASH","SLOWPOKE","SLOWBRO","MAGNEMITE","MAGNETON",
	                    "FARFETCHD","DODUO","DODRIO","SEEL","DEWGONG","GRIMER","MUK",
	                    "SHELLDER","CLOYSTER","GASTLY","HAUNTER","GENGAR","ONIX","DROWZEE",
	                    "HYPNO","KRABBY","KINGLER","VOLTORB","ELECTRODE","EXEGGCUTE",
	                    "EXEGGUTOR","CUBONE","MAROWAK","HITMONLEE","HITMONCHAN","LICKITUNG",
	                    "KOFFING","WEEZING","RHYHORN","RHYDON","CHANSEY","TANGELA","KANGASKHAN",
	                    "HORSEA","SEADRA","GOLDEEN","SEAKING","STARYU","STARMIE","MRMIME",
	                    "SCYTHER","JYNX","ELECTABUZZ","MAGMAR","PINSIR","TAUROS","MAGIKARP",
	                    "GYARADOS","LAPRAS","DITTO","EEVEE","VAPOREON","JOLTEON","FLAREON",
	                    "PORYGON","OMANYTE","OMASTAR","KABUTO","KABUTOPS","AERODACTYL",
	                    "SNORLAX","ARTICUNO","ZAPDOS","MOLTRES","DRATINI","DRAGONAIR",
	                    "DRAGONITE","MEWTWO","MEW","CHIKORITA","BAYLEEF","MEGANIUM","CYNDAQUIL",
	                    "QUILAVA","TYPHLOSION","TOTODILE","CROCONAW","FERALIGATR","SENTRET",
	                    "FURRET","HOOTHOOT","NOCTOWL","LEDYBA","LEDIAN","SPINARAK","ARIADOS",
	                    "CROBAT","CHINCHOU","LANTURN","PICHU","CLEFFA","IGGLYBUFF","TOGEPI",
	                    "TOGETIC","NATU","XATU","MAREEP","FLAAFFY","AMPHAROS","BELLOSSOM",
	                    "MARILL","AZUMARILL","SUDOWOODO","POLITOED","HOPPIP","SKIPLOOM",
	                    "JUMPLUFF","AIPOM","SUNKERN","SUNFLORA","YANMA","WOOPER","QUAGSIRE",
	                    "ESPEON","UMBREON","MURKROW","SLOWKING","MISDREAVUS","UNOWN",
	                    "WOBBUFFET","GIRAFARIG","PINECO","FORRETRESS","DUNSPARCE","GLIGAR",
	                    "STEELIX","SNUBBULL","GRANBULL","QWILFISH","SCIZOR","SHUCKLE","HERACROSS",
	                    "SNEASEL","TEDDIURSA","URSARING","SLUGMA","MAGCARGO","SWINUB","PILOSWINE",
	                    "CORSOLA","REMORAID","OCTILLERY","DELIBIRD","MANTINE","SKARMORY",
	                    "HOUNDOUR","HOUNDOOM","KINGDRA","PHANPY","DONPHAN","PORYGON2","STANTLER",
	                    "SMEARGLE","TYROGUE","HITMONTOP","SMOOCHUM","ELEKID","MAGBY","MILTANK",
	                    "BLISSEY","RAIKOU","ENTEI","SUICUNE","LARVITAR","PUPITAR","TYRANITAR",
	                    "LUGIA","HO-OH","CELEBI","TREECKO","GROVYLE","SCEPTILE","TORCHIC",
	                    "COMBUSKEN","BLAZIKEN","MUDKIP","MARSHTOMP","SWAMPERT","POOCHYENA","MIGHTYENA",
	                    "ZIGZAGOON","LINOONE","WURMPLE","SILCOON","BEAUTIFLY","CASCOON","DUSTOX",
	                    "LOTAD","LOMBRE","LUDICOLO","SEEDOT","NUZLEAF","SHIFTRY","NINCADA",
	                    "NINJASK","SHEDINJA","TAILLOW","SWELLOW","SHROOMISH","BRELOOM","SPINDA",
	                    "WINGULL","PELIPPER","SURSKIT","MASQUERAIN","WAILMER","WAILORD",
	                    "SKITTY","DELCATTY","KECLEON","BALTOY","CLAYDOL","NOSEPASS","TORKOAL",
	                    "SABLEYE","BARBOACH","WHISCASH","LUVDISC","CORPHISH","CRAWDAUNT","FEEBAS",
	                    "MILOTIC","CARVANHA","SHARPEDO","TRAPINCH","VIBRAVA","FLYGON","MAKUHITA",
	                    "HARIYAMA","ELECTRIKE","MANECTRIC","NUMEL","CAMERUPT","SPHEAL","SEALEO",
	                    "WALREIN","CACNEA","CACTURNE","SNORUNT","GLALIE","LUNATONE","SOLROCK",
	                    "AZURILL","SPOINK","GRUMPIG","PLUSLE","MINUN","MAWILE","MEDITITE",
	                    "MEDICHAM","SWABLU","ALTARIA","WYNAUT","DUSKULL","DUSCLOPS","ROSELIA",
	                    "SLAKOTH","VIGOROTH","SLAKING","GULPIN","SWALOT","TROPIUS","WHISMUR",
	                    "LOUDRED","EXPLOUD","CLAMPERL","HUNTAIL","GOREBYSS","ABSOL","SHUPPET",
	                    "BANETTE","SEVIPER","ZANGOOSE","RELICANTH","ARON","LAIRON","AGGRON",
	                    "CASTFORM","VOLBEAT","ILLUMISE","LILEEP","CRADILY","ANORITH","ARMALDO",
	                    "RALTS","KIRLIA","GARDEVOIR","BAGON","SHELGON","SALAMENCE","BELDUM",
	                    "METANG","METAGROSS","REGIROCK","REGICE","REGISTEEL","KYOGRE","GROUDON",
	                    "RAYQUAZA","LATIAS","LATIOS","JIRACHI","DEOXYS","CHIMECHO"
	                   };
	SendMessage(hwndList, CB_RESETCONTENT, 0, 0);
	for (i=0; i<sizeof(pVarName)/sizeof(char*); i++)
		SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)pVarName[i]);

}

DWORD WINAPI GivePokeScript(LPVOID x)
{
	char*buf1,*buf2,*buf3;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Pokemon");
	if (!*GreatBuffer)strcpy(GreatBuffer,"CHARMANDER");
	GetInput2(GetString1(3047),GetString2(3048),GreatBuffer,FillPokemon);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Pokemon",GreatBuffer);
		strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","PokemonLvl");
		if (!*GreatBuffer)strcpy(GreatBuffer,"0x5");
		GetInput(GetString1(3047),GetString2(3049),GreatBuffer);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","PokemonLvl",GreatBuffer);
			strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","Item");
			if (!*GreatBuffer)strcpy(GreatBuffer,"MASTERBALL");
			GetInput2(GetString1(3047),GetString2(3086),GreatBuffer,FillItems);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
				strcpy(buf3=alloca(strlen(GreatBuffer)+1),GreatBuffer);
				LoadRegSettingStr("ScriptGen","Flag");
				if (!*GreatBuffer)strcpy(GreatBuffer,"0x250");
				GetInput(GetString1(3047),GetString2(3046),GreatBuffer);
				while (inputrunning) {
					Sleep(0);
				}
				if (!cancelled)
				{
					SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
					sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlock\ncheckflag %s\nif 0x1 jump :end\nsetflag FR_POKEMON\ncountpokemon\ncompare LASTRESULT 6\nif == jump @noroom\naddpokemon %s %s %s 0 0 0\nsetflag %s\nstorepokemon 0 %s\nmessage @get-msg\nfanfare 0x101\nshowmsg\nwaitfanfare\nwaitbutton\n:end\nrelease\nend\n\n#org @noroom\nmsgbox @noroom-msg\ncallstd MSG_NOCLOSE\nrelease\nend\n\n#org @noroom-msg\n= You don't have enough room in your party.\n\n#org @get-msg\n= You got a \\v\\h02!\n",
					        GreatBuffer,buf1,buf2,buf3,GreatBuffer,buf1);
					SendEditor(SCI_CANCEL,0,0);
					SendEditor(SCI_SETUNDOCOLLECTION,0,0);
					SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_CLEARALL,0,0);
					SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
					SendEditor(SCI_SETSCROLLWIDTH,2048,0);
					SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
					SendEditor(SCI_SETUNDOCOLLECTION,1,0);
					SendEditor(EM_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SendEditor(SCI_GOTOPOS,0,0);
					SendEditor(SCI_CHOOSECARETX,0,0);
					SetForegroundWindow(MainWnd);
					SetFocus(Text);
					SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
					SendEditor(SCI_SETSEL,0,0);
				}
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI BattleScript(LPVOID x)
{
	char*buf1,*buf2,*buf3;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Pokemon");
	if (!*GreatBuffer)strcpy(GreatBuffer,"RAYQUAZA");
	GetInput2(GetString1(3055),GetString2(3048),GreatBuffer,FillPokemon);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Pokemon",GreatBuffer);
		strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","PokemonLvl");
		if (!*GreatBuffer)strcpy(GreatBuffer,"0x5");
		GetInput(GetString1(3055),GetString2(3049),GreatBuffer);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","PokemonLvl",GreatBuffer);
			strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","Item");
			if (!*GreatBuffer)strcpy(GreatBuffer,"MASTERBALL");
			GetInput2(GetString1(3055),GetString2(3043),GreatBuffer,FillItems);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
				strcpy(buf3=alloca(strlen(GreatBuffer)+1),GreatBuffer);
				LoadRegSettingStr("ScriptGen","Flag");
				if (!*GreatBuffer)strcpy(GreatBuffer,"0x250");
				GetInput(GetString1(3055),GetString2(3056),GreatBuffer);
				while (inputrunning) {
					Sleep(0);
				}
				if (!cancelled)
				{
					SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
					sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nspecial 0x187\ncompare LASTRESULT 2\nif == jump 0x81A7AE0\nspecial 0x188\nlock\nfaceplayer\nchecksound\ncry %s 2\nwaitcry\npause 0x14\nplaysound 0x156 0x0\nbattle %s %s %s\nsetflag 0x807\nspecial 0x138\nwaitspecial\nclearflag 0x807\nspecial2 0x800D 0xB4\ncompare LASTRESULT 1\nif == jump 0x8162558\ncompare LASTRESULT 4\nif == jump 0x8162561\ncompare LASTRESULT 5\nif == jump 0x8162561\nsetflag %s\nrelease\nend\n",
					        buf1,buf1,buf2,buf3,GreatBuffer);
					SendEditor(SCI_CANCEL,0,0);
					SendEditor(SCI_SETUNDOCOLLECTION,0,0);
					SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_CLEARALL,0,0);
					SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
					SendEditor(SCI_SETSCROLLWIDTH,2048,0);
					SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
					SendEditor(SCI_SETUNDOCOLLECTION,1,0);
					SendEditor(EM_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SendEditor(SCI_GOTOPOS,0,0);
					SendEditor(SCI_CHOOSECARETX,0,0);
					SetForegroundWindow(MainWnd);
					SetFocus(Text);
					SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
					SendEditor(SCI_SETSEL,0,0);
				}
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI RoadClosedScript(LPVOID x)
{
	char*buf1,*buf2;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Flag");
	if (!*GreatBuffer)strcpy(GreatBuffer,"0x250");
	GetInput(GetString1(3051),GetString2(3052),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
		strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","PersonID");
		if (!*GreatBuffer)strcpy(GreatBuffer,"0x5");
		GetInput(GetString1(3051),GetString2(3053),GreatBuffer);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","PersonID",GreatBuffer);
			strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","Flag");
			if (!*GreatBuffer)strcpy(GreatBuffer,"0x251");
			GetInput(GetString1(3051),GetString2(3054),GreatBuffer);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
				sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlockall\ncheckflag %s\nif 0x1 jump @open\nmessage @text\nshowmsg\nwaitbutton\nreleaseall\nend\n\n#org @text\n= I'm sorry, the road is closed\\nuntil further notice.\n\n#org @open\nmessage @opentxt\nshowmsg\nwaitbutton\nfadescreen 3\nclosemsg\ndisappear %s\nsetflag %s\nfadescreen 2\nreleaseall\nend\n#org @opentxt\n= I'm sorry, th-\\pOh right!\\nThe construction just finished%clyesterday[.]\n",
				        buf1,buf2,GreatBuffer,'\\');
				SendEditor(SCI_CANCEL,0,0);
				SendEditor(SCI_SETUNDOCOLLECTION,0,0);
				SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_CLEARALL,0,0);
				SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
				SendEditor(SCI_SETSCROLLWIDTH,2048,0);
				SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
				SendEditor(SCI_SETUNDOCOLLECTION,1,0);
				SendEditor(EM_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_SETSAVEPOINT,0,0);
				SendEditor(SCI_GOTOPOS,0,0);
				SendEditor(SCI_CHOOSECARETX,0,0);
				SetForegroundWindow(MainWnd);
				SetFocus(Text);
				SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
				SendEditor(SCI_SETSEL,0,0);
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI TrainerBattleScript(LPVOID x)
{
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Trainer");
	if (!*GreatBuffer)strcpy(GreatBuffer,"0xF");
	GetInput(GetString1(3062),GetString2(3063),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Trainer",GreatBuffer);
		sprintf(MegaBuffer,"#dyn 0x740000\n#org @begin\ntrainerbattle 0x0 %s 0x0 @intro @defeat\nmsgbox @afterwards\ncallstd msg_normal\nend\n#org @intro\n= I will crush you!!!\n\n#org @defeat\n= You crushed me[.]\n\n#org @afterwards\n= I can also say something after the battle\n",
		        GreatBuffer);
		SendEditor(SCI_CANCEL,0,0);
		SendEditor(SCI_SETUNDOCOLLECTION,0,0);
		SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_CLEARALL,0,0);
		SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
		SendEditor(SCI_SETSCROLLWIDTH,2048,0);
		SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
		SendEditor(SCI_SETUNDOCOLLECTION,1,0);
		SendEditor(EM_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_SETSAVEPOINT,0,0);
		SendEditor(SCI_GOTOPOS,0,0);
		SendEditor(SCI_CHOOSECARETX,0,0);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
		SendEditor(SCI_SETSEL,0,0);
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

void FillAttacks(HWND hwndList)
{
	int i;
	char *pVarName[] = {"POUND","KARATECHOP","DOUBLESLAP",
	                    "COMETPUNCH","MEGAPUNCH","PAYDAY","FIREPUNCH","ICEPUNCH",
	                    "THUNDERPUNCH","SCRATCH","VICEGRIP","GUILLOTINE","RAZORWIND",
	                    "SWORDSDANCE","CUT","GUST","WINGATTACK","WHIRLWIND",
	                    "FLY","BIND","SLAM","VINEWHIP","STOMP","DOUBLEKICK",
	                    "MEGAKICK","JUMPKICK","ROLLINGKICK","SANDATTACK","HEADBUTT",
	                    "HORNATTACK","FURYATTACK","HORNDRILL","TACKLE","BODYSLAM",
	                    "WRAP","TAKEDOWN","THRASH","DOUBLEEDGE","TAILWHIP","POISONSTING",
	                    "TWINEEDLE","PINMISSLE","LEER","BITE","GROWL","ROAR",
	                    "SING","SUPERSONIC","SONICBOOM","DISABLE","ACID","EMBER",
	                    "FLAMETHROWER","MIST","WATERGUN","HYDROPUMP","SURF","ICEBEAM",
	                    "BLIZZARD","PSYBEAM","BUBBLEBEAM","AURORABEAM","HYPERBEAM","PECK",
	                    "DRILLPECK","SUBMISSION","LOWKICK","COUNTER","SEISMICTOSS","STRENGTH",
	                    "ABSORB","MEGADRAIN","LEECHSEED","GROWTH","RAZORLEAF","SOLARBEAM",
	                    "POISONPOWDER","STUNSPORE","SLEEPPOWDER","PETALDANCE","STRINGSHOT",
	                    "DRAGONRAGE","FIRESPIN","THUNDERSHOCK","THUNDERBOLT","THUNDERWAVE",
	                    "THUNDER","ROCKTHROW","EARTHQUAKE","FISSURE","DIG","TOXIC",
	                    "CONFUSION","PSYCHIC","HYPNOSIS","MEDITATE","AGILITY","QUICKATTACK",
	                    "RAGE","TELEPORT","NIGHTSHADE","MIMIC","SCREECH","DOUBLETEAM",
	                    "RECOVER","HARDEN","MINIMIZE","SMOKESCREEN","CONFUSERAY","WITHDRAW",
	                    "DEFENSECURL","BARRIER","LIGHTSCREEN","HAZE","REFLECT","FOCUSENERGY",
	                    "BIDE","METRONOME","MIRRORMOVE","SELFDESTRUCT","EGGBOMB","LICK",
	                    "SMOG","SLUDGE","BONECLUB","FIREBLAST","WATERFALL","CLAMP",
	                    "SWIFT","SKULLBASH","SPIKECANNON","CONSTRICT","AMNESIA","KINESIS",
	                    "SOFTBOILED","HIJUMPKICK","GLARE","DREAMEATER","POISONGAS","BARRAGE",
	                    "LEECHLIFE","LOVELYKISS","SKYATTACK","TRANSFORM","BUBBLE","DIZZYPUNCH",
	                    "SPORE","FLASH","PSYWAVE","SPLASH","ACIDARMOR","CRABHAMMER",
	                    "EXPLOSION","FURYSWIPES","BONEMERANG","REST","ROCKSLIDE","HYPERFANG",
	                    "SHARPEN","CONVERSION","TRIATTACK","SUPERFANG","SLASH","SUBSTITUTE",
	                    "STRUGGLE","SKETCH","TRIPLEKICK","THIEF","SPIDERWEB","MINDREADER",
	                    "NIGHTMARE","FLAMEWHEEL","SNORE","CURSE","FLAIL","CONVERSION2","AEROBLAST",
	                    "COTTONSPORE","REVERSAL","SPITE","POWDERSNOW","PROTECT",
	                    "MACHPUNCH","SCARYFACE","FAINTATTACK","SWEETKISS","BELLYDRUM",
	                    "SLUDGEBOMB","MUDSLAP","OCTAZOOKA","SPIKES","ZAPCANNON","FORESIGHT",
	                    "DESTINYBOND","PERISHSONG","ICYWIND","DETECT","BONERUSH","LOCKON",
	                    "OUTRAGE","SANDSTORM","GIGADRAIN","ENDURE","CHARM","ROLLOUT",
	                    "FALSESWIPE","SWAGGER","MILKDRINK","SPARK","FURYCUTTER","STEELWING",
	                    "MEANLOOK","ATTRACT","SLEEPTALK","HEALBELL","RETURN","PRESENT",
	                    "FRUSTRATION","SAFEGUARD","PAINSPLIT","SACREDFIRE","MAGNITUTDE","DYNAMICPUNCH",
	                    "MEGAHORN","DRAGONBREATH","BATONPASS","ENCORE","PURSUIT","RAPIDSPIN",
	                    "SWEETSCENT","IRONTAIL","METALCLAW","VITALTHROW","MORNINGSUN","SYNTHESIS",
	                    "MOONLIGHT","HIDDENPOWER","CROSSCHOP","TWISTER","RAINDANCE","SUNNYDAY",
	                    "CRUNCH","MIRRORCOAT","PSYCHUP","EXTREMESPEED","ANCIENTPOWER","SHADOWBALL",
	                    "FUTURESIGHT","ROCKSMASH","WHIRLPOOL","BEATUP","FAKEOUT","UPROAR",
	                    "STOCKPILE","SPITUP","SWALLOW","HEATWAVE","HAIL","TORMENT",
	                    "FLATTER","WILLOWISP","MEMENTO","FACADE","FOCUSPUNCH","SMELLINGSALT",
	                    "FOLLOWME","NATUREPOWER","CHARGE","TAUNT","HELPINGHAND","TRICK",
	                    "ROLEPLAY","WISH","ASSIST","INGRAIN","SUPERPOWER","MAGICCOAT",
	                    "RECYCLE","REVENGE","BRICKBREAK","YAWN","KNOCKOFF","ENDEAVOUR",
	                    "ERUPTION","SKILLSWAP","IMPRISON","REFRESH","GRUDGE","SNATCH",
	                    "SECRETPOWER","DIVE","ARMTHRUST","CAMOUFLAGE","TAILGLOW","LUSTERPURGE",
	                    "MISTBALL","FEATHERDANCE","TEETERDANCE","BLAZEKICK","MUDSPORT","ICEBALL",
	                    "NEEDLEARM","SLACKOFF","HYPERVOICE","POISONFANG","CRUSHCLAW","BLASTBURN",
	                    "HYDROCANNON","METEORMASH","ASTONISH","WEATHERBALL","AROMATHERAPY","FAKETEARS",
	                    "AIRCUTTER","OVERHEAT","ODORSLEUTH","ROCKTOMB","SILVERWIND","METALSOUND",
	                    "GRASSWHISTLE","TICKLE","COSMICPOWER","WATERSPOUT","SIGNALBEAM","SHADOWPUNCH",
	                    "EXTRASENSORY","SKYUPPERCUT","SANDTOMB","SHEERCOLD","MUDDYWATER",
	                    "BULLETSEED","AERIALACE","ICICLESPEAR","IRONDEFENSE","BLOCK","HOWL",
	                    "DRAGONCLAW","FRENZYPLANT","BULKUP","BOUNCE","MUDSHOT","POISONTAIL",
	                    "COVET","VOLTTACKLE","MAGICALLEAF","WATERSPORT","CALMMIND","LEAFBLADE",
	                    "DRAGONDANCE","ROCKBLAST","SHOCKWAVE","WATERPULSE","DOOMDESIRE",
	                    "PSYCHOBOOST"
	                   };
	SendMessage(hwndList, CB_RESETCONTENT, 0, 0);
	for (i=0; i<sizeof(pVarName)/sizeof(char*); i++)
		SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)pVarName[i]);

}



DWORD WINAPI HMMoveScript(LPVOID x)
{
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Attack");
	if (!*GreatBuffer)strcpy(GreatBuffer,"CUT");
	GetInput2(GetString1(3065),GetString2(3064),GreatBuffer,FillAttacks);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Attack",GreatBuffer);
		sprintf(MegaBuffer,"' Also used as a demonstration of the new dynamic offsets:\n\n#dyn 0x740000\n#org @start\nspecial 0x187\ncompare LASTRESULT 2\nif == jump 0x81A7AE0\nlockall\ncheckattack %s\ncompare LASTRESULT 6\nif == jump @donthave\nsetanimation 0 LASTRESULT\nstorepokemonvar 0 LASTRESULT\nstoreattack 1 %s\nmsgbox @doit\ncallstd MSG_YESNO\ncompare LASTRESULT 0\nif == jump 0x81BDF91\nmsgbox 0x81BDFD7\ncallstd MSG_NOCLOSE\nclosemsg\ndoanimation 2\nwaitspecial\n' -------------------\n' Enter the code here for what happens when the attack is used\n\n\n' -------------------\nreleaseall\nend\n\n#org @donthave\nmsgbox @donthavemsg\ncallstd 0x3\nreleaseall\nend\n\n#org @doit\n= Do you want to use that attack%cnon this[.] thing? Replace this message with something else. Please.\n\n#org @donthavemsg\n= You don't have the correct attack. Also replace this message.\n",
		        GreatBuffer,GreatBuffer,'\\');
		SendEditor(SCI_CANCEL,0,0);
		SendEditor(SCI_SETUNDOCOLLECTION,0,0);
		SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_CLEARALL,0,0);
		SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
		SendEditor(SCI_SETSCROLLWIDTH,2048,0);
		SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
		SendEditor(SCI_SETUNDOCOLLECTION,1,0);
		SendEditor(EM_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_SETSAVEPOINT,0,0);
		SendEditor(SCI_GOTOPOS,0,0);
		SendEditor(SCI_CHOOSECARETX,0,0);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
		SendEditor(SCI_SETSEL,0,0);
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI ItemCraftScript(LPVOID x)
{
	char*buf1,*buf2,*buf3;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","ToolItem");
	if (!*GreatBuffer)strcpy(GreatBuffer,"STARPIECE");
	GetInput2(GetString1(3072),GetString2(3073),GreatBuffer,FillItems);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","ToolItem",GreatBuffer);
		strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","Item");
		if (!*GreatBuffer)strcpy(GreatBuffer,"NUGGET");
		GetInput2(GetString1(3072),GetString2(3074),GreatBuffer,FillItems);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
			strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","ItemAmt");
			if (!*GreatBuffer)strcpy(GreatBuffer,"1");
			GetInput(GetString1(3072),GetString2(3075),GreatBuffer);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","ItemAmt",GreatBuffer);
				strcpy(buf3=alloca(strlen(GreatBuffer)+1),GreatBuffer);
				LoadRegSettingStr("ScriptGen","ItemOutput");
				if (!*GreatBuffer)strcpy(GreatBuffer,"MASTERBALL");
				GetInput(GetString1(3072),GetString2(3076),GreatBuffer);
				while (inputrunning) {
					Sleep(0);
				}
				if (!cancelled)
				{
					SaveRegSettingStr("ScriptGen","ItemOutput",GreatBuffer);
					sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlock\nstoreitem 0 %s\nstoreitem 1 %s\ncheckitem %s 1\ncompare LASTRESULT 0\nif == jump @donthave\ncheckitem %s %s\ncompare LASTRESULT 0\nif == jump @donthave\nmsgbox @msg-have\ncallstd MSG_YESNO\ncompare LASTRESULT NO\nif == jump :end\nremoveitem %s %s\nadditem %s 1\nmsgbox @finished\ncallstd MSG_NOCLOSE\n:end\nrelease\nend\n\n#org @donthave\nmsgbox @msg-donthave\ncallstd MSG_NOCLOSE\nstoreitem 0 %s\nmsgbox @msg-donthave2\ncallstd MSG_NOCLOSE\nrelease\nend\n\n#org @msg-donthave\n= You can craft \\v\\h02s here.\n\n#org @msg-donthave2\n= You will need a \\v\\h02,\\nand %s \\v\\h03s.\n\n#org @msg-have\n= Do you want to craft one \\v\\h02\\nwith %s \\v\\h03s?\n\n#org @finished\n= Done! You used %s \\v\\h03s\\nto create a \\v\\h02!\n",
					        GreatBuffer,buf2,buf1,buf2,buf3,buf2,buf3,GreatBuffer,buf1,buf3,buf3,buf3);
					SendEditor(SCI_CANCEL,0,0);
					SendEditor(SCI_SETUNDOCOLLECTION,0,0);
					SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_CLEARALL,0,0);
					SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
					SendEditor(SCI_SETSCROLLWIDTH,2048,0);
					SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
					SendEditor(SCI_SETUNDOCOLLECTION,1,0);
					SendEditor(EM_EMPTYUNDOBUFFER,0,0);
					SendEditor(SCI_SETSAVEPOINT,0,0);
					SendEditor(SCI_GOTOPOS,0,0);
					SendEditor(SCI_CHOOSECARETX,0,0);
					SetForegroundWindow(MainWnd);
					SetFocus(Text);
					SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
					SendEditor(SCI_SETSEL,0,0);
				}
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI QuickStarterScript(LPVOID x)
{
	char *buf1,*buf2,*buf3,*buf4,*buf5,*buf6,*buf7;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Pokemon");
	if (!*GreatBuffer)strcpy(GreatBuffer,"EKANS");
	GetInput2(GetString1(3077),GetString2(3078),GreatBuffer,FillPokemon);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Pokemon",GreatBuffer);
		strcpy(buf5=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","HoldItem");
		if (!*GreatBuffer)strcpy(GreatBuffer,"SITRUSBERRY");
		GetInput2(GetString1(3077),GetString2(3080),GreatBuffer,FillItems);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","HoldItem",GreatBuffer);
			strcpy(buf3=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","Level");
			if (!*GreatBuffer)strcpy(GreatBuffer,"5");
			GetInput(GetString1(3077),GetString2(3079),GreatBuffer);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","Level",GreatBuffer);
				strcpy(buf4=alloca(strlen(GreatBuffer)+1),GreatBuffer);
				LoadRegSettingStr("ScriptGen","TM");
				if (!*GreatBuffer)strcpy(GreatBuffer,"TM40");
				GetInput2(GetString1(3077),GetString2(3081),GreatBuffer,FillItems);
				while (inputrunning) {
					Sleep(0);
				}
				if (!cancelled)
				{
					SaveRegSettingStr("ScriptGen","TM",GreatBuffer);
					strcpy(buf6=alloca(strlen(GreatBuffer)+1),GreatBuffer);
					LoadRegSettingStr("ScriptGen","Item");
					if (!*GreatBuffer)strcpy(GreatBuffer,"POKEBALL");
					GetInput2(GetString1(3077),GetString2(3082),GreatBuffer,FillItems);
					while (inputrunning) {
						Sleep(0);
					}
					if (!cancelled)
					{
						SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
						strcpy(buf7=alloca(strlen(GreatBuffer)+1),GreatBuffer);
						LoadRegSettingStr("ScriptGen","ItemAmt");
						if (!*GreatBuffer)strcpy(GreatBuffer,"3");
						GetInput(GetString1(3077),GetString2(3083),GreatBuffer);
						while (inputrunning) {
							Sleep(0);
						}
						if (!cancelled)
						{
							SaveRegSettingStr("ScriptGen","ItemAmt",GreatBuffer);
							strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
							LoadRegSettingStr("ScriptGen","Item2");
							if (!*GreatBuffer)strcpy(GreatBuffer,"POTION");
							GetInput2(GetString1(3077),GetString2(3084),GreatBuffer,FillItems);
							while (inputrunning) {
								Sleep(0);
							}
							if (!cancelled)
							{
								SaveRegSettingStr("ScriptGen","Item2",GreatBuffer);
								strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
								LoadRegSettingStr("ScriptGen","Item2Amt");
								if (!*GreatBuffer)strcpy(GreatBuffer,"3");
								GetInput(GetString1(3077),GetString2(3083),GreatBuffer);
								while (inputrunning) {
									Sleep(0);
								}
								if (!cancelled)
								{
									SaveRegSettingStr("ScriptGen","Item2Amt",GreatBuffer);
									sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\ncheckflag FR_POKEMON\nif true jump :end\naddpokemon %s %s %s 0 0 0\nsetflag FR_POKEDEX\nsetflag FR_POKEMON\nspecial FR_NATIONAL_DEX\nsethealingplace 1\nsetflag FR_RUNNING_SHOES\nadditem %s 1\nadditem %s %s\nadditem %s %s\n:end\nend\n",
									        buf5,buf4,buf3,buf6,buf7,buf1,buf2,GreatBuffer);
									SendEditor(SCI_CANCEL,0,0);
									SendEditor(SCI_SETUNDOCOLLECTION,0,0);
									SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
									SendEditor(SCI_CLEARALL,0,0);
									SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
									SendEditor(SCI_SETSCROLLWIDTH,2048,0);
									SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
									SendEditor(SCI_SETUNDOCOLLECTION,1,0);
									SendEditor(EM_EMPTYUNDOBUFFER,0,0);
									SendEditor(SCI_SETSAVEPOINT,0,0);
									SendEditor(SCI_GOTOPOS,0,0);
									SendEditor(SCI_CHOOSECARETX,0,0);
									SetForegroundWindow(MainWnd);
									SetFocus(Text);
									SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
									SendEditor(SCI_SETSEL,0,0);
								}
							}
						}
					}
				}
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI RockBlockScript(LPVOID x)
{
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Item");
	if (!*GreatBuffer)strcpy(GreatBuffer,"CLEANSETAG");
	GetInput2(GetString1(3087),GetString2(3074),GreatBuffer,FillItems);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Item",GreatBuffer);
		sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\n'-----------------------------------\nspecial 0x187\ncompare LASTRESULT 2\nif == jump :end\nlockall\nstoreitem 0 %s\ncheckitem %s 1\ncompare LASTRESULT 0\nif == jump @donthave ' Smaller Than\nmsgbox @havemsg ' This rock appears to...\ncallstd MSG_YESNO ' Yes/No message\ncompare LASTRESULT 0\nif == jump :end\nremoveitem %s 1\nmsgbox @IusedThis ' \\v\\h02 used \\v\\h03!\ncallstd MSG_NOCLOSE ' Non-closing message\napplymovement LASTTALKED @move ' rock_animate end\npauseevent 0\ndisappear LASTTALKED\n:end\nreleaseall\nend\n\n#org @donthave\n'-----------------------------------\nmsgbox @donthavemsg ' It's a cursed rock, ...\ncallstd MSG_SIGN\nend\n\n\n\n#org @havemsg\n= This rock appears to be breakable.\\nWould you like to use a \\v\\h02?\n\n#org @IusedThis\n= \\v\\h01 used the \\v\\h02!\n\n#org @donthavemsg\n= It's a cursed rock, but a \\v\\h02\\nmay be able to smash it.\n\n#org @move\nM rock_animate end\n",
		        GreatBuffer,GreatBuffer,GreatBuffer);
		SendEditor(SCI_CANCEL,0,0);
		SendEditor(SCI_SETUNDOCOLLECTION,0,0);
		SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_CLEARALL,0,0);
		SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
		SendEditor(SCI_SETSCROLLWIDTH,2048,0);
		SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
		SendEditor(SCI_SETUNDOCOLLECTION,1,0);
		SendEditor(EM_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_SETSAVEPOINT,0,0);
		SendEditor(SCI_GOTOPOS,0,0);
		SendEditor(SCI_CHOOSECARETX,0,0);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
		SendEditor(SCI_SETSEL,0,0);
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI MoveTutorScript(LPVOID x)
{
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","MTID");
	if (!*GreatBuffer)strcpy(GreatBuffer,"1");
	GetInput(GetString1(3093),GetString2(3094),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","MTID",GreatBuffer);
		sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\n'-----------------------------------\nlock\nfaceplayer\nsetvar 0x8005 %s\nspecial TEACH_ATTACK\nwaitspecial\nlock\nfaceplayer\nrelease\nend\n",
		        GreatBuffer);
		SendEditor(SCI_CANCEL,0,0);
		SendEditor(SCI_SETUNDOCOLLECTION,0,0);
		SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_CLEARALL,0,0);
		SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
		SendEditor(SCI_SETSCROLLWIDTH,2048,0);
		SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
		SendEditor(SCI_SETUNDOCOLLECTION,1,0);
		SendEditor(EM_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_SETSAVEPOINT,0,0);
		SendEditor(SCI_GOTOPOS,0,0);
		SendEditor(SCI_CHOOSECARETX,0,0);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
		SendEditor(SCI_SETSEL,0,0);
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI FlagSkeleton1Script(LPVOID x)
{
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Flag");
	if (!*GreatBuffer)strcpy(GreatBuffer,"0x200");
	GetInput(GetString1(3100),GetString2(3046),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
		sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlock\nfaceplayer\ncheckflag %s\nif true jump @flag-2\n\n//Flag has not been activated\nmsgbox @flag-1-msg\ncallstd MSG_NOCLOSE\n\n\nsetflag %s\nrelease\nend\n\n#org @flag-2\n\n//Flag has been activated:\nmsgbox @flag-2-msg\ncallstd MSG_NOCLOSE\n\n\nrelease\nend\n\n#org @flag-1-msg\n= Hello, I'm Jekyll.\n\n#org @flag-2-msg\n= Hi, I'm Hyde!\n",
		        GreatBuffer,GreatBuffer);
		SendEditor(SCI_CANCEL,0,0);
		SendEditor(SCI_SETUNDOCOLLECTION,0,0);
		SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_CLEARALL,0,0);
		SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
		SendEditor(SCI_SETSCROLLWIDTH,2048,0);
		SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
		SendEditor(SCI_SETUNDOCOLLECTION,1,0);
		SendEditor(EM_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_SETSAVEPOINT,0,0);
		SendEditor(SCI_GOTOPOS,0,0);
		SendEditor(SCI_CHOOSECARETX,0,0);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
		SendEditor(SCI_SETSEL,0,0);
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI FlagSkeleton2Script(LPVOID x)
{
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","Flag");
	if (!*GreatBuffer)strcpy(GreatBuffer,"0x200");
	GetInput(GetString1(3100),GetString2(3046),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","Flag",GreatBuffer);
		sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlock\nfaceplayer\ncheckflag %s\nif true jump :flag-2\n\n//Flag has not been activated\nmsgbox @flag-1-msg\ncallstd MSG_NOCLOSE\n\n\nsetflag %s\n:flag-2\n\n//Always executed whether flag is set or not\nmsgbox @flag-2-msg\ncallstd MSG_NOCLOSE\n\n\nrelease\nend\n\n#org @flag-1-msg\n= Hello, I'm Jekyll.\n\n#org @flag-2-msg\n= Well, goodbye.",
		        GreatBuffer,GreatBuffer);
		SendEditor(SCI_CANCEL,0,0);
		SendEditor(SCI_SETUNDOCOLLECTION,0,0);
		SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_CLEARALL,0,0);
		SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
		SendEditor(SCI_SETSCROLLWIDTH,2048,0);
		SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
		SendEditor(SCI_SETUNDOCOLLECTION,1,0);
		SendEditor(EM_EMPTYUNDOBUFFER,0,0);
		SendEditor(SCI_SETSAVEPOINT,0,0);
		SendEditor(SCI_GOTOPOS,0,0);
		SendEditor(SCI_CHOOSECARETX,0,0);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
		SendEditor(SCI_SETSEL,0,0);
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

DWORD WINAPI EarthquakeScript(LPVOID x)
{
	char*buf1,*buf2;
	AttachThreadInput(GetCurrentThreadId(),maintid,1);
	ShowWindow(GenWin,SW_HIDE);
	LoadRegSettingStr("ScriptGen","XEarthquake");
	if (!*GreatBuffer)strcpy(GreatBuffer,"12");
	GetInput(GetString1(3101),GetString2(3102),GreatBuffer);
	while (inputrunning) {
		Sleep(0);
	}
	if (!cancelled)
	{
		SaveRegSettingStr("ScriptGen","XEarthquake",GreatBuffer);
		strcpy(buf1=alloca(strlen(GreatBuffer)+1),GreatBuffer);
		LoadRegSettingStr("ScriptGen","YEarthquake");
		if (!*GreatBuffer)strcpy(GreatBuffer,"12");
		GetInput(GetString1(3101),GetString2(3103),GreatBuffer);
		while (inputrunning) {
			Sleep(0);
		}
		if (!cancelled)
		{
			SaveRegSettingStr("ScriptGen","YEarthquake",GreatBuffer);
			strcpy(buf2=alloca(strlen(GreatBuffer)+1),GreatBuffer);
			LoadRegSettingStr("ScriptGen","DEarthquake");
			if (!*GreatBuffer)strcpy(GreatBuffer,"3");
			GetInput(GetString1(3101),GetString2(3104),GreatBuffer);
			while (inputrunning) {
				Sleep(0);
			}
			if (!cancelled)
			{
				SaveRegSettingStr("ScriptGen","DEarthquake",GreatBuffer);
				sprintf(MegaBuffer,"#dyn 0x740000\n#org @start\nlockall\nsetvar 0x8004 0x3\nsetvar 0x8005 %s\nsetvar 0x8006 %s\nsetvar 0x8007 %s\nspecial 0x136\nwaitspecial\nreleaseall\nend\n",
								buf1,buf2,GreatBuffer);
				SendEditor(SCI_CANCEL,0,0);
				SendEditor(SCI_SETUNDOCOLLECTION,0,0);
				SendEditor(SCI_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_CLEARALL,0,0);
				SendEditor(SCI_MARKERDELETEALL,(WPARAM)-1,0);
				SendEditor(SCI_SETSCROLLWIDTH,2048,0);
				SendEditor(SCI_ADDTEXT,strlen(MegaBuffer),(LPARAM)MegaBuffer);
				SendEditor(SCI_SETUNDOCOLLECTION,1,0);
				SendEditor(EM_EMPTYUNDOBUFFER,0,0);
				SendEditor(SCI_SETSAVEPOINT,0,0);
				SendEditor(SCI_GOTOPOS,0,0);
				SendEditor(SCI_CHOOSECARETX,0,0);
				SetForegroundWindow(MainWnd);
				SetFocus(Text);
				SendEditor(SCI_COLOURISE,0,(LPARAM)-1);
				SendEditor(SCI_SETSEL,0,0);
			}
		}
	}
	EnableWindow(MainWnd,1);
	SetForegroundWindow(MainWnd);
	SetFocus(Text);
	return 0;
}

BOOL CALLBACK GenFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	DWORD tid;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0x2:
			CreateThread(NULL,0,FindItemScript,(LPVOID)0,0,&tid);
			break;
		case 0x3:
			CreateThread(NULL,0,GiveItemScript,(LPVOID)0,0,&tid);
			break;
		case 0x4:
			CreateThread(NULL,0,GivePokeScript,(LPVOID)0,0,&tid);
			break;
		case 0x7:
			CreateThread(NULL,0,RoadClosedScript,(LPVOID)0,0,&tid);
			break;
		case 0x8:
			CreateThread(NULL,0,HealScript,(LPVOID)0,0,&tid);
			break;
		case 0x9:
			CreateThread(NULL,0,BattleScript,(LPVOID)0,0,&tid);
			break;
		case 0xA:
			CreateThread(NULL,0,TalkScript,(LPVOID)0,0,&tid);
			break;
		case 0xB:
			CreateThread(NULL,0,TrainerBattleScript,(LPVOID)0,0,&tid);
			break;
		case 0xC:
			CreateThread(NULL,0,HMMoveScript,(LPVOID)0,0,&tid);
			break;
		case 0xD:
			CreateThread(NULL,0,ItemCraftScript,(LPVOID)0,0,&tid);
			break;
		case 0xE:
			CreateThread(NULL,0,QuickStarterScript,(LPVOID)0,0,&tid);
			break;
		case 0xF:
			CreateThread(NULL,0,RockBlockScript,(LPVOID)0,0,&tid);
			break;
		case 0x10:
			CreateThread(NULL,0,MoveTutorScript,(LPVOID)0,0,&tid);
			break;
		case 0x11:
			CreateThread(NULL,0,FlagSkeleton1Script,(LPVOID)0,0,&tid);
			break;
		case 0x12:
			CreateThread(NULL,0,FlagSkeleton2Script,(LPVOID)0,0,&tid);
			break;
		case 0x13:
			CreateThread(NULL,0,EarthquakeScript,(LPVOID)0,0,&tid);
			break;
		case 0x5:
			ShellExecute(GenWin,NULL,"mailto:seejay.11@gmail.com?subject=PKSVUI&body=Hi,%20I%20Would%20like...",NULL,NULL,SW_SHOWDEFAULT);
			break;
		case 0x6:
			ShellExecute(GenWin,NULL,"http://www.pokecommunity.com/private.php?do=newpm&u=15354",NULL,NULL,SW_SHOWDEFAULT);
			break;
		case 0x1:
		case 2000:
			ShowWindow(GenWin,SW_HIDE);
			EnableWindow(MainWnd,1);
			SetForegroundWindow(MainWnd);
			SetFocus(Text);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(GenWin,SW_HIDE);
		EnableWindow(MainWnd,1);
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		break;
	default:
		return 0;
	}
	return 1;
}

BOOL CALLBACK Input2Func(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		cancelled=1;
		switch (LOWORD(wParam))
		{
		case 0x1:
			cancelled=0;
			GetDlgItemText(Input2Win,4,GreatBuffer,sizeof(GreatBuffer));
		case 0x2:
		case 2000:
			inputrunning=0;
			ShowWindow(Input2Win,SW_HIDE);
			break;
		}
		break;
	case WM_CLOSE:
		cancelled=1;
		inputrunning=0;
		ShowWindow(Input2Win,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}

BOOL CALLBACK InputFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		cancelled=1;
		switch (LOWORD(wParam))
		{
		case 0x5:
			MainWndProc(0,WM_COMMAND,1103,0);
			break;
		case 0x1:
			cancelled=0;
			GetDlgItemText(InputWin,4,GreatBuffer,sizeof(GreatBuffer));
		case 0x2:
		case 2000:
			inputrunning=0;
			SetForegroundWindow(MainWnd);
			SetFocus(Text);
			ShowWindow(InputWin,SW_HIDE);
			break;
		}
		break;
	case WM_CLOSE:
		cancelled=1;
		inputrunning=0;
		SetForegroundWindow(MainWnd);
		SetFocus(Text);
		ShowWindow(InputWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}

BOOL CALLBACK NoteFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	char thestr[10];
	char buffer[20];
	unsigned int j,k,l;
	unsigned int calc;
	GetDlgItemText(NoteWin,2,thestr,9);
	sscanf(thestr,"%X",&calc);
	switch (msg)
	{
	case WM_COMMAND:
		if ((LOWORD(wParam)&0xFFF0)==0x2000)
		{
			if (CalcPressed)
			{
				sprintf(thestr,"%X",(calc*16)+(wParam&0xF));
				SetDlgItemText(NoteWin,2,thestr);
			}
			else
			{
				LastCalc=calc;
				sprintf(thestr,"%X",(wParam&0xF));
				SetDlgItemText(NoteWin,2,thestr);
				CalcPressed=1;
			}
		}
		else
		{
			switch (LOWORD(wParam))
			{
			case 0x2025:
				GetDlgItemText(NoteWin,3,buffer,19);
				sscanf(buffer,"%X",&j);
				GetDlgItemText(NoteWin,4,buffer,19);
				sscanf(buffer,"%X",&k);
				k<<=8;
				l=j|k;
				l=PointerToOffset(l);
				if (l!=-1)
				{
					sprintf(thestr,"%X",l);
					SetDlgItemText(NoteWin,2,thestr);
				} else {
					MessageBox(NULL,GetString1(3060),GetString2(3001),0);
				}
				break;
			case 0x2026:
				l=OffsetToPointer(calc);
				j=l&0xFF;
				k=l>>8;
				if (l!=-1)
				{
					sprintf(thestr,"%X",j);
					sprintf(buffer,"%X",k);
					SetDlgItemText(NoteWin,3,thestr);
					SetDlgItemText(NoteWin,4,buffer);
				} else {
					MessageBox(NULL,GetString1(3061),GetString2(3001),0);
				}
				break;
			case 0x2010:
				if (CalcPressed)
				{
					LastCalcOp=OP_ADIT;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_ADIT;
				}
				break;
			case 0x2011:
				if (CalcPressed)
				{
					LastCalcOp=OP_SUBT;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_SUBT;
				}
				break;
			case 0x2012:
				if (CalcPressed)
				{
					LastCalcOp=OP_MULT;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_MULT;
				}
				break;
			case 0x2013:
				if (CalcPressed)
				{
					LastCalcOp=OP_DIVI;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_DIVI;
				}
				break;
			case 0x2014:
				sprintf(thestr,"%X",isqrt(calc));
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x2015:
				if (CalcPressed)
				{
					LastCalcOp=OP_POWR;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_POWR;
				}
				break;
			case 0x201A:
				if (CalcPressed)
				{
					LastCalcOp=OP_MODU;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_MODU;
				}
				break;
			case 0x201C:
				if (CalcPressed)
				{
					LastCalcOp=OP_BXOR;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_BXOR;
				}
				break;
			case 0x201D:
				if (CalcPressed)
				{
					LastCalcOp=OP_BWOR;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_BWOR;
				}
				break;
			case 0x201E:
				if (CalcPressed)
				{
					LastCalcOp=OP_BAND;
					CalcPressed=0;
				}
				else
				{
					LastCalcOp=OP_BAND;
				}
				break;
			case 0x2016:
				sprintf(thestr,"%X",calc<<1);
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x2017:
				sprintf(thestr,"%X",calc>>1);
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x2018:
#ifdef MSVC
				__asm
				{
					mov eax,calc
					rol eax,1
					mov calc,eax
				}
#else
				asm(".intel_syntax noprefix\n"
				"mov %0,%1\n"
				"rol %0,1\n"
".att_syntax\n":"=r"(calc):"r"(calc));
				sprintf(thestr,"%X",calc);
#endif
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x2019:
#ifdef MSVC
				__asm
				{
					mov eax,calc
					ror eax,1
					mov calc,eax
				}
#else
				asm(".intel_syntax noprefix\n"
				"mov %0,%1\n"
				"ror %0,1\n"
".att_syntax\n":"=r"(calc):"r"(calc));
				sprintf(thestr,"%X",calc);
#endif
				sprintf(thestr,"%X",calc);
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x201B:
#ifdef MSVC
				__asm
				{
					mov eax,calc
					and eax,0xF
					cmp eax,0xA
					sbb eax,0x69
					das
					and eax,0xFF
					mov calc,eax
				}
#else
				asm(".intel_syntax noprefix\n"
				"mov %0,%1\n"
				"and %0,0xF\n"
				"cmp %0,0xA\n"
				"sbb %0,0x69\n"
				"das\n"
				"and %0,0xFF\n"
".att_syntax\n":"=r"(calc):"r"(calc));
#endif
				sprintf(thestr,"%X",calc);
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x201F:
				CalcMem+=calc;
				break;
			case 0x2020:
				CalcMem-=calc;
				break;
			case 0x2021:
				sprintf(thestr,"%X",CalcMem);
				SetDlgItemText(NoteWin,2,thestr);
				break;
			case 0x2022:
				CalcMem=0;
				break;
			case 0x2024:
				LastCalcOp=OP_NONE;
			case 0x2023:
				SetDlgItemText(NoteWin,2,"0");
				break;
			case 0x1FFF:
				switch (LastCalcOp)
				{
				case OP_ADIT:
					calc=calc+LastCalc;
					break;
				case OP_SUBT:
					calc=LastCalc-calc;
					break;
				case OP_MULT:
					calc=LastCalc*calc;
					break;
				case OP_DIVI:
					calc=(unsigned int)(LastCalc/calc);
					break;
				case OP_POWR:
					calc=(unsigned int)pow(LastCalc,calc);
					break;
				case OP_BXOR:
					calc=calc^LastCalc;
					break;
				case OP_BWOR:
					calc=calc|LastCalc;
					break;
				case OP_BAND:
					calc=calc&LastCalc;
					break;
				case OP_MODU:
					calc=LastCalc%calc;
					break;
				}
				LastCalcOp=OP_NONE;
				sprintf(thestr,"%X",calc);
				SetDlgItemText(NoteWin,2,thestr);
				break;
			}
		}
		break;
	case WM_CLOSE:
		ShowWindow(NoteWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}

BOOL CALLBACK HexFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	char thestr[15];
	unsigned int i;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 3:
			GetDlgItemText(HexWin,1,thestr,15);
			sscanf(thestr,"%X",&i);
			sprintf(thestr,"%u",i);
			SetDlgItemText(HexWin,2,thestr);
			break;
		case 4:
			GetDlgItemText(HexWin,2,thestr,15);
			sscanf(thestr,"%u",&i);
			sprintf(thestr,"%X",i);
			SetDlgItemText(HexWin,1,thestr);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(HexWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}

typedef struct _list
{
	struct _list*prev;
	struct _list*next;
	unsigned int type;
	char*str;
} list;
list*moves=NULL;
unsigned int currtype=0x10;
char*txttype="Walk";

char tbuf[30];
char*GetMoveName(unsigned int p)
{
	switch (p)
	{
	case 254:
		return "end";
	case 0x1:
		return "look_up";
	case 0x2:
		return "look_left";
	case 0x3:
		return "look_right";
	case 0x4:
		return "look_down";
	case 0x8:
		return "walk_down_vslow";
	case 0x9:
		return "walk_up_vslow";
	case 0xA:
		return "walk_left_vslow";
	case 0xB:
		return "walk_right_vslow";
	case 0xC:
		return "walk_down_slow";
	case 0xD:
		return "walk_up_slow";
	case 0xE:
		return "walk_left_slow";
	case 0xF:
		return "walk_right_slow";
	case 0x10:
		return "walk_down";
	case 0x11:
		return "walk_up";
	case 0x12:
		return "walk_left";
	case 0x13:
		return "walk_right";
	case 0x14:
		return "jump_down2";
	case 0x15:
		return "jump_up2";
	case 0x16:
		return "jump_left2";
	case 0x17:
		return "jump_right2";
	case 0x1D:
		return "run_down";
	case 0x1E:
		return "run_up";
	case 0x1F:
		return "run_left";
	case 0x20:
		return "run_right";
	case 0x2E:
		return "walk_to_lasttalk";
	case 0x2F:
		return "walk_to_player";
	case 0x46:
		return "look_left_jump_down";
	case 0x47:
		return "look_down_jump_up";
	case 0x48:
		return "look_up_jump_left";
	case 0x49:
		return "look_left_jump_right";
	case 0x4A:
		return "faceplayer";
	case 0x4B:
		return "face_away";
	case 0x4E:
		return "jump_down1";
	case 0x4F:
		return "jump_up1";
	case 0x50:
		return "jump_left1";
	case 0x51:
		return "jump_right1";
	case 0x52:
		return "jump_down";
	case 0x53:
		return "jump_up";
	case 0x54:
		return "jump_left";
	case 0x55:
		return "jump_right";
	case 0x56:
		return "jump_downup";
	case 0x57:
		return "jump_updown";
	case 0x58:
		return "jump_leftright";
	case 0x59:
		return "jump_rightleft";
	case 0x5A:
		return "face_default";
	case 0x60:
		return "hide";
	case 0x62:
		return "say_!";
	case 0x63:
		return "say_?";
	case 0x64:
		return "say_X";
	case 0x65:
		return "say_!!";
	case 0x66:
		return "say_:)";
	case 0x68:
		return "rock_animate";
	case 0x69:
		return "tree_animate";
	default:
		sprintf(tbuf,"raw_%02X",p);
		return tbuf;
	}
}

void swap(list*first)
{
	list*temp=first;
	if (first==NULL||first->next==NULL)return;
	if (first==moves)moves=first->next;
	if (first->prev)
		first->prev->next=first->next;
	first=first->next;
	first->prev=temp->prev;
	if (first->next)
		first->next->prev=temp;
	temp->next=first->next;
	first->next=temp;
	if (first->next)
		first->next->prev=first;
}
void SwapItems(unsigned int a,unsigned int b)
{
	list*c=NULL;
	list*f=moves;
	unsigned int i;
	for (i=0;i<=a&&f;i++) //loop until end of moves, or until the larger of a or b is reached.
	{
		if (i==a)
			c=f;
		f=f->next;
	}
	if (!c||!(c->next))
		return;
	swap(c);
	SendMessage(GetDlgItem(MovePlanWin,13),LB_RESETCONTENT,0,0);
	f=moves;
	while (f)
	{
		SendMessage(GetDlgItem(MovePlanWin,13),LB_ADDSTRING,0,(LPARAM)f->str);
		f=f->next;
	}
	SendMessage(GetDlgItem(MovePlanWin,13),LB_SETCURSEL,b,0);
	return;
}
void AddToList(unsigned int a,char*b)
{
	char z[30];
	list*c;
	list*d;
	unsigned int y;
	strcpy(z,txttype);
	strcat(z," ");
	strcat(z,b);
	y=a+currtype;
	if (y==0)y=4;
	if (moves==NULL)
	{
		moves=malloc(sizeof(list));
		moves->next=NULL;
		moves->prev=NULL;
		moves->type=y;
		moves->str=malloc(strlen(z)+1);
		strcpy(moves->str,z);
	}
	else
	{
		c=moves;
		while (c!=NULL)
		{
			d=c;
			c=c->next;
		}
		d->next=malloc(sizeof(list));
		c=d;
		d=d->next;
		d->next=NULL;
		d->prev=c;
		d->type=y;
		d->str=malloc(strlen(z)+1);
		strcpy(d->str,z);
	}
	SendMessage(GetDlgItem(MovePlanWin,13),LB_ADDSTRING,0,(LPARAM)z);
}

BOOL CALLBACK MovePlanFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	unsigned int len,i,j,x,y;
	char*ptr;
	char*ptr2;
	list*c;
	list*d;
	HDC dc,dc2;
	HPEN pen;
	HGDIOBJ temp;
	switch (msg)
	{
	case WM_DRAWITEM:
		if (wParam!=14)
			return 0;
		dc=((LPDRAWITEMSTRUCT)lParam)->hDC;
		dc2=CreateCompatibleDC(dc);
		pen=CreatePen(PS_SOLID,0,RGB(255,255,255));
		temp=SelectObject(dc,pen);
		Rectangle(dc,((LPDRAWITEMSTRUCT)lParam)->rcItem.left,((LPDRAWITEMSTRUCT)lParam)->rcItem.top,((LPDRAWITEMSTRUCT)lParam)->rcItem.right,((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom);
		SelectObject(dc,temp);
		DeleteObject(pen);
		SelectObject(dc2,bmpHero);
		x=(((((LPDRAWITEMSTRUCT)lParam)->rcItem.right)-(((LPDRAWITEMSTRUCT)lParam)->rcItem.left))/2)-8;
		y=(((((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom)-(((LPDRAWITEMSTRUCT)lParam)->rcItem.top))/2)-8;
		i=x&15;
		j=((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom-((LPDRAWITEMSTRUCT)lParam)->rcItem.top;
		pen=CreatePen(PS_SOLID,0,RGB(128,128,128));
		temp=SelectObject(dc,pen);
		while (i<(unsigned int)((LPDRAWITEMSTRUCT)lParam)->rcItem.right)
		{
			Rectangle(dc,i,0,i+1,j);
			i+=16;
		}
		i=y&15;
		j=((LPDRAWITEMSTRUCT)lParam)->rcItem.right-((LPDRAWITEMSTRUCT)lParam)->rcItem.left;
		while (i<(unsigned int)((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom)
		{
			Rectangle(dc,0,i,j,i+1);
			i+=16;
		}
		SelectObject(dc,temp);
		DeleteObject(pen);
		BitBlt(dc,x,y-16,16,32,dc2,16,0,SRCAND);
		BitBlt(dc,x,y-16,16,32,dc2,0,0,SRCPAINT);
		c=moves;
		SelectObject(dc2,bmpArrow);
		while (c)
		{
			i=c->type;
			if (i>=1&&i<=4)
			{
				BitBlt(dc,x+4,y+4,9,9,dc2,(i&3)*9,9,SRCAND);
				BitBlt(dc,x+4,y+4,9,9,dc2,(i&3)*9,0,SRCPAINT);
			}
			else if (i>=0x52&&i<=0x55)
			{
				BitBlt(dc,x+3,y+3,11,11,dc2,((i-2)&3)*11+152,11,SRCAND);
				BitBlt(dc,x+3,y+3,11,11,dc2,((i-2)&3)*11+152,0,SRCPAINT);
			}
			else if (i>=0x4E&&i<=0x51)
			{
				if ((i&3)==2) {
					y+=8;
				}
				else if ((i&3)==3) {
					y-=8;
				}
				else if ((i&3)==0) {
					x-=8;
				}
				else if ((i&3)==1) {
					x+=8;
				}
				BitBlt(dc,x+1,y+1,16,16,dc2,((i-2)&3)*16+120,38,SRCAND);
				BitBlt(dc,x+1,y+1,16,16,dc2,((i-2)&3)*16+120,22,SRCPAINT);
				if ((i&3)==2) {
					y+=8;
				}
				else if ((i&3)==3) {
					y-=8;
				}
				else if ((i&3)==0) {
					x-=8;
				}
				else if ((i&3)==1) {
					x+=8;
				}
			}
			else if (i>=0x14&&i<=0x17)
			{
				if ((i&3)==0) {
					y+=16;
				}
				else if ((i&3)==1) {
					y-=16;
				}
				else if ((i&3)==2) {
					x-=16;
				}
				else if ((i&3)==3) {
					x+=16;
				}
				BitBlt(dc,x-7,y-7,30,30,dc2,(i&3)*30,52,SRCAND);
				BitBlt(dc,x-7,y-7,30,30,dc2,(i&3)*30,22,SRCPAINT);
				if ((i&3)==0) {
					y+=16;
				}
				else if ((i&3)==1) {
					y-=16;
				}
				else if ((i&3)==2) {
					x-=16;
				}
				else if ((i&3)==3) {
					x+=16;
				}
			}
			else if (i>=0x8&&i<=0x13)
			{
				if ((i&3)==0) {
					y+=8;
				}
				else if ((i&3)==1) {
					y-=8;
				}
				else if ((i&3)==2) {
					x-=8;
				}
				else if ((i&3)==3) {
					x+=8;
				}
				if (i<0xC) {
					j=72;
				}
				else if (i<0x10) {
					j=36;
				}
				else {
					j=0;
				}
				BitBlt(dc,x+4,y+4,9,9,dc2,(i&3)*9+j,9,SRCAND);
				BitBlt(dc,x+4,y+4,9,9,dc2,(i&3)*9+j,0,SRCPAINT);
				if ((i&3)==0) {
					y+=8;
				}
				else if ((i&3)==1) {
					y-=8;
				}
				else if ((i&3)==2) {
					x-=8;
				}
				else if ((i&3)==3) {
					x+=8;
				}
			}
			else if (i>=0x1D&&i<=0x20)
			{
				if ((i&3)==1) {
					y+=8;
				}
				else if ((i&3)==2) {
					y-=8;
				}
				else if ((i&3)==3) {
					x-=8;
				}
				else if ((i&3)==0) {
					x+=8;
				}
				BitBlt(dc,x+3,y+3,11,11,dc2,((i-1)&3)*11+108,11,SRCAND);
				BitBlt(dc,x+3,y+3,11,11,dc2,((i-1)&3)*11+108,0,SRCPAINT);
				if ((i&3)==1) {
					y+=8;
				}
				else if ((i&3)==2) {
					y-=8;
				}
				else if ((i&3)==3) {
					x-=8;
				}
				else if ((i&3)==0) {
					x+=8;
				}
			}
			else if (i>=0x62&&i<=0x66)
			{
				if (i==0x64)i=120;
				else if (i==0x63)i=134;
				else if (i==0x66)i=148;
				else if (i==0x62)i=162;
				else i=176;
				BitBlt(dc,x+1,y+1,14,14,dc2,i,68,SRCAND);
				BitBlt(dc,x+1,y+1,14,14,dc2,i,54,SRCPAINT);
			}
			c=c->next;
		}
		pen=CreatePen(PS_SOLID,0,RGB(255,0,0));
		temp=SelectObject(dc,pen);
		Rectangle(dc,x+6,y+6,x+10,y+10);
		SelectObject(dc,temp);
		DeleteObject(pen);
		DeleteDC(dc2);
		SetWindowLongPtr(MovePlanWin, DWLP_MSGRESULT, TRUE);
		break;
	case WM_COMMAND:
		InvalidateRect(GetDlgItem(MovePlanWin,14),NULL,0);
		switch (LOWORD(wParam))
		{
		case 1:
			ShowWindow(MovePlanWin,SW_HIDE);
			break;
		case 2:
			if (OpenClipboard(NULL))
			{
				EmptyClipboard();
				c=moves;
				len=0;
				while (c!=NULL)
				{
					len+=strlen(GetMoveName(c->type))+1;
					c=c->next;
				}
				ptr=GlobalAlloc(GMEM_MOVEABLE,len+6);
				ptr2=(char*)GlobalLock(ptr);
				*ptr2=0;
				strcat(ptr2,"m ");
				c=moves;
				while (c!=NULL)
				{
					strcat(ptr2,GetMoveName(c->type));
					strcat(ptr2," ");
					c=c->next;
				}
				strcat(ptr2,"end");
				GlobalUnlock(ptr);
				SetClipboardData(CF_TEXT,ptr);
				CloseClipboard();
			}
			break;
		case 3:
			c=moves;
			len=0;
			while (c!=NULL)
			{
				len+=strlen(GetMoveName(c->type))+1;
				c=c->next;
			}
			ptr2=malloc(len+6);
			*ptr2=0;
			strcat(ptr2,"m ");
			c=moves;
			while (c!=NULL)
			{
				strcat(ptr2,GetMoveName(c->type));
				strcat(ptr2," ");
				c=c->next;
			}
			strcat(ptr2,"end");
			SendMessage(Text,SCI_ADDTEXT,len+5,(LPARAM)ptr2);
			free(ptr2);
			break;
		case 4:
			AddToList(1,"Up");
			break;
		case 5:
			AddToList(3,"Right");
			break;
		case 6:
			AddToList(2,"Left");
			break;
		case 7:
			AddToList(0,"Down");
			break;
		case 8:
			i=SendMessage(GetDlgItem(MovePlanWin,13),LB_GETCURSEL,0,0);
			if (i-1>=0&&i!=LB_ERR)
				SwapItems(i-1,i-1);
			break;
		case 9:
			i=SendMessage(GetDlgItem(MovePlanWin,13),LB_GETCURSEL,0,0);
			if (i!=LB_ERR)
				SwapItems(i,i+1);
			break;
		case 100:
			txttype="Look";
			currtype=0x0;
			break;
		case 101:
			txttype="Walk V-Slow";
			currtype=0x8;
			break;
		case 102:
			txttype="Walk Slow";
			currtype=0xC;
			break;
		case 103:
			txttype="Walk";
			currtype=0x10;
			break;
		case 104:
			txttype="Run";
			currtype=0x1D;
			break;
		case 105:
			txttype="Jump [0]";
			currtype=0x52;
			break;
		case 106:
			txttype="Jump [1]";
			currtype=0x4E;
			break;
		case 107:
			txttype="Jump [2]";
			currtype=0x14;
			break;
		case 10:
			i=(unsigned int)SendMessage(GetDlgItem(MovePlanWin,13),LB_GETCURSEL,0,0);
			if (i==LB_ERR)break;
			SendMessage(GetDlgItem(MovePlanWin,13),LB_DELETESTRING,i,0);
			if (SendMessage(GetDlgItem(MovePlanWin,13),LB_SETCURSEL,i,0)==LB_ERR)
				SendMessage(GetDlgItem(MovePlanWin,13),LB_SETCURSEL,i-1,0);
			if (i==0)
			{
				c=moves;
				d=moves->next;
				if (d==NULL)
				{
					moves=NULL;
				}
				else
				{
					moves=d;
				}
				free(c->str);
				free(c);
			}
			else
			{
				c=moves;
				while (i>0)
				{
					c=c->next;
					i--;
				}
				free(c->str);
				if (c->next!=NULL)
				{
					c->next->prev=c->prev;
				}
				c->prev->next=c->next;
				free(c);
			}
			break;
		case 12:
			if (moves==NULL)break;
			SendMessage(GetDlgItem(MovePlanWin,13),LB_RESETCONTENT,0,0);
			c=moves;
			while (c!=NULL)
			{
				d=c;
				c=c->next;
			}
			c=d;
			while (c!=NULL)
			{
				free(c->str);
				if (c->prev==NULL)
				{
					free(c);
					c=NULL;
					moves=NULL;
				}
				else
				{
					c=c->prev;
					free(c->next);
				}
			}
			break;
		case 15:
			ptr=txttype;
			txttype="Say";
			i=currtype;
			currtype=0;
			AddToList(0x65,"\"!!\"");
			currtype=i;
			txttype=ptr;
			break;
		case 16:
			ptr=txttype;
			txttype="Say";
			i=currtype;
			currtype=0;
			AddToList(0x62,"\"!\"");
			currtype=i;
			txttype=ptr;
			break;
		case 17:
			ptr=txttype;
			txttype="Say";
			i=currtype;
			currtype=0;
			AddToList(0x63,"\"?\"");
			currtype=i;
			txttype=ptr;
			break;
		case 18:
			ptr=txttype;
			txttype="Say";
			i=currtype;
			currtype=0;
			AddToList(0x66,"\"^_^\"");
			currtype=i;
			txttype=ptr;
			break;
		case 19:
			ptr=txttype;
			txttype="Say";
			i=currtype;
			currtype=0;
			AddToList(0x64,"\"X\"");
			currtype=i;
			txttype=ptr;
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(MovePlanWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}

BOOL CALLBACK FlagsFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int x,i;
	char buf[5],buf2[7];
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 2:
			if (HIWORD(wParam)==LBN_SELCHANGE)
			{
				SaveFlag();
				x=SendMessage(GetDlgItem(FlagsWin,2),LB_GETCURSEL,0,0);
				LastFlag=x;
				sprintf(buf,"%04X",FlagArr[x]);
				strcpy(buf2,"0x");
				strcpy(buf2+2,buf);
				LoadRegSettingStr("Flags\\ShortDesc",buf);
				SetDlgItemText(FlagsWin,4,buf2);
				SetDlgItemText(FlagsWin,5,GreatBuffer);
				LoadRegSettingStr("Flags\\LongDesc",buf);
				SetDlgItemText(FlagsWin,6,GreatBuffer);
			} else if (HIWORD(wParam)==LBN_SELCANCEL)
			{
				SaveFlag();
				LastFlag=-1;
			}
			break;
		case 3:
			SaveFlag();
			GetDlgItemText(FlagsWin,1,GreatBuffer,sizeof(GreatBuffer));
			sscanf(GreatBuffer,"%X",&x);
			for (i=0;i<FlagArrLen;i++)
			{
				if (FlagArr[i]==x)
				{
					x=-1;
					break;
				}
			}
			if (x==-1)break;
			sprintf(buf,"%04X",x);
			AddFlag(buf,"Unused","This flag is not used");
			if (FlagArr[LastFlag]>x)
				LastFlag++;
			SortFlags();
			for (i=0;i<FlagArrLen;i++)
				if (FlagArr[i]==x)
				{
					SendMessage(GetDlgItem(FlagsWin,2),LB_SETCURSEL,i,0);
					FlagsFunc(hwnd,WM_COMMAND,(LBN_SELCHANGE<<16)|2,0);
					break;
				}
			break;
		case 11:
			MessageBox(FlagsWin,GetString1(3089),GetString2(3090),0x40);
			break;
		case 8:
			if (MessageBox(FlagsWin,GetString1(3092),GetString1(3091),0x20|MB_YESNO)==6)
			{
				DeleteRegSetting("Flags","FilledList");
				while (FlagArrLen)
				{
					LastFlag=FlagArrLen-1;
					DeleteLastFlag();
				}
				ShowWindow(FlagsWin,SW_HIDE);
				FlagsShown=0;
			}
			break;
		case 7:
			if (LastFlag==-1)break;
			SendMessage(GetDlgItem(FlagsWin,2),LB_DELETESTRING,LastFlag,0);
			x=LastFlag;
			if (x>=FlagArrLen)x=FlagArrLen-1;
			DeleteLastFlag();
			if (FlagArrLen)
			{
				SendMessage(GetDlgItem(FlagsWin,2),LB_SETCURSEL,x,0);
				FlagsFunc(hwnd,WM_COMMAND,(LBN_SELCHANGE<<16)|2,0);
			}
			else
			{
				*GreatBuffer=0;
				SetDlgItemText(FlagsWin,4,GreatBuffer);
				SetDlgItemText(FlagsWin,5,GreatBuffer);
				SetDlgItemText(FlagsWin,6,GreatBuffer);
			}
			break;
		case 9:
			ShowWindow(FlagsWin,SW_HIDE);
			SaveFlag();
			FlagsShown=0;
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(FlagsWin,SW_HIDE);
		SaveFlag();
		FlagsShown=0;
		break;
	default:
		return 0;
	}
	return 1;
}

int ReadWholeFile(char*a,char**mm)
{
	char*m;
	HANDLE b;
	int len;
	DWORD read;
	b=CreateFile(a,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (b==INVALID_HANDLE_VALUE)
	{
		*mm=0;
		return 0;
	}
	len=SetFilePointer(b,0,NULL,FILE_END);
	SetFilePointer(b,0,NULL,FILE_BEGIN);
	m=GlobalAlloc(GPTR,len);
	*mm=m;
	ReadFile(b,m,len,&read,NULL);
	CloseHandle(b);
	return len;
}
int romlen=0,romloaded=0;
char*mem2=NULL;
BOOL CALLBACK ScriptSearchFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int txtlen,i,j,binlen;
	char*mem;
	char buf[50];
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 1:
			ShowWindow(ScriptSearchWin,SW_HIDE);
			romloaded=0;
			if (mem2);
			GlobalFree(mem2);
			mem2=NULL;
			break;
		case 3:
			txtlen=SendMessage(GetDlgItem(ScriptSearchWin,2),WM_GETTEXTLENGTH,0,0);
			mem=GlobalAlloc(GPTR,txtlen+100);
			strcpy(mem,"#org 0x0\n");
			SendMessage(GetDlgItem(ScriptSearchWin,2),WM_GETTEXT,txtlen+1,(LPARAM)mem+strlen("#org 0x0\n"));
			CloseHandle(CreateFile("tmp.bin",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL));
			compile("tmp.bin",mem);
			for (i=4;i<=9;i++)
				EnableWindow(GetDlgItem(ScriptSearchWin,i),1);
			EnableWindow(GetDlgItem(ScriptSearchWin,16),1);
			SetWindowText(GetDlgItem(ScriptSearchWin,4),"000000");
			break;
		case 11:
			ScriptSearchType=11;
			break;
		case 12:
			ScriptSearchType=12;
			break;
		case 13:
			ScriptSearchType=13;
			break;
		case 17:
			ScriptSearchType=17;
			break;
		case 19:
			ScriptSearchType=19;
			break;
		case 21:
			ScriptSearchType=21;
			break;
		case 22:
			ScriptSearchType=22;
			break;
		case 25:
			ScriptSearchType=25;
			break;
		case 26:
			SendEditor(SCI_CLEARALL,0,0);
			break;
		case 5:
			GetWindowText(GetDlgItem(ScriptSearchWin,4),buf,50);
			i=-1;
			sscanf(buf,"0x%X",&i);
			if (i==-1)
				sscanf(buf,"%X",&i);
			if (i==-1)
				sscanf(buf,"$%X",&i);
			binlen=ReadWholeFile("tmp.bin",&mem);
			if (!romloaded)
				romlen=ReadWholeFile(RomOpen2,&mem2);
			romloaded=1;
			if (!mem||!mem2)
			{
				MessageBox(ScriptSearchWin,GetString1(3088),GetString2(3095),0x10);
				if (mem)GlobalFree(mem);
				if (mem2)GlobalFree(mem2);
				romloaded=0;
				break;
			}
			if (IsDlgButtonChecked(ScriptSearchWin,24))
				binlen--;
			for (i++;i<romlen;i++)
			{
				for (j=0;j<binlen;j++)
				{
					if (mem[j]!=mem2[i] && (unsigned char)mem[j]!=(unsigned char)0xBA)
					{
						break;
					}
					i++;
				}
				i-=j;
				if (j==binlen)break;
			}
			GlobalFree(mem);
			if (j==binlen)
			{
				sprintf(buf,"0x%X",i);
				SetWindowText(GetDlgItem(ScriptSearchWin,4),buf);

				switch (ScriptSearchType)
				{
				case 11:
					mem=decompile(RomOpen2,i,0);
					break;
				case 12:
					mem=decompileASM(RomOpen2,i|1);
					break;
				case 13:
					mem=decompileText(RomOpen2,i);
					break;
				case 17:
					mem=decompileLevel(RomOpen2,i);
					break;
				case 19:
					mem=decompileMoves(RomOpen2,i);
					break;
				case 21:
					mem=decompileMart(RomOpen2,i);
					break;
				case 22:
					mem=decompileAttacks(RomOpen2,i);
					break;
				case 25:
					mem=decompilePointer(RomOpen2,i);
					break;
				}
				if (mem)
				{
					SetWindowText(GetDlgItem(ScriptSearchWin,14),mem);
					GlobalFree(mem);
				}
			}
			else
			{
				SetWindowText(GetDlgItem(ScriptSearchWin,4),"000000");
			}
			break;
		case 7:
			GetWindowText(GetDlgItem(ScriptSearchWin,4),buf,50);
			i=-1;
			sscanf(buf,"0x%X",&i);
			if (i==-1)
				sscanf(buf,"%X",&i);
			if (i==-1)
				sscanf(buf,"$%X",&i);
			i--;
			sprintf(buf,"0x%X",i);
			SetWindowText(GetDlgItem(ScriptSearchWin,4),buf);
			switch (ScriptSearchType)
			{
			case 11:
				mem=decompile(RomOpen2,i,0);
				break;
			case 12:
				mem=decompileASM(RomOpen2,i|1);
				break;
			case 13:
				mem=decompileText(RomOpen2,i);
				break;
			case 17:
				mem=decompileLevel(RomOpen2,i);
				break;
			case 19:
				mem=decompileMoves(RomOpen2,i);
				break;
			case 21:
				mem=decompileMart(RomOpen2,i);
				break;
			case 22:
				mem=decompileAttacks(RomOpen2,i);
				break;
			case 25:
				mem=decompilePointer(RomOpen2,i);
				break;
			}
			if (mem)
			{
				SetWindowText(GetDlgItem(ScriptSearchWin,14),mem);
				GlobalFree(mem);
			}
			break;
		case 8:
			GetWindowText(GetDlgItem(ScriptSearchWin,4),buf,50);
			i=-1;
			sscanf(buf,"0x%X",&i);
			if (i==-1)
				sscanf(buf,"%X",&i);
			if (i==-1)
				sscanf(buf,"$%X",&i);
			i++;
			sprintf(buf,"0x%X",i);
			SetWindowText(GetDlgItem(ScriptSearchWin,4),buf);
			switch (ScriptSearchType)
			{
			case 11:
				mem=decompile(RomOpen2,i,0);
				break;
			case 12:
				mem=decompileASM(RomOpen2,i|1);
				break;
			case 13:
				mem=decompileText(RomOpen2,i);
				break;
			case 17:
				mem=decompileLevel(RomOpen2,i);
				break;
			case 19:
				mem=decompileMoves(RomOpen2,i);
				break;
			case 21:
				mem=decompileMart(RomOpen2,i);
				break;
			case 22:
				mem=decompileAttacks(RomOpen2,i);
				break;
			case 25:
				mem=decompilePointer(RomOpen2,i);
				break;
			}
			if (mem)
			{
				SetWindowText(GetDlgItem(ScriptSearchWin,14),mem);
				GlobalFree(mem);
			}
			break;
		case 6:
			GetWindowText(GetDlgItem(ScriptSearchWin,4),buf,50);
			i=-1;
			sscanf(buf,"0x%X",&i);
			if (i==-1)
				sscanf(buf,"%X",&i);
			if (i==-1)
				sscanf(buf,"$%X",&i);
			i-=16;
			sprintf(buf,"0x%X",i);
			SetWindowText(GetDlgItem(ScriptSearchWin,4),buf);
			switch (ScriptSearchType)
			{
			case 11:
				mem=decompile(RomOpen2,i,0);
				break;
			case 12:
				mem=decompileASM(RomOpen2,i|1);
				break;
			case 13:
				mem=decompileText(RomOpen2,i);
				break;
			case 17:
				mem=decompileLevel(RomOpen2,i);
				break;
			case 19:
				mem=decompileMoves(RomOpen2,i);
				break;
			case 21:
				mem=decompileMart(RomOpen2,i);
				break;
			case 22:
				mem=decompileAttacks(RomOpen2,i);
				break;
			case 25:
				mem=decompilePointer(RomOpen2,i);
				break;
			}
			if (mem)
			{
				SetWindowText(GetDlgItem(ScriptSearchWin,14),mem);
				GlobalFree(mem);
			}
			break;
		case 9:
			GetWindowText(GetDlgItem(ScriptSearchWin,4),buf,50);
			i=-1;
			sscanf(buf,"0x%X",&i);
			if (i==-1)
				sscanf(buf,"%X",&i);
			if (i==-1)
				sscanf(buf,"$%X",&i);
			i+=16;
			sprintf(buf,"0x%X",i);
			SetWindowText(GetDlgItem(ScriptSearchWin,4),buf);
			switch (ScriptSearchType)
			{
			case 11:
				mem=decompile(RomOpen2,i,0);
				break;
			case 12:
				mem=decompileASM(RomOpen2,i|1);
				break;
			case 13:
				mem=decompileText(RomOpen2,i);
				break;
			case 17:
				mem=decompileLevel(RomOpen2,i);
				break;
			case 19:
				mem=decompileMoves(RomOpen2,i);
				break;
			case 21:
				mem=decompileMart(RomOpen2,i);
				break;
			case 22:
				mem=decompileAttacks(RomOpen2,i);
				break;
			case 25:
				mem=decompilePointer(RomOpen2,i);
				break;
			}
			if (mem)
			{
				SetWindowText(GetDlgItem(ScriptSearchWin,14),mem);
				GlobalFree(mem);
			}
			break;
		case 16:
			i=SendMessage(GetDlgItem(ScriptSearchWin,14),WM_GETTEXTLENGTH,0,0);
			mem=GlobalAlloc(GPTR,i+3);
			if (mem)
			{
				*mem='\n';
				GetWindowText(GetDlgItem(ScriptSearchWin,14),mem+1,i+1);
				mem[i+2]='\n';
				SendEditor(SCI_ADDTEXT,i,(LPARAM)mem);
				GlobalFree(mem);
			}
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(ScriptSearchWin,SW_HIDE);
		romloaded=0;
		if (mem2);
		GlobalFree(mem2);
		mem2=NULL;
		break;
	default:
		return 0;
	}
	return 1;
}

char detecttype=1;

BOOL CALLBACK LockFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HANDLE file;
	char thing;
	DWORD bytes;
	char buf[50];
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 1:
			ShowWindow(LockWin,SW_HIDE);
			break;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			detecttype=LOWORD(wParam)-2;
			break;
		case 10:
			file=CreateFile(RomOpen2,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
			if (file==INVALID_HANDLE_VALUE)
			{
				MessageBox(LockWin,GetString1(3096),GetString2(3001),0x10);
				break;
			}
			SetFilePointer(file,0xAC,NULL,FILE_BEGIN);
			thing=rand()&0xFF;
			WriteFile(file,&thing,1,&bytes,NULL);
			thing=rand()&0xFF;
			WriteFile(file,&thing,1,&bytes,NULL);
			thing=rand()&0xFF;
			WriteFile(file,&thing,1,&bytes,NULL);
			thing=rand()&0xFF;
			WriteFile(file,&thing,1,&bytes,NULL);
			CloseHandle(file);
			break;
		case 8:
			file=CreateFile(RomOpen2,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
			if (file==INVALID_HANDLE_VALUE)
			{
				MessageBox(LockWin,GetString1(3096),GetString2(3001),0x10);
				break;
			}
			switch (detecttype)
			{
			case 1:
				SetFilePointer(file,0x11B,NULL,FILE_BEGIN);
				ReadFile(file,&thing,1,&bytes,NULL);
				//if not FR or LG, 0x256 - d6 ruby 9e sapphire else ????
				switch (thing)
				{
				case 0:
					SetDlgItemText(LockWin,9,"BPRE");
					break;
				case 0xE1:
					SetFilePointer(file,0x256,NULL,FILE_BEGIN);
					ReadFile(file,&thing,1,&bytes,NULL);
					switch (thing)
					{
					case 0xD6:
						SetDlgItemText(LockWin,9,"AXVE");
						break;
					case 0x9E:
						SetDlgItemText(LockWin,9,"AXPE");
						break;
					default:
						SetDlgItemText(LockWin,9,"????");
					}
					break;
				case 0x6F:
					SetDlgItemText(LockWin,9,"BPGE");
					break;
				case 0x73:
					SetDlgItemText(LockWin,9,"BPEE");
					break;
				default:
					SetDlgItemText(LockWin,9,"????");
				}
				break;
			case 2:
				SetFilePointer(file,0x318,NULL,FILE_BEGIN);
				ReadFile(file,&thing,1,&bytes,NULL);
				switch (thing)
				{
				case 0xCB:
					SetDlgItemText(LockWin,9,"AXPE");
					break;
				case 0xC9:
					SetDlgItemText(LockWin,9,"AXVE");
					break;
				case 0x7C:
					SetFilePointer(file,0x3AE,NULL,FILE_BEGIN);
					ReadFile(file,&thing,1,&bytes,NULL);
					switch (thing)
					{
					case 0xE8:
						SetDlgItemText(LockWin,9,"BPRE");
						break;
					case 0xD6:
						SetDlgItemText(LockWin,9,"BPGE");
						break;
					case 0x7C:
						SetDlgItemText(LockWin,9,"BPEE");
						break;
					default:
						SetDlgItemText(LockWin,9,"????");
					}
					break;
				default:
					SetDlgItemText(LockWin,9,"????");
				}
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			}
			CloseHandle(file);
			break;
		case 2:
			GetDlgItemText(LockWin,9,buf,5);
			file=CreateFile(RomOpen2,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
			if (file==INVALID_HANDLE_VALUE)
			{
				MessageBox(LockWin,GetString1(3096),GetString2(3001),0x10);
				break;
			}
			SetFilePointer(file,0xAC,NULL,FILE_BEGIN);
			modechar=*(unsigned int*)buf;
			WriteFile(file,&buf,4,&bytes,NULL);
			CloseHandle(file);
			break;
		case 11:
			file=CreateFile(RomOpen2,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
			if (file==INVALID_HANDLE_VALUE)
			{
				MessageBox(LockWin,GetString1(3096),GetString2(3001),0x10);
				break;
			}
			SetFilePointer(file,0xAC,NULL,FILE_BEGIN);
			ReadFile(file,&buf,4,&bytes,NULL);
			buf[4]=buf[5]=buf[6]=buf[7]=
			                         buf[8]=buf[9]=buf[10]=buf[11]=0;
			SetDlgItemText(LockWin,9,buf);
			modechar=*(unsigned int*)buf;
			CloseHandle(file);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(LockWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}
#ifdef SCRAP
int DescLen;

int GetVal(char*buf)
{
	int i=-1;
	sscanf(buf,"0x%X",&i);
	if (i==-1)
	{
		sscanf(buf,"$%x",&i);
		if (i==-1)
		{
			sscanf(buf,"%u",&i);
			if (i==-1)i=0;
		}
	}
	return i;
}

BOOL CALLBACK PickupFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int i,j,k,start;
	HANDLE file,fileM;
	char buf[1024],buf2[4096];
	DWORD read;
	switch (msg)
	{
	case WM_NOTIFY:
		switch (LOWORD(wParam))
		{
		case 1000:
			switch (SendMessage(GetDlgItem(PickupWin,1000),TCM_GETCURSEL,0,0))
			{
			case 0:
				for (i=1;i<=10;i++)ShowWindow(GetDlgItem(PickupWin,i),SW_SHOW);
				for (i=50;i<=87;i++)ShowWindow(GetDlgItem(PickupWin,i),SW_HIDE);
				break;
			case 1:
				for (i=50;i<=87;i++)ShowWindow(GetDlgItem(PickupWin,i),SW_SHOW);
				for (i=1;i<=10;i++)if (i!=2)ShowWindow(GetDlgItem(PickupWin,i),SW_HIDE);
				break;
			}
			break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 52:
			start=SendMessage(GetDlgItem(PickupWin,50),CB_GETCURSEL,0,0);
			if (start==-1)break;
			file=CreateFile(RomOpen2,GENERIC_READ|GENERIC_WRITE,
			                FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
			                NULL,OPEN_EXISTING,0,NULL);
			if (file==INVALID_HANDLE_VALUE)
			{
				MessageBox(PickupWin,GetString1(3096),GetString2(3001),0x10);
				break;
			}
			if (modechar=='ERPB')SetFilePointer(file,0x3DB028+0x2C*start,NULL,FILE_BEGIN);
			else if (modechar=='EGPB')SetFilePointer(file,0x3DAE64+0x2C*start,NULL,FILE_BEGIN);
			else {
				CloseHandle(file);
				MessageBox(PickupWin,GetString1(3098),GetString2(3001),0x10);
				break;
			}
			GetDlgItemText(PickupWin,56,buf2,sizeof(buf2));
			Ascii2Game(buf2,buf,0xE);
			buf[0xE]=0xFF;
			WriteFile(file,buf,0xE,&read,NULL);
			GetDlgItemText(PickupWin,57,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x2,&read,NULL);
			GetDlgItemText(PickupWin,61,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x2,&read,NULL);
			GetDlgItemText(PickupWin,79,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x1,&read,NULL);
			GetDlgItemText(PickupWin,76,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x1,&read,NULL);
			GetDlgItemText(PickupWin,75,buf2,sizeof(buf2));
			i=Ascii2Game(buf2,buf,sizeof(buf));
			k=i+1;
			memcpy(buf2,buf,sizeof(buf));
			if (i>DescLen)
			{
				i=0x08000000|FindFreeSpace(RomOpen2,i+100,0x740000);
				WriteFile(file,&i,4,&read,NULL);
			}
			else
			{
				ReadFile(file,&i,4,&read,NULL);
			}
			start=i; //DESCRIPTION POINTER
			GetDlgItemText(PickupWin,63,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x2,&read,NULL);
			for (j=0;j<5;j++)
				if (IsDlgButtonChecked(PickupWin,82+j))break;
			j++;
			if (j>5)
			{
				GetDlgItemText(PickupWin,65,buf,20);
				j=GetVal(buf);
			}
			WriteFile(file,&j,0x1,&read,NULL);
			GetDlgItemText(PickupWin,81,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x1,&read,NULL);
			GetDlgItemText(PickupWin,67,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x4,&read,NULL);
			GetDlgItemText(PickupWin,69,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x4,&read,NULL);
			GetDlgItemText(PickupWin,73,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x4,&read,NULL);
			GetDlgItemText(PickupWin,71,buf,20);
			i=GetVal(buf);
			WriteFile(file,&i,0x4,&read,NULL);
			SetFilePointer(file,start&0x07FFFFFF,NULL,FILE_BEGIN);
			WriteFile(file,buf2,k,&read,NULL);
			CloseHandle(file);
			break;
		case 51:
			SendMessage(GetDlgItem(PickupWin,50),CB_RESETCONTENT,0,0);
			FillItemNames(GetDlgItem(PickupWin,50),RomOpen2);
			break;
		case 50:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				start=SendMessage(GetDlgItem(PickupWin,50),CB_GETCURSEL,0,0);
				if (start==-1)break;
				file=CreateFile(RomOpen2,GENERIC_READ,
				                FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
				                NULL,OPEN_EXISTING,0,NULL);
				if (file==INVALID_HANDLE_VALUE)
				{
					MessageBox(PickupWin,GetString1(3096),GetString2(3001),0x10);
					break;
				}
				if (modechar=='ERPB')SetFilePointer(file,0x3DB028+0x2C*start,NULL,FILE_BEGIN);
				else if (modechar=='EGPB')SetFilePointer(file,0x3DAE64+0x2C*start,NULL,FILE_BEGIN);
				else {
					CloseHandle(file);
					MessageBox(PickupWin,GetString1(3098),GetString2(3001),0x10);
					break;
				}
				EnableWindow(GetDlgItem(PickupWin,52),1);
				ReadFile(file,buf,0xE,&read,NULL);
				Game2Ascii(buf,buf2,0);
				SetDlgItemText(PickupWin,56,buf2);
				i=0;
				ReadFile(file,&i,0x2,&read,NULL);
				sprintf(buf2,"0x%X",i);
				SetDlgItemText(PickupWin,57,buf2);
				i=0;
				ReadFile(file,&i,0x2,&read,NULL);
				sprintf(buf2,"%u",i);
				SetDlgItemText(PickupWin,61,buf2);
				i=0;
				ReadFile(file,&i,0x1,&read,NULL);
				sprintf(buf2,"%u",i);
				SetDlgItemText(PickupWin,79,buf2);
				i=0;
				ReadFile(file,&i,0x1,&read,NULL);
				sprintf(buf2,"%u",i);
				SetDlgItemText(PickupWin,76,buf2);
				ReadFile(file,&i,0x4,&read,NULL);
				start=i;
				i=0;
				ReadFile(file,&i,0x2,&read,NULL);
				sprintf(buf2,"0x%X",i);
				SetDlgItemText(PickupWin,63,buf2);
				i=0;
				ReadFile(file,&i,0x1,&read,NULL);
				sprintf(buf2,"%u",i);
				SetDlgItemText(PickupWin,65,buf2);
				for (j=0;j<5;j++)
					if (i==j+1) CheckDlgButton(PickupWin,82+j,1);
					else       CheckDlgButton(PickupWin,82+j,0);
				if (i==0||i>5)
					CheckDlgButton(PickupWin,87,1);
				ReadFile(file,&i,0x1,&read,NULL);
				sprintf(buf2,"%u",i);
				SetDlgItemText(PickupWin,81,buf2);
				i=0;
				ReadFile(file,&i,0x4,&read,NULL);
				sprintf(buf2,"0x%X",i);
				SetDlgItemText(PickupWin,67,buf2);
				i=0;
				ReadFile(file,&i,0x4,&read,NULL);
				sprintf(buf2,"%u",i);
				SetDlgItemText(PickupWin,69,buf2);
				i=0;
				ReadFile(file,&i,0x4,&read,NULL);
				sprintf(buf2,"0x%X",i);
				SetDlgItemText(PickupWin,73,buf2);
				i=0;
				ReadFile(file,&i,0x4,&read,NULL);
				sprintf(buf2,"0x%X",i);
				SetDlgItemText(PickupWin,71,buf2);
				if (start)
				{
					SetFilePointer(file,start&0x07FFFFFF,NULL,FILE_BEGIN);
					ReadFile(file,buf,0x3FF,&read,NULL);
					buf[0x3FF]=0xFF;
					Game2Ascii(buf,buf2,1);
					SetDlgItemText(PickupWin,75,buf2);
					for (DescLen=0;(unsigned char)buf[DescLen]!=0xFF;DescLen++);
				} else {
					DescLen=0;
					SetDlgItemText(PickupWin,75,"");
				}
				CloseHandle(file);
			}
			break;
		case 5:
			if (HIWORD(wParam)==LBN_SELCHANGE)
			{
				start=SendMessage(GetDlgItem(PickupWin,5),LB_GETCURSEL,0,0);
				sprintf(buf,"%u",*(unsigned short*)(PickupBuf+2+(start<<2)));
				SetDlgItemText(PickupWin,4,buf);
				start=*(unsigned short*)(PickupBuf+(start<<2));
				SendMessage(GetDlgItem(PickupWin,3),CB_SETCURSEL,start,0);
			}
			break;
		case 3:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				i=SendMessage(GetDlgItem(PickupWin,5),LB_GETCURSEL,0,0);
				if (i==-1)break;
				start=SendMessage(GetDlgItem(PickupWin,3),CB_GETCURSEL,0,0);
				if (start!=-1)
				{
					GetItemNameRom(buf2,start,RomOpen2);
					sprintf(buf,"%02u - %s",i+1,buf2);
					SendMessage(GetDlgItem(PickupWin,5),LB_DELETESTRING,i,0);
					SendMessage(GetDlgItem(PickupWin,5),LB_ADDSTRING,0,(LPARAM)buf);
					SendMessage(GetDlgItem(PickupWin,5),LB_SETCURSEL,i,0);
					*(unsigned short*)(PickupBuf+(i<<2))=start;
				}
				GetDlgItemText(PickupWin,4,buf,sizeof(buf));
				sscanf(buf,"%u",&start);
				*(unsigned short*)(PickupBuf+(i<<2)+2)=start;
			}
			break;
		case 1:
			file=CreateFile(RomOpen2,GENERIC_READ|GENERIC_WRITE,
			                FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
			                NULL,OPEN_EXISTING,0,NULL);
			if (file!=INVALID_HANDLE_VALUE)
			{
				SetFilePointer(file,0x2CEE4,NULL,FILE_BEGIN);
				ReadFile(file,&start,4,&read,NULL);
				if (start&0x8000000)
				{
					SetFilePointer(file,start&0x07FFFFFF,NULL,FILE_BEGIN);
					i=SendMessage(GetDlgItem(PickupWin,5),LB_GETCURSEL,0,0);
					GetDlgItemText(PickupWin,3,buf,1024);
					start=GetItemIDRom(buf,RomOpen2);
					if (start!=-1)
					{
						GetItemNameRom(buf2,start,RomOpen2);
						sprintf(buf,"%02u - %s",i+1,buf2);
						SendMessage(GetDlgItem(PickupWin,5),LB_DELETESTRING,i,0);
						SendMessage(GetDlgItem(PickupWin,5),LB_ADDSTRING,0,(LPARAM)buf);
						SendMessage(GetDlgItem(PickupWin,5),LB_SETCURSEL,i,0);
						*(unsigned short*)(PickupBuf+(i<<2))=start;
					}
					GetDlgItemText(PickupWin,4,buf,sizeof(buf));
					sscanf(buf,"%u",&start);
					*(unsigned short*)(PickupBuf+(i<<2)+2)=start;
					WriteFile(file,PickupBuf,0x40,&read,NULL);
				}
				else
				{
					MessageBox(MainWnd,GetString1(3098),GetString2(3028),0x30);
				}
				CloseHandle(file);
			}
			break;
		case 2:
		case 2000:
			ShowWindow(PickupWin,SW_HIDE);
			break;
		case 6:
			SendMessage(GetDlgItem(PickupWin,3),CB_RESETCONTENT,0,0);
			SendMessage(GetDlgItem(PickupWin,5),LB_RESETCONTENT,0,0);
			if (SendMessage(GetDlgItem(PickupWin,1000),TCM_GETCURSEL,0,0)==0)
			{
				FillItemNames(GetDlgItem(PickupWin,3),RomOpen2);
				fileM=CreateFile(RomOpen2,GENERIC_READ,
				                 FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
				                 NULL,OPEN_EXISTING,0,NULL);
				if (fileM!=INVALID_HANDLE_VALUE)
				{
					SetFilePointer(fileM,0x2CEE4,NULL,FILE_BEGIN);
					ReadFile(fileM,&start,4,&read,NULL);
					if (start&0x8000000)
					{
						SetFilePointer(fileM,start&0x07FFFFFF,NULL,FILE_BEGIN);
						ReadFile(fileM,PickupBuf,0x40,&read,NULL);
						start=*(unsigned short*)(PickupBuf);
						SendMessage(GetDlgItem(PickupWin,3),CB_SETCURSEL,start,0);
						sprintf(buf,"%u",*(unsigned short*)(PickupBuf+2));
						SetDlgItemText(PickupWin,4,buf);
						for (i=0;i<16;i++)
						{
							start=*(unsigned short*)(PickupBuf+(i<<2));
							GetItemNameRom(buf2,start,RomOpen2);
							sprintf(buf,"%02u - %s",i+1,buf2);
							SendMessage(GetDlgItem(PickupWin,5),LB_ADDSTRING,0,(LPARAM)buf);
						}
						SendMessage(GetDlgItem(PickupWin,5),LB_SETCURSEL,0,0);
						EnableWindow(GetDlgItem(PickupWin,1),1);
					}
					else
					{
						MessageBox(MainWnd,GetString1(3098),GetString2(3028),0x30);
					}
					CloseHandle(fileM);
				}
			}
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(PickupWin,SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}
#endif

