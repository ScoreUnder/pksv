@echo off
del pksvui_pkg%1.zip
7za a -tzip pksvui_pkg%1.zip license.txt pksv.dll pksvui.exe defines.dat AssociateShell.bat Scintilla.dll
del pksvui_sourcecode%1.zip
7za a -tzip pksvui_sourcecode%1.zip default_flags.h toolbar.bmp vcpksv2.rc AssociateShell.bat license.txt pksv.dll pksvui.exe main.ico pksvui.c WindowsXP.manifest build.bat Package.bat defines.dat ball.ico script.ico Scintilla.dll sqrt.h XPMData.h arrows.bmp hero.bmp down.bmp up.bmp left.bmp right.bmp say_!!.bmp say_!.bmp say_X.bmp say_smile.bmp say_q.bmp
pause