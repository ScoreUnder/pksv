@set path=c:\msys\mingw\bin\;..\..\bin;%PATH%
@windres.exe -o res.o vcpksv2.rc
@gcc.exe -o pksvui.exe pksvui.c res.o -s -Wl,--subsystem,windows -lcomdlg32 -lgdi32 -lwsock32 -lcomctl32 -O3 -frename-registers -funswitch-loops -fweb %*
@echo All done! Press any key to exit.
@pause>nul
