@set path=c:\msys\mingw\bin\;..\..\bin;%PATH%
@windres.exe -o res.o pksv.rc
@gcc.exe -o pksv.exe pksv.c res.o -lcomdlg32
@gcc.exe -o pksvnores.exe pksv.c -lcomdlg32
@echo All done! Press any key to exit.
@pause>nul
